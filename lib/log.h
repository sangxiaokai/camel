///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LOG_H
#define LOG_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <syslog.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace log
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum level
{
    emergency   = LOG_EMERG,
    alert       = LOG_ALERT,
    critical    = LOG_CRIT,
    error       = LOG_ERR,
    warning     = LOG_WARNING,
    notice      = LOG_NOTICE,
    info        = LOG_INFO,
    debug       = LOG_DEBUG
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class streambuf: public std::streambuf
{
protected:
    static constexpr level default_level= info;

    std::streamsize xsputn(const char* message, std::streamsize n) override;

    level current= default_level;
    friend class stream;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class stream: public std::ostream
{
public:
    stream(): std::ostream(&buffer) { }
    stream& operator<<(level);

protected:
    streambuf buffer;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern log::stream logger;

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // LOG_H