#ifndef DB_WORK_H
#define DB_WORK_H

#include <QtSql/QSqlDatabase>

class db_work
{
public:
    static bool init();
    static QSqlDatabase& db();

private:
    static QSqlDatabase s_db;
};

#endif // DB_WORK_H
