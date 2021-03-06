///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CHARPP_HPP
#define CHARPP_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include <memory>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
struct charpp_deleter
{
    void operator()(char* args[]) noexcept
    {
        if(args)
        {
            for(char** ri = args; *ri; ++ri) std::free(*ri);
            std::free(args);
        }
    }
};

///
/// \brief charpp_ptr
///
/// Smart pointer to a NULL-terminated array of char* pointers.
/// These pointers are used, for example, by execve to pass arguments
/// and environment.
///
/// app::arguments and app::environ classes can return charpp_ptr through
/// to_charpp function.
///
typedef std::unique_ptr<char*[], charpp_deleter> charpp_ptr;

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // CHARPP_HPP
