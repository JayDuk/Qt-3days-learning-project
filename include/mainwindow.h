#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <qboxlayout.h>
#include <qlayout.h>
#include <qlistwidget.h>
#include <qmap.h>
#include <qstackedlayout.h>
#include <qwidget.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private:
    QStackedLayout* messageLayout_;
    QMap<QString, QWidget*> messageLayouts_;
    QListWidget* listWidget_;

  private:
    void initSocket();
    void initWidgets();

    void closeEvent(QCloseEvent* event) override;

    QWidget* buildFriendListWidget(QWidget* parent = nullptr);
    QWidget* buildMessageListWidget(QWidget* parent = nullptr);
    QWidget* buildToolWidget(QWidget* parent = nullptr);

  signals:
    void sendMessageOver(QString chatWindow, QString speaker, QString message, QString time);

  private slots:
    void onUserListChanged(int index);
    void onSelectUserToChat(QListWidgetItem* item);

    void onGetMessage(QString chatWindow, QString speaker, QString message, QString time);

  private:
    QWidget* getChatMessageLayout(QString username);
};
#endif  // MAINWINDOW_H
