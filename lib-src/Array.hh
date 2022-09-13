////////////////////////////////////////////////////////////////////////////////
// 
// Array.hh 
//
//    produced: 09/02/98 jr
// last change: 09/02/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef ARRAY_HH
#define ARRAY_HH

#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <assert.h>
#include <limits>

#include "RefCount.hh"
#include "PlainArray.hh"

namespace topcom {
  
  template <class T>
  class Array;

  template <class T>
  std::istream& operator>>(std::istream& ist, Array<T>& a);

  template <class T>
  std::ostream& operator<<(std::ostream& ost, const Array<T>& a);

  template <class T>
  class Array {
  public:
    typedef SmartPtr< PlainArray<T> >              dataptr_type;
    typedef typename PlainArray<T>::iterator       iterator;
    typedef typename PlainArray<T>::const_iterator const_iterator;
  private:
    static const PlainArray<T> default_data;
  private:
    dataptr_type   _data;
  public:
    inline const size_type resize(const size_type);
    inline const size_type resize(const size_type, const T&);
  public:
    // constructors:
    inline Array();
    inline Array(const Array&, size_type min_index = 0UL, size_type max_index = ULONG_MAX);
    inline Array(const PlainArray<T>&);
    inline Array(const size_type);
    inline Array(const size_type, const T&);
    // destructor:
    inline ~Array();
    // assignment:
    inline Array& operator=(const Array&);
    // keys for containers:
    inline unsigned long keysize() const;
    inline unsigned long key(const unsigned long) const;
    // accessors:
    inline const refcount_type refcount() const;
    inline const size_type size() const;
    inline const size_type memsize() const;
    // indexing:
    inline const T& operator[](const size_type) const;
    inline T& operator[](const size_type);
    // operations:
    inline Array& push_back(const T&, const size_type = 1);
    inline Array& push_back(const Array&);
    // boolean expressions:
    inline bool operator==(const Array&) const;
    inline bool operator!=(const Array&) const;
    inline bool lex_compare(const Array&, const size_type = 0) const;
    inline bool operator<(const Array&) const;
    inline bool operator>(const Array&) const;
    // iterator:
    inline const_iterator begin() const;
    inline iterator       begin();
    inline const_iterator end() const;
    inline iterator       end();
    // iostream:
    inline std::istream& read(std::istream&);
    inline std::ostream& write(std::ostream&) const;
    friend std::istream& operator>><>(std::istream& ist, Array<T>& a);
    friend std::ostream& operator<<<>(std::ostream& ost, const Array<T>& a);
  };

  template <class T>
  inline const size_type Array<T>::resize(const size_type new_size) {
    return _data->resize(new_size);
  }
  template <class T>
  inline const size_type Array<T>::resize(const size_type new_size, const T& init_obj) {
    return _data->resize(new_size, init_obj);
  }
  // constructors:
  template <class T>
  inline Array<T>::Array() : 
    _data(default_data) {
  }
  template <class T>
  inline Array<T>::Array(const Array& array, size_type min_index, size_type max_index) : 
    _data(dataptr_type(PlainArray<T>(*(array._data), min_index, max_index))) {
  }
  template <class T>
  inline Array<T>::Array(const PlainArray<T>& plainarray) : 
    _data(plainarray) {
  }
  template <class T>
  inline Array<T>::Array(const size_type init_size) :
    _data(dataptr_type(PlainArray<T>(init_size))) {
  }
  template <class T>
  inline Array<T>::Array(const size_type init_size, const T& obj) :
    _data(dataptr_type(PlainArray<T>(init_size, obj))) {
  }
  // destructor:
  template <class T>
  inline Array<T>::~Array() {
  }
  // assignment:
  template <class T>
  inline Array<T>& Array<T>::operator=(const Array<T>& array) {
    if (this == &array) {
      return *this;
    }
    _data = array._data;
    return *this;
  }
  // accessors:
  template <class T>
  inline const refcount_type Array<T>::refcount() const {
    return _data._ptr->refcount(); 
  }

  template <class T>
  inline const size_type Array<T>::size() const {
    return _data->_size;
  }

  template <class T>
  inline const size_type Array<T>::memsize() const {
    return _data->_memsize;
  }
  // keys for containers:
  // for general types providing key() and keysize():
  template <class T>
  inline unsigned long Array<T>::keysize() const {
    return _data->keysize();
  }
  template <class T>
  inline unsigned long Array<T>::key(const unsigned long n) const {
    return _data->key(n);
  }
  // specialization for int:
  template <>
  inline unsigned long Array<int>::keysize() const {
    return _data->keysize();
  }
  template <>
  inline unsigned long Array<int>::key(const unsigned long n) const {
    return _data->key(n);
  }
  // specialization for size_type:
  template <>
  inline unsigned long Array<size_type>::keysize() const {
    return _data->keysize();
  }
  template <>
  inline unsigned long Array<size_type>::key(const unsigned long n) const {
    return _data->key(n);
  }
  // indexing:
  template <class T>
  inline const T& Array<T>::operator[](const size_type index) const {
    return (*_data)[index];
  }
  template <class T>
  inline T& Array<T>::operator[](const size_type index) {
    return (*_data)[index];
  }
  // operations:
  template <class T>
  inline Array<T>& Array<T>::push_back(const T& obj, const size_type no) {
    _data->push_back(obj, no);
    return *this;
  }
  template <class T>
  inline Array<T>& Array<T>::push_back(const Array& array) {
    _data->push_back(*array._data);
    return *this;
  }
  template <class T>
  inline bool Array<T>::operator==(const Array<T>& a) const {
    return (*_data == *a._data);
  }
  template <class T>
  inline bool Array<T>::operator!=(const Array<T>& a) const {
    return (!(*this == a));
  }
  template <class T>
  inline bool Array<T>::lex_compare(const Array<T>& a, const size_type start) const {
    return _data->lex_compare(a._data, start);
  }
  template <class T>
  inline bool Array<T>::operator<(const Array<T>& a) const {
    return _data->lex_compare(a._data);
  }
  template <class T>
  inline bool Array<T>::operator>(const Array<T>& a) const {
    return (a < *this);
  }
  // iterator:
  template <class T>
  inline typename Array<T>::const_iterator Array<T>::begin() const {
    return _data->begin();
  }
  template <class T>
  inline typename Array<T>::iterator Array<T>::begin() {
    return _data->begin();
  }
  template <class T>
  inline typename Array<T>::const_iterator Array<T>::end() const {
    return _data->end();
  }
  template <class T>
  inline typename Array<T>::iterator Array<T>::end() {
    return _data->end();
  }
  // iostream:
  template <class T>
  inline std::istream& Array<T>::read(std::istream& ist) {
    return ist >> *_data;
  }
  template <class T>
  inline std::ostream& Array<T>::write(std::ostream& ost) const {
    return ost << *_data;
  }

  template<class T>
  const PlainArray<T> Array<T>::default_data;

  // friends:
  template <class T>
  inline std::istream& operator>>(std::istream& ist, Array<T>& a) {
    return a.read(ist);
  }

  template <class T>
  inline std::ostream& operator<<(std::ostream& ost, const Array<T>& a) {
    return a.write(ost);
  }

};

#endif

// eof Array.hh
