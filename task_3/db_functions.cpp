#include "db_functions.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDate>
#include <QDebug>
#include <optional>

#include "db_work.h"

using std::vector;
using std::string;

namespace
{
QDate toQDate(const Contact::Date &d)
{
    if (d.year == 0 || d.month == 0 || d.day == 0)
        return QDate();

    return QDate(d.year, d.month, d.day);
}
}

int insertContact(const Contact &c)
{
    QSqlDatabase &db = db_work::db();

    if (!db.isOpen()) {
        if (!db_work::init()) {
            qWarning() << "insertContact: cannot open DB";
            return -1;
        }
    }

    if (!db.transaction()) {
        qWarning() << "insertContact: cannot start transaction:" << db.lastError().text();
        return -1;
    }

    int newId = -1;

    {
        QSqlQuery q(db);
        if (!q.prepare(
                "INSERT INTO contacts "
                "(name, surname, patronymic, email, address, birth_date) "
                "VALUES (:name, :surname, :patronymic, :email, :address, :birth_date) "
                "RETURNING id"))
        {
            qWarning() << "insertContact (contacts/prepare) error:" << q.lastError().text();
            db.rollback();
            return -1;
        }

        const auto &bd = c.getBirth_date();

        q.bindValue(":name",       QString::fromStdString(c.getName()));
        q.bindValue(":surname",    QString::fromStdString(c.getSurname()));
        q.bindValue(":patronymic", QString::fromStdString(c.getPatronymic()));
        q.bindValue(":email",      QString::fromStdString(c.getemail()));
        q.bindValue(":address",    QString::fromStdString(c.getAddress()));
        q.bindValue(":birth_date", (bd.year == 0 ? QVariant(QVariant::Date) : QDate(bd.year, bd.month, bd.day)));

        if (!q.exec()) {
            qWarning() << "insertContact (contacts) error:" << q.lastError().text();
            db.rollback();
            return -1;
        }

        if (q.next())
            newId = q.value(0).toInt();

        q.finish();
    }

    if (newId < 0) {
        qWarning() << "insertContact: no id returned";
        db.rollback();
        return -1;
    }

    QSqlQuery phoneQuery(db);
    if (!phoneQuery.prepare(
            "INSERT INTO phones (contact_id, phone_type, number) "
            "VALUES (:cid, :type, :number)"))
    {
        qWarning() << "insertContact (phones/prepare) error:" << phoneQuery.lastError().text();
        db.rollback();
        return -1;
    }

    for (const auto &p : c.getPhones()) {
        phoneQuery.bindValue(":cid", newId);
        phoneQuery.bindValue(":type", static_cast<int>(p.type));
        phoneQuery.bindValue(":number", QString::fromStdString(p.number));

        if (!phoneQuery.exec()) {
            qWarning() << "insertContact (phones) error:" << phoneQuery.lastError().text();
            db.rollback();
            return -1;
        }

        phoneQuery.finish();
    }

    if (!db.commit()) {
        qWarning() << "insertContact: commit failed:" << db.lastError().text();
        db.rollback();
        return -1;
    }

    return newId;
}

bool updateContact(int contactId, const Contact &c)
{
    QSqlDatabase &db = db_work::db();
    if (!db.isOpen()) {
        if (!db_work::init()) {
            qWarning() << "updateContact: cannot open DB";
            return false;
        }
    }

    if (!db.transaction()) {
        qWarning() << "updateContact: cannot start transaction:" << db.lastError().text();
        return false;
    }

    // 1) UPDATE contacts
    QSqlQuery q(db);
    if (!q.prepare(
            "UPDATE contacts SET "
            "name = :name, "
            "surname = :surname, "
            "patronymic = :patronymic, "
            "email = :email, "
            "address = :address, "
            "birth_date = :birth_date "
            "WHERE id = :id"))
    {
        qWarning() << "updateContact (prepare contacts) error:" << q.lastError().text();
        db.rollback();
        return false;
    }

    q.bindValue(":name",       QString::fromStdString(c.getName()));
    q.bindValue(":surname",    QString::fromStdString(c.getSurname()));
    q.bindValue(":patronymic", QString::fromStdString(c.getPatronymic()));
    q.bindValue(":email",      QString::fromStdString(c.getemail()));
    q.bindValue(":address",    QString::fromStdString(c.getAddress()));
    q.bindValue(":birth_date", toQDate(c.getBirth_date()));
    q.bindValue(":id",         contactId);

    if (!q.exec()) {
        qWarning() << "updateContact (contacts) error:" << q.lastError().text();
        db.rollback();
        return false;
    }
    q.finish();

    QSqlQuery del(db);
    del.prepare("DELETE FROM phones WHERE contact_id = :cid");
    del.bindValue(":cid", contactId);

    if (!del.exec()) {
        qWarning() << "updateContact (delete phones) error:" << del.lastError().text();
        db.rollback();
        return false;
    }
    del.finish();

    QSqlQuery phoneQuery(db);
    if (!phoneQuery.prepare(
            "INSERT INTO phones (contact_id, phone_type, number) "
            "VALUES (:cid, :type, :number)"))
    {
        qWarning() << "updateContact (prepare phones) error:" << phoneQuery.lastError().text();
        db.rollback();
        return false;
    }

    for (const auto &p : c.getPhones()) {
        phoneQuery.bindValue(":cid", contactId);
        phoneQuery.bindValue(":type", static_cast<int>(p.type));
        phoneQuery.bindValue(":number", QString::fromStdString(p.number));

        if (!phoneQuery.exec()) {
            qWarning() << "updateContact (phones) error:" << phoneQuery.lastError().text();
            db.rollback();
            return false;
        }
        phoneQuery.finish();
    }

    if (!db.commit()) {
        qWarning() << "updateContact: commit failed:" << db.lastError().text();
        db.rollback();
        return false;
    }

    return true;
}

bool deleteContact(int contactId)
{
    QSqlDatabase &db = db_work::db();
    if (!db.isOpen()) {
        if (!db_work::init()) return false;
    }

    QSqlQuery q(db);
    q.prepare("DELETE FROM contacts WHERE id = :id");
    q.bindValue(":id", contactId);

    if (!q.exec()) {
        qWarning() << "deleteContact error:" << q.lastError().text();
        return false;
    }
    return true;
}

QVector<QPair<int, Contact>> selectAllContacts()
{
    QVector<QPair<int, Contact>> res;

    QSqlDatabase &db = db_work::db();
    if (!db.isOpen()) {
        if (!db_work::init()) {
            qWarning() << "selectAllContacts: DB not open";
            return res;
        }
    }

    QSqlQuery q(db);
    if (!q.exec("SELECT id, name, surname, patronymic, email, address, birth_date "
                "FROM contacts ORDER BY surname, name, id"))
    {
        qWarning() << "selectAllContacts (contacts) error:" << q.lastError().text();
        return res;
    }

    QSqlQuery pq(db);
    if (!pq.prepare("SELECT phone_type, number FROM phones "
                    "WHERE contact_id = :cid ORDER BY id"))
    {
        qWarning() << "selectAllContacts (phones/prepare) error:" << pq.lastError().text();
        return res;
    }

    while (q.next()) {
        int id = q.value("id").toInt();

        Contact c;
        c.setName(q.value("name").toString().toStdString());
        c.setSurname(q.value("surname").toString().toStdString());
        c.setPatronymic(q.value("patronymic").toString().toStdString());
        c.setEmail(q.value("email").toString().toStdString());
        c.setAddress(q.value("address").toString().toStdString());

        if (q.value("birth_date").isNull()) {
            c.setDate({0,0,0});
        } else {
            QDate d = q.value("birth_date").toDate();
            c.setDate({d.day(), d.month(), d.year()});
        }

        std::vector<Contact::Phone> phones;
        pq.bindValue(":cid", id);
        if (!pq.exec()) {
            qWarning() << "selectAllContacts (phones) error:" << pq.lastError().text();
        } else {
            while (pq.next()) {
                Contact::Phone p;
                p.type = static_cast<Contact::PhoneType>(pq.value(0).toInt());
                p.number = pq.value(1).toString().toStdString();
                phones.push_back(p);
            }
        }
        pq.finish();

        c.setPhones(phones);

        res.push_back(qMakePair(id, c));
    }

    return res;
}

std::optional<std::pair<int, Contact>> selectContactByEmail(const QString &email)
{
    QSqlDatabase &db = db_work::db();
    if (!db.isOpen()) {
        if (!db_work::init()) {
            qWarning() << "selectContactByEmail: DB not open";
            return std::nullopt;
        }
    }

    QSqlQuery q(db);
    q.prepare(
        "SELECT id, name, surname, patronymic, email, address, birth_date "
        "FROM contacts WHERE email = :email LIMIT 1"
        );
    q.bindValue(":email", email);

    if (!q.exec()) {
        qWarning() << "selectContactByEmail (contacts) error:" << q.lastError().text();
        return std::nullopt;
    }

    if (!q.next())
        return std::nullopt;

    int id = q.value("id").toInt();

    Contact c;
    c.setName(q.value("name").toString().toStdString());
    c.setSurname(q.value("surname").toString().toStdString());
    c.setPatronymic(q.value("patronymic").toString().toStdString());
    c.setEmail(q.value("email").toString().toStdString());
    c.setAddress(q.value("address").toString().toStdString());

    if (q.value("birth_date").isNull()) {
        c.setDate({0,0,0});
    } else {
        QDate d = q.value("birth_date").toDate();
        c.setDate({d.day(), d.month(), d.year()});
    }

    std::vector<Contact::Phone> phones;
    QSqlQuery pq(db);
    pq.prepare("SELECT phone_type, number FROM phones WHERE contact_id = :cid ORDER BY id");
    pq.bindValue(":cid", id);

    if (pq.exec()) {
        while (pq.next()) {
            Contact::Phone p;
            p.type = static_cast<Contact::PhoneType>(pq.value(0).toInt());
            p.number = pq.value(1).toString().toStdString();
            phones.push_back(p);
        }
    } else {
        qWarning() << "selectContactByEmail (phones) error:" << pq.lastError().text();
    }

    c.setPhones(phones);

    return std::make_optional(std::make_pair(id, c));
}

std::optional<std::pair<int, Contact>> selectContactById(int id)
{
    QSqlDatabase &db = db_work::db();
    if (!db.isOpen()) {
        if (!db_work::init()) return std::nullopt;
    }

    QSqlQuery q(db);
    q.prepare(
        "SELECT id, name, surname, patronymic, email, address, birth_date "
        "FROM contacts WHERE id = :id LIMIT 1"
        );
    q.bindValue(":id", id);

    if (!q.exec() || !q.next())
        return std::nullopt;

    Contact c;
    c.setName(q.value("name").toString().toStdString());
    c.setSurname(q.value("surname").toString().toStdString());
    c.setPatronymic(q.value("patronymic").toString().toStdString());
    c.setEmail(q.value("email").toString().toStdString());
    c.setAddress(q.value("address").toString().toStdString());

    if (!q.value("birth_date").isNull()) {
        QDate d = q.value("birth_date").toDate();
        c.setDate({d.day(), d.month(), d.year()});
    }

    std::vector<Contact::Phone> phones;
    QSqlQuery pq(db);
    pq.prepare("SELECT phone_type, number FROM phones WHERE contact_id = :cid ORDER BY id");
    pq.bindValue(":cid", id);

    if (pq.exec()) {
        while (pq.next()) {
            Contact::Phone p;
            p.type = static_cast<Contact::PhoneType>(pq.value(0).toInt());
            p.number = pq.value(1).toString().toStdString();
            phones.push_back(p);
        }
    }
    c.setPhones(phones);

    return std::make_optional(std::make_pair(id, c));
}
