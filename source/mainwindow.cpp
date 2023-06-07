#include "mainwindow.h"

#include <qabstractspinbox.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdebug.h>
#include <qglobal.h>
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
#include <string>

#include "addfrienddialog.hpp"
#include "login.h"
#include "message.h"
#include "messagelist.hpp"
#include "net/netutility.h"
#include "net/regulation.h"
#include "net/request.h"
#include "userlist.hpp"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
    initSocket();
    initWidgets();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initSocket()
{
    NetUtility::instance()->setParent(this);
}

void MainWindow::initWidgets()
{
    this->setWindowTitle("chat");
    this->resize(800, 600);

    LoginDialog* login = new LoginDialog(this);
    connect(login, &LoginDialog::loginSuccess, this, [=](QString username) {
        this->setWindowTitle(this->windowTitle() + "(" + username + ")");
    });
    login->setModal(true);
    login->exec();

    QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);
    this->setCentralWidget(splitter);

    UserList* userList = new UserList(this);
    MessageList* messageList = new MessageList(this);
    QWidget* toolbar = buildToolWidget();

    connect(userList, &UserList::onSelectUserToChat, messageList, &MessageList::onSwitchToChattingWindow);

    splitter->addWidget(userList);
    splitter->addWidget(messageList);
    splitter->addWidget(toolbar);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 10);
    splitter->setStretchFactor(2, 1);
}

QWidget* MainWindow::buildToolWidget(QWidget* parent)
{
    QWidget* widget = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    widget->setLayout(layout);

    QPushButton* button = new QPushButton("添加好友", widget);
    layout->addWidget(button);

    connect(button, &QPushButton::clicked, this, [=]() {
        AddFriendDialog* dialog = new AddFriendDialog(this);
        dialog->setModal(true);
        dialog->exec();
    });

    QPushButton* button1 = new QPushButton("删除好友", widget);
    layout->addWidget(button1);

    QPushButton* button2 = new QPushButton("创建群聊", widget);
    layout->addWidget(button2);

    layout->addStretch();

    return widget;
}
