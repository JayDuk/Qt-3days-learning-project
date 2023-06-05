#ifndef __LOGIN_H__
#define __LOGIN_H__

#include <QDialog>
#include <qdialog.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class LoginDialog : public QDialog
{
    Q_OBJECT

  public:
    LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog();

  signals:
    void loginSuccess(QString username);
};
#endif  //_login_h_