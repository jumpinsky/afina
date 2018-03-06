#include "MapBasedGlobalLockImpl.h"

#include <mutex>

namespace Afina {
namespace Backend {

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Put(const std::string &key, const std::string &value) 
{
    std::lock_guard<std::mutex> lock(_global_mutex);

    size_t additional_size = key.size() + value.size();
    if(additional_size > _max_size)
    	return false;

	auto found = _backend.find(key);
	if(found != _backend.end()) 
	{
		Entry* found_entry = found->second;
		additional_size -= found_entry->size();
		if(_current_size + additional_size > _max_size)
			FreeSpace(additional_size);
		//_current_size += additional_size;
		return Set(found_entry, value);
	}

	if(_current_size + additional_size > _max_size)
		FreeSpace(additional_size);

	_current_size += additional_size;
	Entry *e = _list.Put(key, value);
	_backend[key] = e;

	return true; 
}

void MapBasedGlobalLockImpl::FreeSpace(size_t needed_space)
{
	while(_current_size + needed_space > _max_size)
	{
		Entry* lru_entry = _list.LRU_entry();
		lru_key = lru_entry->key;
		auto found = _backend.find(lru_key);
		
		if(found != _backend.end()) 
		{
			_current_size = _current_size - lru_entry->size();
			_list.Delete(lru_entry);
			_backend.erase(found);
		}
	}
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::PutIfAbsent(const std::string &key, const std::string &value) 
{ 
	std::lock_guard<std::mutex> lock(_global_mutex);

    size_t additional_size = key.size() + value.size();
    if(additional_size > _max_size)
    	return false;

	auto found = _backend.find(key);
	if(found != _backend.end()) 
		return false;

	if(_current_size + additional_size > _max_size)
		FreeSpace(additional_size);

	_current_size += additional_size;

	Entry *e = _list.Put(key, value);
	_backend[key] = e;

	return true; 
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Set(const std::string &key, const std::string &value) 
{ 
	std::lock_guard<std::mutex> lock(_global_mutex);

    size_t additional_size = key.size() + value.size();

	auto found = _backend.find(key);
	if(found != _backend.end()) 
	{
		Entry* found_entry = found->second;
		additional_size -= found_entry->size();
		
		if(_additional_size > _max_size)
			return false;

		if(_current_size + additional_size > _max_size)
			FreeSpace(additional_size);
		
		//_current_size += additional_size;
		return Set(found_entry, value);
	}

	return true; 
}

bool MapBasedGlobalLockImpl::Set(Entry *e, const std::string &value) 
{ 
	std::string key = e->key;
	_current_size -= e->size();
	_list.Delete(e);

	Entry* new_e = _list.Put(key, value);
	_current_size += new_e->size();

	return true;
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Delete(const std::string &key) 
{ 
	std::lock_guard<std::mutex> lock(_global_mutex);

	auto found = _backend.find(key);
	if(found != _backend.end()) 
	{
		Entry* found_entry = found->second;
		_current_size = _current_size - found_entry->size();
		_list.Delete(found_entry);
		_backend.erase(found);
	}
}

// See MapBasedGlobalLockImpl.h
bool MapBasedGlobalLockImpl::Get(const std::string &key, std::string &value) const 
{ 
	std::lock_guard<std::mutex> lock(_global_mutex);

	auto found = _backend.find(key);
	if(found != _backend.end()) 
	{
		Entry* found_entry = found->second;
		value = found_entry->value;

		_current_size -= found_entry->size();
		_list.Delete(found_entry);

		Entry* new_e = _list.Put(key, value);
		_current_size += new_e->size();
		return true;
	}

	return false;
}

} // namespace Backend
} // namespace Afina
