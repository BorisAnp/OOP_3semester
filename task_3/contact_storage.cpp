#include "contact_storage.h"

#include <fstream>
#include <sstream>

namespace
{
using Date      = Contact::Date;
using Phone     = Contact::Phone;
using PhoneType = Contact::PhoneType;

std::string formatDate(const Date& d)
{
    std::ostringstream os;
    if (d.day < 10)   os << '0';
    os << d.day << '.';
    if (d.month < 10) os << '0';
    os << d.month << '.';
    os << d.year;
    return os.str();
}

bool parseDate(const std::string& text, Date& out)
{
    std::istringstream is(text);
    char dot1 = 0, dot2 = 0;
    if (!(is >> out.day >> dot1 >> out.month >> dot2 >> out.year))
        return false;

    if (dot1 != '.' || dot2 != '.')
        return false;

    if (!Contact::isValidDate(out))
        return false;

    return true;
}

std::string phoneTypeToString(Contact::PhoneType t)
{
    switch (t)
    {
    case Contact::PhoneType::Work:   return "Work";
    case Contact::PhoneType::Home:   return "Home";
    case Contact::PhoneType::Service:return "Service";
    }
    return "Unknown";
}

bool stringToPhoneType(const std::string& s, Contact::PhoneType& t)
{
    if (s == "Work")   { t = Contact::PhoneType::Work;   return true; }
    if (s == "Home")   { t = Contact::PhoneType::Home;   return true; }
    if (s == "Service"){ t = Contact::PhoneType::Service;return true; }
    return false;
}

}

bool loadContacts(const std::string& filename, std::vector<Contact>& contacts)
{
    contacts.clear();

    std::ifstream in(filename);
    if (!in.is_open())
        return true;

    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
            continue;

        std::istringstream iss(line);
        std::string name;
        std::string surname;
        std::string patronymic;
        std::string address;
        std::string dateStr;
        std::string email;
        std::string phonesStr;

        if (!std::getline(iss, name,       '|')) continue;
        if (!std::getline(iss, surname,    '|')) continue;
        if (!std::getline(iss, patronymic, '|')) continue;
        if (!std::getline(iss, address,    '|')) continue;
        if (!std::getline(iss, dateStr,    '|')) continue;
        if (!std::getline(iss, email,      '|')) continue;
        std::getline(iss, phonesStr);

        Date birth{};
        if (!parseDate(dateStr, birth))
            continue;

        std::vector<Phone> phones;
        std::istringstream pss(phonesStr);
        std::string phoneToken;

        while (std::getline(pss, phoneToken, ','))
        {
            if (phoneToken.empty())
                continue;

            std::size_t colonPos = phoneToken.find(':');
            if (colonPos == std::string::npos)
                continue;

            std::string typeStr = phoneToken.substr(0, colonPos);
            std::string number  = phoneToken.substr(colonPos + 1);

            PhoneType type;
            if (!stringToPhoneType(typeStr, type))
                continue;

            Phone p{ type, number };
            phones.push_back(p);
        }

        if (phones.empty())
            continue;

        Contact c(name, surname, email, phones);
        c.setPatronymic(patronymic);
        c.setAddress(address);
        c.setDate(birth);

        contacts.push_back(c);
    }

    return true;
}

bool saveContacts(const std::string& filename, const std::vector<Contact>& contacts)
{
    std::ofstream out(filename);
    if (!out.is_open())
        return false;

    for (const Contact& c : contacts)
    {
        const Date& d = c.getBirth_date();
        auto phones   = c.getPhones();

        out << c.getName()        << '|'
            << c.getSurname()     << '|'
            << c.getPatronymic()  << '|'
            << c.getAddress()     << '|'
            << formatDate(d)      << '|'
            << c.getemail()       << '|';

        for (std::size_t i = 0; i < phones.size(); ++i)
        {
            const Phone& p = phones[i];
            out << phoneTypeToString(p.type) << ':' << p.number;
            if (i + 1 < phones.size())
                out << ',';
        }

        out << '\n';
    }

    return true;
}

