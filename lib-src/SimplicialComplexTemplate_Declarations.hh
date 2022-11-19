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
#include <vector>

#include "Permutation.hh"
#include "LabelSet.hh"
#include "IndexTableArray.hh"
#include "Chirotope.hh"
#include "Volumes.hh"

namespace topcom {

  template<class T>
  class SimplicialComplexTemplate;

  template<class T>
  inline std::istream& operator>>(std::istream& ist, SimplicialComplexTemplate<T>& st);

  template<class T>
  inline std::ostream& operator<<(std::ostream& ost, const SimplicialComplexTemplate<T>& st);

  template<class T>
  class __sc_const_iterator;

  template<class T>
  class SimplicialComplexTemplate {
  public:
    // save the empty index set:
    static const T                         empty_indexset;
    static const SimplicialComplexTemplate empty_complex;
    // reset thte index table (invalidates all current simplicial complexes!):
    static void reset_index_table     ();
    
    // preprocess the index table (good for multi-threading)
    static void preprocess_index_table(const parameter_type,
				       const parameter_type,
				       const parameter_type);
    static void preprocess_index_table(const parameter_type,
				       const parameter_type,
				       const parameter_type,
				       const Volumes&,
				       bool = false);
    static void preprocess_index_table(const parameter_type,
				       const parameter_type,
				       const parameter_type,
				       const Chirotope&,
				       bool = false);
    static const bool preprocessed(const parameter_type);
    static const bool preprocessed();
    static void       set_preprocessed(const bool);
    static void       start_multithreading();
    static void       stop_multithreading();
    typedef typename T::iterator         IndexSet_iterator;
    typedef typename T::const_iterator   IndexSet_const_iterator;
  public:
    typedef T                            IndexSet;
  public:
    friend class VertexFacetTable;
    friend class FaceFacetTableArray;
    friend class FreeFaceTableArray;
  public:
    static const size_type init_capacity;
    static const size_type init_size;
  protected:
#ifndef STL_CONTAINERS
    typedef Array<T>       IndexSetArray;
#else
    typedef std::vector<T> IndexSetArray;
#endif
  public:
    friend class __sc_const_iterator<T>;
  public:
    typedef __sc_const_iterator<T>  const_iterator;
    typedef const_iterator          iterator; // no non-const iterators
  private:
    // for a pure complex, we only need a single index set
    // together with the information about the rank
    // (-1 for empty complex, ignored but mostly set to -2 for non-pure complex):
    bool                            _is_pure;
    parameter_type                  _pure_rank;
    // for a non-pure complex, we need to have multiple index sets:
    parameter_type                  _mincard;     // minimum cardinality of a simplex
    parameter_type                  _maxcard;     // maximum cardinality of a simplex plus one
    IndexSetArray                   _index_set;   // index set for each card
    static IndexTableArray          _index_table; // common index table for each card
  private:
    void                  minmaxcard_update();
    inline parameter_type resize(const parameter_type);
    inline void           make_non_pure();
    inline void           make_pure();
  public:
    // constructors:
    inline SimplicialComplexTemplate();
    inline SimplicialComplexTemplate(const SimplicialComplexTemplate&);
    inline SimplicialComplexTemplate(SimplicialComplexTemplate&&);
    inline explicit SimplicialComplexTemplate(const Simplex&,
					      const parameter_type);
    inline explicit SimplicialComplexTemplate(const Simplex&);
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
    inline const  bool             pure()           const { return _is_pure; }
    inline const  parameter_type   pure_rank()      const { return _pure_rank; }
    inline const  parameter_type   mincard()        const { return _mincard; }
    inline const  parameter_type   maxcard()        const { return _maxcard; }
    inline const  IndexSetArray&   index_set()      const { return _index_set; }
    inline static IndexTableArray& index_table()          { return _index_table; }
    inline const  IndexSet&        index_set_pure() const {
      if (empty()) {
	return empty_indexset;
      } else {
	return _index_set[0];
      }
    }
    inline const  IndexSet&        index_set(const parameter_type n) const {
      if (_is_pure) {
	if (n != _pure_rank) {
	  std::cerr << *this << ":" << std::endl;
	  std::cerr << "n = " << n << " != " << _pure_rank << " = _pure_rank - exiting" << std::endl;
	  exit(1);
	}
	else {
	  return _index_set[0];
	}
      }
      else {
	if ((n < _mincard) || (n >= _maxcard)) {
	  std::cerr << *this << ":" << std::endl;
	  std::cerr << "n = " << n << " not in " << "[" << _mincard << "," << _maxcard << ") = [_mincard, _maxcard) - exiting" << std::endl;
	  exit(1);
	}
	else {
	  return _index_set[n];
	}
      }
    }
    static        IndexTable&      index_table(const parameter_type n) { return _index_table[n]; }

    // make the internal indices usable from outside (only in mode "preprocessed"):
    inline static size_type   no_of_simplices(const parameter_type card)  { return _index_table[card].size(); }
    static        size_type   index_of_simplex(const Simplex& simp, const parameter_type card) {
      if (!preprocessed(card)) {
	std::cerr << "SimplicalComplex::index_of_simplex(const Simplex&, const parameter_type): "
		  << "Error - index retrieval not allowed if not preprocessed - exiting."
		  << std:: endl;
	exit(1);
      }
      return _index_table[card].get_index(simp);
    }
    static       size_type   index_of_simplex(const Simplex& simp)       { return index_of_simplex(simp, static_cast<parameter_type>(simp.card())); }
    static       Simplex     simplex_of_index(const size_type index, const parameter_type card) {
      return _index_table[card].get_obj(index);
    }
    inline void replace_indexset(const T& index_set, const parameter_type card) {

      _is_pure = true;
      _maxcard = 1;
      _pure_rank = card;
      resize(_maxcard);
      _index_set[0] = index_set;
    }
    inline void replace_indexset(T&& index_set, const parameter_type card) {
      _is_pure = true;
      _maxcard = 1;
      _pure_rank = card;
      resize(_maxcard);
      _index_set[0] = std::move(index_set);
    }

    // status information:
    const bool           empty() const { return (_maxcard == 0); }
    const parameter_type rank()  const {
      if (_is_pure) {
	return _pure_rank;
      }
      else {
	return _maxcard - 1;
      }
    }
    const int dim() const { return rank() - 1; }

    inline const size_type card(const parameter_type n) const;
    const size_type        card() const;
    
    const bool        contains(const Simplex& simp, const parameter_type card) const;
    inline const bool contains(const Simplex& simp) const;
    const bool        superset(const SimplicialComplexTemplate& sc) const;
    inline const bool subset(const SimplicialComplexTemplate& sc) const;
    const bool        disjoint(const SimplicialComplexTemplate& sc) const;

    const bool        contains_face(const Simplex& simp, const parameter_type card) const;
    inline const bool contains_face(const Simplex& simp) const;
    const bool        contains_face(const Simplex& simp,
				    const parameter_type card, 
				    Simplex& facet) const;
    inline const bool contains_face(const Simplex& simp, Simplex& facet) const;

    const bool        contains_free_face(const Simplex& simp,
					 const parameter_type card,
					 Simplex& facet) const;
    inline const bool contains_free_face(const Simplex& simp,
					 Simplex& facet) const;
    // standard operations on simplicial complexes:
    const Simplex                     support() const;
    SimplicialComplexTemplate         star(const Simplex& simp, const parameter_type card) const;
    inline SimplicialComplexTemplate  star(const Simplex& simp) const;
    SimplicialComplexTemplate         link(const Simplex& simp, const parameter_type card) const;
    inline SimplicialComplexTemplate  link(const Simplex& simp) const;
    SimplicialComplexTemplate&        deletion(const Simplex& simp, const parameter_type card);
    inline SimplicialComplexTemplate& deletion(const Simplex& simp);
  
    // extract all simplices with given cardinality:
    const SimplicialComplexTemplate operator[](const parameter_type) const;

    // skeleton:
    inline SimplicialComplexTemplate skeleton(const parameter_type);

    // boolean operators:
    const bool        operator==  (const SimplicialComplexTemplate&) const;
    inline const bool operator!=  (const SimplicialComplexTemplate&) const;
    const bool        operator<   (const SimplicialComplexTemplate&) const;
    inline const bool operator>   (const SimplicialComplexTemplate&) const;

    // the following lex-orders are w.r.t. the index sets, not the point labels:
    const bool        lexsmaller  (const SimplicialComplexTemplate&, const parameter_type) const;
    const bool        lexsmaller  (const SimplicialComplexTemplate&) const;
    const bool        colexgreater(const SimplicialComplexTemplate&, const parameter_type) const;
    const bool        colexgreater(const SimplicialComplexTemplate&) const;
  
    // modifiers:
    inline SimplicialComplexTemplate& clear();   // erase all

    SimplicialComplexTemplate&        insert_boundary(const Simplex&, const parameter_type);
    inline SimplicialComplexTemplate& insert_boundary(const Simplex&);
    SimplicialComplexTemplate&        insert_faces(const SimplicialComplexTemplate&, const parameter_type);
    SimplicialComplexTemplate&        include_all_faces(const parameter_type = 1);
    inline SimplicialComplexTemplate  faces(const parameter_type = 1);
    SimplicialComplexTemplate&        reduce_to_facets(); // only keep facets
  
    // assignment:
    SimplicialComplexTemplate& operator=(const SimplicialComplexTemplate&);
    SimplicialComplexTemplate& operator=(SimplicialComplexTemplate&&);
  
    // adding and deleting a simplex:
    inline SimplicialComplexTemplate& operator+=(const Simplex&);
    inline SimplicialComplexTemplate& insert(const Simplex&);
    SimplicialComplexTemplate&        insert(const Simplex&, const parameter_type);
    inline SimplicialComplexTemplate& operator-=(const Simplex&);
    inline SimplicialComplexTemplate& erase(const Simplex&);
    SimplicialComplexTemplate&        erase(const Simplex&, const parameter_type);
    inline SimplicialComplexTemplate& operator^=(const Simplex&);
    inline SimplicialComplexTemplate& exclusiveor(const Simplex&);
    SimplicialComplexTemplate&        exclusiveor(const Simplex&, const parameter_type);
  
    // union, difference, and intersection with simplicial complexes:
    SimplicialComplexTemplate& operator+=(const SimplicialComplexTemplate&);
    SimplicialComplexTemplate& operator-=(const SimplicialComplexTemplate&);
    SimplicialComplexTemplate& operator*=(const SimplicialComplexTemplate&);
    SimplicialComplexTemplate& operator^=(const SimplicialComplexTemplate&);
  
    // the same but a new set is returned:
    inline SimplicialComplexTemplate operator+(const Simplex&) const;
    inline SimplicialComplexTemplate insert(const Simplex&)    const;
    inline SimplicialComplexTemplate insert(const Simplex&,
					    const parameter_type)   const;
    inline SimplicialComplexTemplate operator-(const Simplex&) const;
    inline SimplicialComplexTemplate erase(const Simplex&)     const;
    inline SimplicialComplexTemplate erase(const Simplex&,
					   const parameter_type)    const;
    inline SimplicialComplexTemplate operator^(const Simplex&) const;
    inline SimplicialComplexTemplate exclusiveor(const Simplex&)       const;
    inline SimplicialComplexTemplate exclusiveor(const Simplex&,
						 const parameter_type)      const;
    SimplicialComplexTemplate        operator+(const SimplicialComplexTemplate&) const;
    SimplicialComplexTemplate        operator-(const SimplicialComplexTemplate&) const;
    SimplicialComplexTemplate        operator*(const SimplicialComplexTemplate&) const;
    SimplicialComplexTemplate        operator^(const SimplicialComplexTemplate&) const;
  
    // join:
    SimplicialComplexTemplate        join(const Simplex&, const parameter_type)  const;
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
    parameter_type                       _current_card;
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
    _current_indexset_iter(nullptr) {}

  template<class T>
  inline __sc_const_iterator<T>::~__sc_const_iterator() {
    if (_current_indexset_iter) {
      // iter_allocator.destroy(_current_indexset_iter);
      std::allocator_traits<std::allocator<IndexSet_const_iterator> >::destroy(iter_allocator,
									       _current_indexset_iter);
      iter_allocator.deallocate(_current_indexset_iter, 1);
    }
  }

  template<class T>
  inline const bool __sc_const_iterator<T>::operator!=(const __sc_const_iterator& iter) const {
    return !((*this) == iter);
  }

  template<class T>
  inline const Simplex& __sc_const_iterator<T>::operator*() const {
#ifdef SCITER_DEBUG
    std::cerr << "inline const Simplex& __sc_const_iterator<T>::operator*() const: dereferencing _current_indexset_iter at "
	      << _current_indexset_iter << " ..."
	      << std::endl;
#endif
#ifdef INDEXTABLE_DEBUG
    if (CommandlineOptions::verbose()) {
      const parameter_type card = _container->_is_pure ? _container->_pure_rank : _current_card;
      std::cerr << "dereferencing iterator pointing to container "
		<< _container << " with _index_table address "
		<< &_container->_index_table << " and _index_table of current_card "
		<< _container->_index_table[card] << " ..." << std::endl;
      std::cerr << "pointing to objects in "
		<< _container->_index_table[card]._index_data << std::endl;
      std::cerr << "using _current_indexset_iter with address "
		<< _current_indexset_iter << " and value "
		<< **_current_indexset_iter << " ..." << std::endl;
      std::cerr << "picking "
		<< _container->_index_table[card]._index_data[**_current_indexset_iter] << std::endl;
    }
#endif
    if (_container->_is_pure) {
      const Simplex& result(_container->_index_table.get_obj(_container->_pure_rank, **_current_indexset_iter));
#ifdef SCITER_DEBUG
      std::cerr << "... done." << std::endl;
#endif
      return result;
    }
    else {
      const Simplex& result(_container->_index_table.get_obj(_current_card, **_current_indexset_iter));
  
#ifdef INDEXTABLE_DEBUG
      if (CommandlineOptions::verbose()) {
	std::cerr << "... done" << std::endl;
      }
      if (CommandlineOptions::verbose()) {
	std::cerr << "returning reference with address " << &result
		  << " and value " << result << std::endl;
      }
#endif
#ifdef SCITER_DEBUG
      std::cerr << "... done." << std::endl;
#endif
      return result;
    }
  }

  template<class T>
  inline const Simplex* __sc_const_iterator<T>::operator->() const {
#ifdef SCITER_DEBUG
    std::cerr << "inline const Simplex* __sc_const_iterator<T>::operator->() const: dereferencing _current_indexset_iter at "
	      << _current_indexset_iter << " ..."
	      << std::endl;
#endif
    if (_container->_is_pure) {
      const Simplex* result(&_container->_index_table.get_obj(_container->_pure_rank, **_current_indexset_iter));
#ifdef SCITER_DEBUG
      std::cerr << "... done." << std::endl;
#endif
      return result;
    }
    else {
      const Simplex* result(&_container->_index_table.get_obj(_current_card, **_current_indexset_iter));
#ifdef SCITER_DEBUG
      std::cerr << "... done." << std::endl;
#endif
      return result;
    }
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
  inline parameter_type SimplicialComplexTemplate<T>::resize(const parameter_type new_size) {
#ifdef RESIZE_DEBUG
    std::cout << "parameter_type SimplicialComplexTemplate<T>::resize(const parameter_type)" << std::endl;
#endif
    _index_table.resize(new_size);
    _index_set.resize(std::max<parameter_type>(init_size, std::max<parameter_type>(_maxcard, new_size)));
    return _index_set.size();
  }

  // change from pure to non-pure:
  template<class T>
  inline void SimplicialComplexTemplate<T>::make_non_pure() {
#ifdef RESIZE_DEBUG
    std::cout << "void SimplicialComplexTemplate<T>::make_non_pure()" << std::endl;
#endif
    if (!_is_pure) {
      return;
    }
    if (_pure_rank > 0) {
      _maxcard = _pure_rank + 1;
      resize(_maxcard);
      _index_set[_pure_rank] = std::move(_index_set[0]);
      _index_set[0].clear();
    }
    _pure_rank = -2;    
    _is_pure = false;
  }

  // change from non-pure to pure:
  template<class T>
  inline void SimplicialComplexTemplate<T>::make_pure() {
#ifdef RESIZE_DEBUG
    std::cout << "void SimplicialComplexTemplate<T>::make_pure()" << std::endl;
#endif
    if (_is_pure) {
      return;
    }
    if (_mincard >= _maxcard - 1) {
      _is_pure = true;
      _pure_rank = _maxcard - 1;
      if (_mincard == _maxcard - 1) {
	_pure_rank = _mincard;
	if (_mincard > 0) {
	  _index_set[0] = std::move(_index_set[_mincard]);
	}
	_maxcard = 1;
	_mincard = 0;
	resize(_maxcard);
      }
    }
  }

  // constructors:
  template<class T>
  inline SimplicialComplexTemplate<T>::SimplicialComplexTemplate() :
    _is_pure(true),
    _pure_rank(-1),
    _mincard(0),
    _maxcard(0),
    _index_set() {
    _index_set.reserve(init_capacity);
    _index_set.resize(init_size);
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SimplicialComplexTemplate<T>::SimplicialComplexTemplate()" 
	      << std::endl;
#endif
  }

  template<class T>
  inline SimplicialComplexTemplate<T>::SimplicialComplexTemplate(const SimplicialComplexTemplate<T>& s) :
    _is_pure(s._is_pure),
    _pure_rank(s._pure_rank),
    _mincard(s._mincard),
    _maxcard(s._maxcard),
    _index_set(s._index_set) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SimplicialComplexTemplate<T>::SimplicialComplexTemplate(const SimplicialComplexTemplate<T>&)" 
	      << std::endl;
#endif
  }

  template<class T>
  inline SimplicialComplexTemplate<T>::SimplicialComplexTemplate(SimplicialComplexTemplate<T>&& s) :
    _is_pure(s._is_pure),
    _pure_rank(s._pure_rank),
    _mincard(s._mincard),
    _maxcard(s._maxcard),
    _index_set(std::move(s._index_set)) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SimplicialComplexTemplate<T>::SimplicialComplexTemplate(SimplicialComplexTemplate<T>&&)" 
	      << std::endl;
#endif
  }

  template<class T>
  inline SimplicialComplexTemplate<T>::SimplicialComplexTemplate(const Simplex& simp,
								 const parameter_type card) : 
    _is_pure(true),
    _pure_rank(card),
    _index_set() {
    _index_set.reserve(init_capacity);
    _index_set.resize(init_size);
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SimplicialComplexTemplate<T>::SimplicialComplexTemplate"
	      << "(const Simplex&, const parameter_type)" 
	      << std::endl;
#endif
    // _maxcard = card + 1;
    // resize(_maxcard);
    // _mincard = card;
    // // std::shared_lock<std::shared_mutex> index_table_guard(_index_table_mutex);
    // _index_set[card] += _index_table.get_index(card, simp);
    _maxcard = 1;
    resize(_maxcard);
    _mincard = 0;
    _index_set[0] += _index_table.get_index(card, simp);
  }

  template<class T>
  inline SimplicialComplexTemplate<T>::SimplicialComplexTemplate(const Simplex& simp) :
    _is_pure(true),
    _pure_rank(static_cast<int>(simp.card())),
    _index_set() {
    _index_set.reserve(init_capacity);
    _index_set.resize(init_size);
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SimplicialComplexTemplate<T>::SimplicialComplexTemplate(const Simplex&)" << std::endl;
#endif
    const parameter_type card = static_cast<parameter_type>(simp.card());
    // _maxcard = card + 1;
    // resize(_maxcard);
    // _mincard = card;
    // // std::shared_lock<std::shared_mutex> index_table_guard(_index_table_mutex);
    // _index_set[card] += _index_table.get_index(card, simp);
    _maxcard = 1;
    resize(_maxcard);
    _mincard = 0;
    _index_set[0] += _index_table.get_index(card, simp);
  }

  // destructor:
  template<class T>
  inline SimplicialComplexTemplate<T>::~SimplicialComplexTemplate() {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "SimplicialComplexTemplate<T>::~SimplicialComplexTemplate()" 
	      << std::endl;
#endif
  }

  template<class T>
  inline const bool SimplicialComplexTemplate<T>::preprocessed() {
    return _index_table.preprocessed();
  }

  template<class T>
  inline void SimplicialComplexTemplate<T>::set_preprocessed(const bool preprocessed) {
    return _index_table.set_preprocessed(preprocessed);
  }

  template<class T>
  inline const bool SimplicialComplexTemplate<T>::preprocessed(const parameter_type rank) {
    return _index_table.preprocessed(rank);
  }

  template<class T>
  void SimplicialComplexTemplate<T>::start_multithreading() {
    _index_table.start_multithreading();
    for (parameter_type i = 0; i < _index_table.size(); ++i) {
      _index_table[i].start_multithreading();
    }
  }

  template<class T>
  void SimplicialComplexTemplate<T>::stop_multithreading() {
    _index_table.stop_multithreading();
    for (parameter_type i = 0; i < _index_table.size(); ++i) {
      _index_table[i].stop_multithreading();
    }  
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
  inline const size_type SimplicialComplexTemplate<T>::card(const parameter_type n) const {
    if (_is_pure) {
      return _index_set[0].card();
    }
    else {
      return _index_set[n].card();
    }
  }

  template<class T>
  inline const bool SimplicialComplexTemplate<T>::contains(const Simplex& simp) const {
    parameter_type card = static_cast<parameter_type>(simp.card());
    return contains(simp, card);
  }

  template<class T>
  inline const bool SimplicialComplexTemplate<T>::subset(const SimplicialComplexTemplate<T>& sc) const {
    return sc.superset(*this);
  }

  template<class T>
  inline const bool SimplicialComplexTemplate<T>::contains_face(const Simplex& simp) const {
    const parameter_type card = static_cast<parameter_type>(simp.card());
    return contains_face(simp, card);
  }

  template<class T>
  inline const bool SimplicialComplexTemplate<T>::contains_face(const Simplex& simp, 
								Simplex& facet) const {
    const parameter_type card = static_cast<parameter_type>(simp.card());
    return contains_face(simp, card, facet);
  }

  template<class T>
  inline const bool SimplicialComplexTemplate<T>::contains_free_face(const Simplex& simp,
								     Simplex& facet) const {
    const parameter_type card = static_cast<parameter_type>(simp.card());
    return contains_free_face(simp, card, facet);
  }

  // standard operations on simplicial complexes:

  template<class T>
  inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::star(const Simplex& simp) const {
    parameter_type card = static_cast<parameter_type>(simp.card());
    return star(simp, card);
  }
  
  template<class T>
  inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::link(const Simplex& simp) const {
    parameter_type card = static_cast<parameter_type>(simp.card());
    return link(simp, card);
  }

  template<class T>
  inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::deletion(const Simplex& simp) {
    parameter_type card = static_cast<parameter_type>(simp.card());
    return deletion(simp, card);
  }

  // skeleton:
  template<class T>
  inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::skeleton(const parameter_type card) {
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
    return insert_boundary(simp, static_cast<parameter_type>(simp.card()));
  }

  template<class T>
  inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::faces(const parameter_type depth) {
    SimplicialComplexTemplate<T> result(*this);
    return result.include_all_faces(depth);
  }

  // modifiers:
  template<class T>
  inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::clear() {

    // we need to clear some individual index sets,
    // since _maxcard many may contain elements and
    // at least init_size many will not be destroyed by resize:
    if (empty()) {
      return *this;
    }
    for (parameter_type i = _mincard; i < std::min<parameter_type>(init_size, _maxcard); ++i) {
      _index_set[i].clear();
    }
    _mincard = 0;
    _maxcard = 0;
    resize(_maxcard);
    _index_set.reserve(init_capacity);
    return *this;
  }

  // adding and deleting a simplex:
  template<class T>
  inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator+=(const Simplex& simp) {
    return insert(simp, static_cast<parameter_type>(simp.card()));
  }

  template<class T>
  inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::insert(const Simplex& simp) {
    return insert(simp,static_cast<parameter_type>(simp.card()));  
  }

  template<class T>
  inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator-=(const Simplex& simp) {
    return erase(simp,static_cast<parameter_type>(simp.card()));
  }

  template<class T>
  inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::erase(const Simplex& simp) {
    return erase(simp,static_cast<parameter_type>(simp.card()));
  }

  template<class T>
  inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::operator^=(const Simplex& simp) {
    return exclusiveor(simp,static_cast<parameter_type>(simp.card()));
  }

  template<class T>
  inline SimplicialComplexTemplate<T>& SimplicialComplexTemplate<T>::exclusiveor(const Simplex& simp) {
    return exclusiveor(simp,static_cast<parameter_type>(simp.card()));
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
									   const parameter_type card) const {
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
									  const parameter_type card) const {
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
										const parameter_type card) const {
    SimplicialComplexTemplate<T> s(*this);
    s.exclusiveor(simp,card);
    return s;
  }

  // join:

  template<class T>
  inline SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::join(const Simplex& simp) const {
    return join(simp, static_cast<parameter_type>(simp.card()));
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

  template<class T>
  const T                            SimplicialComplexTemplate<T>::empty_indexset = T();

  template<class T>
  const SimplicialComplexTemplate<T> SimplicialComplexTemplate<T>::empty_complex  = SimplicialComplexTemplate<T>();

  
  // the following is necessary on systems where explicit template instantiation
  // does not work as advertized:

}; // namespace topcom

#include "SimplicialComplexTemplate_Definitions.hh"

#endif

// eof SimplicialComplexTemplate_Declarations.hh
