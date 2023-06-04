#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <qboxlayout.h>
#include <qglobal.h>
#include <qlabel.h>
#include <qwidget.h>

#include "net/netutility.h"

class Message : public QWidget
{
    Q_OBJECT
  public:
    Message(const QString& from, const QString& time, const QString& content)
    {
        QVBoxLayout* layout = new QVBoxLayout(this);

        layout->setSpacing(3);
        layout->setAlignment(Qt::AlignLeft);

        QLabel* fromLabel = new QLabel(from);
        QLabel* timeLabel = new QLabel("[" + time + "]");
        QLabel* contentLabel = new QLabel(content);

        if (from.toStdString() == NetUtility::instance()->username())
            fromLabel->setStyleSheet("color: green; font-size: 14px;");
        else
            fromLabel->setStyleSheet("color: blue; font-size: 14px;");

        QHBoxLayout* titleLayout = new QHBoxLayout(this);
        titleLayout->addWidget(fromLabel);
        titleLayout->addWidget(timeLabel);
        titleLayout->setSpacing(10);
        titleLayout->setAlignment(Qt::AlignLeft);

        layout->addLayout(titleLayout);

        layout->addWidget(contentLabel);
        contentLabel->setWordWrap(true);

        this->setLayout(layout);
    }
    ~Message() {}
};

#endif  //_message_h_