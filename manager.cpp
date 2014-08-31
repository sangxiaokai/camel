///////////////////////////////////////////////////////////////////////////////////////////////////
#include "manager.h"
#include "config.h"
#include "x11.h"
#include "pam/pam.h"
#include "pam/pam_error.h"
#include "logger.h"

#include <QApplication>
#include <QtDeclarative/QDeclarativeView>
#include <QDesktopWidget>
#include <QGraphicsObject>
#include <QtNetwork/QHostInfo>
#include <QDir>
#include <QFile>
#include <QStringList>

#include <functional>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////////////////////////
Manager::Manager(QString config_path, QObject* parent):
    QObject(parent)
{
    if(config_path.size()) config.path= config_path;

    config.parse();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int Manager::run()
{
    try
    {
        X11::Server server;

        if(config.server_path.size()) server.set_path(config.server_path);
        if(config.server_auth.size()) server.set_auth(config.server_auth);
        if(config.server_args.size()) server.set_args(config.server_args);

        if(server.start())
        {
            QApplication application(server.display());
            render(&application);

            pam::context context("camel");

            context.set_user_func(std::bind(&Manager::get_user, this, std::placeholders::_1));
            context.set_pass_func(std::bind(&Manager::get_pass, this, std::placeholders::_1));

            context.set_item(pam::item::ruser, "root");
            context.set_item(pam::item::tty, server.name().toStdString());

            while(true)
            {
                emit reset();
                application.exec();

                try
                {
                    context.reset_item(pam::item::user);
                    context.authenticate();

                    QString value= get_session();
                    if(value == "poweroff")
                    {
                    }
                    else if(value == "reboot")
                    {
                    }
                    else if(value == "hibernate")
                    {
                    }
                    else if(value == "suspend")
                    {
                    }
                    else
                    {
                        context.open_session();

                        // get user data
                        // store it in PAM

                        // fork

                        // child: extract user data
                        // child: sessreg?
                        // child: switch user
                        // child: set client auth
                        // child: exec session
                        // child: exit

                        // server: wait for child
                        // server: sessreg?

                        context.close_session();
                    }

                    break;
                }
                catch(pam::pam_error& e)
                {
                    emit error(e.what());

                    std::cerr << e.what() << std::endl;
                    sys::logger << sys::error << e.what();
                }
            }
        }
        else throw std::runtime_error("X server failed to start");

        server.stop();
        return 0;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        sys::logger << sys::error << e.what();
        return 1;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Manager::render(QApplication* application)
{
    QString current= QDir::currentPath();
    try
    {
        username= password= sessions= session= hostname= nullptr;

        if(!QDir::setCurrent(config.theme_path+ "/"+ config.theme_name))
            throw std::runtime_error("Theme dir "+ config.theme_name.toStdString()+ " not found");

        if(!QFile::exists(config.theme_file))
            throw std::runtime_error("Theme file "+ config.theme_file.toStdString()+ " not found");

        QDeclarativeView* view= new QDeclarativeView(QUrl::fromLocalFile(config.theme_file), application->desktop());
        view->setGeometry(application->desktop()->screenGeometry());
        view->show();

        QGraphicsObject* root= view->rootObject();
        connect(this, SIGNAL(reset()), root, SIGNAL(reset()));
        connect(this, SIGNAL(error(QString)), root, SIGNAL(error(QString)));
        connect(root, SIGNAL(quit()), application, SLOT(quit()));

        username= root->findChild<QObject*>("username");
        if(!username) throw std::runtime_error("Missing username element");

        password= root->findChild<QObject*>("password");
        if(!password) throw std::runtime_error("Missing password element");

        sessions= root->findChild<QObject*>("sessions");
        set_sessions();

        session= root->findChild<QObject*>("session");

        hostname= root->findChild<QObject*>("hostname");
        if(hostname) hostname->setProperty("text", QHostInfo::localHostName());

        QDir::setCurrent(current);
    }
    catch(...)
    {
        QDir::setCurrent(current);
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Manager::get_user(std::string& value)
{
    if(username)
    {
        value= username->property("text").toString().toStdString();
        return true;
    }
    else return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Manager::get_pass(std::string& value)
{
    if(password)
    {
        value= password->property("text").toString().toStdString();
        return true;
    }
    else return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Manager::set_sessions()
{
    if(sessions)
    {
        QDir dir("/etc/X11/Sessions");
        if(dir.isReadable())
        {
            QStringList files= dir.entryList(QDir::Files);

            if(config.sessions.size())
                files= files.toSet().intersect(config.sessions.toSet()).toList();

            sessions->setProperty("text", files);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString Manager::get_session()
{
    QString value;
    if(session) value= session->property("text").toString();

    return value.size()? value: "Xsession";
}
