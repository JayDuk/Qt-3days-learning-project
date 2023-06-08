#ifndef __ADD_FRIEND_DIALOG_H__
#define __ADD_FRIEND_DIALOG_H__

#include "net/netutility.h"
#include "net/regulation.h"
#include "net/response.h"
#include <qboxlayout.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlayoutitem.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qstackedlayout.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ItemWidget : public QWidget
{
    Q_OBJECT

  signals:
    void clicked();

  public:
    ItemWidget(QString labelText, QString btnText, QWidget* parent = nullptr)
      : QWidget(parent)
    {
        QHBoxLayout* layout = new QHBoxLayout(this);
        this->setLayout(layout);
        layout->setContentsMargins(0, 0, 0, 0);  // 设置布局的边距

        QLabel* label = new QLabel(labelText, this);
        layout->addWidget(label);

        layout->addStretch();

        QPushButton* button = new QPushButton(btnText, this);
        layout->addWidget(button);

        connect(button, &QPushButton::clicked, this, &ItemWidget::clicked);
    }
};

class ToolBarDialog : public QDialog
{
    Q_OBJECT

    QStackedLayout* stackedLayout_;
    QListWidget* funcListWidget_;

  public:
    ToolBarDialog(QWidget* parent = nullptr)
      : QDialog(parent)
    {
        setWindowTitle("功能列表");

        QVBoxLayout* layout = new QVBoxLayout(this);
        setLayout(layout);

        funcListWidget_ = new QListWidget(this);
        funcListWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        funcListWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        funcListWidget_->setFixedHeight(30);
        funcListWidget_->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        funcListWidget_->setFlow(QListView::LeftToRight);
        funcListWidget_->addItem("添加好友");
        funcListWidget_->addItem("好友申请");
        funcListWidget_->addItem("创建群聊");
        funcListWidget_->addItem("加入群聊");
        funcListWidget_->addItem("群聊申请");
        layout->addWidget(funcListWidget_);
        stackedLayout_ = new QStackedLayout(this);
        stackedLayout_->addWidget(widgetLeft());
        stackedLayout_->addWidget(widgetRight());
        stackedLayout_->addWidget(widgetCreateGroup());
        stackedLayout_->addWidget(widgetAddGroup());
        stackedLayout_->addWidget(widgetGroupAddRequest());
        stackedLayout_->setCurrentIndex(0);
        layout->addLayout(stackedLayout_);

        connect(funcListWidget_, &QListWidget::currentRowChanged, stackedLayout_, &QStackedLayout::setCurrentIndex);
    }
    ~ToolBarDialog()
    {
    }

  private:
    QWidget* widgetLeft()
    {
        QListWidget* widget = new QListWidget(this);
        QJsonObject json;
        json["type"] = 3;
        json["username"] = NetUtility::instance()->username().c_str();
        std::string data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();

        connect(funcListWidget_, &QListWidget::currentRowChanged, widget, [=](int index) {
            if (index != 0)
                return;

            widget->clear();
            NetUtility::instance()
                ->request(Regulation::kInfo, data)
                .then(
                    [=](const Response& response) {
                        QJsonDocument jsonDocument = QJsonDocument::fromJson(response.data().c_str());
                        QJsonArray jsonArray = jsonDocument.array();

                        for (int i = 0; i < jsonArray.size(); ++i)
                        {
                            auto username = jsonArray.at(i).toString();

                            if (username.toStdString() == NetUtility::instance()->username())
                                continue;

                            QListWidgetItem* item = new QListWidgetItem(widget);
                            ItemWidget* itemWidget = new ItemWidget(username, "发送好友申请", widget);

                            widget->addItem(item);
                            widget->setItemWidget(item, itemWidget);

                            connect(itemWidget, &ItemWidget::clicked, this, [=]() {
                                onHandleFriendOperation(Regulation::kAdd, username);
                            });
                        }
                    });
        });
        return widget;
    }

    QWidget*
    widgetRight()
    {
        QListWidget* widget = new QListWidget(this);

        connect(NetUtility::instance(), &NetUtility::onGetAddFriend, widget, [=](QString friend_name) {
            QWidget* itemWidget = new QWidget(widget);
            QHBoxLayout* layout = new QHBoxLayout(itemWidget);
            itemWidget->setLayout(layout);

            layout->setContentsMargins(0, 0, 0, 0);

            QLabel* label = new QLabel(friend_name, itemWidget);
            layout->addWidget(label);
            layout->addStretch();
            QPushButton* button = new QPushButton("同意", itemWidget);
            layout->addWidget(button);

            QPushButton* button2 = new QPushButton("拒绝", itemWidget);
            layout->addWidget(button2);

            QListWidgetItem* item = new QListWidgetItem(widget);
            widget->addItem(item);
            widget->setItemWidget(item, itemWidget);

            connect(button, &QPushButton::clicked, this, [=]() {
                onHandleFriendOperation(Regulation::kAccept, friend_name);
                widget->removeItemWidget(item);
            });

            connect(button2, &QPushButton::clicked, this, [=]() {
                onHandleFriendOperation(Regulation::kRefuse, friend_name);
                widget->removeItemWidget(item);
            });
        });

        return widget;
    }

    QWidget* widgetCreateGroup()
    {
        QWidget* widget = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(widget);
        widget->setLayout(layout);

        QLineEdit* lineEdit = new QLineEdit(widget);
        lineEdit->setPlaceholderText("请输入群聊名");
        layout->addWidget(lineEdit);

        QPushButton* button = new QPushButton("创建群聊", widget);
        layout->addWidget(button);

        connect(button, &QPushButton::clicked, this, [=]() {
            QJsonObject json;
            json["username"] = NetUtility::instance()->username().c_str();
            json["groupname"] = lineEdit->text();
            std::string data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();

            NetUtility::instance()
                ->request(Regulation::kCreate, data)
                .then(
                    [=](const Response& response) {
                        QMessageBox::information(this, "发送成功", response.data().c_str());
                    })
                .err(
                    [=](const Response& response) {
                        QMessageBox::information(this, "发送失败", response.data().c_str());
                    });
        });

        return widget;
    }

    QWidget* widgetAddGroup()
    {
        QWidget* widget = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(widget);
        widget->setLayout(layout);

        QLineEdit* lineEdit = new QLineEdit(widget);
        lineEdit->setPlaceholderText("请输入群聊名");
        layout->addWidget(lineEdit);

        QPushButton* button = new QPushButton("加入群聊", widget);
        layout->addWidget(button);

        connect(button, &QPushButton::clicked, this, [=]() {
            QJsonObject json;
            json["username"] = NetUtility::instance()->username().c_str();
            json["friend"] = lineEdit->text();
            std::string data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();

            NetUtility::instance()
                ->request(Regulation::kAdd, data)
                .then(
                    [=](const Response& response) {
                        QMessageBox::information(this, "发送成功", response.data().c_str());
                    })
                .err(
                    [=](const Response& response) {
                        QMessageBox::information(this, "发送失败", response.data().c_str());
                    });
        });

        return widget;
    }

    QWidget* widgetGroupAddRequest()
    {
        QListWidget* widget = new QListWidget(this);

        connect(NetUtility::instance(), &NetUtility::onGetAddGroup, widget, [=](QString username, QString groupname) {
            QWidget* itemWidget = new QWidget(widget);
            QHBoxLayout* layout = new QHBoxLayout(itemWidget);
            itemWidget->setLayout(layout);

            layout->setContentsMargins(0, 0, 0, 0);

            QLabel* label = new QLabel(username + "->" + groupname, itemWidget);
            layout->addWidget(label);
            layout->addStretch();
            QPushButton* button = new QPushButton("同意", itemWidget);
            layout->addWidget(button);

            QPushButton* button2 = new QPushButton("拒绝", itemWidget);
            layout->addWidget(button2);

            QListWidgetItem* item = new QListWidgetItem(widget);
            widget->addItem(item);
            widget->setItemWidget(item, itemWidget);

            connect(button, &QPushButton::clicked, this, [=]() {
                QJsonObject json;
                json["username"] = username;
                json["friend"] = groupname;
                std::string data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();

                NetUtility::instance()
                    ->request(Regulation::kAccept, data)
                    .then(
                        [=](const Response& response) {
                            if (!response.data().empty())
                                QMessageBox::information(this, "", response.data().c_str());
                        })
                    .err(
                        [=](const Response& response) {
                            QMessageBox::information(this, "", response.data().c_str());
                        });
                widget->removeItemWidget(item);
            });

            connect(button2, &QPushButton::clicked, this, [=]() {
                QJsonObject json;
                json["username"] = username;
                json["friend"] = groupname;
                std::string data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();

                NetUtility::instance()
                    ->request(Regulation::kRefuse, data)
                    .then(
                        [=](const Response& response) {
                            if (!response.data().empty())
                                QMessageBox::information(this, "", response.data().c_str());
                        })
                    .err(
                        [=](const Response& response) {
                            QMessageBox::information(this, "", response.data().c_str());
                        });
                widget->removeItemWidget(item);
            });
        });

        return widget;
    }

  private slots:
    void onHandleFriendOperation(uint32_t oper, QString friend_name)
    {
        QJsonObject json;
        json["username"] = NetUtility::instance()->username().c_str();
        json["friend"] = friend_name;
        std::string data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();

        NetUtility::instance()
            ->request(oper, data)
            .then(
                [=](const Response& response) {
                    if (!response.data().empty())
                        QMessageBox::information(this, "添加好友", response.data().c_str());
                })
            .err(
                [=](const Response& response) {
                    QMessageBox::information(this, "添加好友", response.data().c_str());
                });
    }
};

#endif  //_add friend dialog_h_