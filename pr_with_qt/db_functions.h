#ifndef DB_FUNCTIONS_H
#define DB_FUNCTIONS_H

#include <QVector>
#include <QPair>
#include "contact_class.h"
#include <optional>

int insertContact(const Contact &c);

bool updateContact(int contactId, const Contact &c);

bool deleteContact(int contactId);

QVector<QPair<int, Contact>> selectAllContacts();

std::optional<std::pair<int, Contact>> selectContactByEmail(const QString &email);

std::optional<std::pair<int, Contact>> selectContactById(int id);

#endif // DB_FUNCTIONS_H
