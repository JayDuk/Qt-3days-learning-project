#include "net/NetUtility.h"
#include <QDebug>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QThread>
#include <memory>
#include <qabstractsocket.h>
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

    board_handles_.insert({Regulation::kChat,
                           [=](const Response& response) {
                               QJsonObject json = QJsonDocument::fromJson(response.data().c_str()).object();
                                 emit onGetMessage(json.value("username").toString(), json.value("message").toString());
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
    if (response.isBoardcast())
    {
        auto p_func = board_handles_.find(response.code());
        if (p_func == board_handles_.end())
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
