///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CONFIG_H
#define CONFIG_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <QStringList>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////
struct Config
{
    QString path= "/etc/camel.conf";

    QString server_path;
    QString server_auth;
    QStringList server_args;

    QStringList sessions;

    QString theme_path= "/usr/share/camel/theme";
    QString theme_name= "default";
    QString theme_file= "theme.qml";

    void parse();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // CONFIG_H
