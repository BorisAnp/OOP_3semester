#ifndef CONTACT_APP_H
#define CONTACT_APP_H

#include <vector>
#include "Contact_class.h"

void showContacts  (const std::vector<Contact>& contacts);

void addContact    (std::vector<Contact>& contacts);

void deleteContact (std::vector<Contact>& contacts);

void editContact   (std::vector<Contact>& contacts);

void searchContact (const std::vector<Contact>& contacts);

#endif // CONTACT_APP_H
