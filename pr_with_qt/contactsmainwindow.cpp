#include "contactsmainwindow.h"
#include "ui_contactsmainwindow.h"
#include "contactdialog.h"
#include "db_functions.h"

#include "QMessageBox"
#include <QHeaderView>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QStringList>

namespace
{
static QString phoneTypeToString(Contact::PhoneType t)
{
    switch (t) {
    case Contact::PhoneType::Work:    return "Раб";
    case Contact::PhoneType::Home:    return "Дом";
    case Contact::PhoneType::Service: return "Сервис";
    }
    return "";
}

static QString phonesToTooltip(const Contact &c)
{
    QStringList lines;
    for (const auto &p : c.getPhones()) {
        lines << (phoneTypeToString(p.type) + ": " + QString::fromStdString(p.number));
    }
    return lines.join("\n");
}

static QString phonesToCell(const Contact &c)
{
    const auto &ph = c.getPhones();
    if (ph.empty()) return "";

    QString first = phoneTypeToString(ph[0].type) + ": " + QString::fromStdString(ph[0].number);
    if (ph.size() == 1) return first;

    return QString("%1  (+%2)").arg(first).arg(int(ph.size() - 1));
}
}

ContactsMainWindow::ContactsMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ContactsMainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("PollyCall"));
    setWindowIcon(QIcon(":/img/logo.png"));

    auto *t = ui->tv_contacts;
    t->setColumnCount(7);
    t->setHorizontalHeaderLabels({
        "Фамилия", "Имя", "Отчество", "Телефоны", "Email", "Адрес", "Дата рождения"
    });

    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->verticalHeader()->setVisible(false);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);

    loadContactsFromDb();

}

ContactsMainWindow::~ContactsMainWindow()
{
    delete ui;
}

void ContactsMainWindow::addContactToTable(int id, const Contact &c)
{
    auto *table = ui->tv_contacts;

    int row = table->rowCount();
    table->insertRow(row);

    auto *surnameItem = new QTableWidgetItem(QString::fromStdString(c.getSurname()));
    surnameItem->setData(Qt::UserRole, id);
    table->setItem(row, 0, surnameItem);

    table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.getName())));

    table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(c.getPatronymic())));

    QString cellText = phonesToCell(c);
    QString tipText  = phonesToTooltip(c);
    auto *phonesItem = new QTableWidgetItem(cellText);
    phonesItem->setToolTip(tipText);
    table->setItem(row, 3, phonesItem);
    table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(c.getemail())));
    table->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(c.getAddress())));


    const Contact::Date &bd = c.getBirth_date();
    QString dateStr;
    if (bd.year != 0) {
        dateStr = QString("%1.%2.%3")
        .arg(bd.day,   2, 10, QLatin1Char('0'))
            .arg(bd.month, 2, 10, QLatin1Char('0'))
            .arg(bd.year);
    }
    table->setItem(row, 6, new QTableWidgetItem(dateStr));
}

void ContactsMainWindow::on_pb_addContact_clicked()
{
    ContactDialog dlg(this);
    dlg.setWindowTitle(tr("Добавить контакт"));

    if (dlg.exec() != QDialog::Accepted)
        return;

    Contact newContact = dlg.resultContact();

    int newId = insertContact(newContact);
    if (newId < 0) {
        QMessageBox::warning(
            this,
            tr("Ошибка БД"),
            tr("Не удалось сохранить контакт в базе данных.")
            );
        return;
    }

    addContactToTable(newId, newContact);
}

void ContactsMainWindow::loadContactsFromDb()
{
    ui->tv_contacts->setRowCount(0);

    auto all = selectAllContacts();
    for (const auto &pair : all) {
        addContactToTable(pair.first, pair.second);
    }
}

void ContactsMainWindow::on_pb_editContact_clicked()
{
    int row = ui->tv_contacts->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Редактирование", "Выберите контакт в таблице.");
        return;
    }

    QTableWidgetItem *idItem = ui->tv_contacts->item(row, 0);
    if (!idItem) return;

    int contactId = idItem->data(Qt::UserRole).toInt();

    auto found = selectContactById(contactId);
    if (!found.has_value()) {
        QMessageBox::warning(this, "Редактирование", "Не удалось загрузить контакт из базы.");
        return;
    }

    ContactDialog dlg(found->second, this);
    dlg.setWindowTitle(tr("Редактировать контакт"));
    if (dlg.exec() != QDialog::Accepted)
        return;

    Contact edited = dlg.resultContact();

    if (!updateContact(contactId, edited)) {
        QMessageBox::warning(this, "Редактирование", "Не удалось обновить контакт в базе данных.");
        return;
    }

    auto *t = ui->tv_contacts;

    auto setText = [&](int col, const QString &text) -> QTableWidgetItem* {
        QTableWidgetItem *it = t->item(row, col);
        if (!it) {
            it = new QTableWidgetItem();
            t->setItem(row, col, it);
        }
        it->setText(text);
        return it;
    };

    auto *surnameItem = setText(0, QString::fromStdString(edited.getSurname()));
    surnameItem->setData(Qt::UserRole, contactId);

    setText(1, QString::fromStdString(edited.getName()));
    setText(2, QString::fromStdString(edited.getPatronymic()));

    QTableWidgetItem *phonesItem = setText(3, phonesToCell(edited));
    phonesItem->setData(Qt::ToolTipRole, phonesToTooltip(edited));

    setText(4, QString::fromStdString(edited.getemail()));
    setText(5, QString::fromStdString(edited.getAddress()));

    const auto &bd = edited.getBirth_date();
    QString dateStr;
    if (bd.year != 0) {
        dateStr = QString("%1.%2.%3")
        .arg(bd.day, 2, 10, QLatin1Char('0'))
            .arg(bd.month, 2, 10, QLatin1Char('0'))
            .arg(bd.year);
    }
    setText(6, dateStr);
}

void ContactsMainWindow::on_pb_deleteContact_clicked()
{
    int row = ui->tv_contacts->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Удаление", "Выберите контакт в таблице.");
        return;
    }

    QTableWidgetItem *idItem = ui->tv_contacts->item(row, 0);
    if (!idItem) {
        QMessageBox::warning(this, "Удаление", "Не удалось определить выбранную строку.");
        return;
    }

    int contactId = idItem->data(Qt::UserRole).toInt();

    QString surname = ui->tv_contacts->item(row, 0) ? ui->tv_contacts->item(row, 0)->text() : "";
    QString name    = ui->tv_contacts->item(row, 1) ? ui->tv_contacts->item(row, 1)->text() : "";

    auto ans = QMessageBox::question(
        this,
        "Удаление",
        QString("Удалить контакт \"%1 %2\"?").arg(surname, name),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (ans != QMessageBox::Yes)
        return;

    if (!deleteContact(contactId)) {
        QMessageBox::warning(this, "Удаление", "Не удалось удалить контакт из базы данных.");
        return;
    }

    ui->tv_contacts->removeRow(row);
}

void ContactsMainWindow::on_pb_showContact_clicked()
{
    QString email = ui->le_showContact->text().trimmed();

    if (email.isEmpty()) {
        loadContactsFromDb();
        return;
    }

    auto found = selectContactByEmail(email);
    if (!found.has_value()) {
        QMessageBox::information(this, "Поиск", "Человека с такой почтой нет.");
        return;
    }

    ui->tv_contacts->setRowCount(0);
    addContactToTable(found->first, found->second);
}
