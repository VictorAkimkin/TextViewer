#include "mainwindow.h"

#include <QApplication>
//#include <QtCore5Compat/QTextCodec>

int main(int argc, char *argv[])
{
   //QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));

    QApplication a(argc, argv);
    QString file_name="";
    if (argc > 1)
    file_name=a.arguments().at(1);
    MainWindow w(file_name);

    w.show();
    return a.exec();
}
