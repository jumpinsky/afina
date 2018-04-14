#include "MapBasedGlobalLockImpl.h"

#include <mutex>

namespace Afina {
namespace Backend {


void MapBasedGlobalLockImpl::FreeSpace(size_t needed_space)
{
	while(_current_size + needed_space > _max_size)
	{
		Entry* tail = _list.GetTail();
        _current_size -= tail->size();
        _backend.erase(tail->key);
        _list.Delete(tail);
	}
}

bool MapBasedGlobalLockImpl::_UnsafeSet(Entry *entry, const std::string &key, const std::string &value) 
{
	size_t delta_size = entry->value.size() - value.size();
    size_t new_size = _current_size - delta_size;

    // Can put in
    if (new_size <= _max_size) 
    {
        entry->value = value;
        _list.SetHead(entry);
        _current_size = new_size;
        return true;
    }
    // Not enough space
    return false;
}


bool MapBasedGlobalLockImpl::_UnsafePut(const std::string &key, const std::string &value) 
{
    //size_t additional_size = key.size() + value.size();
    //if(additional_size > _max_size)
    //	return false;
	auto it = _backend.find(key);

	// Update Value section
	if (it != _backend.end()) 
	{
		Entry *entry = it->second;
		if(_UnsafeSet(entry, key, value))
            return true;

		// Not enough space
	    _backend.erase(it);
	    _list.Delete(entry);
	    _current_size -= entry->size();
	}

	// Insert Entry section
    size_t additional_size = key.size() + value.size();
    if(additional_size > _max_size)
    	return false;

	FreeSpace(additional_size);
	Entry *new_entry = new Entry(key, value);
	_list.Add(new_entry);
	_backend.insert(std::make_pair(std::cref(new_entry->key), std::ref(new_entry)));

	_current_size += new_entry->size();
    return true; 
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Put(const std::string &key, const std::string &value) 
{
    size_t additional_size = key.size() + value.size();
    if(additional_size > _max_size)
    	return false;

    std::lock_guard<std::mutex> lock(_global_mutex);

    return _UnsafePut(key, value);
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::PutIfAbsent(const std::string &key, const std::string &value) 
{ 
	size_t additional_size = key.size() + value.size();
    if(additional_size > _max_size)
    	return false;

    std::lock_guard<std::mutex> lock(_global_mutex);

	auto it = _backend.find(key);
	if(it != _backend.end()) 
		return false;

    return _UnsafePut(key, value);
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Set(const std::string &key, const std::string &value) 
{ 
	size_t additional_size = key.size() + value.size();
    if(additional_size > _max_size)
    	return false;

    std::lock_guard<std::mutex> lock(_global_mutex);

	auto it = _backend.find(key);
	if(it != _backend.end()) 
		return _UnsafeSet(it->second, key, value);
		
    return false;
}


// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Delete(const std::string &key) 
{ 
	std::lock_guard<std::mutex> lock(_global_mutex);

	auto it = _backend.find(key);
	if(it != _backend.end()) 
	{
		Entry* entry = it->second;
		_current_size -= entry->size();
		_backend.erase(it);
		_list.Delete(entry);
		return true;
	}
	return false;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Get(const std::string &key, std::string &value) const 
{ 
	std::lock_guard<std::mutex> lock(_global_mutex);

	auto it = _backend.find(key);
	if(it != _backend.end()) 
	{
		Entry* entry = it->second;
	    value = entry->value;
	    _list.SetHead(entry);
		return true;
	}

	return false;
}

} // namespace Backend
} // namespace Afina
