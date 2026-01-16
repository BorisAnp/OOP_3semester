#include "contactdialog.h"
#include "ui_contactdialog.h"

#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>

namespace
{
    bool parseBirthDate(const QString &birthStr,
                        Contact::Date &outDate,
                        QString &errorMsg)
    {
        if (birthStr.isEmpty())
        {
            return true;
        }

        const auto parts = birthStr.split('.');
        if (parts.size() != 3)
        {
            errorMsg = "Неверный формат даты. Используйте формат ДД.ММ.ГГГГ.";
            return false;
        }

        bool okDay = false, okMonth = false, okYear = false;
        Contact::Date d{};
        d.day   = parts[0].toInt(&okDay);
        d.month = parts[1].toInt(&okMonth);
        d.year  = parts[2].toInt(&okYear);

        if (!okDay || !okMonth || !okYear)
        {
            errorMsg = "Неверная дата рождения.";
            return false;
        }

        if (!Contact::isValidDate(d))
        {
            errorMsg = "Некорректная дата рождения (такого дня нет или он в будущем).";
            return false;
        }

        outDate = d;
        return true;
    }

    QString birthDateToString(const Contact::Date& d)
    {
        if (d.year == 0)
            return {};

        return QString("%1.%2.%3")
            .arg(d.day,   2, 10, QLatin1Char('0'))
            .arg(d.month, 2, 10, QLatin1Char('0'))
            .arg(d.year);
    }

    void showError(QWidget *parent, const QString &msg, QWidget *focus = nullptr)
    {
        QMessageBox::warning(parent, "Ошибка", msg);
        if (focus)
            focus->setFocus();
    }
}

ContactDialog::ContactDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ContactDialog)
{
    ui->setupUi(this);
    initTelephonesTable();
}

ContactDialog::ContactDialog(const Contact &contact, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ContactDialog)
    , isEdit_(true)
    , contact_(contact)
{
    ui->setupUi(this);
    initTelephonesTable();
    fillFromContact(contact_);
}

ContactDialog::~ContactDialog()
{
    delete ui;
}

void ContactDialog::initTelephonesTable()
{
    ui->tw_telephones->setColumnCount(2);
    QStringList headers;
    headers << "Тип" << "Номер";
    ui->tw_telephones->setHorizontalHeaderLabels(headers);

    ui->tw_telephones->horizontalHeader()
        ->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tw_telephones->horizontalHeader()
        ->setSectionResizeMode(1, QHeaderView::Stretch);
}

std::vector<Contact::Phone> ContactDialog::collectPhonesFromTable() const
{
    std::vector<Contact::Phone> result;

    const int rows = ui->tw_telephones->rowCount();
    for (int row = 0; row < rows; ++row)
    {
        auto *combo = qobject_cast<QComboBox*>(
            ui->tw_telephones->cellWidget(row, 0));
        auto *edit = qobject_cast<QLineEdit*>(
            ui->tw_telephones->cellWidget(row, 1));

        if (!combo || !edit)
            continue;

        const QString num = edit->text().trimmed();
        if (num.isEmpty())
            continue;

        Contact::Phone p;
        p.number = num.toStdString();
        p.type   = static_cast<Contact::PhoneType>(combo->currentData().toInt());
        result.push_back(std::move(p));
    }

    return result;
}

void ContactDialog::fillFromContact(const Contact &c)
{
    ui->le_name->setText(QString::fromStdString(c.getName()));
    ui->le_surname->setText(QString::fromStdString(c.getSurname()));
    ui->le_patronymic->setText(QString::fromStdString(c.getPatronymic()));
    ui->le_email->setText(QString::fromStdString(c.getemail()));
    ui->le_adress->setText(QString::fromStdString(c.getAddress()));

    const Contact::Date& d = c.getBirth_date();
    if (d.year != 0)
        ui->le_birth_date->setText(birthDateToString(d));
    else
        ui->le_birth_date->clear();

    ui->tw_telephones->setRowCount(0);

    const auto &phones = c.getPhones();
    for (const auto &p : phones)
    {
        on_pb_addTelephone_clicked();
        int row = ui->tw_telephones->rowCount() - 1;

        auto *combo = qobject_cast<QComboBox*>(
            ui->tw_telephones->cellWidget(row, 0));
        auto *edit = qobject_cast<QLineEdit*>(
            ui->tw_telephones->cellWidget(row, 1));

        if (!combo || !edit)
            continue;

        int idx = combo->findData(static_cast<int>(p.type));
        if (idx >= 0)
            combo->setCurrentIndex(idx);

        edit->setText(QString::fromStdString(p.number));
    }
}

void ContactDialog::on_pb_backToMain_clicked()
{
    reject();
}

void ContactDialog::on_pb_addTelephone_clicked()
{
    int row = ui->tw_telephones->rowCount();
    ui->tw_telephones->insertRow(row);

    auto *combo = new QComboBox(ui->tw_telephones);
    combo->addItem("Рабочий", static_cast<int>(Contact::PhoneType::Work));
    combo->addItem("Домашний", static_cast<int>(Contact::PhoneType::Home));
    combo->addItem("Сервисный", static_cast<int>(Contact::PhoneType::Service));
    ui->tw_telephones->setCellWidget(row, 0, combo);

    auto *edit = new QLineEdit(ui->tw_telephones);
    ui->tw_telephones->setCellWidget(row, 1, edit);

    ui->tw_telephones->setCurrentCell(row, 1);
    edit->setFocus();
}

void ContactDialog::on_pb_deleteTelephone_clicked()
{
    int row = ui->tw_telephones->currentRow();
    if (row < 0)
        return;

    ui->tw_telephones->removeRow(row);
}

void ContactDialog::on_pb_addContact_clicked()
{
    const QString name      = ui->le_name->text().trimmed();
    const QString surname   = ui->le_surname->text().trimmed();
    const QString patronym  = ui->le_patronymic->text().trimmed();
    const QString email     = ui->le_email->text().trimmed();
    const QString address   = ui->le_adress->text().trimmed();
    const QString birthStr  = ui->le_birth_date->text().trimmed();

    auto phones = collectPhonesFromTable();

    if (!Contact::isValidPersonalName(name.toStdString()))
    {
        showError(this, "Некорректное имя.", ui->le_name);
        return;
    }

    if (!Contact::isValidPersonalName(surname.toStdString()))
    {
        showError(this, "Некорректная фамилия.", ui->le_surname);
        return;
    }

    if (!patronym.isEmpty() &&
        !Contact::isValidPersonalName(patronym.toStdString()))
    {
        showError(this, "Некорректное отчество.", ui->le_patronymic);
        return;
    }

    if (!Contact::isValidEmail(email.toStdString()))
    {
        showError(this, "Некорректный e-mail.", ui->le_email);
        return;
    }

    if (!Contact::isValidPhones(phones))
    {
        showError(this, "Должен быть хотя бы один корректный номер телефона.");
        return;
    }

    Contact::Date bd{};
    QString dateError;
    if (!parseBirthDate(birthStr, bd, dateError))
    {
        showError(this, dateError, ui->le_birth_date);
        return;
    }

    Contact result;
    result.setName(name.toStdString());
    result.setSurname(surname.toStdString());
    result.setPatronymic(patronym.toStdString());
    result.setEmail(email.toStdString());
    result.setAddress(address.toStdString());

    if (!birthStr.isEmpty())
        result.setDate(bd);

    result.setPhones(phones);

    contact_ = std::move(result);
    accept();
}


