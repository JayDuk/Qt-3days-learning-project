#ifndef __USER_LIST_H__
#define __USER_LIST_H__

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlistwidget.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "net/netutility.h"

class UserList : public QWidget
{
    Q_OBJECT
    QListWidget* list_;
    bool isSingleChat_ = false;
    QString activeWindow_;

  signals:
    void onSelectUserToChat(QString username, bool isSingleChat);

  public:
    UserList(QWidget* parent = nullptr)
      : QWidget(parent)
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        this->setLayout(layout);

        QComboBox* comboBox = new QComboBox(this);
        comboBox->addItem("用户列表");
        comboBox->addItem("好友列表");
        comboBox->addItem("群聊列表");
        layout->addWidget(comboBox);

        list_ = new QListWidget(this);
        list_->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        list_->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        list_->setUniformItemSizes(true);
        layout->addWidget(list_);

        connect(comboBox, &QComboBox::activated, this, &UserList::onSelectUserList);
        onSelectUserList(0);

        connect(list_, &QListWidget::itemActivated, this, [this](QListWidgetItem* item) {
            activeWindow_ = item->text().replace("(新消息)", "");
            item->setText(activeWindow_);
            emit onSelectUserToChat(activeWindow_, isSingleChat_);
        });

        connect(NetUtility::instance(), &NetUtility::onGetMessage, this, &UserList::onNewChatFromChatwindow);

        layout->setStretch(0, 3);
        layout->setStretch(1, 7);
    }

    ~UserList()
    {
    }

  private slots:
    void onNewChatFromChatwindow(QString chatwindow)
    {
        if (chatwindow == activeWindow_)
            return;

        QString newChatwindow = chatwindow + "(新消息)";
        for (int i = 0; i < list_->count(); ++i)
        {
            auto item = list_->item(i);
            if (item->text() == chatwindow)
                item->setText(newChatwindow);

            if (item->text() != newChatwindow)
                continue;

            list_->takeItem(i);
            list_->insertItem(0, item);
            return;
        }

        QListWidgetItem* item = new QListWidgetItem(chatwindow + "(新消息)", list_);
        item->setSizeHint(QSize(0, 30));
        item->setTextAlignment(Qt::AlignCenter);
        item->setToolTip(chatwindow);

        list_->insertItem(0, item);
    }
    void onSelectUserList(int index)
    {
        isSingleChat_ = index == 0 || index == 1;
        QJsonObject json;
        json.insert("type", index);
        std::string request_username = NetUtility::instance()->username();
        json.insert("username", request_username.c_str());
        std::string data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();
        NetUtility::instance()
            ->request(Regulation::kInfo, data)
            .then(
                [=](const Response& response) {
                    list_->clear();
                    QJsonDocument jsonDocument = QJsonDocument::fromJson(response.data().c_str());
                    QJsonArray jsonArray = jsonDocument.array();

                    for (int i = 0; i < jsonArray.size(); ++i)
                    {
                        auto username = jsonArray.at(i).toString();
                        if (username.toStdString() == request_username)
                            continue;

                        QListWidgetItem* item = new QListWidgetItem(username, list_);

                        item->setSizeHint(QSize(0, 30));
                        item->setTextAlignment(Qt::AlignCenter);
                        item->setToolTip(username);

                        list_->addItem(item);
                    }
                })
            .err(
                [=](const Response& response) {
                    auto messageBox = new QMessageBox(QMessageBox::Icon::Critical, "错误", "获取用户列表失败", QMessageBox::Ok, this);
                    messageBox->show();
                });
    }
};

#endif  //_user list_h_