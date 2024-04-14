#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int num = 11;
    QString str = QString("%1").arg(num, 0, 10);//11  十进制
    str = QString("%1").arg(num, 0, 16);//b	十六进制
    //str = QString("%1").arg(num, 0, 2);//1011	二进制
    //str = QString("%1").arg(num, 0, 8);//13	八进制
    //str = QString("%1").arg(str, 0, 8);//13	八进制

    MainWindow w;
    w.show();
    return a.exec();
}
