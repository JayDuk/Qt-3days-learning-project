// #include "mainwindow.h"

// #include <QApplication>
// #include <qlabel.h>
// #include <qpushbutton.h>

// int main(int argc, char* argv[])
// {
//     QApplication app(argc, argv);

//     // 创建 QListWidget
//     QListWidget listWidget;

//     // 添加多个列表项
//     for (int i = 0; i < 5; ++i)
//     {
//         // 创建 QListWidgetItem
//         QListWidgetItem* item = new QListWidgetItem;

//         // 创建自定义部件，包含一个标签和一个按钮
//         QWidget* widget = new QWidget;
//         QLabel* label = new QLabel(QString("Item %1").arg(i));
//         QPushButton* button = new QPushButton("Button");

//         // 创建水平布局
//         QHBoxLayout* layout = new QHBoxLayout(widget);
//         layout->addWidget(label);
//         layout->addWidget(button);
//         layout->setContentsMargins(0, 0, 0, 0);  // 设置布局的边距

//         // 将自定义部件设置为列表项的部件
//         listWidget.setItemWidget(item, widget);

//         // 设置列表项的大小
//         item->setSizeHint(widget->sizeHint());

//         // 添加列表项到 QListWidget
//         listWidget.addItem(item);
//     }

//     listWidget.show();
//     return app.exec();
// }

#include "mainwindow.h"
#include <qapplication.h>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
//
//
//
//
//
//
//
// #include <QApplication>
// #include <QHBoxLayout>
// #include <QLabel>
// #include <QListWidget>
// #include <QPushButton>

// int main(int argc, char* argv[])
// {
//     QApplication app(argc, argv);

//     // 创建 QListWidget
//     QListWidget listWidget;

//     // 添加多个列表项
//     for (int i = 0; i < 5; ++i)
//     {
//         // 创建 QListWidgetItem
//         QListWidgetItem* item = new QListWidgetItem(&listWidget);

//         // 创建自定义部件，包含一个标签和一个按钮
//         QWidget* widget = new QWidget;
//         QLabel* label = new QLabel(QString("Item %1").arg(i));
//         QPushButton* button = new QPushButton("Button");

//         // 创建水平布局
//         QHBoxLayout* layout = new QHBoxLayout(widget);
//         layout->addWidget(label);
//         layout->addWidget(button);
//         layout->setContentsMargins(0, 0, 0, 0);  // 设置布局的边距

//         // 设置自定义部件为列表项的部件
//         listWidget.setItemWidget(item, widget);
//     }

//     listWidget.show();
//     return app.exec();
// }
