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
#include <qpushbutton.h>
#include <qstackedlayout.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "net/netutility.h"

class UserList : public QWidget
{
    Q_OBJECT
    QString activeWindow_;

  signals:
    void onSelectUserToChat(QString username);

  public:
    UserList(QWidget* parent = nullptr)
      : QWidget(parent)
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        this->setLayout(layout);

        QComboBox* comboBox = new QComboBox(this);
        comboBox->addItem("消息列表");
        comboBox->addItem("在线列表");
        comboBox->addItem("好友列表");
        comboBox->addItem("群聊列表");
        layout->addWidget(comboBox);

        QPushButton* refresh = new QPushButton("刷新", this);
        connect(refresh, &QPushButton::clicked, this, [=]() {
            emit comboBox->activated(comboBox->currentIndex());
        });
        layout->addWidget(refresh);

        QStackedLayout* stackedLayout = new QStackedLayout(this);
        for (int i = 0; i < 4; ++i)
        {
            QListWidget* list = new QListWidget(this);
            stackedLayout->addWidget(list);

            connect(comboBox, &QComboBox::activated, this, [=](int index) {
                if (index == i)
                    onSelectUserList(index, list);
            });

            connect(list, &QListWidget::itemActivated, this, [this](QListWidgetItem* item) {
                activeWindow_ = item->text().replace("(新消息)", "");
                item->setText(activeWindow_);
                emit onSelectUserToChat(activeWindow_);
            });
        }
        connect(comboBox, &QComboBox::activated, stackedLayout, &QStackedLayout::setCurrentIndex);
        layout->addLayout(stackedLayout);

        layout->setStretch(0, 1);
        layout->setStretch(1, 1);
        layout->setStretch(2, 10);
    }

    ~UserList()
    {
    }

  private slots:
    // void onNewChatFromChatwindow(QString chatwindow)
    // {
    //     if (chatwindow == activeWindow_)
    //         return;

    //     QString newChatwindow = chatwindow + "(新消息)";
    //     for (int i = 0; i < list_->count(); ++i)
    //     {
    //         auto item = list_->item(i);
    //         if (item->text() == chatwindow)
    //             item->setText(newChatwindow);

    //         if (item->text() != newChatwindow)
    //             continue;

    //         list_->takeItem(i);
    //         list_->insertItem(0, item);
    //         return;
    //     }

    //     QListWidgetItem* item = new QListWidgetItem(chatwindow + "(新消息)", list_);
    //     item->setSizeHint(QSize(0, 30));
    //     item->setTextAlignment(Qt::AlignCenter);
    //     item->setToolTip(chatwindow);

    //     list_->insertItem(0, item);
    // }

    void onSelectUserList(int index, QListWidget* list)
    {
        if (--index < 0)
            return;

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

                        QListWidgetItem* item = new QListWidgetItem(username, list);

                        item->setSizeHint(QSize(0, 30));
                        item->setTextAlignment(Qt::AlignCenter);
                        item->setToolTip(username);

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