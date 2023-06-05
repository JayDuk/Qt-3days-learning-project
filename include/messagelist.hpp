#ifndef __MESSAGE_LIST_H__
#define __MESSAGE_LIST_H__

#include "message.h"
#include "net/NetUtility.h"
#include "net/netutility.h"

#include <qboxlayout.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qmap.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qsplitter.h>
#include <qstackedlayout.h>
#include <qtextedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class MessageList : public QWidget
{
    Q_OBJECT
    QPushButton* chattingUsername_;
    QMap<QString, QWidget*> userMessageWidgets_;
    QMap<QString, QVBoxLayout*> userMessageLayouts_;
    QStackedLayout* messageLayout_;
    bool isSingleChat_ = false;

  public:
    MessageList(QWidget* parent = nullptr)
      : QWidget(parent)
    {
        QVBoxLayout* layout = new QVBoxLayout(this);

        chattingUsername_ = new QPushButton("聊天窗口", this);
        chattingUsername_->setStyleSheet("color: blue; font-size: 14px;");
        layout->addWidget(chattingUsername_);

        messageLayout_ = new QStackedLayout(this);
        // messageLayout_->addWidget(new QWidget(this));
        buildChatMessageOfUser("TEST");

        layout->addLayout(messageLayout_);

        layout->setStretch(0, 1);

        connect(NetUtility::instance(), &NetUtility::onGetMessage, this, &MessageList::onGetMessage);
    }

    ~MessageList()
    {
    }

  public slots:
    void sendMessage(QString message)
    {
        QJsonObject object;
        object["username"] = NetUtility::instance()->username().c_str();
        object["to"] = chattingUsername_->text();
        object["message"] = message;
        object["single"] = isSingleChat_;

        std::string data = QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();

        NetUtility::instance()
            ->request(Regulation::kChat, data)
            .err([this](const Response& response) {
                QMessageBox::information(this,
                                         tr("message list.hpp"),
                                         tr("发送失败"),
                                         QMessageBox::Ok,
                                         QMessageBox::Ok);
            });
    }

    void onSwitchToChattingWindow(QString username, bool isSingleChat)
    {
        isSingleChat_ = isSingleChat;
        chattingUsername_->setText(username);

        if (!userMessageWidgets_.contains(username))
            buildChatMessageOfUser(username);
        messageLayout_->setCurrentWidget(userMessageWidgets_[username]);
    }

    void onGetMessage(QString chatWindow, QString speaker, QString message, QString time)
    {
        if (!userMessageWidgets_.contains(chatWindow))
            buildChatMessageOfUser(chatWindow);

        QVBoxLayout* layout = userMessageLayouts_[chatWindow];
        layout->addWidget(new Message(speaker, time, message));
    }

  private:
    QWidget* buildChatMessageOfUser(QString username)
    {
        QSplitter* splitter = new QSplitter(Qt::Vertical, this);

        QScrollArea* scrollArea = new QScrollArea(this);
        QWidget* widget = new QWidget(this);
        scrollArea->setWidget(widget);

        QVBoxLayout* layout = new QVBoxLayout(widget);

        layout->setSpacing(3);
        layout->setAlignment(Qt::AlignLeft);

        // layout->addStretch();

        splitter->addWidget(scrollArea);

        QWidget* inputWidget = new QWidget(this);
        QHBoxLayout* inputLayout = new QHBoxLayout(inputWidget);
        inputLayout->setSpacing(0);

        QTextEdit* input = new QTextEdit(inputWidget);
        input->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        inputLayout->addWidget(input);

        QPushButton* send = new QPushButton("发送", inputWidget);
        send->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        inputLayout->addWidget(send);

        connect(input, &QTextEdit::textChanged, send, [=]() {
            send->setDisabled(input->toPlainText().isEmpty());
        });

        connect(send, &QPushButton::clicked, this, [=]() {
            sendMessage(input->toPlainText());
            input->clear();
            input->setFocus();
        });

        inputLayout->setStretch(0, 9);
        inputLayout->setStretch(1, 1);

        splitter->addWidget(inputWidget);

        splitter->setStretchFactor(0, 1500);
        splitter->setStretchFactor(1, 1);

        userMessageLayouts_[username] = layout;
        userMessageWidgets_[username] = splitter;
        messageLayout_->addWidget(splitter);
        return splitter;
    }
};

#endif  //_message list_h_