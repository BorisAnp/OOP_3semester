#ifndef CONTACT_STORAGE_H
#define CONTACT_STORAGE_H

#include <vector>
#include <string>
#include "Contact_class.h"

bool loadContacts(const std::string& filename, std::vector<Contact>&       contacts);

bool saveContacts(const std::string& filename, const std::vector<Contact>& contacts);

#endif // CONTACT_STORAGE_H


//C:\OOP_uni\pr_without_qt\build\Desktop_Qt_6_9_2_MinGW_64_bit-Debug\contacts.txt
