#include "db_work.h"
#include <QtSql/QSqlError>
#include <QDebug>

static const char* CONNECTION_NAME = "main_connection";

QSqlDatabase db_work::s_db;

bool db_work::init()
{
    if (!QSqlDatabase::isDriverAvailable("QPSQL"))
    {
        qDebug() << "QPSQL driver is NOT available!";
        return false;
    }

    if (!s_db.isValid())
    {
        s_db = QSqlDatabase::addDatabase("QPSQL", CONNECTION_NAME);
        s_db.setHostName("localhost");
        s_db.setPort(5432);
        s_db.setDatabaseName("contacts_db");
        s_db.setUserName("postgres");
        s_db.setPassword("oop2025");
    }

    if (!s_db.isOpen())
    {
        if (!s_db.open())
        {
            qDebug() << "DB error:" << s_db.lastError().text();
            return false;
        }
        qDebug() << "DB connected!";
    }

    return true;
}

QSqlDatabase& db_work::db()
{
    return s_db;
}

