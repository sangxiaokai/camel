///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ARGUMENTS_H
#define ARGUMENTS_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "container.h"
#include "utility.h"

#include <initializer_list>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
class arguments: public container< std::vector<std::string> >
{
public:
    arguments() = default;
    arguments(std::initializer_list<value_type> x) { _M_c=x; }

    ////////////////////
    void push_back(const value_type& x) { _M_c.push_back(x); }
    void push_back(value_type&& x) { _M_c.push_back(std::move(x)); }

    void push_back(const arguments& x)
    {
        _M_c.insert(end(), x.begin(), x.end());
    }
    void push_back(std::initializer_list<value_type> x)
    {
        _M_c.insert(end(), x);
    }

    ////////////////////
    charpp_ptr to_charpp() const;
    charpp_ptr to_charpp(const std::string& prepend) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // ARGUMENTS_H
