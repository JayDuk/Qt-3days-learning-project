#include "mainwindow.h"
#include "login.h"
#include "message.h"
#include "net/NetUtility.h"
#include "net/regulation.h"
#include "net/request.h"
#include <QComboBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpacerItem>
#include <QSplitter>
#include <QStackedLayout>
#include <QThread>
#include <iostream>
#include <qabstractspinbox.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdebug.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlayoutitem.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qsizepolicy.h>
#include <qsplitter.h>
#include <qstackedlayout.h>
#include <qtextedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
    initSocket();

    connect(NetUtility::instance(), &NetUtility::onGetMessage, this, &MainWindow::onGetMessage);

    LoginDialog* login = new LoginDialog(this);
    login->setModal(true);
    login->exec();

    initWidgets();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initSocket()
{
    NetUtility::instance()->setParent(this);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    NetUtility::instance()->close();
    QMainWindow::closeEvent(event);
}

void MainWindow::initWidgets()
{
    this->setWindowTitle("chat");
    this->resize(800, 600);

    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);

    QWidget* left = buildFriendListWidget(splitter);
    splitter->addWidget(left);

    QWidget* mid = buildMessageListWidget(splitter);
    splitter->addWidget(mid);

    QWidget* right = buildToolWidget(splitter);
    splitter->addWidget(right);

    splitter->setStretchFactor(0, 5);
    splitter->setStretchFactor(1, 10);
    splitter->setStretchFactor(2, 5);
    this->setCentralWidget(splitter);

    QLabel* label = new QLabel(this);
    label->setText("hello world");
}
QWidget* MainWindow::buildFriendListWidget(QWidget* parent)
{
    QWidget* userListWidget = new QWidget(parent);
    QVBoxLayout* userListLayout = new QVBoxLayout(userListWidget);

    QComboBox* comboBox = new QComboBox(userListWidget);
    comboBox->addItem("用户列表");
    comboBox->addItem("好友列表");
    comboBox->addItem("群聊列表");
    userListLayout->addWidget(comboBox);

    listWidget_ = new QListWidget(userListWidget);

    listWidget_->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    listWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    listWidget_->setUniformItemSizes(true);

    connect(comboBox, &QComboBox::activated, this, &MainWindow::onUserListChanged);
    onUserListChanged(0);
    userListLayout->addWidget(listWidget_);

    userListLayout->setStretch(0, 3);
    userListLayout->setStretch(1, 7);

    return userListWidget;
}
QWidget* MainWindow::buildMessageListWidget(QWidget* parent)
{
    QSplitter* splitter = new QSplitter(parent);
    splitter->setOrientation(Qt::Vertical);

    QWidget* messageListWidget = new QWidget(parent);
    splitter->addWidget(messageListWidget);
    QVBoxLayout* messageListLayout = new QVBoxLayout(messageListWidget);
    messageListLayout->setSpacing(1);
    messageListWidget->setLayout(messageListLayout);

    QPushButton* button = new QPushButton("", messageListWidget);
    button->setDisabled(true);
    messageListLayout->addWidget(button);

    connect(listWidget_, &QListWidget::itemActivated, button, [=](QListWidgetItem* item) {
        button->setText(item->text());
    });

    QScrollArea* messageScrollArea = new QScrollArea(messageListWidget);
    messageScrollArea->setWidgetResizable(true);
    messageScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    messageScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    messageListLayout->addWidget(messageScrollArea);

    QWidget* messages = new QWidget();
    messageLayout_ = new QStackedLayout(messages);

    messageScrollArea->setWidget(messages);
    messageScrollArea->verticalScrollBar()->setValue(messageScrollArea->verticalScrollBar()->maximum());

    connect(listWidget_, &QListWidget::itemActivated, this, &MainWindow::onSelectUserToChat);

    QWidget* inputWidget = new QWidget(splitter);
    splitter->addWidget(inputWidget);

    QHBoxLayout* inputLayout = new QHBoxLayout(inputWidget);
    inputWidget->setLayout(inputLayout);
    QTextEdit* textEdit = new QTextEdit(inputWidget);
    QPushButton* sendButton = new QPushButton("send", inputWidget);
    sendButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    inputLayout->addWidget(textEdit);
    inputLayout->addWidget(sendButton);
    inputLayout->setSpacing(1);
    inputLayout->setStretch(0, 10);
    inputLayout->setStretch(1, 1);

    splitter->setStretchFactor(0, 10);
    splitter->setStretchFactor(1, 1);

    connect(textEdit, &QTextEdit::textChanged, sendButton, [=]() {
        QString text = textEdit->toPlainText();
        if (text.isEmpty())
            sendButton->setDisabled(true);
        else
            sendButton->setDisabled(false);
    });
    connect(this, &MainWindow::sendMessageOver, this, &MainWindow::onGetMessage);
    connect(sendButton, &QPushButton::clicked, this, [=]() {
        QString text = textEdit->toPlainText();
        textEdit->clear();
        textEdit->setFocus();
        QJsonObject json;
        json.insert("username", NetUtility::instance()->username().c_str());
        json.insert("message", text);
        json.insert("to", button->text());

        std::string data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();
        NetUtility::instance()
            ->request(4, data)
            .then(
                [=](const Response& response) {
                    emit sendMessageOver(button->text(), text);
                })
            .err(
                [=](const Response& response) {
                    auto ret = QMessageBox::warning(this, "error", QString::fromStdString(response.data()));
                });
    });

    return splitter;
}
QWidget* MainWindow::buildToolWidget(QWidget* parent)
{
    return new QWidget(parent);
}

void MainWindow::onUserListChanged(int index)
{
    QJsonObject json;
    json.insert("type", index);
    std::string request_username = NetUtility::instance()->username();
    json.insert("username", request_username.c_str());
    std::string data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();
    NetUtility::instance()
        ->request(2, data)
        .then(
            [=](const Response& response) {
                listWidget_->clear();
                QJsonDocument jsonDocument = QJsonDocument::fromJson(response.data().c_str());
                QJsonArray jsonArray = jsonDocument.array();

                for (int i = 0; i < jsonArray.size(); ++i)
                {
                    auto username = jsonArray.at(i).toString();
                    if (username.toStdString() == request_username)
                        continue;

                    QListWidgetItem* item = new QListWidgetItem(listWidget_);
                    item->setSizeHint(QSize(0, 50));
                    item->setTextAlignment(Qt::AlignCenter);
                    item->setText(username);

                    listWidget_->addItem(item);
                }
            })
        .err(
            [=](const Response& response) {
                QMessageBox::warning(this, "error", QString::fromStdString(response.data()));
            });
}
void MainWindow::onSelectUserToChat(QListWidgetItem* item)
{
    QString text = item->text();
    QWidget* widget = getChatMessageLayout(text);
    messageLayout_->setCurrentWidget(widget);
}

QWidget* MainWindow::getChatMessageLayout(QString username)
{
    if (messageLayouts_.contains(username))
        return messageLayouts_[username];

    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setSpacing(1);
    widget->setLayout(layout);

    messageLayout_->addWidget(widget);

    QSpacerItem* spacerItem = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addSpacerItem(spacerItem);

    messageLayouts_[username] = widget;
    return widget;
}

void MainWindow::onGetMessage(QString username, QString message)
{
    QWidget* widget = getChatMessageLayout(username);
    int index = widget->layout()->count() - 1;
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(widget->layout());
    layout->insertWidget(index, new Message(username, "2020-10-10", message));
}