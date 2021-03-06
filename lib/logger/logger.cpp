///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "logger.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace log
{

///////////////////////////////////////////////////////////////////////////////////////////////////
void logger_base::putchar(char c) noexcept
{
    if(c == _n)
    {
        syslog(level, "%s", buffer.data());

        buffer.clear();
        level = default_level;
    }
    else buffer += c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
logger_base::~logger_base()
{
    if(buffer.size()) putchar(_n);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
thread_local log::logger_stream<char> logger;

///////////////////////////////////////////////////////////////////////////////////////////////////
}
