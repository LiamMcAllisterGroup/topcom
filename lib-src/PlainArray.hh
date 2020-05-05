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

#include <memory>

#include "Global.hh"
#include "RefCount.hh"
#include "HashKey.hh"

template <class T>
class PlainArray;

template <class T>
std::istream& operator>>(std::istream& ist, PlainArray<T>& a);

template <class T>
std::ostream& operator<<(std::ostream& ost, const PlainArray<T>& a);

template <class T>
class Array;

template <class T>
class PlainArray {
  friend class Array<T>;
private:
  enum { base_size = 0 };
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
  size_type               _size;
  size_type               _maxindex;
private:
  inline const size_type _get_new_size(const size_type);
  inline const size_type _resize(const size_type);
public:
  inline const size_type resize(const size_type, const T& = default_obj);
  // constructors:
  inline PlainArray();
  inline PlainArray(const PlainArray&);
  inline PlainArray(const size_type, const T& = default_obj);
  // destructor:
  inline ~PlainArray();
  // assignment:
  inline PlainArray& operator=(const PlainArray&);
  // keys for containers:
  inline unsigned long keysize() const;
  inline unsigned long key(const unsigned long) const;
  // accessors:
  inline const size_type maxindex() const;
  // indexing:
  inline const T& operator[](const size_type) const;
  inline T& operator[](const size_type);
  // operations:
  inline PlainArray<T>& append(const T&, const size_type = 1);
  inline PlainArray<T>& append(const PlainArray&);
  // boolean expressions:
  inline bool operator==(const PlainArray&) const;
  inline bool operator!=(const PlainArray&) const;
  inline bool lex_compare(const PlainArray&, const size_type = 0) const;
  inline bool operator<(const PlainArray&) const;
  inline bool operator>(const PlainArray&) const;
  // iostream:
  inline std::istream& read(std::istream&);
  inline std::ostream& write(std::ostream&) const;
  // friends (have to be inlined right here):
  friend std::istream& operator>><>(std::istream& ist, PlainArray<T>& a);
  friend std::ostream& operator<<<>(std::ostream& ost, const PlainArray<T>& a);
};

template <class T>
inline const size_type PlainArray<T>::_get_new_size(const size_type new_maxindex) {
  size_type new_size = _size;
  if (new_size == 0) {
    ++new_size;
  }
  while (4 * new_maxindex < new_size) {
    new_size /= 2;
  }
  while (new_maxindex > new_size) {
    new_size *= 2;
  }
  if (new_size < base_size) {
    new_size = base_size;
  }
  return new_size;
}
template <class T>
inline const size_type PlainArray<T>::_resize(const size_type new_size) {
#ifdef RESIZE_DEBUG
  std::cout << "PlainArray<T>::_resize("<< new_size << ')' << std::endl;
#endif
  if (_size == new_size) {
    return _size;
  }
  dataptr_type* new_data = dataptr_allocator.allocate(new_size);
  const size_type min_index = _maxindex < new_size ? _maxindex : new_size;
  for (size_type i = 0; i < min_index; ++i) {
    dataptr_allocator.construct(&new_data[i], _data[i]);
  }
  if (_data) {
    for (size_type i = 0; i < _maxindex; ++i) {
      dataptr_allocator.destroy(&_data[i]);
    }
    dataptr_allocator.deallocate(_data, _size);
  }
  _data = new_data;
  _size = new_size;
  return _size;
}
template <class T>
inline const size_type PlainArray<T>::resize(const size_type new_maxindex, const T& init_obj) {
  _resize(_get_new_size(new_maxindex));
  for (size_type i = _maxindex; i < new_maxindex; ++i) {
    dataptr_allocator.construct(&_data[i], dataptr_type(init_obj));
  }
  _maxindex = new_maxindex;  
  return _maxindex;
}
template <class T>
inline PlainArray<T>::PlainArray() : 
  _data(0), _size(0), _maxindex(0) {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "PlainArray::PlainArray()" << std::endl;
#endif
}
template <class T>
inline PlainArray<T>::PlainArray(const PlainArray& plainarray) : 
  _size(plainarray._size), _maxindex(plainarray._maxindex) {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "PlainArray::PlainArray(const PlainArray&)" << std::endl;
#endif
  _data = dataptr_allocator.allocate(_size);
  for (size_type index = 0; index < _maxindex; ++index) {
    dataptr_allocator.construct(&_data[index], plainarray._data[index]);
  }
}
template <class T>
inline PlainArray<T>::PlainArray(const size_type init_size, const T& obj) : 
  _size(init_size), _maxindex(init_size) {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "PlainArray::PlainArray(const size_type, const T&)" << std::endl;
#endif
  _data = dataptr_allocator.allocate(_size);
  for (size_type index = 0; index < _size; ++index) {
    dataptr_allocator.construct(&_data[index], dataptr_type(obj));
  }
}
template <class T>
inline PlainArray<T>::~PlainArray() {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "PlainArray::~PlainArray()" << std::endl;
#endif
  if (_data){
    for (size_type i = 0; i < _maxindex; ++i) {
      dataptr_allocator.destroy(&_data[i]);
    }
    dataptr_allocator.deallocate(_data, _size);
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
  size_type new_maxindex = plainarray._maxindex;
  _resize(_get_new_size(new_maxindex));
  const size_type min_index = _maxindex < new_maxindex ? _maxindex : new_maxindex;
  for (size_type i = 0; i < min_index; ++i) {
    _data[i] = plainarray._data[i];
  }
  for (size_type i = min_index; i < new_maxindex; ++i) {
    dataptr_allocator.construct(&_data[i], plainarray._data[i]);
  }
  _maxindex = new_maxindex;
  return *this;
}
// keys for containers:
// for general types providing key() and keysize():
template <class T>
inline unsigned long PlainArray<T>::keysize() const {
  unsigned long res = 0;
  for (unsigned long i = 0; i < maxindex(); ++i) {
    res += _hashkeysize((*this)[i]);
  }
  return res;
}
template <class T>
inline unsigned long PlainArray<T>::key(const unsigned long n) const {
  unsigned long block_no = n + 1;
  unsigned long i = 0;
  while ((block_no > 0) && (i < maxindex())) {
    if (block_no > _hashkeysize((*this)[i])) {
      block_no -= _hashkeysize((*this)[i++]);
    }
    else {
      return _hashkey((*this)[i], block_no - 1);
    }
  }
#ifdef DEBUG
  std::cerr << "key(const unsigned long): no valid key found for "
       << *this << " at " << n << std::endl;

#endif
  return 0;
}
// specialization for int:
template <>
inline unsigned long PlainArray<int>::keysize() const {
  return maxindex();
}
template <>
inline unsigned long PlainArray<int>::key(const unsigned long n) const {
  return (*this)[n];
}
// specialization for size_type:
template <>
inline unsigned long PlainArray<size_type>::keysize() const {
  return maxindex();
}
template <>
inline unsigned long PlainArray<size_type>::key(const unsigned long n) const {
  return (*this)[n];
}
template <class T>
inline const size_type PlainArray<T>::maxindex() const {
  return _maxindex;
}
template <class T>
inline const T& PlainArray<T>::operator[](const size_type index) const {
#ifdef INDEX_CHECK
  assert(index < _maxindex);
#endif
  return *_data[index];
}
template <class T>
inline T& PlainArray<T>::operator[](const size_type index) {
#ifdef INDEX_CHECK
  assert(index < _maxindex);
#endif
  return *_data[index];
}
template <class T>
inline PlainArray<T>& PlainArray<T>::append(const T& init_obj, const size_type num) {
  if (num == 0) {
    return *this;
  }
  size_type new_maxindex = _maxindex + num;
  _resize(_get_new_size(new_maxindex));
  for (size_type i = _maxindex; i < new_maxindex; ++i) {
    dataptr_allocator.construct(&_data[i], dataptr_type(init_obj));
  }
  _maxindex = new_maxindex;
  return *this;
}
template <class T>
inline PlainArray<T>& PlainArray<T>::append(const PlainArray& plainarray) {
  if (plainarray._maxindex == 0) {
    return *this;
  }
  size_type new_maxindex = _maxindex + plainarray._maxindex;
  _resize(_get_new_size(new_maxindex));
  for (size_type i = _maxindex; i < new_maxindex; ++i) {
    dataptr_allocator.construct(&_data[i], plainarray._data[i - _maxindex]);
  }
  _maxindex = new_maxindex;
  return *this;
}
template <class T>
inline bool PlainArray<T>::operator==(const PlainArray<T>& a) const {
  if (maxindex() != a.maxindex()) {
    return false;
  }
  for (size_type i = 0; i < maxindex(); ++i) {
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
  if (start == a.maxindex()) {
    return false;
  }
  else if (start == maxindex()) {
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
  const size_type minmaxindex = maxindex() < a.maxindex() ? maxindex() : a.maxindex();
  for (size_type i = 0; i < minmaxindex; ++i) {
    if ((*this)[i] < a[i]) {
      return true;
    }
    else if (a[i] < (*this)[i]) {
      return false;
    }
  }
  if (maxindex() < a.maxindex()) {
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
	append(elem);
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
  if (_maxindex > 0) {
    for (size_type i = 0; i < _maxindex - 1; ++i) {
      ost << (*this)[i] << delim_char;
    }
    if (_maxindex > 0) {
      ost << (*this)[_maxindex - 1];
    }
  }
  ost << end_char;
  return ost;
}

template<class T>
inline std::istream& operator>>(std::istream& ist, PlainArray<T>& a) {
  return a.read(ist);
}

template<class T>
inline std::ostream& operator<<(std::ostream& ost, const PlainArray<T>& a) {
  return a.write(ost);
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

#endif

// eof PlainArray.hh
