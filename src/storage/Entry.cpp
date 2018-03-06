#ifndef AFINA_STORAGE_ENTRY_H
#define AFINA_STORAGE_ENTRY_H

#include <map>
#include <mutex>
#include <string>


namespace Afina {
namespace Backend {

class Entry
{
public:
    std::string value;
    std::string key;

    Entry(const std::string& key, const std::string& value, Entry* prev, Entry* next) :
            value(value), key(key), _prev(prev), _next(next)
    {}

    Entry()
    {
        key = "default";
        value = "";
        _prev = nullptr;
        _next = nullptr;
    }

    size_t size() const
    { // noexcept?
        return key.size() + value.size();
    }

    Entry* get_next() const
    {
        return _next;
    }

    Entry* set_next(Entry* new_next)
    {
        _next = new_next;
        return _next;
    }

    Entry* get_previous() const
    {
        return _prev;
    }

    Entry* set_previous(Entry* new_prev)
    {
        _prev = new_prev;
        return _prev;
    }

private:
    Entry* _next;
    Entry* _prev;

};

class List
{
public:
    List()
    {
        _head = nullptr;
        _tail = nullptr;
    }

    ~List()
    {
        Entry* current_entry = _head;
        while(current_entry != nullptr){
            Entry *temp = current_entry->get_next();
            delete current_entry;
            current_entry = temp;
        }
    }

    Entry* Put(const std::string& key, const std::string& value) 
    {
        Entry* e = new Entry(key, value, nullptr, _head);
        _head->set_previous(e);
        _head = e;
        return e;
    }

    Entry* LRU_Entry()
    {
        return _tail;
    }

    void Delete(Entry* e)
    {
        Entry* next_e = e->get_next();
        Entry* prev_e = e->get_previous();

        next_e->set_previous(prev_e);
        prev_e->set_next(next_e);

        delete e;
    }

private:
    Entry* _head;
    Entry* _tail;
};



}
}

#endif