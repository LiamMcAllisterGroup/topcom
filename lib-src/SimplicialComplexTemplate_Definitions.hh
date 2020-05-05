////////////////////////////////////////////////////////////////////////////////
// 
// SimplicialComplexTemplate_Definitions.hh 
//
//    produced: 12/06/98 jr
// last change: 12/06/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef SIMPLICIALCOMPLEXTEMPLATE_DEFINITIONS_HH
#define SIMPLICIALCOMPLEXTEMPLATE_DEFINITIONS_HH

#include "SimplicialComplexTemplate_Declarations.hh"

// =============================================
// SimplicialComplexTemplate<T>::const_iterator:
// =============================================

template<class T>
__sc_const_iterator<T>::__sc_const_iterator(const __sc_const_iterator& iter) :
  _container(iter._container),
  _current_card(iter._current_card),
  _current_indexset_iter(NULL) {
  if (iter._current_indexset_iter) {
    _current_indexset_iter = iter_allocator.allocate(1);
    iter_allocator.construct(_current_indexset_iter, *iter._current_indexset_iter);
  }
  else {
    _current_card = _container->_maxcard;
  }
}

template<class T>
__sc_const_iterator<T>::__sc_const_iterator(const SimplicialComplexTemplate<T>& s) :
  _container(&s), 
  _current_card(_container->_mincard),
  _current_indexset_iter(NULL) {
  while (_current_card < _container->_maxcard) {
    if (!_container->_index_set[_current_card].is_empty()) {
      _current_indexset_iter = iter_allocator.allocate(1);
      iter_allocator.construct(_current_indexset_iter, _container->_index_set[_current_card].begin());
      return;
    }
    else {
      ++_current_card;
    }
  }
}

template<class T>
__sc_const_iterator<T> __sc_const_iterator<T>::operator=(const __sc_const_iterator& iter) {
  if (this == &iter) {
    return *this;
  }
  _container = iter._container;
  _current_card = iter._current_card;
  if (_current_indexset_iter) {
    iter_allocator.deallocate(_current_indexset_iter);
  }
  if (iter._current_indexset_iter) {
    _current_indexset_iter = iter_allocator.allocate(1);
    iter_allocator.construct(_current_indexset_iter, *iter._current_indexset_iter);
  }
  else {
    _current_card = _container->_maxcard;
  }
  return *this;     
}

template<class T>
const bool __sc_const_iterator<T>::operator==(const __sc_const_iterator& iter) const {
  if (_container != iter._container) {
    return false;
  }
  if (_current_indexset_iter == iter._current_indexset_iter) {
    return true;
  }
  if ((_current_indexset_iter == NULL) || (iter._current_indexset_iter == NULL)) {
    return false;
  }
  return (*_current_indexset_iter == *iter._current_indexset_iter);
}

template<class T>
__sc_const_iterator<T>& __sc_const_iterator<T>::operator++() {
  if (!_current_indexset_iter) {
    return *this;
  }
  if (_current_card >= _container->_maxcard) {           
    iter_allocator.destroy(_current_indexset_iter);
    iter_allocator.deallocate(_current_indexset_iter, 1);
    _current_indexset_iter = NULL;
    return *this;
  }
  ++(*_current_indexset_iter);
  if (*_current_indexset_iter != _container->_index_set[_current_card].end()) {
    return *this;
  }
  ++_current_card;
  while (_current_card < _container->_maxcard) {
    if (!_container->_index_set[_current_card].is_empty()) {
      *_current_indexset_iter = _container->_index_set[_current_card].begin();
      return *this;
    }
    ++_current_card;
  }
  iter_allocator.destroy(_current_indexset_iter);
  iter_allocator.deallocate(_current_indexset_iter, 1);
  _current_indexset_iter = NULL;
  return *this;
}

// =============================
// SimplicialComplexTemplate<T>:
// =============================

// tightens the interval between _mincard and _maxcard if possible:

template<class T>
void SimplicialComplexTemplate<T>::minmaxcard_update() {
  while (_maxcard > 0) {
    if (_index_set[_maxcard-1].is_empty()) {
      --_maxcard;
    }
    else {
      break;
    }
  }
  resize(_maxcard);  
  if (_maxcard == 0) {
    _mincard = _maxcard;
  }
  else {
    while (_mincard < _maxcard) {
      if (_index_set[_mincard].is_empty()) {
	++_mincard;
      }
      else {
	break;
      }
    }
  }
}

// keys for a container:

template<class T>
const size_type SimplicialComplexTemplate<T>::keysize() const {
//   size_type res = 0;
//   for (size_type i = _mincard; i < _maxcard; ++i) {
//     res += _index_set[i].keysize();
//   }
//   return res;
  return _maxcard - _mincard;
}

template<class T>
const size_type SimplicialComplexTemplate<T>::key(const size_type n) const {
//   size_type block_no = n + 1;
//   size_type i = _mincard;
//   while ((block_no > 0) && (i < _maxcard)) {
//     if (block_no > _index_set[i].keysize()) {
//       block_no -= _index_set[i++].keysize();
//     }
//     else {
//       return _index_set[i].key(block_no - 1);
//     }
//   }
// #ifdef DEBUG
//   std::cerr << "key(const size_type): no valid key found for "
//        << *this << " at " << n << std::endl;

// #endif
//   return 0;
  return _index_set[_mincard + n].key(0);
}

// status information:

template<class T>
const size_type SimplicialComplexTemplate<T>::card() const {
  size_type res = 0;
  for (size_type i = _mincard; i < _maxcard; ++i) {
    res += card(i);
  }
  return res;
}
    
template<class T>
const bool SimplicialComplexTemplate<T>::contains(const Simplex& simp, 
						  const size_type card) const {
  if (card >= _maxcard) {
    return false;
  }
  const IndexTable::dataptr_type& indexptr(_index_table[card].member(simp));
  if (indexptr) {
    return _index_set[card].contains(*indexptr);
  }
  else {
    return false;
  }
}

template<class T>
const bool SimplicialComplexTemplate<T>::superset(const SimplicialComplexTemplate<T>& sc) const {
  if (sc.is_empty()) {
    return true;
  }
  if (_mincard > sc._mincard) {
    return false;
  }
  if (_maxcard < sc._maxcard) {
    return false;
  }
  for (size_type i = sc._mincard; i < sc._maxcard; ++i) {
    if (!_index_set[i].superset(sc._index_set[i])) {
      return false;
    }
  }
  return true;
}

template<class T>
const bool SimplicialComplexTemplate<T>::disjoint(const SimplicialComplexTemplate<T>& sc) const {
  if (sc.is_empty()) {
    return true;
  }
  if (_maxcard <= sc._mincard) {
    return true;
  }
  if (_mincard >= sc._maxcard) {
    return true;
  }
  size_type minmaxcard = _maxcard < sc._maxcard ? _maxcard : sc._maxcard;
  size_type maxmincard = _mincard > sc._mincard ? _mincard : sc._mincard;
  for (size_type i = maxmincard; i < minmaxcard; ++i) {
    if (!_index_set[i].disjoint(sc._index_set[i])) {
      return false;
    }
  }
  return true;
}

template<class T>
const bool SimplicialComplexTemplate<T>::contains_face(const Simplex& simp, 
						       const size_type card) const {
  if (card >= _maxcard) {
    return false;
  }
  for (size_type i = card + 1; i < _maxcard; ++i) {
    for (IndexSet_iterator iter = _index_set[i].begin();
         iter != _index_set[i].end(); 
         ++iter) {
      if (_index_table[i].get_obj(*iter).superset(simp)) {
        return true;
      }
    }
  }
  return false;
}

template<class T>
const bool SimplicialComplexTemplate<T>::contains_face(const Simplex& simp,
						       const size_type card, 
						       Simplex& facet) const {
  if (card >= _maxcard) {
    return false;
  }
  for (size_type i = card + 1; i < _maxcard; ++i) {
    for (IndexSet_iterator iter = _index_set[i].begin();
         iter != _index_set[i].end(); 
         ++iter) {
      facet = _index_table[i].get_obj(*iter);
      if (facet.superset(simp)) {
        return true;
      }
    }
  }
  return false;
}

template<class T>
const bool SimplicialComplexTemplate<T>::contains_free_face(const Simplex& simp,
							    const size_type card,
							    Simplex& facet) const {
  if (card >= _maxcard) {
    return false;
  }
  bool is_in_unique_facet_so_far(false);
  for (size_type i = card + 1; i < _maxcard; ++i) {
    for (IndexSet_iterator iter = _index_set[i].begin();
         iter != _index_set[i].end(); 
         ++iter) {
      const Simplex& tmp = _index_table[i].get_obj(*iter);
      if (tmp.superset(simp)) {
        if (is_in_unique_facet_so_far) {
	  // another facet found
          return false;
	}
        else {
	  // first facet found
          is_in_unique_facet_so_far = true;
          facet = tmp;
        }
      }
    }
  }
  return is_in_unique_facet_so_far;
}
 
// standard operations on simplicial complexes:

template<class T>
const Simplex SimplicialComplexTemplate<T>::support() const {
  Simplex result = Simplex();
  for (typename SimplicialComplexTemplate<T>::iterator iter = begin(); iter != end(); ++iter) {
    result += *iter;
  }
  return result;
}

template<class T>
SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::star(const Simplex& simp,
								const size_type card) const {
  SimplicialComplexTemplate<T> result;
  if (card >= _maxcard) {
    return result;
  }
  if (contains(simp)) {
    result.insert(simp, card);
  }
  for (size_type i = card + 1; i < _maxcard; ++i) {
    for (IndexSet_iterator iter = _index_set[i].begin();
         iter != _index_set[i].end(); 
         ++iter) {
      const Simplex& tmp = _index_table[i].get_obj(*iter);
      if (tmp.superset(simp)) {
        result.insert(tmp, i);
      }
    }
  }
  return result;
}

template<class T>
SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::link(const Simplex& simp,
								const size_type card) const {
  SimplicialComplexTemplate<T> result;
  if (card >= _maxcard) {
    return result;
  }
  if (contains(simp)) {
    result.insert(Simplex(), 0);
  }
  for (size_type i = card + 1; i < _maxcard; ++i) {
    for (IndexSet_iterator iter = _index_set[i].begin();
         iter != _index_set[i].end(); 
         ++iter) {
      const Simplex& tmp = _index_table[i].get_obj(*iter);
      if (tmp.superset(simp)) {
        result.insert(tmp - simp, i - card);
      }
    }
  }
  return result;
}

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::deletion(const Simplex& simp,
								     const size_type card) {
  if (card >= _maxcard) {
    return *this;
  }
  for (size_type i = card; i < _maxcard; ++i) {
    for (iterator iter = begin(); iter != end(); ++iter) {
      if (iter->superset(simp)) {
	erase(*iter);
      }
    }
  }
  return *this;
}

// extract simplices with given cardinality:

template<class T>
const SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator[](const size_type card) const {
  SimplicialComplexTemplate<T> result;
  if ((card < _mincard) || (card + 1 > _maxcard)) {
    return result;
  }
  result._mincard = card;
  result._maxcard = result.resize(card + 1);
  result._index_set[card] = _index_set[card];
  return result;
}

// boolean operators:

template<class T>
inline const bool SimplicialComplexTemplate<T>::operator==(const SimplicialComplexTemplate<T>& s) const {
  if ((_mincard != s._mincard) || (_maxcard != s._maxcard)) {
    return false;
  }
  for (size_type i = _mincard; i < _maxcard; ++i){
    if (_index_set[i] != s._index_set[i]) {
      return false;
    }
  }
  return true;
}

template<class T>
const bool SimplicialComplexTemplate<T>::operator<(const SimplicialComplexTemplate<T>& s) const {
  if (_mincard < s._mincard) {
    return true;
  }
  if (s._mincard < _mincard) {
    return false;
  }
  size_type minmaxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
  for (size_type i = _mincard; i < minmaxcard; ++i) {
    if (_index_set[i] < s._index_set[i]){
      return true;
    }
    if (_index_set[i] > s._index_set[i]) {
      return false;
    }
  }
  if (_maxcard < s._maxcard) {
    return true;
  }
  else {
    return false;
  }
}

// boundary computation:

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::insert_boundary(const Simplex& simp,
									    const size_type card) {
  if (card == 0) {
    return *this;
  }
  Simplex tmp(simp);
  for (Simplex::const_iterator iter = simp.begin(); iter != simp.end(); ++iter) {
    tmp -= *iter;
    insert(tmp, card - 1);
    tmp += *iter;
  }
  return *this;
}

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::insert_faces(const SimplicialComplexTemplate<T>& sct, 
									 const size_type card) {
  if (card + 1 > _maxcard) {
    resize(card + 1);
  }
  const IndexSet index_set_copy(sct._index_set[card]);
  for (IndexSet_const_iterator simp_iter = index_set_copy.begin();
       simp_iter != index_set_copy.end();
       ++simp_iter) {
    const Simplex& simp = sct._index_table[card].get_obj(*simp_iter);
    Simplex face(simp);
    for (Simplex::const_iterator iter = simp.begin(); iter != simp.end(); ++iter) {
      face -= *iter;
      insert(face, card - 1);
      face += *iter;
    }
  }
  return *this;
}


// modifiers:

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::include_all_faces(const size_type depth) {
  if (_maxcard < 2) {
    return *this;
  }
#ifdef SUPER_SUPER_VERBOSE
  std::cout << "SimplicialComplexTemplate<T>::include_allfaces(const size_type depth):"
       << std::endl;
  std::cout << "_maxcard = " << _maxcard << std::endl;
  std::cout << "reducing to facets ... " << flush;
#endif
  reduce_to_facets();
#ifdef SUPER_SUPER_VERBOSE
  std::cout << "_maxcard = " << _maxcard << std::endl;
  std::cout << "depth = " << depth << std::endl;
  std::cout << "done." << std::endl;
#endif
  for (size_type i = _maxcard - 1; i > depth; --i) {
#ifdef SUPER_SUPER_VERBOSE
    std::cout << "including faces of dimension " << i - 2 << " ... " << flush;
#endif
    insert_faces(*this, i);
#ifdef SUPER_SUPER_VERBOSE
    std::cout << "done." << std::endl;
#endif
  }
  return *this;
}

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::reduce_to_facets() {
  if (_maxcard < 3) {
    return *this;
  }
  for (size_type i = _maxcard - 2; i + 1 > _mincard; --i) {
    const IndexSet index_set_copy(_index_set[i]);
    for (IndexSet_const_iterator iter = index_set_copy.begin();
	 iter != index_set_copy.end();
	 ++iter) {
      const Simplex& simp = _index_table[i].get_obj(*iter);
      if (contains_face(simp, i)) {
	erase(simp, i);
      }
    }
  }
  return *this;
}

// assigment:

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator=(const SimplicialComplexTemplate<T>& s) {
  if (this == &s) {
    return *this;
  }
  _maxcard = resize(s._maxcard);
  _mincard = s._mincard;
  for (size_type i = _mincard; i < _maxcard; ++i) {
    _index_set[i] = s._index_set[i];
  }
  return *this;
}

// adding and deleting a simplex:

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::insert(const Simplex& simp, 
								   const size_type card) {
#ifdef DEBUG
  if (simp.card() != card) {
    std::cerr << "SimplicialComplexTemplate<T>::insert(const Simplex&, const size_type): "
	 << "wrong cardinality in second parameter. "
	 << "card(" << simp << " != " << card << std::endl;
    exit(1);
  }
#endif
  if (_mincard + 1 > _maxcard) {
    _mincard = card;
  }
  if (card + 1 > _maxcard) {
    _maxcard = resize(card + 1);
  }
  if (card < _mincard) {
    _mincard = card;
  }
  _index_set[card] += _index_table[card].get_index(simp);
  //   minmaxcard_update();
#ifdef COMPUTATIONS_DEBUG
  if (!contains(simp)) {
    std::cerr << "SimplicialComplexTemplate<T>::insert"
	 << "(const Simplex&, const size_type): "
	 << "computational error."
	 << std::endl;
    std::cerr << *this << " does not contain inserted simplex " << simp
	 << std::endl;
    exit(1);
  }
  for (IndexSet_const_iterator test_iter = _index_set[card].begin();
       test_iter != _index_set[card].end();
       ++test_iter) {
    if (*test_iter + 1 > _index_table[card].maxindex()) {
      std::cerr << "SimplicialComplexTemplate<T>::insert"
	   << "(const Simplex&, const size_type): "
	   << "computational error."
	   << std::endl;
      std::cerr << "index " << *test_iter << " in _index_set[" << card << "] "
	   << "is out of range."
	   << std::endl;
      exit(1);
    }
  }
#endif
  return *this;
}

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::erase(const Simplex& simp,
								  const size_type card) {
#ifdef DEBUG
  if (simp.card() != card) {
    std::cerr << "SimplicialComplexTemplate<T>::erase(const Simplex&, const size_type): "
	 << "wrong cardinality in second parameter." << std::endl;
  }
#endif
  if ((card + 1 > _maxcard) || (card < _mincard)) {
    return *this;
  }
  const IndexTable::dataptr_type& indexptr(_index_table[card].member(simp));
  if (!indexptr) {
    return *this;
  }
  _index_set[card] -= *indexptr;  
  if (card == _maxcard - 1) {
    while (_maxcard > 0) {
      if (_index_set[_maxcard - 1].is_empty()) {
	--_maxcard;
      }
      else {
	break;
      }
    }
    if (_maxcard <= card) {
      resize(_maxcard);
    }
  }
  if (_maxcard == 0) {
    _mincard = 0;
  }
  else if (card == _mincard) {
    while (_mincard < _maxcard) {
      if (_index_set[_mincard].is_empty()) {
	++_mincard;
      }
      else {
	break;
      }
    }	  
  }
  //   minmaxcard_update();
#ifdef COMPUTATIONS_DEBUG
  if (contains(simp)) {
    std::cerr << "SimplicialComplexTemplate<T>::erase"
	 << "(const Simplex&, const size_type): "
	 << "computational error."
	 << std::endl;
    std::cerr << *this << " contains erased simplex " << simp
	 << std::endl;
    exit(1);
  }
#endif
  return *this;
}

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::exclusiveor(const Simplex& simp,
									const size_type card) {
#ifdef DEBUG
  if (simp.card() != card) {
    std::cerr << "SimplicialComplexTemplate<T>::exclusiveor(const Simplex&, const size_type): "
	 << "wrong cardinality in second parameter." << std::endl;
  }
#endif
  if (_mincard + 1 > _maxcard) {
    _mincard = card;
  }
  if (card + 1 > _maxcard) {
    _maxcard = resize(card + 1);
  }
  if (card < _mincard) {
    _mincard = card;
  }
  _index_set[card] ^= _index_table[card].get_index(simp);
  minmaxcard_update();
  return *this;
}

// union, difference, and intersection with simplicial complexes:

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator+=(const SimplicialComplexTemplate<T>& s) {
  if (s.is_empty()) {
    return *this;
  }
  if (_mincard + 1 > _maxcard) {
    _mincard = s._mincard;
  }
  if (_maxcard < s._maxcard) {
    _maxcard = resize(s._maxcard);
  }
  else {
    _mincard = _mincard < s._mincard ? _mincard : s._mincard;
  }
  _maxcard = _maxcard > s._maxcard ? _maxcard : s._maxcard;
  for (size_type i = s._mincard; i < s._maxcard; ++i) {
    _index_set[i] += s._index_set[i];
  }
  //   minmaxcard_update();
#ifdef COMPUTATIONS_DEBUG
  if (!superset(s)) {
    std::cerr << "SimplicialComplexTemplate<T>::operator+=(const SimplicialComplexTemplate<T>&): "
    << "computational error."
    << std::endl;
    std::cerr << *this << " not a superset of sum operand " << s
    << std::endl;
    exit(1);
  }
#endif
  return *this;
}

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator-=(const SimplicialComplexTemplate<T>& s) {
  if (s.is_empty()) {
    return *this;
  }
  size_type minmaxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
  size_type maxmincard = _mincard > s._mincard ? _mincard : s._mincard;
  if (minmaxcard <= maxmincard) {
    return *this;
  }
  for (size_type i = maxmincard; i < minmaxcard; ++i) {
    _index_set[i] -= s._index_set[i];
  }
  minmaxcard_update();
#ifdef COMPUTATIONS_DEBUG
  if (!disjoint(s)) {
    std::cerr << "SimplicialComplexTemplate<T>::operator-=(const SimplicialComplexTemplate<T>&): "
    << "computational error." 
    << std::endl;
    std::cerr << *this << " not disjoint to difference operand " << s
    << std::endl;
    exit(1);
  }
#endif
  return *this;
}

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator*=(const SimplicialComplexTemplate<T>& s) {
  if (s.is_empty()) {
    return clear();
  }
  size_type minmaxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
  size_type maxmincard = _mincard > s._mincard ? _mincard : s._mincard;
  if (minmaxcard <= maxmincard) {
    return clear();
  }
  for (size_type i = _mincard; i < maxmincard; ++i) {
    _index_set[i].clear();
  }
  for (size_type i = maxmincard; i < minmaxcard; ++i) {
    _index_set[i] *= s._index_set[i];
  }
  for (size_type i = minmaxcard; i < _maxcard; ++i) {
    _index_set[i].clear();
  }
  _maxcard = minmaxcard;
  _mincard = maxmincard;
  minmaxcard_update();
#ifdef COMPUTATIONS_DEBUG
  if (!subset(s)) {
    std::cerr << "SimplicialComplexTemplate<T>::operator*=(const SimplicialComplexTemplate<T>&): "
    << "computational error:" << std::endl;
    std::cerr << *this << " not a subset of intersection operand " << s
    << std::endl;
    exit(1);
  }
#endif
  return *this;
}

template<class T>
SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator^=(const SimplicialComplexTemplate<T>& s) {
  if (s.is_empty()) {
    return *this;
  }
  if (_mincard + 1 > _maxcard) {
    _mincard = s._mincard;
  }
  if (_maxcard < s._maxcard) {
    _maxcard = resize(s._maxcard);
  }
  else {
    _mincard = _mincard < s._mincard ? _mincard : s._mincard;
  }
  _maxcard = _maxcard > s._maxcard ? _maxcard : s._maxcard;
  for (size_type i = s._mincard; i < s._maxcard; ++i) {
    _index_set[i] ^= s._index_set[i];
  }
  minmaxcard_update();
  return *this;
}

// join:

template<class T>
SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::join(const Simplex& simp,
								const size_type card) const {
  if (simp.is_empty()) {
    return *this;
  }
  SimplicialComplexTemplate<T> result;
  for (size_type i = _mincard; i < _maxcard; ++i) {
    for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter = 
	   _index_set[i].begin();
	 iter != _index_set[i].end();
	 ++iter) {
      const Simplex& s = _index_table[i].get_obj(*iter);
#ifdef COMPUTATIONS_DEBUG
      if (!s.disjoint(simp))
	std::cerr << "join(const SimplicialComplexTemplate<T>&, const Simplex& simp, "
	     << "const size_type card): "
	     << "simplices not disjoint." << std::endl;
		  
#endif
      result.insert(s + simp, card + i);
    }
  }
  return result;
}

template<class T>
SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::join(const SimplicialComplexTemplate<T>& s) const {
  SimplicialComplexTemplate<T> result;
  if (is_empty() || s.is_empty()) {
    return result;
  }
  if (dim() == -1) {
    return s;
  }
  if (s.dim() == -1) {
    return *this;
  }
  for (size_type i = _maxcard; i > _mincard; --i) {
    for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter1 = 
	   _index_set[i - 1].begin();
	 iter1 != _index_set[i - 1].end();
	 ++iter1) {
      const Simplex& simp1 = _index_table[i - 1].get_obj(*iter1);
      for (size_type j = s._maxcard; j > s._mincard; --j) {
	for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter2 = s._index_set[j - 1].begin();
	     iter2 != s._index_set[j - 1].end();
	     ++iter2) {
	  const Simplex& simp2 = s._index_table[j - 1].get_obj(*iter2);
#ifdef COMPUTATIONS_DEBUG
	  if (!simp1.disjoint(simp2)) {
	    std::cerr << "SimplicialComplexTemplate<T>::join(const SimplicialComplexTemplate<T>&): "
		 << simp1 << " not disjoint from " << simp2 << std::endl;
	    exit(1);
	  }
#endif
	  result.insert(simp1 + simp2, i - 1 + j - 1);
	}
      }
    }
  }
  return result;
}

// iostream:

template<class T>
std::ostream& SimplicialComplexTemplate<T>::write(std::ostream& ost) const {
  size_type count = 0;
  const size_type card = this->card();
  
  ost << "{";
  for (typename SimplicialComplexTemplate<T>::const_iterator iter = begin();
       iter != end(); 
       ++iter) {
    ost << *iter;			// we need the stream output for Simplex
    if (++count < card) {
      ost << ",";
    }
  }
  ost << "}";
  return ost;
}

template<class T>
std::istream& SimplicialComplexTemplate<T>::read(std::istream& ist) {
  char c;
  Simplex simp;
  
  clear();	// initialize to empty complex
  ist >> std::ws >> c;
  if (c == '{') {
    while (ist >> std::ws >> c) {
      if (isspace(c)) {
	continue;
      }
      if (c == '}') {
	break;
      }
      if (c == ',') {
	continue;
      }
      ist.putback(c);
      if (ist >> simp) {
	*this += simp;		// add one simplex at a time
      }
      else {
#ifdef READ_DEBUG
	std::cerr << "SimplicialComplexTemplate<T>::read(std::istream&): Not a simplex."
	     << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
    }
  }
  else {
#ifdef READ_DEBUG
    std::cerr << "SimplicialComplexTemplate<T>::read(std::istream&): Missing ``{''." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  ist.clear(std::ios::goodbit);
  return ist;
}

#endif

// eof SimplicialComplexTemplate.hh
