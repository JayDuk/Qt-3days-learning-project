#include "net/response.h"
#include <QtEndian>

Response::Response(QTcpSocket& socket)
{
    // socket.waitForReadyRead();
    while (socket.bytesAvailable() < 4)
    {
        continue;
    }
    int str_len;
    socket.read((char*)&str_len, 4);
    str_len = qFromBigEndian(str_len);

    while (socket.bytesAvailable() < 4)
    {
        continue;
    }
    socket.read((char*)&seq_, 4);
    seq_ = qFromBigEndian(seq_);

    while (socket.bytesAvailable() < 4)
    {
        continue;
    }
    socket.read((char*)&code_, 4);
    code_ = qFromBigEndian(code_);

    while (socket.bytesAvailable() < str_len)
    {
        continue;
    }
    content_.resize(str_len);
    socket.read(content_.data(), str_len);

    // QDataStream in(&socket);
    // while (socket.bytesAvailable() < 4)
    //     continue;

    // int msg_len;
    // in >> msg_len;

    // while (socket.bytesAvailable() < msg_len)
    //     continue;

    // in >> seq_ >> (uint32_t&)code_ >> content_;
}
Response Response::buildResponse(QTcpSocket& socket)
{
    return Response(socket);
}
uint32_t Response::seq() const
{
    return seq_;
}
bool Response::isBroadcast() const
{
    return seq_ == Regulation::Boradcast_Seq;
}
uint32_t Response::code() const
{
    return code_;
}
