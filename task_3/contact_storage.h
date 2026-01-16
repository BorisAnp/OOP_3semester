#ifndef CONTACT_STORAGE_H
#define CONTACT_STORAGE_H

#include <vector>
#include <string>
#include "Contact_class.h"

bool loadContacts(const std::string& filename, std::vector<Contact>&       contacts);

bool saveContacts(const std::string& filename, const std::vector<Contact>& contacts);


#endif // CONTACT_STORAGE_H
