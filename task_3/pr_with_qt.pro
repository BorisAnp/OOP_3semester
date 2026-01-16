QT       += core gui
QT       += sql
QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    contact_class.cpp \
    contact_storage.cpp \
    contactdialog.cpp \
    db_functions.cpp \
    db_work.cpp \
    main.cpp \
    contactsmainwindow.cpp

HEADERS += \
    contact_class.h \
    contact_storage.h \
    contactdialog.h \
    contactsmainwindow.h \
    db_functions.h \
    db_work.h

FORMS += \
    contactdialog.ui \
    contactsmainwindow.ui

win32:RC_FILE = file_icon.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon_file_for_main_window.qrc
