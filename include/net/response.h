#ifndef RESPONSE_H
#define RESPONSE_H

#include <QString>
#include <stdint.h>
#include <string>

#include "QTcpSocket"
#include "regulation.h"

class Response
{
    uint32_t seq_;
    uint32_t code_;
    std::string content_;

  private:
    Response(QTcpSocket& socket);

  public:
    static Response buildResponse(QTcpSocket& socket);

  public:
    uint32_t seq() const;

    bool isBroadcast() const;

    uint32_t code() const;
    std::string data() const
    {
        return content_;
    }
};

#endif  // RESPONSE_H
