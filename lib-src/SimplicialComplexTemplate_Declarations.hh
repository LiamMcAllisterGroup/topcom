////////////////////////////////////////////////////////////////////////////////
// 
// SimplicialComplexTemplate_Declarations.hh
//
//    produced: 12/06/98/98 jr
// last change: 12/06/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef SIMPLICIALCOMPLEXTEMPLATE_DECLARATIONS_HH
#define SIMPLICIALCOMPLEXTEMPLATE_DECLARATIONS_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>

#include <memory>

#include "IntegerSet.hh"
#include "PlainArray.hh"
#include "Array.hh"
#include "PlainHashIndexTable.hh"
#include "IndexTableArray.hh"

typedef IntegerSet Simplex;

template<class T>
class SimplicialComplexTemplate;

template<class T>
inline std::istream& operator>>(std::istream& ist, SimplicialComplexTemplate<T>& st);

template<class T>
inline std::ostream& operator<<(std::ostream& ost, const SimplicialComplexTemplate<T>& st);

template<class T>
class __sc_const_iterator;

template<class T>
class SimplicialComplexTemplate : public IndexTableArray {
  typedef typename T::iterator         IndexSet_iterator;
  typedef typename T::const_iterator   IndexSet_const_iterator;
public:
  typedef T                            IndexSet;
public:
  friend class VertexFacetTable;
  friend class FaceFacetTableArray;
  friend class FreeFaceTableArray;
public:
  enum { init_size = 0 };
  enum { init_mincard = 0, init_maxcard = 0 };
private:
  typedef Array<T> _IndexSetArray;
public:
  class IndexSetArray : public _IndexSetArray {
  };
  friend class IndexSetArray;
  friend class __sc_const_iterator<T>;
public:
  typedef __sc_const_iterator<T>  const_iterator;
  typedef const_iterator          iterator; // no non-const iterators
private:
  size_type                       _mincard;
  size_type                       _maxcard;
  IndexSetArray                   _index_set; // index set for each card.
private:
  void             minmaxcard_update();
  inline size_type resize(const size_type new_size);
public:
  // constructors:
  inline SimplicialComplexTemplate();
  inline SimplicialComplexTemplate(const SimplicialComplexTemplate&);
  inline SimplicialComplexTemplate(const Simplex&, const size_type);
  inline SimplicialComplexTemplate(const Simplex&);
public:
  // destructor:
  inline ~SimplicialComplexTemplate();

  // iterator functions:
  inline const_iterator begin() const;
  inline const_iterator end()   const;
 
  // keys for a container:
  const size_type keysize() const;
  const size_type key(const size_type n) const;

  // accessors:
  const size_type        mincard()     const { return _mincard; }
  const size_type        maxcard()     const { return _maxcard; }
  const IndexSetArray&   index_set()   const { return _index_set; }
  const IndexTableArray& index_table() const { return _index_table; }

  const IndexSet&   index_set(const size_type n)   const { return _index_set[n]; }
  const IndexTable& index_table(const size_type n) const { return _index_table[n]; }

  // status information:
  const bool is_empty() const { return (_maxcard == 0); }
  const long dim()      const { return (long)_maxcard - 2; }

  inline const size_type card(const size_type n) const;
  const size_type        card() const;
    
  const bool        contains(const Simplex& simp, const size_type card) const;
  inline const bool contains(const Simplex& simp) const;
  const bool        superset(const SimplicialComplexTemplate& sc) const;
  inline const bool subset(const SimplicialComplexTemplate& sc) const;
  const bool        disjoint(const SimplicialComplexTemplate& sc) const;

  const bool        contains_face(const Simplex& simp, const size_type card) const;
  inline const bool contains_face(const Simplex& simp) const;
  const bool        contains_face(const Simplex& simp,
				  const size_type card, 
				  Simplex& facet) const;
  inline const bool contains_face(const Simplex& simp, Simplex& facet) const;

  const bool        contains_free_face(const Simplex& simp,
				       const size_type card,
				       Simplex& facet) const;
  inline const bool contains_free_face(const Simplex& simp,
				       Simplex& facet) const;
  // standard operations on simplicial complexes:
  const Simplex                     support() const;
  SimplicialComplexTemplate         star(const Simplex& simp, const size_type card) const;
  inline SimplicialComplexTemplate  star(const Simplex& simp) const;
  SimplicialComplexTemplate         link(const Simplex& simp, const size_type card) const;
  inline SimplicialComplexTemplate  link(const Simplex& simp) const;
  SimplicialComplexTemplate&        deletion(const Simplex& simp, const size_type card);
  inline SimplicialComplexTemplate& deletion(const Simplex& simp);
  
  // extract all simplices with given cardinality:
  const SimplicialComplexTemplate operator[](const size_type) const;

  // skeleton:
  inline SimplicialComplexTemplate skeleton(const size_type);

  // boolean operators:
  const bool        operator==(const SimplicialComplexTemplate&) const;
  inline const bool operator!=(const SimplicialComplexTemplate&) const;
  const bool        operator<(const SimplicialComplexTemplate&)  const;
  inline const bool operator>(const SimplicialComplexTemplate&)  const;
  
  // modifiers:
  inline SimplicialComplexTemplate& clear();   // erase all

  SimplicialComplexTemplate&        insert_boundary(const Simplex&, const size_type);
  inline SimplicialComplexTemplate& insert_boundary(const Simplex&);
  SimplicialComplexTemplate&        insert_faces(const SimplicialComplexTemplate&, const size_type);
  SimplicialComplexTemplate&       include_all_faces(const size_type = 1);
  inline SimplicialComplexTemplate faces(const size_type = 1);
  SimplicialComplexTemplate&       reduce_to_facets(); // only keep facets
  
  // assignment:
  SimplicialComplexTemplate& operator=(const SimplicialComplexTemplate&);
  
  // adding and deleting a simplex:
  inline SimplicialComplexTemplate& operator+=(const Simplex&);
  inline SimplicialComplexTemplate& insert(const Simplex&);
  SimplicialComplexTemplate&        insert(const Simplex&, const size_type);
  inline SimplicialComplexTemplate& operator-=(const Simplex&);
  inline SimplicialComplexTemplate& erase(const Simplex&);
  SimplicialComplexTemplate&        erase(const Simplex&, const size_type);
  inline SimplicialComplexTemplate& operator^=(const Simplex&);
  inline SimplicialComplexTemplate& exclusiveor(const Simplex&);
  SimplicialComplexTemplate&        exclusiveor(const Simplex&, const size_type);
  
  // union, difference, and intersection with simplicial complexes:
  SimplicialComplexTemplate& operator+=(const SimplicialComplexTemplate&);
  SimplicialComplexTemplate& operator-=(const SimplicialComplexTemplate&);
  SimplicialComplexTemplate& operator*=(const SimplicialComplexTemplate&);
  SimplicialComplexTemplate& operator^=(const SimplicialComplexTemplate&);
  
  // the same but a new set is returned:
  inline SimplicialComplexTemplate operator+(const Simplex&) const;
  inline SimplicialComplexTemplate insert(const Simplex&)    const;
  inline SimplicialComplexTemplate insert(const Simplex&,
					  const size_type)   const;
  inline SimplicialComplexTemplate operator-(const Simplex&) const;
  inline SimplicialComplexTemplate erase(const Simplex&)     const;
  inline SimplicialComplexTemplate erase(const Simplex&,
					 const size_type)    const;
  inline SimplicialComplexTemplate operator^(const Simplex&) const;
  inline SimplicialComplexTemplate exclusiveor(const Simplex&)       const;
  inline SimplicialComplexTemplate exclusiveor(const Simplex&,
				       const size_type)      const;
  inline SimplicialComplexTemplate operator+(const SimplicialComplexTemplate&) const;
  inline SimplicialComplexTemplate operator-(const SimplicialComplexTemplate&) const;
  inline SimplicialComplexTemplate operator*(const SimplicialComplexTemplate&) const;
  inline SimplicialComplexTemplate operator^(const SimplicialComplexTemplate&) const;
  
  // join:
  SimplicialComplexTemplate        join(const Simplex&, const size_type)  const;
  inline SimplicialComplexTemplate join(const Simplex&)                   const;
  SimplicialComplexTemplate        join(const SimplicialComplexTemplate&) const;
  
  // iostream:
  std::istream& read(std::istream&);
  std::ostream& write(std::ostream&) const;
  friend std::istream& operator>><>(std::istream& ist, SimplicialComplexTemplate& st);
  friend std::ostream& operator<<<>(std::ostream& ost, const SimplicialComplexTemplate& st);
};

template<class T>
class __sc_const_iterator {
  typedef typename T::iterator         IndexSet_iterator;
  typedef typename T::const_iterator   IndexSet_const_iterator;
public:
  std::allocator<IndexSet_const_iterator> iter_allocator;
private:
  const SimplicialComplexTemplate<T>*  _container;
  size_type                            _current_card;
  IndexSet_const_iterator*             _current_indexset_iter;
private:
  __sc_const_iterator();
public: 
  __sc_const_iterator(const SimplicialComplexTemplate<T>& s);
  inline __sc_const_iterator(const SimplicialComplexTemplate<T>& s, int);
  __sc_const_iterator(const __sc_const_iterator& iter);
  inline ~__sc_const_iterator();
  inline __sc_const_iterator operator=(const __sc_const_iterator& iter);
  
  const bool operator==(const __sc_const_iterator& iter) const;
  inline const bool operator!=(const __sc_const_iterator& iter) const;
  
  inline const Simplex& operator*() const;
  inline const Simplex* operator->() const;
  __sc_const_iterator& operator++();
  inline __sc_const_iterator operator++(int);
};

// ==============================================================
// inlines for class SimplicialComplexTemplate<T>::const_iterator
// ==============================================================

template<class T>
inline __sc_const_iterator<T>::__sc_const_iterator(const SimplicialComplexTemplate<T>& s, int) :
  _container(&s),
  _current_card(_container->_maxcard),
  _current_indexset_iter(NULL) {}

template<class T>
inline __sc_const_iterator<T>::~__sc_const_iterator() {
  if (_current_indexset_iter) {
    iter_allocator.destroy(_current_indexset_iter);
    iter_allocator.deallocate(_current_indexset_iter, 1);
  }
}

template<class T>
inline const bool __sc_const_iterator<T>::operator!=(const __sc_const_iterator& iter) const {
  return !((*this) == iter);
}

template<class T>
inline const Simplex& __sc_const_iterator<T>::operator*() const {
  return _container->_index_table[_current_card].get_obj(**_current_indexset_iter);
}

template<class T>
inline const Simplex* __sc_const_iterator<T>::operator->() const {
  return &_container->_index_table[_current_card].get_obj(**_current_indexset_iter);
}

template<class T>
inline __sc_const_iterator<T> __sc_const_iterator<T>::operator++(int) {
  __sc_const_iterator tmp(*this);
  ++(*this);
  return tmp;
}

// ===============================================
// inlines for class SimplicialComplexTemplate<T>:
// ===============================================

// resize:
template<class T>
inline size_type SimplicialComplexTemplate<T>::resize(const size_type new_size) {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "SimplicialComplexTemplate<T>::resize(const size_type)" << std::endl;
#endif
  _index_table.resize(new_size);
  _index_set.resize(new_size);
  return new_size;
}

// constructors:
template<class T>
inline SimplicialComplexTemplate<T>::SimplicialComplexTemplate() : _mincard(0), _maxcard(0), _index_set() {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "SimplicialComplexTemplate<T>::SimplicialComplexTemplate()" 
       << std::endl;
#endif
}

template<class T>
inline SimplicialComplexTemplate<T>::SimplicialComplexTemplate(const SimplicialComplexTemplate<T>& s) :
  _mincard(s._mincard), _maxcard(s._maxcard), _index_set(s._index_set) {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "SimplicialComplexTemplate<T>::SimplicialComplexTemplate(const SimplicialComplexTemplate<T>&)" 
       << std::endl;
#endif
}

template<class T>
inline SimplicialComplexTemplate<T>::SimplicialComplexTemplate(const Simplex& simp, const size_type card) : 
  _index_set() {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "SimplicialComplexTemplate<T>::SimplicialComplexTemplate"
       << "(const Simplex&, const size_type)" 
       << std::endl;
#endif
  _maxcard = resize(card + 1);
  _mincard = card;
  _index_set[card] += _index_table[card].get_index(simp);
}

template<class T>
inline SimplicialComplexTemplate<T>::SimplicialComplexTemplate(const Simplex& simp) : _index_set() {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "SimplicialComplexTemplate<T>::SimplicialComplexTemplate(const Simplex&)" << std::endl;
#endif
  size_type card = simp.card();
  _maxcard = resize(card + 1);
  _mincard = card;
  _index_set[card] += _index_table[card].get_index(simp);
}

// destructor:
template<class T>
inline SimplicialComplexTemplate<T>::~SimplicialComplexTemplate()	 {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "SimplicialComplexTemplate<T>::~SimplicialComplexTemplate()" 
       << std::endl;
#endif
}

// iterator functions:
template<class T>
inline typename SimplicialComplexTemplate<T>::const_iterator SimplicialComplexTemplate<T>::begin() const {
  return __sc_const_iterator<T>(*this);
}

template<class T>
inline typename SimplicialComplexTemplate<T>::const_iterator SimplicialComplexTemplate<T>::end() const {
  return __sc_const_iterator<T>(*this,0);
}

// status information:
template<class T>
inline const size_type SimplicialComplexTemplate<T>::card(const size_type n) const {
  return _index_set[n].card();
}

template<class T>
inline const bool SimplicialComplexTemplate<T>::contains(const Simplex& simp) const {
  size_type card = simp.card();
  return contains(simp, card);
}

template<class T>
inline const bool SimplicialComplexTemplate<T>::subset(const SimplicialComplexTemplate<T>& sc) const {
  return sc.superset(*this);
}

template<class T>
inline const bool SimplicialComplexTemplate<T>::contains_face(const Simplex& simp) const {
  const size_type card = simp.card();
  return contains_face(simp, card);
}

template<class T>
inline const bool SimplicialComplexTemplate<T>::contains_face(const Simplex& simp, 
							      Simplex& facet) const {
  const size_type card = simp.card();
  return contains_face(simp, card, facet);
}

template<class T>
inline const bool SimplicialComplexTemplate<T>::contains_free_face(const Simplex& simp,
								   Simplex& facet) const {
  const size_type card = simp.card();
  return contains_free_face(simp, card, facet);
}

// standard operations on simplicial complexes:

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::star(const Simplex& simp) const {
  size_type card = simp.card();
  return star(simp, card);
}
  
template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::link(const Simplex& simp) const {
  size_type card = simp.card();
  return link(simp, card);
}

template<class T>
inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::deletion(const Simplex& simp) {
  size_type card = simp.card();
  return deletion(simp, card);
}

// skeleton:
template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::skeleton(const size_type card) {
  SimplicialComplexTemplate<T> result(*this);
  return result.include_all_faces(card)[card];
}

// boolean operators:

template<class T>
inline const bool SimplicialComplexTemplate<T>::operator!=(const SimplicialComplexTemplate<T>& s) const {
  return !((*this) == s);
}

template<class T>
inline const bool SimplicialComplexTemplate<T>::operator>(const SimplicialComplexTemplate<T>& s) const {
  return (s < (*this));
}

template<class T>
inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::insert_boundary(const Simplex& simp) {
  return insert_boundary(simp, simp.card());
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::faces(const size_type depth) {
  SimplicialComplexTemplate<T> result(*this);
  return result.include_all_faces(depth);
}

// modifiers:
template<class T>
inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::clear() {
  _mincard = 0;
  _maxcard = resize(0);
  return *this;
}

// adding and deleting a simplex:
template<class T>
inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator+=(const Simplex& simp) {
  return insert(simp,simp.card());
}

template<class T>
inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::insert(const Simplex& simp) {
  return insert(simp,simp.card());  
}

template<class T>
inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator-=(const Simplex& simp) {
  return erase(simp,simp.card());
}

template<class T>
inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::erase(const Simplex& simp) {
  return erase(simp,simp.card());
}

template<class T>
inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator^=(const Simplex& simp) {
  return exclusiveor(simp,simp.card());
}

template<class T>
inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::exclusiveor(const Simplex& simp) {
  return exclusiveor(simp,simp.card());
}

// union, difference, and intersection with simplicial complexes, a new set is returned:

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator+(const Simplex& simp) const {
  SimplicialComplexTemplate<T> s(*this);
  s += simp;
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::insert(const Simplex& simp) const {
  SimplicialComplexTemplate<T> s(*this);
  s.insert(simp);
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::insert(const Simplex& simp,
									 const size_type card) const {
  SimplicialComplexTemplate<T> s(*this);
  s.insert(simp, card);
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator-(const Simplex& simp) const {
  SimplicialComplexTemplate<T> s(*this);
  s -= simp;
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::erase(const Simplex& simp) const {
  SimplicialComplexTemplate<T> s(*this);
  s.erase(simp);
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::erase(const Simplex& simp,
									const size_type card) const {
  SimplicialComplexTemplate<T> s(*this);
  s.erase(simp,card);
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator^(const Simplex& simp) const {
  SimplicialComplexTemplate<T> s(*this);
  s ^= simp;
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::exclusiveor(const Simplex& simp) const {
  SimplicialComplexTemplate<T> s(*this);
  s.exclusiveor(simp);
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::exclusiveor(const Simplex& simp,
								      const size_type card) const {
  SimplicialComplexTemplate<T> s(*this);
  s.exclusiveor(simp,card);
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator+(const SimplicialComplexTemplate<T>& s1) const {
  SimplicialComplexTemplate<T> s(*this);
  s += s1;
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator-(const SimplicialComplexTemplate<T>& s1) const {
  SimplicialComplexTemplate<T> s(*this);
  s -= s1;
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator*(const SimplicialComplexTemplate<T>& s1) const {
  SimplicialComplexTemplate<T> s(*this);
  s *= s1;
  return s;
}

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator^(const SimplicialComplexTemplate<T>& s1) const {
  SimplicialComplexTemplate<T> s(*this);
  s ^= s1;
  return s;
}

// join:

template<class T>
inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::join(const Simplex& simp) const {
  return join(simp, simp.card());
}

// friends:
template<class T>
inline std::istream& operator>>(std::istream& ist, SimplicialComplexTemplate<T>& st) {
  return st.read(ist);
}

template<class T>
inline std::ostream& operator<<(std::ostream& ost, const SimplicialComplexTemplate<T>& st) {
  return st.write(ost);
}

// the following is necessary on systems where explicit template instantiation
// does not work as advertized:

#include "SimplicialComplexTemplate_Definitions.hh"

#endif

// eof SimplicialComplexTemplate_Declarations.hh
