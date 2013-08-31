///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef APPLICATION_H
#define APPLICATION_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <QApplication>
#include <exception>

///////////////////////////////////////////////////////////////////////////////////////////////////
class Application: public QApplication
{
public:
    Application(int& argc, char* argv[]);
    bool notify(QObject* receiver, QEvent* event);

    ////////////////////
    virtual int proc() { return exec(); }
    int run();

    virtual int message(const std::string&, const std::string& = std::string());

private:
    std::exception_ptr _M_e= nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // APPLICATION_H
