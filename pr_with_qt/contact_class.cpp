#include "contact_class.h"

#include <regex>
#include <ctime>

namespace
{
std::string trim(const std::string& str)
{
    const char* ws = " \t\n\r\f\v";

    std::size_t first = str.find_first_not_of(ws);
    if (first == std::string::npos)
        return {};

    std::size_t last = str.find_last_not_of(ws);
    return str.substr(first, last - first + 1);
}

bool isLeap(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int month, int year)
{
    switch (month)
    {
    case 1: case 3: case 5: case 7:
    case 8: case 10: case 12:
        return 31;
    case 4: case 6: case 9: case 11:
        return 30;
    case 2:
        return isLeap(year) ? 29 : 28;
    default:
        return 0;
    }
}

Contact::Date currentDate()
{
    std::time_t t = std::time(nullptr);
    std::tm* lt = std::localtime(&t);

    Contact::Date d{};
    d.day   = lt->tm_mday;
    d.month = lt->tm_mon + 1;
    d.year  = lt->tm_year + 1900;
    return d;
}

bool isValidPhoneNumber(const std::string& raw_number)
{
    std::string s = trim(raw_number);
    if (s.empty())
        return false;

    static const std::regex re(
        R"(^(?:\+7|8)(?:\d{10}|\(\d{3}\)\d{7}|\(\d{3}\)\d{3}-\d{2}-\d{2})$)"
        );

    return std::regex_match(s, re);
}

}

bool Contact::setName             (const std::string&        raw_name)
{
    std::string name = trim(raw_name);

    if (name.empty())
        return false;

    if (!isValidPersonalName(name))
        return false;

    name_ = name;
    return true;
}
bool Contact::setSurname          (const std::string&        raw_surname)
{
    std::string surname = trim(raw_surname);

    if (surname.empty())
        return false;

    if (!isValidPersonalName(surname))
        return false;

    surname_ = surname;
    return true;
}
bool Contact::setPatronymic       (const std::string&        raw_patronymic)
{
    std::string patronymic = trim(raw_patronymic);

    if (patronymic.empty())
        return false;

    if (!isValidPersonalName(patronymic))
        return false;

    patronymic_ = patronymic;
    return true;
}
bool Contact::setEmail            (const std::string&        raw_email)
{
    std::string all = trim(raw_email);

    std::size_t at_pos = all.find('@');
    if (at_pos == std::string::npos) {
        return false;
    }

    std::string user_part   = all.substr(0, at_pos);
    std::string domain_part = all.substr(at_pos + 1);

    user_part   = trim(user_part);
    domain_part = trim(domain_part);

    if (user_part.empty() || domain_part.empty()) {
        return false;
    }

    std::string normalized = user_part + "@" + domain_part;

    if (!isValidEmail(normalized)) {
        return false;
    }

    email_ = normalized;
    return true;
}
bool Contact::setDate             (const Date&               birth_date)
{
    if (!isValidDate(birth_date))
        return false;

    birth_date_ = birth_date;
    return true;
}
bool Contact::setPhones           (const std::vector<Phone>& phones)
{
    if (!isValidPhones(phones)) {
        return false;
    }

    phones_ = phones;
    return true;
}
bool Contact::setAddress          (const std::string&        raw_address)
{
    std::string address = trim(raw_address);
    if (address.empty())
        return false;
    address_ = address;
    return true;
}

bool Contact::isValidPersonalName (const std::string&        personal_name)
{
    static const std::regex re(R"(^[[:alpha:]](?:[[:alnum:] -]*[[:alnum:]])?$)");
    return std::regex_match(personal_name, re);
}
bool Contact::isValidEmail        (const std::string&        email)
{
    static const std::regex re(
        R"(^[A-Za-z0-9]+(\.[A-Za-z0-9]+)*@[A-Za-z0-9]+(\.[A-Za-z0-9]+)+$)"
        );
    return std::regex_match(email, re);
}
bool Contact::isValidDate         (const Date&               birth_date)
{
    if (birth_date.month < 1 || birth_date.month > 12)
        return false;

    int maxDay = daysInMonth(birth_date.month, birth_date.year);
    if (maxDay == 0)
        return false;

    if (birth_date.day < 1 || birth_date.day > maxDay)
        return false;

    Date today = currentDate();

    if (birth_date.year > today.year)
        return false;

    if (birth_date.year == today.year &&
        birth_date.month > today.month)
        return false;

    if (birth_date.year == today.year &&
        birth_date.month == today.month &&
        birth_date.day >= today.day)
        return false;

    return true;
}
bool Contact::isValidPhones       (const std::vector<Phone>& phones)
{
    if (phones.empty())
        return false;

    for (const auto& p : phones) {
        if (!isValidPhoneNumber(p.number)) {
            return false;
        }
    }

    return true;
}

Contact::Contact(const std::string& name, const std::string& surname, const std::string& email, const std::vector<Phone>& phones)
    : name_(name),surname_(surname),patronymic_(),email_(email),address_(),birth_date_{},phones_(phones) {}

