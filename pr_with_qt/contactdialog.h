#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>
#include "contact_class.h"

namespace Ui {
class ContactDialog;
}

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(QWidget *parent = nullptr);
    explicit ContactDialog(const Contact &contact, QWidget *parent = nullptr);

    ~ContactDialog();

    Contact resultContact() const { return contact_; }
    bool isEditMode() const { return isEdit_; }


private slots:
    void on_pb_backToMain_clicked();

    void on_pb_addTelephone_clicked();

    void on_pb_deleteTelephone_clicked();

    void on_pb_addContact_clicked();

private:
    Ui::ContactDialog *ui;

    bool isEdit_ = false;
    int  contactId_ = -1;

    Contact contact_;

    void initTelephonesTable();
    void fillFromContact(const Contact &c);
    std::vector<Contact::Phone> collectPhonesFromTable() const;
};

#endif // CONTACTDIALOG_H
