#ifndef CONTACT_CLASS_H
#define CONTACT_CLASS_H

#include <string>
#include <vector>

#include <atomic>
#include <cstddef>

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

    struct Stats {
        long long constructed{};
        long long destroyed{};
        long long copyCtor{};
        long long moveCtor{};
        long long copyAssign{};
        long long moveAssign{};
        long long newCalls{};
        long long deleteCalls{};
    };

    Contact();
    Contact(const Contact& other);
    Contact(Contact&& other) noexcept;
    Contact& operator=(const Contact& other);
    Contact& operator=(Contact&& other) noexcept;
    ~Contact();

    static void* operator new(std::size_t sz);
    static void  operator delete(void* p) noexcept;

    static Stats stats();
    static void resetStats();

    Contact(const std::string& name, const std::string& surname,
            const std::string& email, const std::vector<Phone>& phones);

    const std::string&       getName()       const noexcept {return name_;}
    const std::string&       getSurname()    const noexcept {return surname_;}
    const std::string&       getPatronymic() const noexcept {return patronymic_;}
    const std::string&       getemail()      const noexcept {return email_;}
    const Date&              getBirth_date() const noexcept {return birth_date_;}
    const std::string&       getAddress()    const noexcept {return address_;}
    const std::vector<Phone> getPhones()     const noexcept {return phones_;}

    const std::vector<Phone>& getPhonesRef() const noexcept { return phones_; }

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

    static std::atomic<long long> s_constructed;
    static std::atomic<long long> s_destroyed;
    static std::atomic<long long> s_copyCtor;
    static std::atomic<long long> s_moveCtor;
    static std::atomic<long long> s_copyAssign;
    static std::atomic<long long> s_moveAssign;
    static std::atomic<long long> s_newCalls;
    static std::atomic<long long> s_deleteCalls;
};

#endif // CONTACT_CLASS_H
