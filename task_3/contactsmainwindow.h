#ifndef CONTACTSMAINWINDOW_H
#define CONTACTSMAINWINDOW_H

#include <QMainWindow>
#include "contact_class.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ContactsMainWindow;
}
QT_END_NAMESPACE

class ContactsMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ContactsMainWindow(QWidget *parent = nullptr);
    ~ContactsMainWindow();

private slots:
    void on_pb_addContact_clicked();

    void on_pb_editContact_clicked();

    void on_pb_deleteContact_clicked();

    void on_pb_showContact_clicked();

private:
    Ui::ContactsMainWindow *ui;

    void addContactToTable(int id, const Contact &c);

    void loadContactsFromDb();

};
#endif // CONTACTSMAINWINDOW_H
