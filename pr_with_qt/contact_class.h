#ifndef CONTACT_CLASS_H
#define CONTACT_CLASS_H

#include <string>
#include <vector>


class Contact
{
public:

    struct Date
    {
        int day{};
        int month{};
        int year{};
    };

    enum class PhoneType {Work, Home, Service};
    struct Phone
    {
        PhoneType   type;
        std::string number;
    };

    Contact() = default;
    Contact(const std::string& name,const std::string& surname,const std::string& email,const std::vector<Phone>& phones);

    const std::string&       getName()       const noexcept {return name_;}
    const std::string&       getSurname()    const noexcept {return surname_;}
    const std::string&       getPatronymic() const noexcept {return patronymic_;}
    const std::string&       getemail()      const noexcept {return email_;}
    const Date&              getBirth_date() const noexcept {return birth_date_;}
    const std::string&       getAddress()    const noexcept {return address_;}
    const std::vector<Phone> getPhones()     const noexcept {return phones_;}

    bool setName       (const std::string&         name);
    bool setSurname    (const std::string&         surname);
    bool setPatronymic (const std::string&         patronymic);
    bool setEmail      (const std::string&         email);
    bool setDate       (const Date&                birth_date);
    bool setAddress    (const std::string&         address);
    bool setPhones     (const::std::vector<Phone>& phones);

    static bool isValidPersonalName (const std::string& name);
    static bool isValidEmail        (const std::string& email);
    static bool isValidDate         (const Date& birth_date);
    static bool isValidPhones       (const std::vector<Phone>& phones);

private:
    std::string name_;
    std::string surname_;
    std::string patronymic_;
    std::string email_;
    std::string address_;
    Date birth_date_;
    std::vector<Phone> phones_;
};

#endif // CONTACT_CLASS_H
