#include <iostream>
#include <string>

#include "Contact_class.h"
#include "contact_app.h"
#include "contact_storage.h"

#include <limits>
#include <vector>

int main()
{
    const std::string filename = "contacts.txt";
    std::vector<Contact> contacts;

    if (!loadContacts(filename, contacts))
    {
        std::cout << "Cannot load contacts file. Starting with empty list.\n";
    }

    while (true)
        {
            std::cout << "\n===== MENU =====\n"
                        "1. Show contacts\n"
                        "2. Add contact\n"
                        "3. Delete contact\n"
                        "4. Edit contact\n"
                        "5. Search contact\n"
                        "6. Exit\n"
                        "Your choice: ";

            int choice = {};
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //

            if (choice < 1 || choice > 6)
            {
                std::cout<<"Your choiсe is wrong!";
                continue;
            }

            switch (choice)
            {
            case 1:
                showContacts(contacts);
                break;
            case 2:
                addContact(contacts);
                saveContacts(filename, contacts);
                break;
            case 3:
                deleteContact(contacts);
                saveContacts(filename, contacts);
                break;
            case 4:
                editContact(contacts);
                saveContacts(filename, contacts);
                break;
            case 5:
                searchContact(contacts);
                break;
            case 6:
                std::cout<<"Thanks for using our program! Bye!\n";
                return 0;
            }

        }
    return 0;
}
