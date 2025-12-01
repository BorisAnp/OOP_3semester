#include "contact_app.h"
#include "Contact_class.h"

#include <iostream>
#include <limits>
#include <algorithm>

namespace
{
    std::string phoneTypeToString(Contact::PhoneType t)
    {
        switch (t)
        {
        case Contact::PhoneType::Work:    return "Work";
        case Contact::PhoneType::Home:    return "Home";
        case Contact::PhoneType::Service: return "Service";
        }
        return "Unknown";
    }

    std::string trim(const std::string& str)
    {
        const char* ws = " \t\n\r\f\v";
        std::size_t first = str.find_first_not_of(ws);
        if (first == std::string::npos)
            return {};

        std::size_t last = str.find_last_not_of(ws);
        return str.substr(first, last - first + 1);
    }
}

void addContact    (std::vector<Contact>&       contacts)
{
    using std::cin;
    using std::cout;
    using std::string;

    string name;
    while (true)
    {
        cout << "\nEnter name: ";
        std::getline(cin, name);

        if (Contact::isValidPersonalName(name))
            break;

        cout << "Name is invalid, try again.\n";
    }

    string surname;
    while (true)
    {
        cout << "Enter surname: ";
        std::getline(cin, surname);

        if (Contact::isValidPersonalName(surname))
            break;

        cout << "Surname is invalid, try again.\n";
    }

    string email;
    while (true)
    {
        cout << "Enter email: ";
        std::getline(cin, email);

        if (Contact::isValidEmail(email))
            break;

        cout << "E-mail is invalid, try again.\n";
    }

    std::vector<Contact::Phone> phones;

    while (true)
    {
        Contact::Phone p{};
        int typeChoice = 0;

        cout << "\nPhone type (1 - Work, 2 - Home, 3 - Service): ";
        if (!(cin >> typeChoice))
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Input error, try again.\n";
            continue;
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (typeChoice)
        {
        case 1:
            p.type = Contact::PhoneType::Work;    break;
        case 2:
            p.type = Contact::PhoneType::Home;    break;
        case 3:
            p.type = Contact::PhoneType::Service; break;
        default:
            cout << "Unknown type, try again.\n";
            continue;
        }

        cout << "Enter phone number: ";
        std::getline(cin, p.number);

        std::vector<Contact::Phone> tmp{p};
        if (!Contact::isValidPhones(tmp))
        {
            cout << "Phone number is invalid, try again.\n";
            continue;
        }

        phones.push_back(p);

        cout << "Add one more phone? (y/n): ";
        char ans = 'n';
        cin >> ans;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (ans != 'y' && ans != 'Y')
            break;
    }

    if (phones.empty())
    {
        cout << "Contact must have at least one phone. Contact was not added.\n";
        return;
    }

    Contact c(name, surname, email, phones);

    cout << "\nEnter patronymic (or just press Enter to skip): ";
    string patronymic;
    std::getline(cin, patronymic);
    if (!patronymic.empty())
    {
        if (!c.setPatronymic(patronymic))
            cout << "Patronymic is invalid, it was not saved.\n";
    }

    cout << "Enter address (or just press Enter to skip): ";
    string address;
    std::getline(cin, address);
    if (!address.empty())
    {
        if (!c.setAddress(address))
            cout << "Address is invalid, it was not saved.\n";
    }

    cout << "Enter birth date (dd mm yyyy) or just press Enter to skip: ";

    if (cin.peek() != '\n')
    {
        int d = 0, m = 0, y = 0;
        if (cin >> d >> m >> y)
        {
            Contact::Date bd{};
            bd.day = d;
            bd.month = m;
            bd.year = y;

            if (Contact::isValidDate(bd))
            {
                if (!c.setDate(bd))
                    cout << "Birth date failed to set (setter returned false).\n";
            }
            else
            {
                cout << "Birth date is invalid, it was not saved.\n";
            }
        }
        else
        {
            cin.clear();
            cout << "Birth date input error, field was skipped.\n";
        }
    }
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    contacts.push_back(c);
    cout << "\nContact successfully added.\n";
}

void showContacts  (const std::vector<Contact>& contacts)
{

    using std::cout;
    using std::endl;

    if (contacts.empty())
    {
        cout << "\nNo contacts.\n";
        return;
    }

    cout << "\n===== CONTACT LIST =====\n";

    for (std::size_t i = 0; i < contacts.size(); ++i)
    {
        const Contact& c = contacts[i];
        const Contact::Date& d = c.getBirth_date();
        const auto& phones = c.getPhones();

        cout << "\n#" << (i + 1) << endl;
        cout << "Name:      " << c.getName() << endl;
        cout << "Surname:   " << c.getSurname() << endl;

        if (!c.getPatronymic().empty())
            cout << "Patronymic:" << c.getPatronymic() << endl;

        if (!c.getAddress().empty())
            cout << "Address:   " << c.getAddress() << endl;

        cout << "Birthdate: " << d.day << '.' << d.month << '.' << d.year << endl;
        cout << "Email:     " << c.getemail() << endl;

        if (!phones.empty())
        {
            cout << "Phones:\n";
            for (const auto& p : phones)
            {
                cout << "  - [" << phoneTypeToString(p.type) << "] " << p.number << endl;
            }
        }
    }

    cout << "\n========================\n";
}

void deleteContact (std::vector<Contact>&       contacts)
{
    using std::cout;
    using std::cin;
    using std::endl;

    if (contacts.empty())
    {
        cout << "\nNo contacts to delete.\n";
        return;
    }

    std::string email;
    cout << "\nEnter e-mail of contact to delete: ";
    std::getline(cin, email);

    email = trim(email);
    if (email.empty())
    {
        cout << "Empty e-mail, cancelling.\n";
        return;
    }

    std::size_t at_pos = email.find('@');
    if (at_pos != std::string::npos)
    {
        std::string user   = trim(email.substr(0, at_pos));
        std::string domain = trim(email.substr(at_pos + 1));
        email = user + "@" + domain;
    }

    if (!Contact::isValidEmail(email))
    {
        cout << "E-mail format is invalid.\n";
        return;
    }

    auto it = std::find_if(contacts.begin(), contacts.end(),[&](const Contact& c)
        {
            return c.getemail() == email;
        });

    if (it == contacts.end())
    {
        cout << "Contact with this e-mail not found.\n";
        return;
    }

    cout << "Deleting contact: "
         << it->getName() << ' ' << it->getSurname()
         << " (" << it->getemail() << ")\n";

    contacts.erase(it);

    cout << "Contact deleted.\n";
}

void editContact   (std::vector<Contact>&       contacts)
{
    using std::cout;
    using std::cin;
    using std::string;

    if (contacts.empty())
    {
        cout << "\nNo contacts to edit.\n";
        return;
    }

    cout << "\nHow do you want to select a contact?\n"
         << "1. By e-mail\n"
         << "2. By name + surname\n"
         << "Your choice: ";

    int mode{};
    if (!(cin >> mode))
    {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Input error.\n";
        return;
    }
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto it = contacts.end();

    if (mode == 1)
    {
        cout << "Enter e-mail of contact to edit: ";
        string email;
        std::getline(cin, email);

        if (!Contact::isValidEmail(email))
        {
            cout << "E-mail format is invalid.\n";
            return;
        }

        it = std::find_if(contacts.begin(), contacts.end(),
                          [&](const Contact& c)
                          {
                              return c.getemail() == email;
                          });
    }
    else if (mode == 2)
    {
        string name, surname;

        cout << "Enter name: ";
        std::getline(cin, name);

        cout << "Enter surname: ";
        std::getline(cin, surname);

        it = std::find_if(contacts.begin(), contacts.end(),
                          [&](const Contact& c)
                          {
                              return c.getName()    == name &&
                                     c.getSurname() == surname;
                          });
    }
    else
    {
        cout << "No such menu item.\n";
        return;
    }

    if (it == contacts.end())
    {
        cout << "Contact not found.\n";
        return;
    }

    Contact& c = *it;

    while (true)
    {
        cout << "\nEditing contact: "
             << c.getName() << ' ' << c.getSurname()
             << " (" << c.getemail() << ")\n";

        cout << "1. Edit name\n"
             << "2. Edit surname\n"
             << "3. Edit patronymic\n"
             << "4. Edit address\n"
             << "5. Edit birth date\n"
             << "6. Edit e-mail\n"
             << "7. Edit phones\n"
             << "8. Finish editing\n"
             << "Your choice: ";

        int choice{};
        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Wrong input, try again.\n";
            continue;
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 8)
            break;

        switch (choice)
        {
        case 1:
        {
            string name;
            cout << "Enter new name: ";
            std::getline(cin, name);

            if (!Contact::isValidPersonalName(name))
            {
                cout << "Name is invalid, not changed.\n";
            }
            else if (!c.setName(name))
            {
                cout << "Name rejected by setter.\n";
            }
            else
            {
                cout << "Name updated.\n";
            }
            break;
        }
        case 2:
        {
            string surname;
            cout << "Enter new surname: ";
            std::getline(cin, surname);

            if (!Contact::isValidPersonalName(surname))
            {
                cout << "Surname is invalid, not changed.\n";
            }
            else if (!c.setSurname(surname))
            {
                cout << "Surname rejected by setter.\n";
            }
            else
            {
                cout << "Surname updated.\n";
            }
            break;
        }
        case 3:
        {
            string patronymic;
            cout << "Enter new patronymic (or empty to clear): ";
            std::getline(cin, patronymic);

            if (patronymic.empty())
            {
                if (!c.setPatronymic(patronymic))
                    cout << "Failed to clear patronymic.\n";
                else
                    cout << "Patronymic cleared.\n";
            }
            else if (!Contact::isValidPersonalName(patronymic))
            {
                cout << "Patronymic is invalid, not changed.\n";
            }
            else if (!c.setPatronymic(patronymic))
            {
                cout << "Patronymic rejected by setter.\n";
            }
            else
            {
                cout << "Patronymic updated.\n";
            }
            break;
        }
        case 4:
        {
            string addr;
            cout << "Enter new address (may be empty): ";
            std::getline(cin, addr);

            if (!c.setAddress(addr))
                cout << "Address is invalid, not changed.\n";
            else
                cout << "Address updated.\n";
            break;
        }
        case 5:
        {
            Contact::Date d{};
            cout << "Enter new birth date.\n";
            cout << "  Day:   ";
            cin >> d.day;
            cout << "  Month: ";
            cin >> d.month;
            cout << "  Year:  ";
            cin >> d.year;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (!c.setDate(d))
                cout << "Birth date is invalid, not changed.\n";
            else
                cout << "Birth date updated.\n";
            break;
        }
        case 6:
        {
            string newEmail;
            cout << "Enter new e-mail: ";
            std::getline(cin, newEmail);

            if (!Contact::isValidEmail(newEmail))
            {
                cout << "E-mail format is invalid, not changed.\n";
            }
            else if (!c.setEmail(newEmail))
            {
                cout << "E-mail rejected by setter.\n";
            }
            else
            {
                cout << "E-mail updated.\n";
            }
            break;
        }
        case 7:
        {
            std::vector<Contact::Phone> phones;
            cout << "Re-enter phone list (at least one number).\n";

            while (true)
            {
                int tChoice{};
                cout << "\nPhone type:\n"
                     << " 1 - Work\n"
                     << " 2 - Home\n"
                     << " 3 - Service\n"
                     << " 0 - Finish\n"
                     << "Your choice: ";
                if (!(cin >> tChoice))
                {
                    cin.clear();
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    cout << "Wrong input.\n";
                    continue;
                }
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (tChoice == 0)
                    break;
                if (tChoice < 1 || tChoice > 3)
                {
                    cout << "No such type.\n";
                    continue;
                }

                Contact::Phone p{};
                switch (tChoice)
                {
                case 1: p.type = Contact::PhoneType::Work;    break;
                case 2: p.type = Contact::PhoneType::Home;    break;
                case 3: p.type = Contact::PhoneType::Service; break;
                }

                cout << "Enter phone number: ";
                std::getline(cin, p.number);

                phones.push_back(p);
            }

            if (phones.empty())
            {
                cout << "At least one phone is required. Phones not changed.\n";
            }
            else if (!c.setPhones(phones))
            {
                cout << "Phones are invalid, not changed.\n";
            }
            else
            {
                cout << "Phones updated.\n";
            }
            break;
        }
        default:
            cout << "Wrong menu item.\n";
            break;
        }
    }

    cout << "\nEditing finished.\n";
}

void searchContact(const std::vector<Contact>& contacts)
{
    using std::cout;
    using std::cin;
    using std::string;

    if (contacts.empty())
    {
        cout << "\nNo contacts to search.\n";
        return;
    }

    cout << "\nHow do you want to search?\n"
         << "1. By e-mail\n"
         << "2. By name + surname\n"
         << "Your choice: ";

    int mode{};
    if (!(cin >> mode))
    {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Input error.\n";
        return;
    }
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<Contact> found;

    if (mode == 1)
    {
        string email;
        cout << "Enter e-mail: ";
        std::getline(cin, email);

        if (!Contact::isValidEmail(email))
        {
            cout << "E-mail format is invalid.\n";
            return;
        }

        auto it = std::find_if(contacts.begin(), contacts.end(),
                               [&](const Contact& c)
                               {
                                   return c.getemail() == email;
                               });

        if (it != contacts.end())
        {
            found.push_back(*it);
        }
    }
    else if (mode == 2)
    {
        string name, surname;

        cout << "Enter name: ";
        std::getline(cin, name);

        cout << "Enter surname: ";
        std::getline(cin, surname);

        for (const auto& c : contacts)
        {
            if (c.getName() == name && c.getSurname() == surname)
            {
                found.push_back(c);
            }
        }
    }
    else
    {
        cout << "No such menu item.\n";
        return;
    }

    if (found.empty())
    {
        cout << "No contacts found.\n";
        return;
    }

    cout << "\nSearch result:\n";
    showContacts(found);
}
