#include "MainWindow.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //--- Для функций перевода tr()
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    //--- Для CString
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
    //--- Для локали
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

    MainWindow w;
    w.show();
    return a.exec();
}
