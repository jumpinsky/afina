#ifndef AFINA_STORAGE_ENTRY_H
#define AFINA_STORAGE_ENTRY_H

#include <unordered_map>
#include <mutex>
#include <string>
#include <iostream>

namespace Afina {
namespace Backend {

class Entry
{
public:
    std::string value;
    std::string key;

    Entry(const std::string& key, const std::string& value, Entry* prev=nullptr, Entry* next=nullptr) :
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
    List(): _head(nullptr), _tail(nullptr) {};

    ~List()
    {
        Entry* temp = _head;
        while(temp != nullptr)
        {
            //std::cout << "Not null " << temp  << " with " 
            //            << temp->key << "->" << temp->value << std::endl;
            Entry *next = temp;
            temp = temp->get_next();
            //std::cout << "Getting next " << next << std::endl;
            delete next;
        }
        
    }

    void Add(Entry* entry) 
    {
        entry->set_next(_head);
        entry->set_previous(nullptr);

        if (_head != nullptr) 
        {
            _head->set_previous(entry);
        }
        if (_tail == nullptr) 
        {
            _tail = entry;
        }
        _head = entry;
    }

    Entry* GetTail() const {return _tail;}

    void SetHead(Entry* entry)
    {
        if (entry == _head) return;
        
        Entry* entry_next = entry->get_next();
        Entry* entry_previous = entry->get_previous();


        entry_previous->set_next(entry_next);
        if (entry == _tail) {
            entry_previous->set_next(nullptr);
            _tail = entry_previous;
        } 
        else {
            entry_next->set_previous(entry_previous);
        }

        entry->set_previous(nullptr);
        entry->set_next(_head);

        _head->set_previous(entry);
        _head = entry;
    }

    void Delete(Entry* entry)
    {
        Entry* entry_next = entry->get_next();
        Entry* entry_previous = entry->get_previous();

        if (entry == _head) {
            _head = entry_next;
        } 
        else {
            entry_previous->set_next(entry_next);
        }

        if (entry == _tail) {
            entry_previous->set_next(nullptr);
            _tail = entry_previous;
        } 
        else {
            entry_next->set_previous(entry_previous);
        }

        delete entry;
    }

private:
    Entry* _head;
    Entry* _tail;
};



}
}

#endif