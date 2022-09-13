////////////////////////////////////////////////////////////////////////////////
// 
// PlainArray.hh 
//
//    produced: 09/02/98 jr
// last change: 09/02/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PLAINARRAY_HH
#define PLAINARRAY_HH

#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <assert.h>
#include <limits>

#include <memory>

#include "Global.hh"
#include "RefCount.hh"
#include "HashKey.hh"

namespace topcom {

  template <class T>
  class PlainArray;

  template <class T>
  std::istream& operator>>(std::istream& ist, PlainArray<T>& a);

  template <class T>
  std::ostream& operator<<(std::ostream& ost, const PlainArray<T>& a);

  template <class T>
  class Array;

  template <class T>
  class __pa_iterator;

  template <class T>
  class __pa_const_iterator;

  template <class T>
  class PlainArray {
  public:
    friend class __pa_const_iterator<T>;
    friend class __pa_iterator<T>;
    typedef __pa_const_iterator<T> const_iterator;
    typedef __pa_iterator<T>       iterator;
    friend class Array<T>;
  private:
    enum { base_memsize = 0 };
  private:
    typedef T                   data_type;
    typedef SmartPtr<data_type> dataptr_type;
  private:
    std::allocator<dataptr_type> dataptr_allocator;
  private:
    static HashKeySize<data_type> _hashkeysize;
    static HashKey<data_type>     _hashkey;
  private:
    static const char start_char;
    static const char end_char;
    static const char delim_char;
  private:
    static T default_obj;
  private:
    dataptr_type*           _data;
    size_type               _memsize;
    size_type               _size;
  private:
    inline const size_type _get_new_memsize(const size_type);
    inline const size_type _resize(const size_type);
  public:
    inline const size_type reserve(const size_type);
    inline const size_type resize(const size_type, const T& = default_obj);
    // constructors:
    inline PlainArray();
    inline PlainArray(const PlainArray& pa, const size_type min_index = 0UL, const size_type max_index = ULONG_MAX);
    inline PlainArray(const size_type, const T& = default_obj);
    // destructor:
    inline ~PlainArray();
    // assignment:
    inline PlainArray& operator=(const PlainArray&);
    // keys for containers:
    inline unsigned long keysize() const;
    inline unsigned long key(const unsigned long) const;
    // accessors:
    inline const bool empty() const;
    inline const size_type size() const;
    // indexing:
    inline const T& operator[](const size_type) const;
    inline T& operator[](const size_type);
    // operations:
    inline void clear();
    inline PlainArray<T>& push_back(const T&, const size_type = 1);
    inline PlainArray<T>& push_back(const PlainArray&);
    // boolean expressions:
    inline bool operator==(const PlainArray&) const;
    inline bool operator!=(const PlainArray&) const;
    inline bool lex_compare(const PlainArray&, const size_type = 0) const;
    inline bool operator<(const PlainArray&) const;
    inline bool operator>(const PlainArray&) const;
    // iterator:
    inline const_iterator begin() const;
    inline iterator       begin();
    inline const_iterator end() const;
    inline iterator       end();
    // iostream:
    inline std::istream& read(std::istream&);
    inline std::ostream& write(std::ostream&) const;
    // friends (have to be inlined right here):
    friend std::istream& operator>><>(std::istream& ist, PlainArray<T>& a);
    friend std::ostream& operator<<<>(std::ostream& ost, const PlainArray<T>& a);
  };

  template <class T>
  class __pa_iterator {
  private:
    PlainArray<T>* _container;
    size_type      _current_index;
  public:
    // constructors:
    inline __pa_iterator(PlainArray<T>&);
    inline __pa_iterator(PlainArray<T>&, int);
    inline __pa_iterator(PlainArray<T>&, size_type);
    inline __pa_iterator(const __pa_iterator&);
    // destructor:
    inline ~__pa_iterator();
    // assignment:
    inline __pa_iterator& operator=(const __pa_iterator&);
    // accessors:
    inline PlainArray<T>* container() const;
    inline size_type      current_index() const;
    // functions:
    inline bool operator==(const __pa_iterator&) const;
    inline bool operator!=(const __pa_iterator&) const;
    inline T& operator*();
    inline T* operator->();
    inline __pa_iterator& operator++();
    inline __pa_iterator operator++(int);
  };

  template <class T>
  class __pa_const_iterator {
  private:
    const PlainArray<T>* _container;
    size_type            _current_index;
  public:
    // constructors:
    inline __pa_const_iterator(const PlainArray<T>&);
    inline __pa_const_iterator(const PlainArray<T>&, int);
    inline __pa_const_iterator(const PlainArray<T>&, size_type);
    inline __pa_const_iterator(const __pa_const_iterator&);
    inline __pa_const_iterator(const __pa_iterator<T>&);
    // destructor:
    inline ~__pa_const_iterator();
    // assignment:
    inline __pa_const_iterator& operator=(const __pa_const_iterator&);
    // accessors:
    inline PlainArray<T>* container() const;
    inline size_type      current_index() const;
    // functions:
    inline bool operator==(const __pa_const_iterator&) const;
    inline bool operator!=(const __pa_const_iterator&) const;
    inline const T& operator*() const;
    inline const T* operator->() const;
    inline __pa_const_iterator& operator++();
    inline __pa_const_iterator operator++(int);
  };

  template <class T>
  inline const size_type PlainArray<T>::_get_new_memsize(const size_type new_size) {
    if (new_size == 0UL) {
      return 0UL;
    }
    size_type new_memsize = _memsize;
    if (new_memsize == 0) {
      ++new_memsize;
    }
    while (4 * new_size < new_memsize) {
      new_memsize /= 2;
    }
    while (new_size > new_memsize) {
      new_memsize *= 2;
    }
    if (new_memsize < base_memsize) {
      new_memsize = base_memsize;
    }
    return new_memsize;
  }
  template <class T>
  inline const size_type PlainArray<T>::_resize(const size_type new_memsize) {
#ifdef RESIZE_DEBUG
    std::cout << "PlainArray<T>::_resize("<< new_memsize << ')' << std::endl;
#endif
    if (new_memsize == 0UL) {
      if (_data) {
	for (size_type i = 0; i < _size; ++i) {
	  dataptr_allocator.destroy(&_data[i]);
	}
	dataptr_allocator.deallocate(_data, _memsize);
      }
      _data = 0UL;
      _size = 0UL;
      _memsize = 0UL;
      return 0UL;
    }
    if (_memsize == new_memsize) {
      return _memsize;
    }
    dataptr_type* new_data = dataptr_allocator.allocate(new_memsize);
    const size_type min_index = _size < new_memsize ? _size : new_memsize;
    for (size_type i = 0; i < min_index; ++i) {
      dataptr_allocator.construct(&new_data[i], _data[i]);
    }
    if (_data) {
      for (size_type i = 0; i < _size; ++i) {
	dataptr_allocator.destroy(&_data[i]);
      }
      dataptr_allocator.deallocate(_data, _memsize);
    }
    _data = new_data;
    _memsize = new_memsize;
    return _memsize;
  }
  template <class T>
  inline const size_type PlainArray<T>::reserve(const size_type new_memsize) {
    if (new_memsize > _memsize) {
      return _resize(new_memsize);
    }
    else {
      return _memsize;
    }
  }

  template <class T>
  inline const size_type PlainArray<T>::resize(const size_type new_size, const T& init_obj) {
    _resize(_get_new_memsize(new_size));
    for (size_type i = _size; i < new_size; ++i) {
      dataptr_allocator.construct(&_data[i], dataptr_type(init_obj));
    }
    _size = new_size;  
    return _size;
  }
  template <class T>
  inline PlainArray<T>::PlainArray() : 
    _data(0), _memsize(0), _size(0) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "PlainArray::PlainArray()" << std::endl;
#endif
  }
  template <class T>
  inline PlainArray<T>::PlainArray(const PlainArray& plainarray, const size_type min_index, const size_type max_index) : 
    _memsize(plainarray._memsize), _size(plainarray._size) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "PlainArray::PlainArray(const PlainArray&)" << std::endl;
#endif
    if ((min_index >= max_index) || (min_index >= _size)){
      _data = 0;
      _memsize = 0;
      _size = 0;
    }
    else {
      if (max_index < _size) {
	_size = max_index - min_index;
      }
      else {
	_size -= min_index;
      }
      _memsize = _get_new_memsize(_size);
      _data = dataptr_allocator.allocate(_memsize);
      for (size_type index = 0; index < _size; ++index) {
	dataptr_allocator.construct(&_data[index], plainarray._data[index + min_index]);
      }
    }
  }
  template <class T>
  inline PlainArray<T>::PlainArray(const size_type init_memsize, const T& obj) : 
    _memsize(init_memsize), _size(init_memsize) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "PlainArray::PlainArray(const size_type, const T&)" << std::endl;
#endif
    _data = dataptr_allocator.allocate(_memsize);
    for (size_type index = 0; index < _memsize; ++index) {
      dataptr_allocator.construct(&_data[index], dataptr_type(obj));
    }
  }
  template <class T>
  inline PlainArray<T>::~PlainArray() {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "PlainArray::~PlainArray()" << std::endl;
#endif
    if (_data){
      for (size_type i = 0; i < _size; ++i) {
	dataptr_allocator.destroy(&_data[i]);
      }
      dataptr_allocator.deallocate(_data, _memsize);
    }
  }
  template <class T>
  inline PlainArray<T>& PlainArray<T>::operator=(const PlainArray& plainarray) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "PlainArray::operator=(const PlainArray&)" << std::endl;
#endif
    if (&plainarray == this) {
      return *this;
    }
    size_type new_size = plainarray._size;
    _resize(_get_new_memsize(new_size));
    const size_type min_index = _size < new_size ? _size : new_size;
    for (size_type i = 0; i < min_index; ++i) {
      _data[i] = plainarray._data[i];
    }
    for (size_type i = min_index; i < new_size; ++i) {
      dataptr_allocator.construct(&_data[i], plainarray._data[i]);
    }
    _size = new_size;
    return *this;
  }
  // keys for containers:
  // for general types providing key() and keysize():
  template <class T>
  inline unsigned long PlainArray<T>::keysize() const {
#ifdef HASH_DEBUG
    std::cerr << "PlainArray<T>::keysize() was called - return value = " << size() << std::endl;
#endif
    unsigned long res = 0;
    for (unsigned long i = 0; i < size(); ++i) {
      res += _hashkeysize((*this)[i]);
    }
    return res;
  }
  template <class T>
  inline unsigned long PlainArray<T>::key(const unsigned long n) const {
    unsigned long block_no = n + 1;
    unsigned long i = 0;
    while ((block_no > 0) && (i < size())) {
      if (block_no > _hashkeysize((*this)[i])) {
	block_no -= _hashkeysize((*this)[i++]);
      }
      else {
#ifdef HASH_DEBUG
	std::cerr << "PlainArray<T>::key(" << n << ") was called - return value = " << (*this)[n] << std::endl;
#endif
	return _hashkey((*this)[i], block_no - 1);
      }
    }
    std::cerr << "key(const unsigned long): no valid key found for "
	      << *this << " at " << n << std::endl;

    return 0UL;
  }
  // specialization for int:
  template <>
  inline unsigned long PlainArray<int>::keysize() const {
#ifdef HASH_DEBUG
    std::cerr << "PlainArray<int>::keysize() was called - return value = " << size() << std::endl;
#endif
    return size();
  }
  template <>
  inline unsigned long PlainArray<int>::key(const unsigned long n) const {
#ifdef HASH_DEBUG
    std::cerr << "PlainArray<int>::key(" << n << ") was called - return value = " << (*this)[n] << std::endl;
#endif
    return (*this)[n];
  }
  // specialization for size_type:
  template <>
  inline unsigned long PlainArray<size_type>::keysize() const {
#ifdef HASH_DEBUG
    std::cerr << "PlainArray<size_type>::keysize() was called - return value = " << size() << std::endl;
#endif
    return size();
  }
  template <>
  inline unsigned long PlainArray<size_type>::key(const unsigned long n) const {
#ifdef HASH_DEBUG
    std::cerr << "PlainArray<size_type>::key(" << n << ") was called - return value = " << (*this)[n] << std::endl;
#endif
    return (*this)[n];
  }

  template <class T>
  inline const bool PlainArray<T>::empty() const {
    return (this->_size == 0);
  }

  template <class T>
  inline const size_type PlainArray<T>::size() const {
    return _size;
  }
  template <class T>
  inline const T& PlainArray<T>::operator[](const size_type index) const {
#ifdef INDEX_CHECK
    assert(index < _size);
#endif
    return *_data[index];
  }
  template <class T>
  inline T& PlainArray<T>::operator[](const size_type index) {
#ifdef INDEX_CHECK
    assert(index < _size);
#endif
    return *_data[index];
  }
  template <class T>
  inline void PlainArray<T>::clear() {
    this->resize(0);
  }
  template <class T>
  inline PlainArray<T>& PlainArray<T>::push_back(const T& init_obj, const size_type num) {
    if (num == 0) {
      return *this;
    }
    size_type new_size = _size + num;
    _resize(_get_new_memsize(new_size));
    for (size_type i = _size; i < new_size; ++i) {
      dataptr_allocator.construct(&_data[i], dataptr_type(init_obj));
    }
    _size = new_size;
    return *this;
  }
  template <class T>
  inline PlainArray<T>& PlainArray<T>::push_back(const PlainArray& plainarray) {
    if (plainarray._size == 0) {
      return *this;
    }
    size_type new_size = _size + plainarray._size;
    _resize(_get_new_memsize(new_size));
    for (size_type i = _size; i < new_size; ++i) {
      dataptr_allocator.construct(&_data[i], plainarray._data[i - _size]);
    }
    _size = new_size;
    return *this;
  }
  template <class T>
  inline bool PlainArray<T>::operator==(const PlainArray<T>& a) const {
    if (size() != a.size()) {
      return false;
    }
    for (size_type i = 0; i < size(); ++i) {
      if ((*this)[i] != a[i]) {
	return false;
      }
    }
    return true;
  }
  template <class T>
  inline bool PlainArray<T>::operator!=(const PlainArray<T>& a) const {
    return (!(*this == a));
  }
  template <class T>
  inline bool PlainArray<T>::lex_compare(const PlainArray<T>& a, const size_type start) const {
    if (start == a.size()) {
      return false;
    }
    else if (start == size()) {
      return true;
    }
    else if ((*this)[start] > a[start]) {
      return false;
    }
    else if ((*this)[start] < a[start]) {
      return true;
    }
    else {
      return lex_compare(a, start + 1);
    }
  }
  template <class T>
  inline bool PlainArray<T>::operator<(const PlainArray<T>& a) const {
    //   return lex_compare(a);
    const size_type minsize = size() < a.size() ? size() : a.size();
    for (size_type i = 0; i < minsize; ++i) {
      if ((*this)[i] < a[i]) {
	return true;
      }
      else if (a[i] < (*this)[i]) {
	return false;
      }
    }
    if (size() < a.size()) {
      return true;
    }
    else {
      return false;
    }
  }
  template <class T>
  inline bool PlainArray<T>::operator>(const PlainArray<T>& a) const {
    return (a < *this);
  }

  // iterator:
  template <class T>
  inline typename PlainArray<T>::const_iterator PlainArray<T>::begin() const {
    return __pa_const_iterator<T>(*this);
  }

  template <class T>
  inline typename PlainArray<T>::iterator       PlainArray<T>::begin() {
    return __pa_iterator<T>(*this);
  }

  template <class T>
  inline typename PlainArray<T>::const_iterator PlainArray<T>::end() const {
    return __pa_const_iterator<T>(*this, 0);
  }

  template <class T>
  inline typename PlainArray<T>::iterator       PlainArray<T>::end() {
    return __pa_iterator<T>(*this, 0);
  }

  // iostream:
  template <class T>
  std::istream& PlainArray<T>::read(std::istream& ist) {
    char c;
    T elem;

    resize(0);
    ist >> std::ws >> c;
    if (c == start_char) {
      while (ist >> std::ws >> c) {
	if (c == end_char) {
	  break;
	}
	if (c == delim_char) {
	  continue;
	}
	ist.putback(c);
	if (ist >> elem) {
	  push_back(elem);
	}
	else {
#ifdef READ_DEBUG
	  std::cerr << "PlainArray<T>::read(std::istream& ist):"
		    << c << " not of appropriate type." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "PlainArray<T>::read(std::istream& ist):"
		<< "missing `" << start_char << "'." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }
  template <class T>
  inline std::ostream& PlainArray<T>::write(std::ostream& ost) const {
    ost << start_char;
    if (_size > 0) {
      for (size_type i = 0; i < _size - 1; ++i) {
	ost << (*this)[i] << delim_char;
      }
      if (_size > 0) {
	ost << (*this)[_size - 1];
      }
    }
    ost << end_char;
    return ost;
  }

  template<class T>
  inline std::istream& operator>>(std::istream& ist, PlainArray<T>& pa) {
    return pa.read(ist);
  }

  template<class T>
  inline std::ostream& operator<<(std::ostream& ost, const PlainArray<T>& pa) {
    return pa.write(ost);
  }

  template<class T>
  const char PlainArray<T>::start_char = '[';
  template<class T>
  const char PlainArray<T>::end_char   = ']';
  template<class T>
  const char PlainArray<T>::delim_char = ',';

  template<class T>
  T PlainArray<T>::default_obj;

  // Hash Objects:
  template <class T>
  HashKeySize<typename PlainArray<T>::data_type> PlainArray<T>::_hashkeysize;

  template <class T>
  HashKey<typename PlainArray<T>::data_type>     PlainArray<T>::_hashkey;

  // class iterator:
  // constructors:
  template <class T>
  inline __pa_iterator<T>::__pa_iterator(PlainArray<T>& pa) :
    _container(&pa),
    _current_index(0) {
  }

  template <class T>
  inline __pa_iterator<T>::__pa_iterator(PlainArray<T>& pa, int) :
    _container(&pa),
    _current_index(_container->_size) {
  }

  template <class T>
  inline __pa_iterator<T>::__pa_iterator(PlainArray<T>& pa, size_type start_index) :
    _container(&pa),
    _current_index(start_index) {
    if (_current_index > _container->_size ) {
      _current_index = _container->_size;
    }
  }

  template <class T>
  inline __pa_iterator<T>::__pa_iterator(const __pa_iterator<T>& iter) :
    _container(iter._container),
    _current_index(iter._current_index) {
  }

  // destructor:
  template <class T>
  inline __pa_iterator<T>::~__pa_iterator() {}

  // assignment:
  template <class T>
  inline __pa_iterator<T>& __pa_iterator<T>::operator=(const __pa_iterator& iter) {
    if (this == &iter) {
      return *this;
    }
    _container = iter._container;
    _current_index = iter._current_index;
    return *this;
  }

  // accessors:
  template <class T>
  inline PlainArray<T>* __pa_iterator<T>::container() const {
    return _container;
  }

  template <class T>
  inline size_type      __pa_iterator<T>::current_index() const {
    return _current_index;
  }

  // functions:
  template <class T>
  inline bool __pa_iterator<T>::operator==(const __pa_iterator<T>& iter) const {
    return (_current_index == iter._current_index);
  }

  template <class T>
  inline bool __pa_iterator<T>::operator!=(const __pa_iterator<T>& iter) const {
    return (!operator==(iter));
  }

  template <class T>
  inline T& __pa_iterator<T>::operator*() {
    return _container->operator[](_current_index);
  }

  template <class T>
  inline T* __pa_iterator<T>::operator->() {
    return &_container->operator[](_current_index);
  }

  template <class T>
  inline __pa_iterator<T>& __pa_iterator<T>::operator++() {
    ++_current_index;
    return *this;
  }

  template <class T>
  inline __pa_iterator<T> __pa_iterator<T>::operator++(int) {
    __pa_iterator tmp(*this);
    ++(*this);
    return tmp;
  }

  // class const iterator:
  // constructors:
  template <class T>
  inline __pa_const_iterator<T>::__pa_const_iterator(const PlainArray<T>& pa) :
    _container(&pa),
    _current_index(0) {
  }

  template <class T>
  inline __pa_const_iterator<T>::__pa_const_iterator(const PlainArray<T>& pa, int) :
    _container(&pa),
    _current_index(_container->_size) {
  }

  template <class T>
  inline __pa_const_iterator<T>::__pa_const_iterator(const PlainArray<T>& pa, size_type start_index) :
    _container(&pa),
    _current_index(start_index) {
    if (_current_index > _container->_size ) {
      _current_index = _container->_size;
    }
  }

  template <class T>
  inline __pa_const_iterator<T>::__pa_const_iterator(const __pa_const_iterator<T>& iter) :
    _container(iter._container),
    _current_index(iter._current_index) {
  }

  template <class T>
  inline __pa_const_iterator<T>::__pa_const_iterator(const __pa_iterator<T>& iter) :
    _container(iter.container()),
    _current_index(iter.current_index()) {
  }

  // destructor:
  template <class T>
  inline __pa_const_iterator<T>::~__pa_const_iterator() {}

  // assignment:
  template <class T>
  inline __pa_const_iterator<T>& __pa_const_iterator<T>::operator=(const __pa_const_iterator& iter) {
    if (this == &iter) {
      return *this;
    }
    _container = iter._container;
    _current_index = iter._current_index;
    return *this;
  }

  // accessors:
  template <class T>
  inline PlainArray<T>* __pa_const_iterator<T>::container() const {
    return _container;
  }

  template <class T>
  inline size_type      __pa_const_iterator<T>::current_index() const {
    return _current_index;
  }

  // functions:
  template <class T>
  inline bool __pa_const_iterator<T>::operator==(const __pa_const_iterator& iter) const {
    return (_current_index == iter._current_index);
  }

  template <class T>
  inline bool __pa_const_iterator<T>::operator!=(const __pa_const_iterator& iter) const {
    return (!operator==(iter));
  }

  template <class T>
  inline const T& __pa_const_iterator<T>::operator*() const {
    return _container->operator[](_current_index);
  }

  template <class T>
  inline const T* __pa_const_iterator<T>::operator->() const {
    return &_container->operator[](_current_index);
  }

  template <class T>
  inline __pa_const_iterator<T>& __pa_const_iterator<T>::operator++() {
    ++_current_index;
    return *this;
  }

  template <class T>
  inline __pa_const_iterator<T> __pa_const_iterator<T>::operator++(int) {
    __pa_const_iterator<T> tmp(*this);
    ++(*this);
    return tmp;
  }

}; // namespace topcom

#endif

// eof PlainArray.hh
