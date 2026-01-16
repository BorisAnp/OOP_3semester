#include "contactsmainwindow.h"
#include "db_work.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!db_work::init())
        return -1;

    ContactsMainWindow w;
    w.show();
    return a.exec();
}
