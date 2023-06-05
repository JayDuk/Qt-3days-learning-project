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
    void initSocket();
    void initWidgets();

    QWidget* buildToolWidget(QWidget* parent = nullptr);
};
#endif  // MAINWINDOW_H
