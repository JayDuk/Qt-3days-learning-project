#include "login.h"
#include "net/netutility.h"
#include "net/regulation.h"

#include <qboxlayout.h>
#include <qfont.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <string>

LoginDialog::LoginDialog(QWidget* parent)
  : QDialog(parent)
{
    Qt::WindowFlags flags = this->windowFlags();
    flags |= Qt::WindowCloseButtonHint;   // 显示关闭按钮
    flags &= ~Qt::WindowCloseButtonHint;  // 隐藏关闭按钮
    this->setWindowFlags(flags);

    this->setWindowTitle("login");
    this->resize(300, 200);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLineEdit* username = new QLineEdit(this);
    username->setPlaceholderText("username");
    layout->addWidget(username);

    QLineEdit* password = new QLineEdit(this);
    password->setPlaceholderText("password");
    layout->addWidget(password);

    QLabel* tips = new QLabel(this);
    tips->setFont(QFont("Microsoft YaHei", 10, 10, true));
    tips->setStyleSheet("color:red");
    layout->addWidget(tips);

    QPushButton* login = new QPushButton(this);
    login->setText("login");
    layout->addWidget(login);

    QPushButton* exit = new QPushButton(this);
    exit->setText("exit");
    layout->addWidget(exit);

    this->setLayout(layout);

    connect(login, &QPushButton::clicked, this, [=]() {
        if (username->text().isEmpty() || password->text().isEmpty())
        {
            tips->setText("用户名和密码不能为空");
            return;
        }
        QJsonObject json;
        json.insert("username", username->text());
        json.insert("password", password->text());
        std::string data = QJsonDocument(json).toJson(QJsonDocument::Compact).toStdString();
        NetUtility::instance()
            ->request(Regulation::kLogin, data)
            .then([=](const Response& response) {
                NetUtility::instance()->set_username(username->text().toStdString());
                this->close();
            })
            .err([=](const Response& response) {
                tips->setText(QString::fromStdString(response.data()));
            });
    });

    connect(exit, &QPushButton::clicked, this, [=]() {
        ::exit(0);
    });
}

LoginDialog::~LoginDialog()
{
}