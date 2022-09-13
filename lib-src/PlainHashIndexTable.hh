////////////////////////////////////////////////////////////////////////////////
// 
// PlainHashIndexTable.hh
//    produced: 06/04/98 jr
// last change: 06/04/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PLAINHASHINDEXTABLE_HH
#define PLAINHASHINDEXTABLE_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>

#include <vector>

#include <mutex>
#include <shared_mutex>
#include <atomic>

#include "Global.hh"
#include "HashKey.hh"

#ifndef STL_CONTAINERS
#include "RefCount.hh"
#include "Array.hh"
#include "PlainHashMap.hh"
#include "PlainArray.hh"
namespace topcom {
  template<class Key>
  using _key_index_map_type = PlainHashMap<Key, size_type>;
  template<class Key>
  using _index_table_type   = PlainArray<Key>;
};
#else
#include <unordered_map>
#include <vector>
namespace topcom {
  template<class Key>
  using _key_index_map_type = std::unordered_map<Key, size_type, Hash<Key> >;
  template<class Key>
  using _index_table_type   = std::vector<Key>;
};
#endif

namespace topcom {

  template<class Key>
  class PlainHashIndexTable : public _key_index_map_type<Key> {
  public:
    typedef _key_index_map_type<Key> key_table_type;
    typedef _index_table_type<Key>   index_table_type;
  public:
    index_table_type          _index_data;
    mutable bool              _preprocessed;
    mutable bool              _inside_multithreading;
    mutable std::shared_mutex _index_data_mutex; 
  public:
    // constructors:
    inline PlainHashIndexTable(const bool inside_multithreading = false) :
      key_table_type(),
      _index_data(),
      _preprocessed(false),
      _inside_multithreading(inside_multithreading) {}
    inline PlainHashIndexTable(const PlainHashIndexTable& phit) :
      key_table_type(phit),
      _index_data(phit._index_data),
      _preprocessed(phit._preprocessed),
      _inside_multithreading(phit._inside_multithreading) {}
    // destructor:
    inline ~PlainHashIndexTable() {}
    // preprocess option (if container is preprocessed, locking is disabled):
    inline void set_preprocessed(const bool preprocessed) {
      _preprocessed = preprocessed;
    }
    inline void start_multithreading() {
      _inside_multithreading = true;
    }
    inline void stop_multithreading() {
      _inside_multithreading = false;
    }
    // assignment:
    inline PlainHashIndexTable& operator=(const PlainHashIndexTable& phit) {
      if (this == &phit) {
	return *this;
      }
      if (!_inside_multithreading || _preprocessed) {
	key_table_type::operator=(phit);
	_index_data = phit._index_data;
	_preprocessed = phit._preprocessed;
      }
      else {
	std::lock_guard<std::mutex> _index_data_guard(_index_data_mutex);
	key_table_type::operator=(phit);
	_index_data = phit._index_data;
	_preprocessed = phit._preprocessed;
      }
      return *this;
    }
    // accessors:
    inline const bool preprocessed() const {
      return _preprocessed;
    }
    inline const size_type maxindex() const {
      return _index_data.size();
    }
    // functions:
    inline const Key& get_obj(const size_type index) const {
#ifdef INDEX_CHECK
      if (index >= _index_data.size()) {
	std::cerr << "Key& get_obj(const size_type): "
		  << "index out of range." << std::endl;
      }
#endif
      if (!_inside_multithreading || _preprocessed) {
	return _index_data[index];
      }
      else {
	// std::unique_lock<std::shared_mutex> _index_data_guard(_index_data_mutex);
	std::shared_lock<std::shared_mutex> _index_data_guard(_index_data_mutex);
	return _index_data[index];
      }
    }
    inline const size_type get_index(const Key& key) {
#ifdef DEBUG
      std::cerr << "getting index of key " << key << " in " << *this << " ..." << std::endl;
#endif
      if (_preprocessed) {      
	auto finder(key_table_type::find(key));
	if (finder != key_table_type::end()) {
	  return finder->second;
	}
	else {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "PlainHashIndexTable has *** not *** been preprocessed properly:" << std::endl;
	  std::cerr << key << " not in table " << *this << " - exiting." << std::endl;
	  exit(1);
	}
      }
      else {
	if (_inside_multithreading) {
	  {
	    // std::lock_guard<std::mutex> _index_data_guard(_index_data_mutex);
	    std::unique_lock<std::shared_mutex> _index_data_guard(_index_data_mutex);
	    auto finder(key_table_type::find(key));
	    if (finder != key_table_type::end()) {
	      return finder->second;
	    }
	  }
	  // write access from here - guard:
	  // std::lock_guard<std::mutex> _index_data_guard(_index_data_mutex);
	  std::unique_lock<std::shared_mutex> _index_data_guard(_index_data_mutex);
	  const size_type new_index(_index_data.size());
	  key_table_type::insert(std::pair<Key, size_type>(key, new_index));
	  // _index_data.resize(new_index + 1);
	  // _index_data[new_index] = key;
	  _index_data.push_back(key);
	  return new_index;
	}
	else {
	  auto finder(key_table_type::find(key));
	  if (finder != key_table_type::end()) {
	    return finder->second;
	  }
	  const size_type new_index(_index_data.size());
	  key_table_type::insert(std::pair<Key, size_type>(key, new_index));
	  // _index_data.resize(new_index + 1);
	  // _index_data[new_index] = key;
	  _index_data.push_back(key);
	  return new_index;
	}
      }
    }
    inline void insert(const Key& key) {
      get_index(key);
    }
  private:
    void erase(const Key&);		// no erase allowed!
    size_type operator[](const Key&);	// no non-const reference to Data allowed!
  };

}; // namespace topcom

#endif
// eof PlainHashIndexTable.hh
