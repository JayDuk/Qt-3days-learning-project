#include "net/NetUtility.h"
#include <QDebug>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QThread>
#include <memory>
#include <qabstractsocket.h>
#include <qglobal.h>
#include <qhostaddress.h>
#include <qjsonobject.h>
#include <stdint.h>
#include <string>
#include <thread>

#include "net/regulation.h"
#include "net/request.h"
#include "net/requesthandler.h"
#include "net/response.h"

NetUtility* NetUtility::instance()
{
    static NetUtility netUtility;
    return &netUtility;
}

NetUtility::NetUtility()
{
    socket_.connectToHost(QHostAddress("1.14.110.4"), 9999);

    while (socket_.waitForConnected() == false)
    {
    }

    connect(&socket_, &QTcpSocket::readyRead, this, &NetUtility::handleRead);

    broad_handles_
        .insert({Regulation::kChat,
                 [=](const Response& response) {
                     QJsonObject json = QJsonDocument::fromJson(response.data().c_str()).object();

                     QString chatWindow = json["chatwindow"].toString();
                     QString speaker = json["speaker"].toString();
                     QString message = json["message"].toString();
                     qint64 time = json["time"].toDouble();

                     QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(time);
                     emit onGetMessage(chatWindow, speaker, message, dateTime.toString("yyyy-MM-dd hh:mm:ss"));
                 }});
}

RequestHandler& NetUtility::request(uint32_t oper, std::string data)
{
    Request request(oper, data);

    socket_.write(request.toBytes());

    auto request_handler = std::shared_ptr<RequestHandler>(new RequestHandler(request.seq()));
    reply_handles_.insert(std::make_pair(request.seq(), request_handler));
    return *request_handler;
}

void NetUtility::close()
{
    request(Regulation::kExit, "");
    socket_.close();
}

std::function<void(const Response&)> NetUtility::dispatch(const Response& response)
{
    if (response.isBroadcast())
    {
        auto p_func = broad_handles_.find(response.code());
        if (p_func == broad_handles_.end())
        {
            qDebug() << "no handle to accept response with operator " << response.code();
            return nullptr;
        }
        return p_func->second;
    }

    auto p_handler = reply_handles_.find(response.seq());
    if (p_handler == reply_handles_.end())
    {
        qDebug() << "no handle to accept response with seq " << response.seq();
        return nullptr;
    }
    auto func = (p_handler->second)->get(response.code());
    reply_handles_.erase(p_handler);
    return func;
}

void NetUtility::handleRead()
{
    auto response = Response::buildResponse(socket_);
    qDebug() << "handleRead";
    auto func = dispatch(response);
    if (func != nullptr)
        std::thread(func, response).detach();
}
