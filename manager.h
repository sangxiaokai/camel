///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef MANAGER_H
#define MANAGER_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "config.h"

#include <QObject>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////
class QApplication;

class Manager: public QObject
{
    Q_OBJECT
public:
    explicit Manager(QString config_path= QString(), QObject* parent= nullptr);
    int run();

signals:
    void reset();
    void error(const QString&);

private:
    Config config;

    QObject* username;
    QObject* password;

    QObject* sessions;
    QObject* session;

    QObject* hostname;

    void render(QApplication*);

    bool get_user(std::string&);
    bool get_pass(std::string&);

    void set_sessions();
    QString get_session();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // MANAGER_H
