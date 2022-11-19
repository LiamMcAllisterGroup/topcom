////////////////////////////////////////////////////////////////////////////////
// 
// ContainerIO.hh
//
//    produced: 13/02/2020 jr
// last change: 13/02/2020 jr
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef CONTAINERIO_HH
#define CONTAINERIO_HH

#include <iostream>
#include <deque>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include "Global.hh"
#include "HashKey.hh"

namespace topcom {

  class IO_sync {
  public:
    static std::mutex mutex;
  };

  class ContainerChars {
  public:
    static const char    list_start_char;
    static const char    list_end_char;
    static const char    set_start_char;
    static const char    set_end_char;
    static const char    delim_char;
    static const char*   map_chars;
  };

  // output for pairs:
  template <class FIRST, class SECOND>
  inline std::ostream& operator<<(std::ostream& ost, const std::pair<FIRST,SECOND>& p) {
    ost << '[' << p.first << ContainerChars::delim_char << p.second << ']';
    return ost;
  }

  // input for pairs:
  template <class FIRST, class SECOND>
  inline std::istream& operator>>(std::istream& ist,
				  std::pair<FIRST,SECOND>& p) {
    char c;
    ist >> std::ws >> c >> std::ws >> p.first >> std::ws >> c >> std::ws >> p.second >> std::ws >> c;
    return ist;
  }

  // output for std::deque:
  template <class T>
  inline std::ostream& operator<<(std::ostream& ost,
				  const std::deque<T>& container) {
    ost << ContainerChars::list_start_char;
    if (!container.empty()) {
      typename std::deque<T>::const_iterator iter = container.begin();
      ost << *iter;
      while(++iter != container.end()) {
	ost << ContainerChars::delim_char << *iter;
      }
    }
    ost << ContainerChars::list_end_char;
    return ost;
  }

  // input for std::deque:
  template <class T>
  inline std::istream& operator>>(std::istream& ist,
				  std::deque<T>& container) {
    char c;
    T elem;

    container.clear();
    ist >> std::ws >> c;
    if (c == ContainerChars::list_start_char) {
      while (ist >> std::ws >> c) {
	if (c == ContainerChars::list_end_char) {
	  break;
	}
	if (c == ContainerChars::delim_char) {
	  continue;
	}
	ist.putback(c);
	if (ist >> elem) {
	  container.push_back(elem);
	}
	else {
#ifdef READ_DEBUG
	  std::cerr << "std::istream& operator>>(std::istream&, const std::vector&):"
		    << c << " not of appropriate type." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "std::istream& operator>>(std::istream&, const std::vector&):"
		<< "missing `" << ContainerChars::list_start_char << "'." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  // output for std::vector:
  template <class T>
  inline std::ostream& operator<<(std::ostream& ost,
				  const std::vector<T>& container) {
    ost << ContainerChars::list_start_char;
    if (!container.empty()) {
      size_type i = 0;
      ost << container[i];
      while(++i < container.size()) {
	ost << ContainerChars::delim_char << container[i];
      }
    }
    ost << ContainerChars::list_end_char;
    return ost;
  }

  // input for std::vector:
  template <class T>
  inline std::istream& operator>>(std::istream& ist,
				  std::vector<T>& container) {
    char c;
    T elem;

    container.resize(0);
    ist >> std::ws >> c;
    if (c == ContainerChars::list_start_char) {
      while (ist >> std::ws >> c) {
	if (c == ContainerChars::list_end_char) {
	  break;
	}
	if (c == ContainerChars::delim_char) {
	  continue;
	}
	ist.putback(c);
	if (ist >> elem) {
	  container.push_back(elem);
	}
	else {
#ifdef READ_DEBUG
	  std::cerr << "std::istream& operator>>(std::istream&, const std::vector&):"
		    << c << " not of appropriate type." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "std::istream& operator>>(std::istream&, const std::vector&):"
		<< "missing `" << ContainerChars::list_start_char << "'." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  // output for std::set:
  template <class Key>
  inline std::ostream& operator<<(std::ostream& ost,
				  const std::set<Key>& container) {
    ost << ContainerChars::set_start_char;
    if (!container.empty()) {
      typename std::set<Key, Hash<Key> >::const_iterator iter = container.begin();
      ost << *iter;
      while (++iter != container.end()) {
	ost << ContainerChars::delim_char << *iter;
      }
    }
    ost << ContainerChars::set_end_char;
    return ost;
  }

  // input for std::set:
  template <class Key>
  inline std::istream& operator>>(std::istream& ist,
				  std::set<Key>& container) {
    char dash;
    char arrow;
  
    Key key_reader;
  
    char c;

    container.clear();
    ist >> std::ws >> c;
    if (c == ContainerChars::set_start_char) {
      while (ist >> std::ws >> c) {
	if (c == ContainerChars::set_end_char) {
	  break;
	}
	if (c == ContainerChars::delim_char) {
	  continue;
	}
	ist.putback(c);
	if (!(ist >> std::ws >> key_reader)) {
#ifdef READ_DEBUG
	  std::cerr << "std::istream& operator>>(std::istream&, std::unordered_set&: "
		    << "key not found." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	container.insert(key_reader);
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "std::istream& operator>>(std::istream&, std::unordered_set&): "
		<< "missing `" << ContainerChars::set_start_char
		<< "' for std::unordered_set." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  // output for std::map:
  template <class Key, class Data>
  inline std::ostream& operator<<(std::ostream& ost,
				  const std::map<Key, Data>& container) {
    ost << ContainerChars::set_start_char;	 
    if (!container.empty()) {
      auto iter = container.begin();
      ost << iter->first << ContainerChars::map_chars << iter->second;
      while (++iter != container.end()) {
	ost << ContainerChars::delim_char << iter->first << ContainerChars::map_chars << iter->second;
      }
    }
    ost << ContainerChars::set_end_char;
    return ost;
  }

  // input for std::unordered_map:
  template <class Key, class Data>
  inline std::istream& operator>>(std::istream& ist,
				  std::map<Key, Data>& container) {
    char dash;
    char arrow;
  
    Key key_reader;
    Data data_reader;
  
    char c;

    container.clear();
    ist >> std::ws >> c;
    if (c == ContainerChars::set_start_char) {
      // a tnode_container_type was opened:
      while (ist >> std::ws >> c) {
	if (c == ContainerChars::set_end_char) {
	  break;
	}
	if (c == ContainerChars::delim_char) {
	  continue;
	}
	ist.putback(c);
	if (!(ist >> std::ws >> key_reader)) {
#ifdef READ_DEBUG
	  std::cerr << "std::istream& operator>>(std::istream&, std::unordered_map&): "
		    << "key not found." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	if (!(ist >> std::ws >> dash >> arrow)) {
#ifdef READ_DEBUG
	  std::cerr << "std::istream& operator>>(std::istream&, std::unordered_map&): "
		    << "`->' not found." << std::endl;
#endif 
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	if (!(ist >> std::ws >> data_reader)) {
#ifdef READ_DEBUG
	  std::cerr << "std::istream& operator>>(std::istream&, std::unordered_map&): "
		    << "data not found." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	container[key_reader] = data_reader;
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "std::istream& operator>>(std::istream&, std::unordered_map&): "
		<< "missing `" << ContainerChars::set_start_char
		<< "' for std::unordered_map." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  // output for std::unordered_set:
  template <class Key>
  inline std::ostream& operator<<(std::ostream& ost,
				  const std::unordered_set<Key, Hash<Key> >& container) {
    ost << ContainerChars::set_start_char;
    if (!container.empty()) {
      typename std::unordered_set<Key, Hash<Key> >::const_iterator iter = container.begin();
      ost << *iter;
      while (++iter != container.end()) {
	ost << ContainerChars::delim_char << *iter;
      }
    }
    ost << ContainerChars::set_end_char;
    return ost;
  }

  // input for std::unordered_set:
  template <class Key>
  inline std::istream& operator>>(std::istream& ist,
				  std::unordered_set<Key, Hash<Key> >& container) {
    char dash;
    char arrow;
  
    Key key_reader;
  
    char c;

    container.clear();
    ist >> std::ws >> c;
    if (c == ContainerChars::set_start_char) {
      while (ist >> std::ws >> c) {
	if (c == ContainerChars::set_end_char) {
	  break;
	}
	if (c == ContainerChars::delim_char) {
	  continue;
	}
	ist.putback(c);
	if (!(ist >> std::ws >> key_reader)) {
#ifdef READ_DEBUG
	  std::cerr << "std::istream& operator>>(std::istream&, std::unordered_set&: "
		    << "key not found." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	container.insert(key_reader);
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "std::istream& operator>>(std::istream&, std::unordered_set&): "
		<< "missing `" << ContainerChars::set_start_char
		<< "' for std::unordered_set." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  // output for std::unordered_map:
  template <class Key, class Data>
  inline std::ostream& operator<<(std::ostream& ost,
				  const std::unordered_map<Key, Data, Hash<Key> >& container) {
    ost << ContainerChars::set_start_char;	 
    if (!container.empty()) {
      auto iter = container.begin();
      ost << iter->first << ContainerChars::map_chars << iter->second;
      while (++iter != container.end()) {
	ost << ContainerChars::delim_char << iter->first << ContainerChars::map_chars << iter->second;
      }
    }
    ost << ContainerChars::set_end_char;
    return ost;
  }

  // input for std::unordered_map:
  template <class Key, class Data>
  inline std::istream& operator>>(std::istream& ist,
				  std::unordered_map<Key, Data, Hash<Key> >& container) {
    char dash;
    char arrow;
  
    Key key_reader;
    Data data_reader;
  
    char c;

    container.clear();
    ist >> std::ws >> c;
    if (c == ContainerChars::set_start_char) {
      // a tnode_container_type was opened:
      while (ist >> std::ws >> c) {
	if (c == ContainerChars::set_end_char) {
	  break;
	}
	if (c == ContainerChars::delim_char) {
	  continue;
	}
	ist.putback(c);
	if (!(ist >> std::ws >> key_reader)) {
#ifdef READ_DEBUG
	  std::cerr << "std::istream& operator>>(std::istream&, std::unordered_map&): "
		    << "key not found." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	if (!(ist >> std::ws >> dash >> arrow)) {
#ifdef READ_DEBUG
	  std::cerr << "std::istream& operator>>(std::istream&, std::unordered_map&): "
		    << "`->' not found." << std::endl;
#endif 
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	if (!(ist >> std::ws >> data_reader)) {
#ifdef READ_DEBUG
	  std::cerr << "std::istream& operator>>(std::istream&, std::unordered_map&): "
		    << "data not found." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
	container[key_reader] = data_reader;
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "std::istream& operator>>(std::istream&, std::unordered_map&): "
		<< "missing `" << ContainerChars::set_start_char
		<< "' for std::unordered_map." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

}; // namespace topcom

#endif

// eof ContainerIO.hh
