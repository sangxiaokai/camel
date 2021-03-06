///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ENVIRON_HPP
#define ENVIRON_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "charpp.hpp"
#include "container.hpp"

#include <cstddef>
#include <iterator>
#include <map>
#include <string>
#include <utility>

#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace app
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class environ: public container<std::map<std::string, std::string>>
{
public:
    typedef typename container_type::key_type name_type;
    typedef typename container_type::mapped_type value_type;

public:
    environ() = default;
    environ(const environ&) = default;
    environ(environ&&) = default;

    environ& operator=(const environ&) = default;
    environ& operator=(environ&&) = default;

    ////////////////////
    value_type& get(const name_type& name) { return _M_c.at(name); }
    const value_type& get(const name_type& name) const { return _M_c.at(name); }

    ////////////////////
    template<typename NameType, typename ValueType>
    void insert(NameType&& name, ValueType&& value)
    {
        insert(std::make_pair(std::forward<NameType>(name), std::forward<ValueType>(value)));
    }
    using container::insert;

    using container::erase;
    void erase(const name_type& name) { _M_c.erase(name); }

    ////////////////////
    size_type count(const name_type& name) const { return _M_c.count(name); }

    iterator find(const name_type& name) { return _M_c.find(name); }
    const_iterator find(const name_type& name) const { return _M_c.find(name); }

    ////////////////////
    charpp_ptr to_charpp() const;
    static environ from_charpp(char*[], bool free = false);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace this_environ
{

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::string name_type;
typedef std::string value_type;
typedef size_t size_type;

class iterator;
typedef iterator const_iterator;
typedef std::reverse_iterator<iterator> reverse_iterator;
typedef std::reverse_iterator<iterator> const_reverse_iterator;

///////////////////////////////////////////////////////////////////////////////////////////////////
name_type name(char*, bool* found);
value_type value(char*, bool* found);

template<typename Iterator>
name_type name(Iterator ri, bool* found = nullptr) { return name(*ri, found); }

template<typename Iterator>
value_type value(Iterator ri, bool* found = nullptr) { return value(*ri, found); }

///////////////////////////////////////////////////////////////////////////////////////////////////
class iterator: public std::iterator<std::bidirectional_iterator_tag, char*, ptrdiff_t, char**, char*>
{
public:
    iterator() noexcept: _M_p(nullptr) { }
    iterator(pointer x) noexcept: _M_p(x) { }

    iterator(const iterator& x) noexcept { _M_p = x._M_p; }
    iterator& operator=(const iterator& x) noexcept { _M_p = x._M_p; return (*this); }

    reference operator*() const noexcept { return _M_p ? *_M_p : nullptr; }

    this_environ::name_type name(bool* found = nullptr) const { return this_environ::name(*this, found); }
    this_environ::value_type value(bool* found = nullptr) const { return this_environ::value(*this, found); }

    iterator& operator++() noexcept { ++_M_p; return (*this); }
    iterator operator++(int) noexcept
    {
        iterator x(*this);
        operator++();
        return x;
    }

    iterator& operator--() noexcept { --_M_p; return (*this); }
    iterator operator--(int) noexcept
    {
        iterator x(*this);
        operator--();
        return x;
    }

    friend bool operator==(const iterator& x, const iterator& y) noexcept { return x._M_p == y._M_p; }
    friend bool operator!=(const iterator& x, const iterator& y) noexcept { return x._M_p != y._M_p; }
    friend size_type size() noexcept;

private:
    pointer _M_p;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
value_type get(const name_type& name, bool* found = nullptr) noexcept;
void insert(const name_type& name, const value_type& value, bool over = true);
void erase(const std::string& name);

////////////////////
inline iterator begin() noexcept { return iterator(::environ); }
inline iterator end() noexcept
{
    iterator ri = begin(); while(*ri) ++ri;
    return ri;
}

inline reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
inline reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

inline const_iterator cbegin() noexcept { return begin(); }
inline const_iterator cend() noexcept { return end(); }

inline const_reverse_iterator crbegin() noexcept { return rbegin(); }
inline const_reverse_iterator crend() noexcept { return rend(); }

////////////////////
inline bool empty() noexcept { return *begin() == nullptr; }
inline size_type size() noexcept { return end()._M_p - begin()._M_p; }

size_type count(const name_type& name) noexcept;

inline app::environ environ() { return app::environ::from_charpp(::environ); }

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // ENVIRON_HPP
