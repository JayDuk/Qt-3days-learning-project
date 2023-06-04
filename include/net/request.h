#ifndef REQUEST_H
#define REQUEST_H

#include <QIODevice>
#include <QString>
#include <iostream>
#include <qarraydata.h>
#include <qdebug.h>
#include <stdint.h>
#include <string>

class Request
{
    uint32_t oper_;
    uint32_t seq_;
    std::string content_;

  public:
    Request(uint32_t oper, std::string content)
      : oper_(oper)
      , seq_(buildSequence())
      , content_(content)
    {
    }

    int seq()
    {
        return seq_;
    }

    QByteArray toBytes()
    {
        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);

        out.setByteOrder(QDataStream::BigEndian);
        out << (uint32_t)content_.size() << seq_ << oper_;

        bytes.append(content_.c_str(), content_.size());
        return bytes;
    }

  private:
    static uint32_t buildSequence()
    {
        static uint32_t seq = 1;
        return seq++;
    }
};

#endif  // REQUEST_H
