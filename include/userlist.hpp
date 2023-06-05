#ifndef __USER_LIST_H__
#define __USER_LIST_H__

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlistwidget.h>
#include <qmessagebox.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "net/netutility.h"

class UserList : public QWidget
{
    Q_OBJECT
    QListWidget* list;
    bool isSingleChat_ = false;

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

        list = new QListWidget(this);
        list->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        list->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        list->setUniformItemSizes(true);
        layout->addWidget(list);

        connect(comboBox, &QComboBox::activated, this, &UserList::onSelectUserList);
        onSelectUserList(0);

        connect(list, &QListWidget::itemActivated, this, [this](QListWidgetItem* item) {
            emit onSelectUserToChat(item->text(), isSingleChat_);
        });

        layout->setStretch(0, 3);
        layout->setStretch(1, 7);
    }

    ~UserList()
    {
    }

  private slots:
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
                    list->clear();
                    QJsonDocument jsonDocument = QJsonDocument::fromJson(response.data().c_str());
                    QJsonArray jsonArray = jsonDocument.array();

                    for (int i = 0; i < jsonArray.size(); ++i)
                    {
                        auto username = jsonArray.at(i).toString();
                        if (username.toStdString() == request_username)
                            continue;

                        QListWidgetItem* item = new QListWidgetItem(list);
                        item->setSizeHint(QSize(0, 50));
                        item->setTextAlignment(Qt::AlignCenter);
                        item->setText(username);

                        list->addItem(item);
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