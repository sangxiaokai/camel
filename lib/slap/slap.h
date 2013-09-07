///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SLAP_H
#define SLAP_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "slap_error.h"
#include "optional.h"
#include "convert.h"

#include <functional>
#include <initializer_list>
#include <type_traits>
#include <vector>
#include <set>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace slap
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class operation
{
    add,
    remove,
    replace
};

typedef std::vector<std::string> values;

///////////////////////////////////////////////////////////////////////////////////////////////////
class attribute
{
public:
    explicit attribute(const std::string& _m_name, slap::operation _m_mod= operation::add):
        _M_name(_m_name), _M_operation(_m_mod)
    { }

    attribute(const std::string& _m_name, std::initializer_list<std::string> _m_values):
        _M_name(_m_name), _M_operation(operation::add), _M_values(_m_values)
    { }

    attribute(const std::string& _m_name, slap::operation _m_mod, std::initializer_list<std::string> _m_values):
        _M_name(_m_name), _M_operation(_m_mod), _M_values(_m_values)
    { }

   ~attribute() { delete_mod(); }

    ////////////////////
    attribute(const attribute& other):
        _M_name      (other._M_name),
        _M_operation (other._M_operation),
        _M_values    (other._M_values)
    { }
    attribute& operator=(const attribute& other)
    {
        _M_name=      other._M_name;
        _M_operation= other._M_operation;
        _M_values=    other._M_values;
        return *this;
    }

    ////////////////////
    attribute(attribute&& other):
        _M_name      (std::move(other._M_name)),
        _M_operation (std::move(other._M_operation)),
        _M_values    (std::move(other._M_values)),
        _M_mod       (std::move(other._M_mod))
    { other._M_mod= nullptr; }

    attribute& operator=(attribute&& other)
    {
        _M_name=      std::move(other._M_name);
        _M_operation= std::move(other._M_operation);
        _M_values=    std::move(other._M_values);
        _M_mod=       std::move(other._M_mod);
        other._M_mod= nullptr;
        return *this;
    }

    ////////////////////
    std::string name() const { return _M_name; }
    slap::operation operation() const { return _M_operation; }

    bool empty() const { return _M_values.empty(); }
    size_t size() const { return _M_values.size(); }

    const slap::values& values() { return _M_values; }

    const std::string& value(size_t index=0) const { return _M_values.at(index); }
          std::string& value(size_t index=0)       { return _M_values.at(index); }

    ////////////////////
    template<typename T, typename std::enable_if< !std::is_same<T, bool>::value, int >::type=0>
    T to(size_t index=0) const
    {
        return convert::to<T>(value(index));
    }

    template<typename T, typename std::enable_if< std::is_same<T, bool>::value, int >::type=0>
    T to(size_t index=0) const
    {
        return (value(index)=="TRUE")? true: false;
    }

    ////////////////////
    template<typename T>
    bool get(T& value, size_t index=0) const
    {
        try
        {
            value= to<T>(index);
            return true;
        }
        catch(...) { return false; }
    }

    ////////////////////
    void insert(const std::string& value) { _M_values.push_back(value); }
    void insert(std::string&& value) { _M_values.push_back(value); }

    template<typename... Args>
    void emplace(Args&&... args) { _M_values.emplace_back(args...); }

private:
    std::string     _M_name;
    slap::operation _M_operation;
    slap::values    _M_values;

    mutable LDAPMod* _M_mod= nullptr;

    void create_mod() const;
    void delete_mod() const;

    friend class entry;
};

inline bool operator<(const attribute& x, const attribute& y) { return x.name() < y.name(); }

typedef std::set<attribute> attributes;

///////////////////////////////////////////////////////////////////////////////////////////////////
class connection;

class entry
{
public:
    explicit entry(const std::string& _m_dn): _M_dn(_m_dn) { }

   ~entry() { delete_mod(); }

    ////////////////////
    entry(const entry& other):
        _M_dn         (other._M_dn),
        _M_attributes (other._M_attributes)
    { }
    entry& operator=(const entry& other)
    {
        _M_dn=         other._M_dn;
        _M_attributes= other._M_attributes;
        return *this;
    }

    ////////////////////
    entry(entry&& other):
        _M_dn         (std::move(other._M_dn)),
        _M_attributes (std::move(other._M_attributes)),
        _M_mod        (std::move(other._M_mod))
    { other._M_mod= nullptr; }

    entry& operator=(entry&& other)
    {
        _M_dn=         std::move(other._M_dn);
        _M_attributes= std::move(other._M_attributes);
        _M_mod=        std::move(other._M_mod);
        other._M_mod= nullptr;
        return *this;
    }

    ////////////////////
    std::string dn() const { return _M_dn; }

    bool empty() const { return _M_attributes.empty(); }
    size_t size() const { return _M_attributes.size(); }

    int count(const std::string& name) const { return _M_attributes.count(slap::attribute(name)); }

    const slap::attributes& attributes() { return _M_attributes; }

    const slap::attribute& attribute(const std::string& name) const
    {
        if(count(name))
            return *_M_attributes.find(slap::attribute(name));
        else throw std::out_of_range("entry::attribute");
    }

    const std::string& value(const std::string& name, size_t index=0) const
    {
        return attribute(name).value(index);
    }

    ////////////////////
    template<typename T>
    T to(const std::string name, size_t index=0) const
    {
        return _M_to<T>::func(this, name, index);
    }

    ////////////////////
    template<typename T>
    bool get(T& value, const std::string& name, size_t index=0) const
    {
        try
        {
            value= to<T>(name, index);
            return true;
        }
        catch(...) { return false; }
    }

    ////////////////////
    bool insert(const slap::attribute& x) { return _M_attributes.insert(x).second; }
    bool insert(slap::attribute&& x) { return _M_attributes.insert(x).second; }

private:
    std::string      _M_dn;
    slap::attributes _M_attributes;

    mutable LDAPMod** _M_mod= nullptr;

    void create_mod() const;
    void delete_mod() const;

    friend class connection;

    ////////////////////
    template<typename T>
    struct _M_to
    {
        static T func(const slap::entry* e, const std::string name, size_t index=0)
        {
            return e->attribute(name).to<T>(index);
        }
    };

    template<typename T>
    struct _M_to<optional<T>>
    {
        static optional<T> func(const slap::entry* e, const std::string name, size_t index=0)
        {
            if(e->count(name))
                return e->attribute(name).to<T>(index);
            else return optional<T>();
        }
    };
};

typedef std::vector<entry> entries;

///////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::vector<std::string> names;
typedef std::function<bool(const entry&, const entry&)> order_func;

const extern order_func order_none;

enum class scope
{
    base,
    one_level,
    subtree,
    children
};

class connection
{
public:
    connection() { }
    connection(const std::string& uri) { open(uri); }

   ~connection() { close(); }

    ////////////////////
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    connection(connection&&) = delete;
    connection& operator=(connection&&) = delete;

    ////////////////////
    void open(const std::string& uri, bool start_TLS= true);
    void close();
    void bind(const std::string& dn= std::string(), const std::string& passwd= std::string());

    void add(const entry& e);
    void remove(const std::string& dn);
    void modify(const entry& e);

    void rename(const std::string& dn,
                const std::string& new_rdn,
                bool remove_old= true,
    const std::string& new_parent= std::string());

    bool compare(const std::string& dn, const attribute&);

    entries search(const std::string& base,
                   scope = scope::base,
                   const std::string& filter= "objectClass=*",
                   const order_func& func= order_none,
                   const names& = names(),
    bool value= true);

    ////////////////////
    std::string uri() const { return _M_uri; }
    std::string bind_dn() const { return _M_bind_dn; }

private:
    std::string _M_uri;
    std::string _M_bind_dn;
    mutable LDAP* _M_ldap= nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // SLAP_H