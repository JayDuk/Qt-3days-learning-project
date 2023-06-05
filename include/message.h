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
        this->setLayout(layout);

        layout->setSpacing(0);
        layout->setAlignment(Qt::AlignLeft);
        layout->setContentsMargins(0, 0, 0, 0);

        QLabel* fromLabel = new QLabel(from);
        QLabel* timeLabel = new QLabel("[" + time + "]");
        QLabel* contentLabel = new QLabel(content);

        if (from.toStdString() == NetUtility::instance()->username())
            fromLabel->setStyleSheet("color: green; font-size: 14px;");
        else
            fromLabel->setStyleSheet("color: blue; font-size: 14px;");

        QWidget* titleWidget = new QWidget(this);
        QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);
        titleWidget->setLayout(titleLayout);

        titleLayout->addWidget(fromLabel);
        titleLayout->addWidget(timeLabel);
        titleLayout->setSpacing(10);
        titleLayout->setContentsMargins(0, 0, 0, 0);
        titleLayout->setAlignment(Qt::AlignLeft);

        layout->addWidget(titleWidget);

        layout->addWidget(contentLabel);
        contentLabel->setWordWrap(true);
        contentLabel->setMaximumWidth(this->width() - 20);
    }
    ~Message() {}
};

#endif  //_message_h_