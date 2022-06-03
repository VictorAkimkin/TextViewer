#include "mainwindow.h"

#include <QApplication>
//#include <QtCore5Compat/QTextCodec>

int main(int argc, char *argv[])
{
//   QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
