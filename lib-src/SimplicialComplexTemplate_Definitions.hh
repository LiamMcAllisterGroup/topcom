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

#include <random>
#include <algorithm>

#include "SimplicialComplexTemplate_Declarations.hh"

namespace topcom {

  template<class T>
  IndexTableArray SimplicialComplexTemplate<T>::_index_table;
  
  template<class T>
  const size_type SimplicialComplexTemplate<T>::init_capacity = 8;

  template<class T>
  const size_type SimplicialComplexTemplate<T>::init_size = 0;

  template<class T>
  __sc_const_iterator<T>::__sc_const_iterator(const __sc_const_iterator& iter) :
    _container(iter._container),
    _current_card(iter._current_card),
    _current_indexset_iter(nullptr) {
    if (iter._current_indexset_iter) {
      _current_indexset_iter = iter_allocator.allocate(1);
      std::allocator_traits<std::allocator<IndexSet_const_iterator> >::construct(iter_allocator,
										 _current_indexset_iter,
										 *iter._current_indexset_iter);
    }
    else {
      _current_card = _container->_maxcard;
    }
  }

  template<class T>
  __sc_const_iterator<T>::__sc_const_iterator(const SimplicialComplexTemplate<T>& s) :
    _container(&s), 
    _current_card(_container->_mincard),
    _current_indexset_iter(nullptr) {
    while (_current_card < _container->_maxcard) {
      if (!_container->_index_set[_current_card].empty()) {
	_current_indexset_iter = iter_allocator.allocate(1);
	std::allocator_traits<std::allocator<IndexSet_const_iterator> >::construct(iter_allocator,
										   _current_indexset_iter,
										   _container->_index_set[_current_card].begin());
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
    if (_current_indexset_iter && (_current_indexset_iter != iter._current_indexset_iter)) {
      iter_allocator.deallocate(_current_indexset_iter);
    }
    if (iter._current_indexset_iter) {
      _current_indexset_iter = iter_allocator.allocate(1);
      std::allocator_traits<std::allocator<IndexSet_const_iterator> >::construct(iter_allocator,
										 _current_indexset_iter,
										 *iter._current_indexset_iter);
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
    if ((_current_indexset_iter == nullptr) || (iter._current_indexset_iter == nullptr)) {
      return false;
    }
    return (*_current_indexset_iter == *iter._current_indexset_iter);
  }

  template<class T>
  __sc_const_iterator<T>& __sc_const_iterator<T>::operator++() {
    if (!_current_indexset_iter) {
#ifdef SCITER_DEBUG
      std::cerr << "__sc_const_iterator<T>& __sc_const_iterator<T>::operator++(): _current_indexset_iter == nullptr already" << std::endl;
#endif
      return *this;
    }
    if (_current_card >= _container->_maxcard) {           
      std::allocator_traits<std::allocator<IndexSet_const_iterator> >::destroy(iter_allocator,
									       _current_indexset_iter);
      iter_allocator.deallocate(_current_indexset_iter, 1);
      _current_indexset_iter = nullptr;
#ifdef SCITER_DEBUG
      std::cerr << "__sc_const_iterator<T>& __sc_const_iterator<T>::operator++(): "
	<< _current_card << " = _current_card >= _maxcard =" << _container->_maxcard << " => _current_indexset_iter set to nullptr" << std::endl;
#endif
      return *this;
    }
    ++(*_current_indexset_iter);
    if (*_current_indexset_iter != _container->_index_set[_current_card].end()) {
      return *this;
    }
    ++_current_card;
    while (_current_card < _container->_maxcard) {
      if (!_container->_index_set[_current_card].empty()) {
	*_current_indexset_iter = _container->_index_set[_current_card].begin();
	return *this;
      }
      ++_current_card;
    }
    std::allocator_traits<std::allocator<IndexSet_const_iterator> >::destroy(iter_allocator,
									     _current_indexset_iter);
    iter_allocator.deallocate(_current_indexset_iter, 1);
    _current_indexset_iter = nullptr;
#ifdef SCITER_DEBUG
    std::cerr << "__sc_const_iterator<T>& __sc_const_iterator<T>::operator++(): upcoming indexsets empty => _current_indexset_iter set to nullptr" << std::endl;
#endif
    return *this;
  }

  // =============================
  // SimplicialComplexTemplate<T>:
  // =============================

  // reset thte index table (invalidates all current simplicial complexes!):
  template<class T>
  void SimplicialComplexTemplate<T>::reset_index_table() {
    _index_table.clear();
  }
  
  // preprocess the index table (good for multi-threading)
  template<class T>
  void SimplicialComplexTemplate<T>::preprocess_index_table(const parameter_type maxno,
							    const parameter_type minrank,
							    const parameter_type maxrank) {
    std::random_device rd;
    std::mt19937 g(rd());
    if (maxrank > _index_table.size()) {
      _index_table.resize(maxrank);
    }
    for (parameter_type i = minrank; i <= maxrank; ++i) {
      std::vector<Simplex> sort_vec;
      Permutation perm(maxno, i);
      do {
	const Simplex simp(perm);
	sort_vec.push_back(simp);
      } while (perm.lexnext());
      if (CommandlineOptions::use_random_order()) {
	std::shuffle(sort_vec.begin(), sort_vec.end(), g);
      }
      for (std::vector<Simplex>::const_iterator sort_iter = sort_vec.begin();
	   sort_iter != sort_vec.end();
	   ++sort_iter) {
	const Simplex& simp(*sort_iter);
	const size_type idx = _index_table.get_index(i, simp);
      }
      _index_table.set_preprocessed(i, true);
    }
    if ((minrank == 0) && (maxrank == maxno)) {

      // all cards have been preprocessed:
      _index_table.set_preprocessed(true);
    }
  }

  // the same but sorted by volume if requested:
  template<class T>
  void SimplicialComplexTemplate<T>::preprocess_index_table(const parameter_type maxno,
							    const parameter_type minrank,
							    const parameter_type maxrank,
							    const Volumes&       voltable,
							    bool                 only_independent) {
    std::random_device rd;
    std::mt19937 g(rd());
    if (!voltable.empty()) {
      const parameter_type volrank = voltable.begin()->first.card();
      if (volrank + 1 > _index_table.size()) {
	_index_table.resize(volrank + 1);
      }
      CompareSimplexVolumes comparator(voltable);
      std::vector<Simplex> sort_vec;
      Permutation perm(maxno, volrank);
      do {
	const Simplex simp(perm);
	if (only_independent && (voltable.find(simp) == voltable.end())) {
	  continue;
	}
	sort_vec.push_back(simp);
      } while (perm.lexnext());
      if (CommandlineOptions::use_random_order()) {
	std::shuffle(sort_vec.begin(), sort_vec.end(), g);
      }
      if (CommandlineOptions::use_volume_order()) {
	std::stable_sort(sort_vec.begin(), sort_vec.end(), comparator);
      }
      for (std::vector<Simplex>::const_iterator sort_iter = sort_vec.begin();
	   sort_iter != sort_vec.end();
	   ++sort_iter) {
	const Simplex& simp(*sort_iter);
	_index_table.get_index(volrank, simp);
      }
      _index_table.set_preprocessed(volrank, true);
      preprocess_index_table(maxno, minrank, volrank - 1);
      preprocess_index_table(maxno, volrank + 1, maxrank);
    }
  }

  // the same but with distinction between full-dimensional and not:
  template<class T>
  void SimplicialComplexTemplate<T>::preprocess_index_table(const parameter_type maxno,
							    const parameter_type minrank,
							    const parameter_type maxrank,
							    const Chirotope&     chiro,
							    bool                 only_independent) {
    std::random_device rd;
    std::mt19937 g(rd());
    const parameter_type chirorank = chiro.rank();
    if (chirorank + 1 > _index_table.size()) {
      _index_table.resize(chirorank + 1);
    }
    std::vector<Simplex> sort_vec;
    Permutation perm(maxno, chirorank);
    do {
      const Simplex simp(perm);
      if (only_independent && (chiro(simp) == 0)) {
	continue;
      }
      sort_vec.push_back(simp);
    } while (perm.lexnext());
    if (CommandlineOptions::use_random_order()) {
      std::shuffle(sort_vec.begin(), sort_vec.end(), g);
    }
    for (std::vector<Simplex>::const_iterator sort_iter = sort_vec.begin();
	 sort_iter != sort_vec.end();
	 ++sort_iter) {
      const Simplex& simp(*sort_iter);
      _index_table.get_index(chirorank, simp);
    }
    _index_table.set_preprocessed(chirorank, true);
    preprocess_index_table(maxno, minrank, chirorank - 1);
    preprocess_index_table(maxno, chirorank + 1, maxrank);
  }

  // tightens the interval between _mincard and _maxcard if possible:
  template<class T>
  void SimplicialComplexTemplate<T>::minmaxcard_update() {
    while (_maxcard > 0) {
      if (_index_set[_maxcard - 1].empty()) {
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
	if (_index_set[_mincard].empty()) {
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
    size_type res = 0;
    for (parameter_type i = _mincard; i < _maxcard; ++i) {
      res += _index_set[i].keysize();
    }
    return res;
    // return _maxcard - _mincard;
  }

  template<class T>
  const size_type SimplicialComplexTemplate<T>::key(const size_type n) const {
    size_type block_no = n + 1;
    parameter_type i = _mincard;
    while ((block_no > 0) && (i < _maxcard)) {
      const size_type keysize(_index_set[i].keysize());
      if (block_no > keysize) {
	block_no -= keysize;
	++i;
      }
      else {
	return _index_set[i].key(block_no - 1);
      }
    }
    std::cerr << "key(const size_type): no valid key found for "
	      << *this << " at " << n << std::endl;

    return 0;
    // return _index_set[_mincard + n].key(0);
  }

  // status information:

  template<class T>
  const size_type SimplicialComplexTemplate<T>::card() const {
    size_type res = 0;
    for (parameter_type i = _mincard; i < _maxcard; ++i) {
      res += card(i);
    }
    return res;
  }
    
  template<class T>
  const bool SimplicialComplexTemplate<T>::contains(const Simplex& simp, 
						    const parameter_type card) const {
    if (_is_pure) {
      if (card != _pure_rank) {
	return false;
      }
      IndexTable::const_iterator finder = _index_table.find(_pure_rank, simp);
      if (finder != _index_table.end(_pure_rank)) {
	return _index_set[0].contains(finder->second);
      }
      else {
	return false;
      }
    }
    else {
      if (card >= _maxcard) {
	return false;
      }
      IndexTable::const_iterator finder = _index_table.find(card, simp);
      // const IndexTable::dataptr_type& indexptr(_index_table[card].member(simp));
      if (finder != _index_table.end(card)) {
	return _index_set[card].contains(finder->second);
      }
      else {
	return false;
      }
    }
  }

  template<class T>
  const bool SimplicialComplexTemplate<T>::superset(const SimplicialComplexTemplate<T>& sc) const {
    if (sc.empty()) {
      return true;
    }
    else if (empty()) {
      return false;
    }
    if (_is_pure) {
      if (!sc._is_pure) {
	return false;
      }
      else if (_pure_rank != sc._pure_rank) {
	return false;
      }
      else {
	return _index_set[0].superset(sc._index_set[0]);
      }
    }
    else {
      if (sc._is_pure) {
	if ((sc._pure_rank < _mincard) || (sc._pure_rank >= _maxcard)) {
	  return false;
	}
	else {
	  return _index_set[sc._pure_rank].superset(sc._index_set[0]);
	}
      }
      else {
	if (_mincard > sc._mincard) {
	  return false;
	}
	if (_maxcard < sc._maxcard) {
	  return false;
	}
	for (parameter_type i = sc._mincard; i < sc._maxcard; ++i) {
	  if (!_index_set[i].superset(sc._index_set[i])) {
	    return false;
	  }
	}
	return true;
      }
    }
  }

  template<class T>
  const bool SimplicialComplexTemplate<T>::disjoint(const SimplicialComplexTemplate<T>& sc) const {
    if (sc.empty()) {
      return true;
    }
    if (empty()) {
      return true;
    }
    if (_is_pure) {
      if (sc._is_pure) {
	return _index_set[0].disjoint(sc._index_set[0]);
      }
      else {
	if ((_pure_rank < sc._mincard) || (_pure_rank >= sc._maxcard)) {
	  return true;
	}
	else {
	  return _index_set[0].disjoint(sc._index_set[_pure_rank]);
	}
      }
    }
    else {
      if (sc._is_pure) {
	if ((sc._pure_rank < _mincard) || (sc._pure_rank >= _maxcard)) {
	  return true;
	}
	else {
	  return _index_set[sc._pure_rank].disjoint(sc._index_set[0]);
	}
      }
      else {
	if (_maxcard <= sc._mincard) {
	  return true;
	}
	if (_mincard >= sc._maxcard) {
	  return true;
	}
	parameter_type minmaxcard = _maxcard < sc._maxcard ? _maxcard : sc._maxcard;
	parameter_type maxmincard = _mincard > sc._mincard ? _mincard : sc._mincard;
	for (parameter_type i = maxmincard; i < minmaxcard; ++i) {
	  if (!_index_set[i].disjoint(sc._index_set[i])) {
	    return false;
	  }
	}
	return true;
      }
    }
  }

  template<class T>
  const bool SimplicialComplexTemplate<T>::contains_face(const Simplex& simp, 
							 const parameter_type card) const {
    if (_is_pure) {
      if (card >= _pure_rank) {
	return false;
      }
      else {
	for (IndexSet_iterator iter = _index_set[0].begin();
	     iter != _index_set[0].end();
	     ++iter) {
	  if (_index_table.get_obj(_pure_rank, *iter).superset(simp)) {
	    return true;
	  }
	}
	return false;
      }
    }
    else {
      if (card >= _maxcard - 1) {
	return false;
      }
      for (parameter_type i = card + 1; i < _maxcard; ++i) {
	for (IndexSet_iterator iter = _index_set[i].begin();
	     iter != _index_set[i].end(); 
	     ++iter) {
	  if (_index_table.get_obj(i, *iter).superset(simp)) {
	    return true;
	  }
	}
      }
      return false;
    }
  }

  template<class T>
  const bool SimplicialComplexTemplate<T>::contains_face(const Simplex& simp,
							 const parameter_type card, 
							 Simplex& facet) const {
    if (_is_pure) {
      if (card >= _pure_rank) {
	return false;
      }
      else {
	for (IndexSet_iterator iter = _index_set[0].begin();
	     iter != _index_set[0].end();
	     ++iter) {
	  facet = _index_table.get_obj(_pure_rank, *iter);
	  if (facet.superset(simp)) {
	    return true;
	  }
	}
	return false;
      }
    }
    else {
      if (card >= _maxcard - 1) {
	return false;
      }
      for (parameter_type i = card + 1; i < _maxcard; ++i) {
	for (IndexSet_iterator iter = _index_set[i].begin();
	     iter != _index_set[i].end(); 
	     ++iter) {
	  facet = _index_table.get_obj(i, *iter);
	  if (facet.superset(simp)) {
	    return true;
	  }
	}
      }
      return false;
    }
  }

  template<class T>
  const bool SimplicialComplexTemplate<T>::contains_free_face(const Simplex& simp,
							      const parameter_type card,
							      Simplex& facet) const {
    if (_is_pure) {
      if (card >= _pure_rank) {
	return false;
      }
      bool is_in_unique_facet_so_far(false);
      for (IndexSet_iterator iter = _index_set[0].begin();
	   iter != _index_set[0].end(); 
	   ++iter) {
	Simplex tmp(_index_table.get_obj(_pure_rank, *iter));
	if (tmp.superset(simp)) {
	  if (is_in_unique_facet_so_far) {
	    // another facet found
	    return false;
	  }
	  else {
	    // first facet found
	    is_in_unique_facet_so_far = true;
	    facet = std::move(tmp);
	  }
	}
      }
      return is_in_unique_facet_so_far;
    }
    else {
      if (card >= _maxcard - 1) {
	return false;
      }
      bool is_in_unique_facet_so_far(false);
      for (parameter_type i = card + 1; i < _maxcard; ++i) {
	for (IndexSet_iterator iter = _index_set[i].begin();
	     iter != _index_set[i].end(); 
	     ++iter) {
	  Simplex tmp(_index_table.get_obj(i, *iter));
	  if (tmp.superset(simp)) {
	    if (is_in_unique_facet_so_far) {
	      // another facet found
	      return false;
	    }
	    else {
	      // first facet found
	      is_in_unique_facet_so_far = true;
	      facet = std::move(tmp);
	    }
	  }
	}
      }
      return is_in_unique_facet_so_far;
    }
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
								  const parameter_type card) const {
    SimplicialComplexTemplate<T> result;
    if (_is_pure) {
      if (card > _pure_rank) {
	return result;
      }
      if (contains(simp)) {
	result.insert(simp, card);
      }
      for (IndexSet_iterator iter = _index_set[0].begin();
	   iter != _index_set[0].end(); 
	   ++iter) {
	const Simplex tmp(_index_table.get_obj(_pure_rank, *iter));
	if (tmp.superset(simp)) {
	  result.insert(tmp, _pure_rank);
	}
      }
      return result;
    }
    else {
      if (card >= _maxcard) {
	return result;
      }
      if (contains(simp)) {
	result.insert(simp, card);
      }
      for (parameter_type i = card + 1; i < _maxcard; ++i) {
	for (IndexSet_iterator iter = _index_set[i].begin();
	     iter != _index_set[i].end(); 
	     ++iter) {
	  const Simplex tmp(_index_table.get_obj(i, *iter));
	  if (tmp.superset(simp)) {
	    result.insert(tmp, i);
	  }
	}
      }
      return result;
    }
  }

  template<class T>
  SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::link(const Simplex& simp,
								  const parameter_type card) const {
    SimplicialComplexTemplate<T> result;
    if (_is_pure) {
      if (card > _pure_rank) {
	return result;
      }
      if (contains(simp)) {
	result.insert(Simplex(), 0);
      }
      else {
	for (IndexSet_iterator iter = _index_set[0].begin();
	     iter != _index_set[0].end(); 
	     ++iter) {
	  const Simplex tmp(_index_table.get_obj(_pure_rank, *iter));
	  if (tmp.superset(simp)) {
	    result.insert(tmp - simp, _pure_rank - card);
	  }
	}
      }
      return result;
    }
    else {
      if (card >= _maxcard) {
	return result;
      }
      if (contains(simp)) {
	result.insert(Simplex(), 0);
      }
      for (parameter_type i = card + 1; i < _maxcard; ++i) {
	for (IndexSet_iterator iter = _index_set[i].begin();
	     iter != _index_set[i].end(); 
	     ++iter) {
	  const Simplex tmp(_index_table.get_obj(i, *iter));
	  if (tmp.superset(simp)) {
	    result.insert(tmp - simp, i - card);
	  }
	}
      }
      return result;
    }
  }

  template<class T>
  SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::deletion(const Simplex& simp,
								       const parameter_type card) {
    if (_is_pure) {
      if (card > _pure_rank) {
	return *this;
      }
    }
    else {
      if (card >= _maxcard) {
	return *this;
      }
    }
    for (iterator iter = begin(); iter != end(); ++iter) {
      if (iter->superset(simp)) {
	erase(*iter);
      }
    }
    return *this;
  }

  // extract simplices with given cardinality:

  template<class T>
  const SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator[](const parameter_type card) const {
    SimplicialComplexTemplate<T> result;
    if (_is_pure) {
      if (card != _pure_rank) {
	return result;
      }
      else {
	return *this;
      }
    }
    else {  
      if ((card < _mincard) || (card >= _maxcard)) {
	return result;
      }
      result._is_pure = true;
      result._pure_rank = card;
      result._mincard = 0;
      result._maxcard = 1;
      result.resize(result._maxcard);
      result._index_set[0] = _index_set[card];
      return result;
    }
  }

  // boolean operators:

  template<class T>
  inline const bool SimplicialComplexTemplate<T>::operator==(const SimplicialComplexTemplate<T>& s) const {
    if (empty()) {
      return s.empty();
    }
    else if (s.empty()) {
      return false;
    }
    if (_is_pure) {
      if (!s._is_pure) {
	return false;
      }
      else if (_pure_rank != s._pure_rank) {
	return false;
      }
      else {
	return (_index_set[0] == s._index_set[0]);
      }
    }
    else {
      if (s._is_pure) {
	return false;
      }
      else if ((_mincard != s._mincard) || (_maxcard != s._maxcard)) {
	return false;
      }
      else {
	for (parameter_type i = _mincard; i < _maxcard; ++i){
	  if (_index_set[i] != s._index_set[i]) {
	    return false;
	  }
	}
	return true;
      }
    }
  }

  template<class T>
  const bool SimplicialComplexTemplate<T>::operator<(const SimplicialComplexTemplate<T>& s) const {
    if (s.empty()) {
      return false;
    }
    else if (empty()) {
      return true;
    }
    if (_is_pure) {
      if (s._is_pure) {
	if (_pure_rank < s._pure_rank) {
	  return true;
	}
	else if (s._pure_rank < _pure_rank) {
	  return false;
	}
	else if (_index_set[0] < s._index_set[0]) {
	  return true;
	}
	else {
	  return false;
	}
      }
      else {
	if (_pure_rank < s._mincard) {
	  return true;
	}
	else if (s._mincard < _pure_rank) {
	  return false;
	}
	else if (_index_set[0] < s._index_set[s._mincard]) {
	  return true;
	}
	else {
	  return false;
	}
      }
    }
    else {
      if (s._is_pure) {
	if (_mincard < s._pure_rank) {
	  return true;
	}
	else if (s._pure_rank < _mincard) {
	  return false;
	}
	else if (_index_set[_mincard] < s._index_set[0]) {
	  return true;
	}
	else {
	  return false;
	}
      }
      else {
	if (_mincard < s._mincard) {
	  return true;
	}
	if (s._mincard < _mincard) {
	  return false;
	}
	parameter_type minmaxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
	for (parameter_type i = _mincard; i < minmaxcard; ++i) {
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
    }
  }

  template<class T>
  const bool SimplicialComplexTemplate<T>::lexsmaller(const SimplicialComplexTemplate<T>& s, const parameter_type card) const {
    // this is a specialization where the user can give the hint card to accelerate the comparison;
    // it is assumed without checks that the index sets for cardinality card all exist:
    if (_is_pure) {
      if (s._is_pure) {
	return _index_set[0].lexsmaller(s._index_set[0]);
      }
      else {
	return _index_set[0].lexsmaller(s._index_set[card]);
      }
    }
    else {
      if (s._is_pure) {
	return _index_set[card].lexsmaller(s._index_set[0]);
      }
      else {
	return _index_set[card].lexsmaller(s._index_set[card]);
      }
    }
  }

  template<class T>
  const bool SimplicialComplexTemplate<T>::lexsmaller(const SimplicialComplexTemplate<T>& s) const {
    // here we have to compare without any hint:
    if (s.empty()) {
      return false;
    }
    if (empty()) {
      return true;
    }
    if (_is_pure) {
      if (s._is_pure) {
	if (_pure_rank < s._pure_rank) {
	  return true;
	}
	if (s._pure_rank < _pure_rank) {
	  return false;
	}
	if (_index_set[0].lexsmaller(s._index_set[0])) {
	  return true;
	}
	else {
	  return false;
	}
      }
      else {
	if (_pure_rank < s._mincard) {
	  return true;
	}
	if (s._mincard < _pure_rank) {
	  return false;
	}
	if (_index_set[0].lexsmaller(s._index_set[s._mincard])) {
	  return true;
	}
	else {
	  return false;
	}
      }
    }
    else {
      if (s._is_pure) {
	if (_mincard < s._pure_rank) {
	  return true;
	}
	if (s._pure_rank < _mincard) {
	  return false;
	}
	if (_index_set[_mincard].lexsmaller(s._index_set[s._pure_rank])) {
	  return true;
	}
	else {
	  return false;
	}
      }
      else {
	if (_mincard < s._mincard) {
	  return true;
	}
	if (s._mincard < _mincard) {
	  return false;
	}
	parameter_type minmaxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
	for (parameter_type i = _mincard; i < minmaxcard; ++i) {
	  if (_index_set[i].lexsmaller(s._index_set[i])) {
	    return true;
	  }
	  if (s._index_set[i].lexsmaller(_index_set[i])) {
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
    }
  }

  template<class T>
  const bool SimplicialComplexTemplate<T>::colexgreater(const SimplicialComplexTemplate<T>& s, const parameter_type card) const {
    // this is a specialization where the user can give the hint card to accelerate the comparison;
    // it is assumed without checks that the index sets for cardinality card all exist:
    if (_is_pure) {
      if (s._is_pure) {
	return _index_set[0].colexgreater(s._index_set[0]);
      }
      else {
	return _index_set[0].colexgreater(s._index_set[card]);
      }
    }
    else {
      if (s._is_pure) {
	return _index_set[card].lexsmaller(s._index_set[0]);
      }
      else {
	return _index_set[card].lexsmaller(s._index_set[card]);
      }
    }
  }

  template<class T>
  const bool SimplicialComplexTemplate<T>::colexgreater(const SimplicialComplexTemplate<T>& s) const {
    // here we have to compare without any hint:
    if (empty()) {
      return false;
    }
    if (s.empty()) {
      return true;
    }
    if (_is_pure) {
      if (s._is_pure) {
	if (_pure_rank > s._pure_rank) {
	  return true;
	}
	if (s._pure_rank > _pure_rank) {
	  return false;
	}
	if (_index_set[0].colexgreater(s._index_set[0])) {
	  return true;
	}
	else {
	  return false;
	}
      }
      else {
	if (_pure_rank > s._maxcard - 1) {
	  return true;
	}
	if (s._maxcard - 1 > _pure_rank) {
	  return false;
	}
	if (_index_set[0].colexgreater(s._index_set[s._maxcard - 1])) {
	  return true;
	}
	else {
	  return false;
	}
      }
    }
    else {
      if (s._is_pure) {
	if (_maxcard - 1 > s._pure_rank) {
	  return true;
	}
	if (s._pure_rank < _maxcard - 1) {
	  return false;
	}
	if (_index_set[_maxcard - 1].colexgreater(s._index_set[0])) {
	  return true;
	}
	else {
	  return false;
	}
      }
      else {
	if (_maxcard > s._maxcard) {
	  return true;
	}
	if (s._maxcard > _maxcard) {
	  return false;
	}
	parameter_type maxmincard = _mincard > s._mincard ? _mincard : s._mincard;
	for (parameter_type i = _maxcard; i > maxmincard; --i) {
	  if (_index_set[i - 1].colexgreater(s._index_set[i - 1])) {
	    return true;
	  }
	  if (s._index_set[i - 1].colexgreater(_index_set[i - 1])) {
	    return false;
	  }
	}
	if (_mincard < s._mincard) {
	  return true;
	}
	else {
	  return false;
	}
      }
    }
  }

  // boundary computation:

  template<class T>
  SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::insert_boundary(const Simplex& simp,
									      const parameter_type card) {
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
									   const parameter_type card) {
    if (card + 1 > _maxcard) {
      _maxcard = card + 1;
      resize(_maxcard);
    }
    const parameter_type idx = sct._is_pure? 0 : card;
    const IndexSet index_set_copy(sct._index_set[idx]); // in case (&sct == this)
    for (IndexSet_const_iterator simp_iter = index_set_copy.begin();
	 simp_iter != index_set_copy.end();
	 ++simp_iter) {
      const Simplex simp(sct._index_table.get_obj(card, *simp_iter));
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
  SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::include_all_faces(const parameter_type depth) {
    if (_maxcard < 2) {
      return *this;
    }
#ifdef SUPER_SUPER_VERBOSE
    std::cout << "SimplicialComplexTemplate<T>::include_allfaces(const parameter_type depth):"
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
    for (parameter_type i = _maxcard - 1; i > depth; --i) {
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
    if (_is_pure) {
      return *this;
    }
    if (_maxcard < 3) {
      return *this;
    }
    for (parameter_type i = _maxcard - 2; i + 1 > _mincard; --i) {
      const IndexSet index_set_copy(_index_set[i]);
      for (IndexSet_const_iterator iter = index_set_copy.begin();
	   iter != index_set_copy.end();
	   ++iter) {
	const Simplex simp(_index_table.get_obj(i, *iter));
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
#ifdef COMPUTATIONS_DEBUG
    std::cerr << "copy-assigning " << s << " to *this = " << *this << " ..." << std::endl;
#endif
    if (this == &s) {
      return *this;
    }
    _is_pure = s._is_pure;
    _pure_rank = s._pure_rank;
    _maxcard = s._maxcard;
    resize(_maxcard);
    _mincard = s._mincard;
    for (parameter_type i = _mincard; i < _maxcard; ++i) {
      _index_set[i] = s._index_set[i];
    }
#ifdef COMPUTATIONS_DEBUG
    std::cerr << "... done - result = " << *this << std::endl;
#endif
    return *this;
  }

  template<class T>
  SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator=(SimplicialComplexTemplate<T>&& s) {
#ifdef COMPUTATIONS_DEBUG
    std::cerr << "move-assigning " << s << " to *this = " << *this << " ..." << std::endl;
#endif
    if (this == &s) {
      return *this;
    }
    _is_pure = s._is_pure;
    _pure_rank = s._pure_rank;
    _maxcard = s._maxcard;
    resize(_maxcard);
    _mincard = s._mincard;
    _index_set = std::move(s._index_set);
#ifdef COMPUTATIONS_DEBUG
    std::cerr << "... done - result = " << *this << std::endl;
#endif
    return *this;
  }

  // adding and deleting a simplex:

  template<class T>
  SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::insert(const Simplex& simp, 
								     const parameter_type card) {
#ifdef DEBUG
    if (static_cast<parameter_type>(simp.card()) != card) {
      std::cerr << "SimplicialComplexTemplate<T>::insert(const Simplex&, const parameter_type): "
		<< "wrong cardinality in second parameter. "
		<< "card(" << simp << ") != " << card << std::endl;
      exit(1);
    }
#endif
    if (empty()) {
      _is_pure = true;
      _pure_rank = card;
      _mincard = 0;
      _maxcard = 1;
      resize(_maxcard);
      _index_set[0] += _index_table.get_index(card, simp);
      return *this;
    }
    if (_is_pure) {
      if (_pure_rank == card) {
	_index_set[0] += _index_table.get_index(card, simp);
	return *this;
      }
      else {
	make_non_pure();
      }
    }
    _mincard = _mincard < card ? _mincard : card;
    _maxcard = _maxcard > card + 1 ? _maxcard : card + 1;
    resize(_maxcard);
    _index_set[card] += _index_table.get_index(card, simp);
#ifdef COMPUTATIONS_DEBUG
    if (!contains(simp)) {
      std::cerr << "SimplicialComplexTemplate<T>::insert"
		<< "(const Simplex&, const parameter_type): "
		<< "computational error."
		<< std::endl;
      std::cerr << *this << " does not contain inserted simplex " << simp
		<< std::endl;
      exit(1);
    }
    for (IndexSet_const_iterator test_iter = _index_set[card].begin();
	 test_iter != _index_set[card].end();
	 ++test_iter) {
      if (*test_iter + 1 > cardsize(card)) {
	std::cerr << "SimplicialComplexTemplate<T>::insert"
		  << "(const Simplex&, const parameter_type): "
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
								    const parameter_type card) {
#ifdef DEBUG
    if (static_cast<parameter_type>(simp.card()) != card) {
      std::cerr << "SimplicialComplexTemplate<T>::erase(const Simplex&, const parameter_type): "
		<< "wrong cardinality in second parameter." << std::endl;
    }
#endif
    if (empty()) {
      return *this;
    }
    if (_is_pure) {
      if (card != _pure_rank) {
	return *this;
      }
      else {
	auto finder = _index_table.find(card, simp);
	if (finder == _index_table.end(card)) {
	  return *this;
	}
	else {
	  _index_set[0] -= finder->second;
	  if (_index_set[0].empty()) {
	    return clear();
	  }
	}
      }
    }
    else {
      if ((card + 1 > _maxcard) || (card < _mincard)) {
	return *this;
      }
      else {
	auto finder = _index_table.find(card, simp);
	if (finder == _index_table.end(card)) {
	  return *this;
	}
	else {
	  _index_set[card] -= finder->second;
	}
      }

      // the following is a specialization of minmaxcard_update:
      if (card == _maxcard - 1) {
	while (_maxcard > 0) {
	  if (_index_set[_maxcard - 1].empty()) {
	    --_maxcard;
	  }
	  else {
	    break;
	  }
	}
	if (_maxcard <= card) {
	  resize(_maxcard);
	}
	if (_maxcard == 0) {
	  _mincard = 0;
	}
      }
      else if (card == _mincard) {
	while (_mincard < _maxcard) {
	  if (_index_set[_mincard].empty()) {
	    ++_mincard;
	  }
	  else {
	    break;
	  }
	}	  
      }
      if (_mincard >= _maxcard - 1) {
	make_pure();
      }
    }
    //   minmaxcard_update();
#ifdef COMPUTATIONS_DEBUG
    if (contains(simp)) {
      std::cerr << "SimplicialComplexTemplate<T>::erase"
		<< "(const Simplex&, const parameter_type): "
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
									  const parameter_type card) {
#ifdef DEBUG
    if (static_cast<parameter_type>(simp.card()) != card) {
      std::cerr << "SimplicialComplexTemplate<T>::exclusiveor(const Simplex&, const parameter_type): "
		<< "wrong cardinality in second parameter." << std::endl;
    }
#endif
    if (empty()) {
      _is_pure = true;
      _pure_rank = card;
      _mincard = 0;
      _maxcard = 1;
      resize(_maxcard);
      _index_set[0] ^= _index_table.get_index(card, simp);
      return *this;
    }
    if (_is_pure) {
      if (card == _pure_rank) {
	_index_set[0] ^= _index_table.get_index(card, simp);
	minmaxcard_update();
	return *this;
      }
      else {
	make_non_pure();
      }
    }
    if (_mincard + 1 > _maxcard) {
      _mincard = card;
    }
    if (card + 1 > _maxcard) {
      _maxcard = card + 1;
      resize(_maxcard);
    }
    if (card < _mincard) {
      _mincard = card;
    }
    _index_set[card] ^= _index_table.get_index(card, simp);
    minmaxcard_update();
    return *this;
  }

  // union, difference, and intersection with simplicial complexes:

  template<class T>
  SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator+=(const SimplicialComplexTemplate<T>& s) {
    if (s.empty()) {
      return *this;
    }
    if (empty()) {
      return operator=(s);
    }
    if (_is_pure) {
      if (!s._is_pure) {
	make_non_pure();
      }
      else if (_pure_rank != s._pure_rank) {
	make_non_pure();
      }
    }
    if (_is_pure) {
      _index_set[0] += s._index_set[0];
    }
    else {
      if (s._is_pure) {
	if (_mincard + 1 > _maxcard) {
	  _mincard = s._pure_rank;
	}
	if (_maxcard < s._pure_rank + 1) {
	  _maxcard = s._pure_rank + 1;
	  resize(_maxcard);
	}
	else {
	  _mincard = _mincard < s._pure_rank ? _mincard : s._pure_rank;
	}
	_maxcard = _maxcard > s._pure_rank + 1 ? _maxcard : s._pure_rank + 1;
	_index_set[s._pure_rank] += s._index_set[0];
      }
      else {
	if (_mincard + 1 > _maxcard) {
	  _mincard = s._mincard;
	}
	if (_maxcard < s._maxcard) {
	  _maxcard = s._maxcard;
	  resize(_maxcard);
	}
	else {
	  _mincard = _mincard < s._mincard ? _mincard : s._mincard;
	}
	_maxcard = _maxcard > s._maxcard ? _maxcard : s._maxcard;
	for (parameter_type i = s._mincard; i < s._maxcard; ++i) {
	  _index_set[i] += s._index_set[i];
	}
      }
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
    if (s.empty()) {
      return *this;
    }
    if (empty()) {
      return *this;
    }
    if (_is_pure) {
      if (s._is_pure) {
	if (_pure_rank != s._pure_rank) {
	  return *this;
	}
	else {
	  if ((_index_set[0] -= s._index_set[0]).empty()) {
	    return clear();
	  }
	  else {
	    return *this;
	  }
	}
      }
      else {
	if ((_pure_rank < s._mincard) || (_pure_rank >= s._maxcard)) {
	  return *this;
	}
	else {
	  if ((_index_set[0] -= s._index_set[_pure_rank]).empty()) {
	    return clear();
	  }
	  else {
	    return *this;
	  }
	}
      }
    }
    else {
      if (s._is_pure) {
	if ((s._pure_rank < _mincard) || (s._pure_rank >= _maxcard)) {
	  return *this;
	}
	else {
	  _index_set[s._pure_rank] -= s._index_set[0];
	}
      }
      else {
	parameter_type minmaxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
	parameter_type maxmincard = _mincard > s._mincard ? _mincard : s._mincard;
	if (minmaxcard <= maxmincard) {
	  return *this;
	}
	for (parameter_type i = maxmincard; i < minmaxcard; ++i) {
	  _index_set[i] -= s._index_set[i];
	}
      }
      minmaxcard_update();
      if (_mincard >= _maxcard - 1) {
	make_pure();
      }
    }
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
    if (s.empty()) {
      return clear();
    }
    if (empty()) {
      return *this;
    }
    if (_is_pure) {
      if (s._is_pure) {
	if (_pure_rank != s._pure_rank) {
	  return clear();
	}
	else {
	  if ((_index_set[0] *= s._index_set[0]).empty()) {
	    return clear();
	  }
	  else {
	    return *this;
	  }
	}
      }
      else {
	if ((_pure_rank < s._mincard) || (_pure_rank >= s._maxcard)) {
	  return clear();
	}
	else {
	  if ((_index_set[0] *= s._index_set[_pure_rank]).empty()) {
	    return clear();
	  }
	  else {
	    return *this;
	  }
	}
      }
    }
    else {
      if (s._is_pure) {
	if ((s._pure_rank < _mincard) || (s._pure_rank >= _maxcard)) {
	  return clear();
	}
	else {
	  _index_set[s._pure_rank] *= s._index_set[0];
	  if (_index_set[s._pure_rank].empty()) {
	    return clear();
	  }
	  else {
	    if (s._pure_rank > 0) {
	      _index_set[0] = std::move(_index_set[s._pure_rank]);
	    }
	    _mincard = 0;
	    _maxcard = 1;
	    resize(_maxcard);
	    _pure_rank = s._pure_rank;
	    _is_pure = true;
	  }
	}      
      }
      else {
	parameter_type minmaxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
	parameter_type maxmincard = _mincard > s._mincard ? _mincard : s._mincard;
	if (minmaxcard <= maxmincard) {
	  return clear();
	}
	for (parameter_type i = _mincard; i < maxmincard; ++i) {
	  _index_set[i].clear();
	}
	for (parameter_type i = maxmincard; i < minmaxcard; ++i) {
	  _index_set[i] *= s._index_set[i];
	}
	for (parameter_type i = minmaxcard; i < _maxcard; ++i) {
	  _index_set[i].clear();
	}
	_maxcard = minmaxcard;
	_mincard = maxmincard;
	minmaxcard_update();
	if (_mincard >= _maxcard - 1) {
	  make_pure();
	}
      }
    }
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
    if (s.empty()) {
      return *this;
    }
    if (empty()) {
      return operator=(s);
    }
    if (_is_pure) {
      if (!s._is_pure) {
	make_non_pure();
      }
      else if (_pure_rank != s._pure_rank) {
	make_non_pure();
      }
    }
    if (_is_pure) {
      _index_set[0] ^= s._index_set[0];    
    }
    else {
      if (s._is_pure) {
	if (_mincard + 1 > _maxcard) {
	  _mincard = s._pure_rank;
	}
	if (_maxcard < s._pure_rank + 1) {
	  _maxcard = s._pure_rank + 1;
	  resize(_maxcard);
	}
	else {
	  _mincard = _mincard < s._pure_rank ? _mincard : s._pure_rank;
	}
	_maxcard = _maxcard > s._pure_rank + 1 ? _maxcard : s._pure_rank + 1;
	_index_set[s._pure_rank] ^= s._index_set[0];
      }
      else {
	if (_mincard + 1 > _maxcard) {
	  _mincard = s._mincard;
	}
	if (_maxcard < s._maxcard) {
	  _maxcard = s._maxcard;
	  resize(_maxcard);
	}
	else {
	  _mincard = _mincard < s._mincard ? _mincard : s._mincard;
	}
	_maxcard = _maxcard > s._maxcard ? _maxcard : s._maxcard;
	for (parameter_type i = s._mincard; i < s._maxcard; ++i) {
	  _index_set[i] ^= s._index_set[i];
	}
      }
    }
    minmaxcard_update();
    if (_mincard >= _maxcard - 1) {
      make_pure();
    }
    return *this;
  }

  // template<class T>
  // SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator+(const SimplicialComplexTemplate<T>& s1) const {
  //   SimplicialComplexTemplate<T> s(*this);
  //   s += s1;
  //   return s;
  // }

  // template<class T>
  // SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator-(const SimplicialComplexTemplate<T>& s1) const {
  //   SimplicialComplexTemplate<T> s(*this);
  //   s -= s1;
  //   return s;
  // }

  // template<class T>
  // SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator*(const SimplicialComplexTemplate<T>& s1) const {
  //   SimplicialComplexTemplate<T> s(*this);
  //   s *= s1;
  //   return s;
  // }

  // template<class T>
  // SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator^(const SimplicialComplexTemplate<T>& s1) const {
  //   SimplicialComplexTemplate<T> s(*this);
  //   s ^= s1;
  //   return s;
  // }
 
  template<class T>
  SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator+(const SimplicialComplexTemplate& s) const {

    // check for trivial empty result:
    if (empty()) {
      return s;
    }
    if (s.empty()) {
      return *this;
    }

    // from now on we can assume that both operands are non-empty:
    SimplicialComplexTemplate<T> result;

    if (_is_pure) {
      if (s._is_pure) {
	if (_pure_rank == s._pure_rank) {
	  result._pure_rank = _pure_rank;
	  result._mincard = 0;
	  result._maxcard = 1;
	  result.resize(result._maxcard);
	  result._index_set[0] = _index_set[0] + s._index_set[0];
	}
	else {
	  result._is_pure = false;
	  result._pure_rank = -2;
	  result._mincard = _pure_rank < s._pure_rank ? _pure_rank : s._pure_rank;
	  result._maxcard = (_pure_rank > s._pure_rank ? _pure_rank : s._pure_rank) + 1;
	  result.resize(result._maxcard);
	  result._index_set[_pure_rank] = _index_set[0];
	  result._index_set[s._pure_rank] = s._index_set[0];
	}
      }
      else {
	result._is_pure = false;
	result._pure_rank = -2;
      
	// determine the potential size of the result:      
	result._mincard = s._mincard < _pure_rank ? s._mincard : _pure_rank;
	result._maxcard = s._maxcard > _pure_rank + 1 ? s._maxcard : _pure_rank + 1;      
	result.resize(result._maxcard);
      
	// assign the corresponding indexsets:
	for (parameter_type i = s._mincard; i < _pure_rank; ++i) {
	  result._index_set[i] = s._index_set[i];
	}
	result._index_set[_pure_rank] = _index_set[0] + s._index_set[_pure_rank];
	for (parameter_type i = _pure_rank + 1; i < s._maxcard; ++i) {
	  result._index_set[i] = s._index_set[i];
	}
      }
    }
    else {
      if (s._is_pure) {
	result._is_pure = false;
	result._pure_rank = -2;
      
	// determine the potential size of the result:      
	result._mincard = _mincard < s._pure_rank ? _mincard : s._pure_rank;
	result._maxcard = _maxcard > s._pure_rank + 1 ? _maxcard : s._pure_rank + 1;      
	result.resize(result._maxcard);
      
	// assign the corresponding indexsets:
	for (parameter_type i = _mincard; i < s._pure_rank; ++i) {
	  result._index_set[i] = _index_set[i];
	}
	result._index_set[s._pure_rank] = _index_set[s._pure_rank] + s._index_set[0];
	for (parameter_type i = s._pure_rank + 1; i < _maxcard; ++i) {
	  result._index_set[i] = _index_set[i];
	}
      }
      else {
	result._is_pure = false;
	result._pure_rank = -2;
     
	// determine the potential size of the result:
	parameter_type minmaxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
	parameter_type maxmincard = _mincard > s._mincard ? _mincard : s._mincard;
      
	result._maxcard = _maxcard > s._maxcard ? _maxcard : s._maxcard;
	result._mincard = _mincard < s._mincard ? _mincard : s._mincard;
	result.resize(result._maxcard);
      
	// assign the corresponding indexsets:
	for (parameter_type i = _mincard; i < s._mincard; ++i) {
	  result._index_set[i] = _index_set[i];
	}
	for (parameter_type i = s._mincard; i < _mincard; ++i) {
	  result._index_set[i] = s._index_set[i];
	}
	for (parameter_type i = maxmincard; i < minmaxcard; ++i) {
	  result._index_set[i] = _index_set[i] + s._index_set[i];
	}
	for (parameter_type i = s._maxcard; i < _maxcard; ++i) {
	  result._index_set[i] = _index_set[i];
	}
	for (parameter_type i = _maxcard; i < s._maxcard; ++i) {
	  result._index_set[i] = s._index_set[i];
	}
      }
    }
      
    return result;
  }

  template<class T>
  SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator-(const SimplicialComplexTemplate& s) const {

    // check for trivial empty result:
    if (empty() || s.empty()) {
      return *this;
    }

    // from now on we can assume that both operands are non-empty:
    SimplicialComplexTemplate<T> result;
  
    if (_is_pure) {
      if (s._is_pure) {
	if (_pure_rank != s._pure_rank) {
	  return *this;
	}
	else {
	  IndexSet tmp(_index_set[0] - s._index_set[0]);
	  if (tmp.empty()) {
	    return result;
	  }
	  else {
	    result._pure_rank = _pure_rank;
	    result._mincard = 0;
	    result._maxcard = 1;
	    result.resize(_maxcard);
	    result._index_set[0] = std::move(tmp);
	  }
	}
      }
      else {
	if ((_pure_rank < s._mincard) || (_pure_rank >= s._maxcard)) {
	  return *this;
	}
	else {
	  IndexSet tmp(_index_set[0] - s._index_set[_pure_rank]);
	  if (tmp.empty()) {
	    return result;
	  }
	  else {
	    result._pure_rank = _pure_rank;
	    result._mincard = 0;
	    result._maxcard = 1;
	    result.resize(result._maxcard);
	  
	    // assign the corresponding indexsets:
	    result._index_set[0] = std::move(tmp);
	  }
	}
      }
    }
    else {
      if (s._is_pure) {
	if ((s._pure_rank < _mincard) || (s._pure_rank >= _maxcard)) {
	  return *this;
	}
	else {
	  result._is_pure = false;
	  result._pure_rank = -2;
	  result._mincard = _mincard;
	  result._maxcard = _maxcard;
	  result.resize(result._maxcard);
	
	  // assign the corresponding indexsets:
	  for (parameter_type i = result._mincard; i < s._pure_rank; ++i) {
	    result._index_set[i] = _index_set[i];
	  }
	  result._index_set[s._pure_rank] = _index_set[s._pure_rank] - s._index_set[0];
	  for (parameter_type i = s._pure_rank + 1; i < _maxcard; ++i) {
	    result._index_set[i] = _index_set[i];
	  }
	}
      }
      else {
	result._is_pure = false;
	result._pure_rank = -2;
	result._mincard = _mincard < s._mincard ? _mincard : s._mincard;
	result._maxcard = _maxcard > s._maxcard ? _maxcard : s._maxcard;
	result.resize(result._maxcard);
      
	// determine overlap of the result:
	parameter_type minmaxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
	parameter_type maxmincard = _mincard > s._mincard ? _mincard : s._mincard;
      
	// assign the corresponding indexsets:
	for (parameter_type i = _mincard; i < maxmincard; ++i) {
	  result._index_set[i] = _index_set[i];
	}
	for (parameter_type i = maxmincard; i < minmaxcard; ++i) {
	  result._index_set[i] = _index_set[i] - s._index_set[i];
	}
	for (parameter_type i = minmaxcard; i < _maxcard; ++i) {
	  result._index_set[i] = _index_set[i];
	}
      }
    
      // determine the size of the result again:
      result.minmaxcard_update();
      if (result._mincard >= result._maxcard - 1) {
	result.make_pure();
      }
    }

    return result;
  }

  template<class T>
  SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator*(const SimplicialComplexTemplate& s) const {

    // check for trivial empty result:
    if (empty()) {
      return *this;
    }
    if (s.empty()) {
      return s;
    }

    // from now on we can assume that both operands are non-empty:
    SimplicialComplexTemplate<T> result;

    if (_is_pure) {
      if (s._is_pure) {
	if (_pure_rank != s._pure_rank) {
	  return result;
	}
	else {
	  IndexSet tmp(_index_set[0] * s._index_set[0]);
	  if (tmp.empty()) {
	    return result;
	  }
	  else {
	    result._pure_rank = _pure_rank;
	    result._mincard = 0;
	    result._maxcard = 1;
	    result.resize(_maxcard);
	    result._index_set[0] = std::move(tmp);
	  }
	}
      }
      else {
	if ((_pure_rank < s._mincard) || (_pure_rank >= s._maxcard)) {
	  return result;
	}
	else {
	  IndexSet tmp(_index_set[0] * s._index_set[_pure_rank]);
	  if (tmp.empty()) {
	    return result;
	  }
	  else {
	    result._pure_rank = _pure_rank;
	    result._mincard = 0;
	    result._maxcard = 1;
	    result.resize(_maxcard);
	    result._index_set[0] = std::move(tmp);
	  }
	}
      }
    }
    else {
      if (s._is_pure) {
	if ((s._pure_rank < _mincard) || (s._pure_rank >= _maxcard)) {
	  return result;
	}
	else {
	  IndexSet tmp(_index_set[s._pure_rank] * s._index_set[0]);
	  if (tmp.empty()) {
	    return result;
	  }
	  else {
	    result._pure_rank = s._pure_rank;
	    result._mincard = 0;
	    result._maxcard = 1;
	    result.resize(_maxcard);
	    result._index_set[0] = std::move(tmp);
	  }
	}
      }
      else {
	result._is_pure = false;
	result._pure_rank = -2;
      
	// determine the potential size of the result:
	result._mincard = _mincard > s._mincard ? _mincard : s._mincard;
	result._maxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
	if (result._mincard >= result._maxcard) {
	  return result;
	}
	else if (result._mincard == result._maxcard - 1) {

	  // the result is actually pure:
	  IndexSet tmp(_index_set[result._mincard] * s._index_set[result._mincard]);
	  if (tmp.empty()) {
	    return result;
	  }
	  else {
	    result._is_pure = true;
	    result._mincard = 0;
	    result._maxcard = 1;
	    result._pure_rank = result._maxcard - 1;
	    result.resize(result._maxcard);
	    result._index_set[0] = std::move(tmp);
	  }
	}
	else {
	  result._is_pure = false;
	  result._pure_rank = -2;
	  result.resize(result._maxcard);
	
	  // assign the corresponding indexsets:
	  for (parameter_type i = result._mincard; i < result._maxcard; ++i) {
	    result._index_set[i] = _index_set[i] * s._index_set[i];
	  }

	  // determine the size of the result again:
	  result.minmaxcard_update();
	  if (result._mincard >= result._maxcard - 1) {
	    result.make_pure();
	  }
	}
      }
    }
  
  
    return result;
  }

  template<class T>
  SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::operator^(const SimplicialComplexTemplate& s) const {

    // check for trivial empty result:
    if (empty()) {
      return s;
    }
    if (s.empty()) {
      return *this;
    }

    // from now on we can assume that both operands are non-empty:
    SimplicialComplexTemplate<T> result;

    if (_is_pure) {
      if (s._is_pure) {
	if (_pure_rank == s._pure_rank) {
	  IndexSet tmp(_index_set[0] ^ s._index_set[0]);
	  if (tmp.empty()) {
	    return result;
	  }
	  else {
	    result._pure_rank = _pure_rank;
	    result._mincard = 0;
	    result._maxcard = 1;
	    result.resize(result._maxcard);
	    result._index_set[0] = std::move(tmp);
	  }	
	}
	else {
	  result._is_pure = false;
	  result._pure_rank = -2;
	  result._mincard = _pure_rank < s._pure_rank ? _pure_rank : s._pure_rank;
	  result._maxcard = (_pure_rank > s._pure_rank ? _pure_rank : s._pure_rank) + 1;
	  result.resize(result._maxcard);
	  result._index_set[_pure_rank] = _index_set[0];
	  result._index_set[s._pure_rank] = s._index_set[0];
	}
      }
      else {
	result._is_pure = false;
	result._pure_rank = -2;
      
	// determine the potential size of the result:
	result._mincard = s._mincard < _pure_rank ? s._mincard : _pure_rank;
	result._maxcard = s._maxcard > _pure_rank + 1 ? s._maxcard : _pure_rank + 1;
	result.resize(result._maxcard);
      
	// assign the corresponding indexsets:
	for (parameter_type i = s._mincard; i < _pure_rank; ++i) {
	  result._index_set[i] = s._index_set[i];
	}
	result._index_set[_pure_rank] = s._index_set[_pure_rank] ^ _index_set[0];
	for (parameter_type i = _pure_rank + 1; i < s._maxcard; ++i) {
	  result._index_set[i] = s._index_set[i];
	}
      }
    }
    else {
      if (s._is_pure) {
	result._is_pure = false;
	result._pure_rank = -2;
      
	// determine the potential size of the result:
	result._mincard = _mincard < s._pure_rank ? _mincard : s._pure_rank;
	result._maxcard = _maxcard > s._pure_rank + 1 ? _maxcard : s._pure_rank + 1;
	result.resize(result._maxcard);
      
	// assign the corresponding indexsets:
	for (parameter_type i = _mincard; i < s._pure_rank; ++i) {
	  result._index_set[i] = _index_set[i];
	}
	result._index_set[s._pure_rank] = _index_set[s._pure_rank] ^ s._index_set[0];
	for (parameter_type i = s._pure_rank + 1; i < _maxcard; ++i) {
	  result._index_set[i] = _index_set[i];
	}
      }
      else {

	// determine the potential size of the result:
	parameter_type minmaxcard = _maxcard < s._maxcard ? _maxcard : s._maxcard;
	parameter_type maxmincard = _mincard > s._mincard ? _mincard : s._mincard;
      
	result._mincard = _mincard < s._mincard ? _mincard : s._mincard;
	result._maxcard = _maxcard > s._maxcard ? _maxcard : s._maxcard;
	result.resize(result._maxcard);
      
	// assign the corresponding indexsets:
	for (parameter_type i = _mincard; i < s._mincard; ++i) {
	  result._index_set[i] = _index_set[i];
	}
	for (parameter_type i = s._mincard; i < _mincard; ++i) {
	  result._index_set[i] = s._index_set[i];
	}
	for (parameter_type i = maxmincard; i < minmaxcard; ++i) {
	  result._index_set[i] = _index_set[i] ^ s._index_set[i];
	}
	for (parameter_type i = s._maxcard; i < _maxcard; ++i) {
	  result._index_set[i] = _index_set[i];
	}
	for (parameter_type i = _maxcard; i < s._maxcard; ++i) {
	  result._index_set[i] = s._index_set[i];
	}
      }
    }

    // determine the size of the result again:
    result.minmaxcard_update();
    if (result._mincard >= result._maxcard - 1) {
      result.make_pure();
    }
  
    return result;
  }

  // join:

  template<class T>
  SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::join(const Simplex& simp,
								  const parameter_type card) const {
    if (empty()) {
      return *this;
    }
    if (simp.empty()) {
      return *this;
    }
    SimplicialComplexTemplate<T> result;
    if (_is_pure) {
      for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter = 
	     _index_set[0].begin();
	   iter != _index_set[0].end();
	   ++iter) {
	const Simplex s(_index_table.get_obj(_pure_rank, *iter));
#ifdef COMPUTATIONS_DEBUG
	if (!s.disjoint(simp))
	  std::cerr << "join(const SimplicialComplexTemplate<T>&, const Simplex& simp, "
		    << "const parameter_type card): "
		    << "simplices not disjoint." << std::endl;
      
#endif
	result.insert(s + simp, card + _pure_rank);
      }
    }
    else {
      for (parameter_type i = _mincard; i < _maxcard; ++i) {
	for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter = 
	       _index_set[i].begin();
	     iter != _index_set[i].end();
	     ++iter) {
	  const Simplex s(_index_table.get_obj(i, *iter));
#ifdef COMPUTATIONS_DEBUG
	  if (!s.disjoint(simp))
	    std::cerr << "join(const SimplicialComplexTemplate<T>&, const Simplex& simp, "
		      << "const parameter_type card): "
		      << "simplices not disjoint." << std::endl;
	
#endif
	  result.insert(s + simp, card + i);
	}
      }
    }
    return result;
  }

  template<class T>
  SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::join(const SimplicialComplexTemplate<T>& s) const {
    SimplicialComplexTemplate<T> result;
    if (empty() || s.empty()) {
      return result;
    }
    if (dim() == -1) {
      return s;
    }
    if (s.dim() == -1) {
      return *this;
    }
    if (_is_pure) {
      if (s._is_pure) {
	for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter1 = 
	       _index_set[0].begin();
	     iter1 != _index_set[0].end();
	     ++iter1) {
	  const Simplex simp1(_index_table.get_obj(_pure_rank, *iter1));
	  for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter2 = s._index_set[0].begin();
	       iter2 != s._index_set[0].end();
	       ++iter2) {
	    const Simplex simp2(s._index_table.get_obj(s._pure_rank, *iter2));
#ifdef COMPUTATIONS_DEBUG
	    if (!simp1.disjoint(simp2)) {
	      std::cerr << "SimplicialComplexTemplate<T>::join(const SimplicialComplexTemplate<T>&): "
			<< simp1 << " not disjoint from " << simp2 << std::endl;
	      exit(1);
	    }
#endif
	    result.insert(simp1 + simp2, _pure_rank + s._pure_rank);
	  }
	}
      }
      else {
	for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter1 = 
	       _index_set[0].begin();
	     iter1 != _index_set[0].end();
	     ++iter1) {
	  const Simplex simp1(_index_table.get_obj(_pure_rank, *iter1));
	  for (parameter_type j = s._maxcard; j > s._mincard; --j) {
	    for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter2 = s._index_set[j - 1].begin();
		 iter2 != s._index_set[j - 1].end();
		 ++iter2) {
	      const Simplex simp2(s._index_table.get_obj(j - 1, *iter2));
#ifdef COMPUTATIONS_DEBUG
	      if (!simp1.disjoint(simp2)) {
		std::cerr << "SimplicialComplexTemplate<T>::join(const SimplicialComplexTemplate<T>&): "
			  << simp1 << " not disjoint from " << simp2 << std::endl;
		exit(1);
	      }
#endif
	      result.insert(simp1 + simp2, _pure_rank + j - 1);
	    }
	  }
	}
      }
    }
    else {
      if (s._is_pure) {
	for (parameter_type i = _maxcard; i > _mincard; --i) {
	  for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter1 = 
		 _index_set[i - 1].begin();
	       iter1 != _index_set[i - 1].end();
	       ++iter1) {
	    const Simplex simp1(_index_table.get_obj(_pure_rank, *iter1));
	    for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter2 = s._index_set[0].begin();
		 iter2 != s._index_set[0].end();
		 ++iter2) {
	      const Simplex simp2(s._index_table.get_obj(s._pure_rank, *iter2));
#ifdef COMPUTATIONS_DEBUG
	      if (!simp1.disjoint(simp2)) {
		std::cerr << "SimplicialComplexTemplate<T>::join(const SimplicialComplexTemplate<T>&): "
			  << simp1 << " not disjoint from " << simp2 << std::endl;
		exit(1);
	      }
#endif
	      result.insert(simp1 + simp2, i - 1 + _pure_rank);
	    }
	  }
	}
      }
      else {
	for (parameter_type i = _maxcard; i > _mincard; --i) {
	  for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter1 = 
		 _index_set[i - 1].begin();
	       iter1 != _index_set[i - 1].end();
	       ++iter1) {
	    const Simplex simp1(_index_table.get_obj(i - 1, *iter1));
	    for (parameter_type j = s._maxcard; j > s._mincard; --j) {
	      for (typename SimplicialComplexTemplate<T>::IndexSet_const_iterator iter2 = s._index_set[j - 1].begin();
		   iter2 != s._index_set[j - 1].end();
		   ++iter2) {
		const Simplex simp2(s._index_table.get_obj(j - 1, *iter2));
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
    if (!(ist >> std::ws >> c)) {
      ist.clear(std::ios::failbit);
      return ist;
    }
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

}; // namespace topcom

#endif

// eof SimplicialComplexTemplate.hh
