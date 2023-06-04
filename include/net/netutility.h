#ifndef NETUTILITY_H
#define NETUTILITY_H

#include <QtNetwork/QTcpSocket>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "net/regulation.h"
#include "net/requesthandler.h"
#include "net/response.h"

class NetUtility : public QObject
{
    Q_OBJECT
  public:
    static NetUtility* instance();

  private:
    NetUtility();
    NetUtility(const NetUtility&) = delete;
    NetUtility(NetUtility&&) = delete;

  private:
    QTcpSocket socket_;

    std::map<uint32_t, std::shared_ptr<RequestHandler>> reply_handles_;
    std::map<uint32_t, std::function<void(const Response&)>> board_handles_;

    std::string username_;

  signals:
    void onGetMessage(QString username, QString message);

  public:
    std::string username() const
    {
        return username_;
    }

    void set_username(std::string username)
    {
        qDebug() << "set " << username.c_str();
        username_ = username;
    }

  public:
    RequestHandler& request(uint32_t oper, std::string data);
    void close();

  private:
    std::function<void(const Response&)> dispatch(const Response& response);

  private slots:
    void handleRead();
};

#endif  // NETUTILITY_H
