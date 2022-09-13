//////////////////////////////////////////////////////////////////////////
// Symmetry.hh
// produced: 29/06/98 jr
// last change: 29/06/98 jr
/////////////////////////////////////////////////////////////////////////
#ifndef SYMMETRY_HH
#define SYMMETRY_HH

#include <assert.h>
#include <iostream>
#include <iterator>
#include <vector>

#include "Field.hh"
#include "FastSimplicialComplex.hh"
#include "SparseSimplicialComplex.hh"

#include "CommandlineOptions.hh"
#include "Statistics.hh"

#include "IntegerSet.hh"
#include "IntegerSet64.hh"
#include "Permutation.hh"
#include "Vector.hh"
#include "Matrix.hh"

#include "TriangNode.hh"
#include "Flip.hh"

namespace topcom {
  
  class Symmetry : public Permutation {
  public:
    static const char elem_delim_char;
  private:
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
    IntegerSet _fixed;
    IntegerSet _nonincreasing;
#endif
  public:
    inline Symmetry();
  public:
    inline Symmetry(const Symmetry& s);
    inline Symmetry(const Symmetry&& s);
    inline explicit Symmetry(const parameter_type n);
    // initialize by permutation of length p.k() on numbers up to p.n()
    // interpreted as a cycle (cycle=true) or permutation:
    Symmetry(const Permutation& p, const bool cycle=true);
    inline Symmetry(const parameter_type, const permutation_data&);
    inline ~Symmetry();
    inline const Symmetry& operator=(const Symmetry&);
    inline Symmetry& operator=(Symmetry&&);
  public:
    inline bool operator==(const Symmetry&) const;
  public:
    // actions on various data types:
    inline parameter_type   map(const parameter_type)                const;
    inline parameter_type   map(const size_type)                     const;
    IntegerSet64            map(const IntegerSet64&)                 const;
    IntegerSet              map(const IntegerSet&)                   const;
    FastSimplicialComplex   map(const FastSimplicialComplex&)        const;
    SparseSimplicialComplex map(const SparseSimplicialComplex&)      const;
    TriangNode              map(const TriangNode&)                   const;
    FlipRep                 map(const FlipRep&)                      const;
    Permutation             map(const Permutation&)                  const;
    Vector                  map(const Vector&)                       const;
    
    std::vector<parameter_type> map(const std::vector<parameter_type>&)  const;
    std::vector<size_type>      map(const std::vector<size_type>&)       const;

    // actions filling a prepared result data structure in place:
    void                    map_into(const IntegerSet64&, IntegerSet64&)   const;
    void                    map_into(const IntegerSet&, IntegerSet&)       const;
    void                    map_into(const Vector&, Vector&)               const;
    void                    map_into(const TriangNode&, TriangNode&)       const;

    // tests:
    bool        is_identity()                                              const;
    bool        maps(const SimplicialComplex&, const SimplicialComplex&)   const;
    bool        fixes(const SimplicialComplex&)                            const;
    bool        maps(const IntegerSet64&, const IntegerSet64&)             const;
    bool        fixes(const IntegerSet64&)                                 const;
    bool        maps(const IntegerSet&, const IntegerSet&)                 const;
    bool        fixes(const IntegerSet&)                                   const;
    bool        maps(const Vector&, const Vector&)                         const;
    bool        fixes(const Vector&)                                       const;
    bool        maps(const TriangNode&, const TriangNode&)                 const;
    bool        fixes(const TriangNode&)                                   const;
    bool        lex_decreases(const IntegerSet&)                           const;
    bool        lex_decreases(const IntegerSet64&)                         const;
    bool        lex_decreases(const SimplicialComplex&, const size_type)   const;
    bool        lex_decreases(const SimplicialComplex&)                    const;
    bool        colex_increases(const IntegerSet&)                         const;
    bool        colex_increases(const IntegerSet64&)                       const;
    bool        colex_increases(const SimplicialComplex&, const size_type) const;
    bool        colex_increases(const SimplicialComplex&)                  const;
    bool        decreases_simpidx(const Simplex&)                          const;
    
    // direct operations in place:
    Symmetry&  push_back(const parameter_type);
    Symmetry&  transpose(const parameter_type, const parameter_type);
    Symmetry&  left_multiply(const Symmetry& s); // in place
    
    // functions:
    Field      lex_index()                                               const;
    Symmetry   reverse()                                                 const; 
    Symmetry   inverse()                                                 const;
    Symmetry   simpidx_symmetry(const parameter_type rank)               const;
    
    // concatenation:
    Symmetry operator* (const Symmetry& s)                               const;

    // output transformation:
    Matrix     PermutationMatrix()                                       const;
    Matrix     ReducedPermutationMatrix()                                const;

    // stream operations:
    std::ostream& pretty_print(std::ostream& ost)                        const;
    friend std::istream& operator>>(std::istream& ist, Symmetry& s);
  };

  typedef const Symmetry* symmetryptr_type;
  
  class SymmetryPtrEqual {
  public:
    bool operator()(const symmetryptr_type& s1, const symmetryptr_type& s2) const {
      return s1->operator==(*s2);
    }
  };
  
  class SymmetryPtrHash {
  public:
    inline size_type operator()(const symmetryptr_type& key) const {
      return reinterpret_cast<size_type>(key);
    }
  };

  // storage for const pointers to symmetries:
  class SymmetryPtrVector : public std::vector<const Symmetry*> {
    friend std::ostream& operator<<(std::ostream& ost, const SymmetryPtrVector symptrvec) {
      for (SymmetryPtrVector::const_iterator iter = symptrvec.begin();
	   iter != symptrvec.end();
	   ++iter) {
	ost << **iter << " ";
      }
      return ost;
    }
  };

  inline Symmetry::Symmetry() {}

  inline Symmetry::Symmetry(const parameter_type n) :
    Permutation(n, n)
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
    ,
    _fixed(IntegerSet(0, n)),
    _nonincreasing(IntegerSet(0, n))
#endif
  {}

  inline Symmetry::Symmetry(const Symmetry& s) :
    Permutation(s)  
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
    ,
    _fixed(s._fixed),
    _nonincreasing(s._nonincreasing)
#endif
  { 
    assert(n() == k());
  }
  inline Symmetry::Symmetry(const Symmetry&& s) :
    Permutation(std::move(s))
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
    ,
    _fixed(s._fixed),
    _nonincreasing(s._nonincreasing)
#endif
  { 
    assert(n() == k());
  }

  inline Symmetry::Symmetry(const parameter_type n, const permutation_data& data) :
    Permutation(n, n, data) {}

  inline Symmetry::~Symmetry() {}

  inline const Symmetry& Symmetry::operator=(const Symmetry& s) {
    if (this == &s) return *this;
    Permutation::operator=(s);
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
    _fixed = s._fixed;
    _nonincreasing = s._nonincreasing;
#endif
    return *this;
  }
  inline Symmetry& Symmetry::operator=(Symmetry&& s) {
    if (this == &s) return *this;
    Permutation::operator=(std::move(s));
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
    _fixed = s._fixed;
    _nonincreasing = s._nonincreasing;
#endif
    return *this;
  }

  inline bool Symmetry::operator==(const Symmetry& s) const {
    if (_n != s._n) {
      return false;
    }
    for (parameter_type i = 0; i < _n; ++i) {
      if ((*this)(i) != s(i)) {
	return false;
      }
    }
    return true;
  }

  inline parameter_type Symmetry::map(const parameter_type i) const {
    return (*this)(i);
  }

  inline parameter_type Symmetry::map(const size_type i) const {
    assert(i < std::numeric_limits<parameter_type>::max());
    return (*this)(i);
  }

#ifndef STL_SYMMETRIES
#include "PlainArray.hh"
#include "PlainHashSet.hh"
  typedef PlainHashSet<Symmetry>                        symmetry_collectordata;
  typedef PlainArray<Symmetry>                          symmetry_iterdata;
#else
#include <unordered_set>
#include <vector>
  typedef std::unordered_set<Symmetry, Hash<Symmetry> > symmetry_collectordata;
  typedef std::vector<Symmetry>                         symmetry_iterdata;
#endif

  // for some routines, it is sufficient to receive pointers to symmetries;
  // depending on the use case, we use vectors or hash sets of pointers:
  class symmetryptr_collectordata : public std::unordered_set<symmetryptr_type, SymmetryPtrHash, SymmetryPtrEqual > {
    friend std::ostream& operator<<(std::ostream& ost, const symmetryptr_collectordata symptrvec) {
      for (symmetryptr_collectordata::const_iterator iter = symptrvec.begin();
	   iter != symptrvec.end();
	   ++iter) {
	ost << **iter << " ";
      }
      return ost;
    }
  };

  // for some routines, it is sufficient to receive pointers to symmetries:
  class symmetryptr_iterdata : public std::vector<symmetryptr_type> {
    friend std::ostream& operator<<(std::ostream& ost, const symmetryptr_iterdata symptrvec) {
      for (symmetryptr_iterdata::const_iterator iter = symptrvec.begin();
	   iter != symptrvec.end();
	   ++iter) {
	ost << **iter << " ";
      }
      return ost;
    }
  };

  typedef std::pair<symmetryptr_iterdata, symmetryptr_collectordata> symmetryptr_datapair;
  
  // switched to an array as the plain data container, since
  // most of the time the SymmetryGroup is iterated over:

  class SymmetryGroup : public symmetry_iterdata {
  private:
    class Worker {
    private:
      int                     _ID;
      symmetry_iterdata       _input;
      symmetry_iterdata       _output;
    private:
      Worker() = delete;
    public:
      inline Worker(const Worker& init_worker) : _ID(init_worker._ID), _input(init_worker._input), _output(init_worker._output) {}
      inline Worker(const Worker&& init_worker) : _ID(init_worker._ID), _input(std::move(init_worker._input)), _output(std::move(init_worker._output)) {}
      inline Worker(const int init_ID, const symmetry_iterdata& init_input) : _ID(init_ID), _input(init_input), _output() {}
      inline Worker& operator=(const Worker& worker) {
	if (this == &worker) {
	  return *this;
	}
	_ID = worker._ID;
	_input = worker._input;
	_output = worker._output;
	return *this;
      }
      inline Worker& operator=(const Worker&& worker) {
	if (this == &worker) {
	  return *this;
	}
	_ID = worker._ID;
	_input = std::move(worker._input);
	_output = std::move(worker._output);
	return *this;
      }
      void operator()(const parameter_type, SymmetryGroup&);
    public:
      inline       int                ID    () const { return _ID; }
      inline const symmetry_iterdata& output() const { return _output; }
    };
  public:
    static const char start_char;
    static const char end_char;
    static const char delim_char;
  public:
    typedef symmetry_iterdata::iterator       iterator;
    typedef symmetry_iterdata::const_iterator const_iterator;
  private:
    parameter_type                      _n;
    symmetry_collectordata              _generators;
    bool                                _complete;
    Symmetry                            _identity; // store the identity once and for all
    static std::mutex                   _mutex;
  private:
    SymmetryGroup() = delete;
  public:
    inline SymmetryGroup                (const parameter_type& n);
    inline SymmetryGroup                (const SymmetryGroup& sg);
    inline SymmetryGroup                (SymmetryGroup&& sg);
    inline SymmetryGroup                (const parameter_type& n, const symmetry_collectordata& sd, const bool closure = true);
    inline ~SymmetryGroup               ();
    inline SymmetryGroup operator=      (const SymmetryGroup& sg);
    inline SymmetryGroup operator=      (SymmetryGroup&& sg);
    inline void          extend         (const Symmetry& s);
    const SimplicialComplex map         (const SimplicialComplex& fixed,
					 const Simplex& simp) const;
    // accessors:
    inline const parameter_type          n         () const { return _n; }
    inline const symmetry_collectordata& generators() const { return _generators; }
    inline const bool                    complete  () const { return _complete; }
    inline const Symmetry&               identity  () const { return _identity; }
    inline const size_type               order     () const { return size() + 1; }

    // comparison:
    inline const bool operator==(const SymmetryGroup&) const;
    inline const bool operator!=(const SymmetryGroup&) const;

    // permutation action on a SymmetryGroup in place:
    SymmetryGroup&                          closure   ();
    SymmetryGroup&                          transform (const Symmetry&);

    // computation of interesting groups:
    inline SymmetryGroup                    stabilizer(const IntegerSet&,
						       const bool = true) const;
    inline SymmetryGroup                    stabilizer(const SimplicialComplex&,
						       const bool = true) const;
    inline SymmetryGroup                    stabilizer(const SimplicialComplex&,
						       const Vector&,
						       const bool = true) const;
    
    inline symmetryptr_datapair             stabilizer_ptrs(const IntegerSet&,
							    const bool = true) const;
    inline symmetryptr_datapair             stabilizer_ptrs(const SimplicialComplex&,
							    const bool = true) const;
    inline symmetryptr_datapair             stabilizer_ptrs(const SimplicialComplex&,
							    const Vector&,
							    const bool = true) const;
    // shortcut for the order of a stabilizer:
    inline size_type                        stabilizer_card(const IntegerSet64&) const;
    inline size_type                        stabilizer_card(const IntegerSet&) const;
    inline size_type                        stabilizer_card(const SimplicialComplex&) const;
  
    // compute the simplex-index representation:
    SymmetryGroup                           simpidx_symmetries         (const parameter_type) const;
    SymmetryGroup                           parallel_simpidx_symmetries(const parameter_type) const;

  
    // for permutation polytopes like the Birkhoff polytope:
    std::pair<PointConfiguration, SymmetryGroup> permutation_polytope       (const bool = false)        const;
    symmetry_collectordata                       rowperm_symmetry_generators(const PointConfiguration&) const;
    SymmetryGroup                                rowperm_symmetry_group     (const PointConfiguration&) const;

    std::ostream& pretty_print_generators    (std::ostream& ost) const;
    std::ostream& pretty_print               (std::ostream& ost) const;
    inline std::ostream& write_generators    (std::ostream& ost) const;
    inline std::ostream& write               (std::ostream& ost) const;
    std::istream& read_generators            (std::istream& ist);
    inline std::istream& read                (std::istream& ist);
    inline friend std::istream& operator<<   (std::istream& ist, SymmetryGroup& sg);
    inline friend std::ostream& operator>>   (std::ostream& ost, const SymmetryGroup& sg);
  private:
    void _closure                            (const symmetry_collectordata& sd, symmetry_collectordata& collector);
    void _update_iterarray                   (const symmetry_collectordata& sd);
    void _generate_collector                 (symmetry_collectordata& collector);
  };

  inline SymmetryGroup::SymmetryGroup(const parameter_type& n) :
    symmetry_iterdata(),
    _n(n),
    _generators(),
    _complete(true),
    _identity(n) {
  }

  inline SymmetryGroup::SymmetryGroup(const SymmetryGroup& sg) : 
    symmetry_iterdata(sg),
    _n(sg._n),
    _generators(sg._generators),
    _complete(sg._complete),
    _identity(sg._identity) {
  }

  inline SymmetryGroup::SymmetryGroup(SymmetryGroup&& sg) : 
    symmetry_iterdata(std::move(sg)),
    _n(sg._n),
    _generators(std::move(sg._generators)),
    _complete(sg._complete),
    _identity(std::move(sg._identity)) {
  }

  inline SymmetryGroup::SymmetryGroup(const parameter_type& n, const symmetry_collectordata& sd, const bool closure) : 
    symmetry_iterdata(),
    _n(n),
    _generators(sd),
    _complete(false),
    _identity(n) {
    symmetry_collectordata collector(sd);
    if (closure) {
      _closure(sd, collector);
    }
    else {
      _update_iterarray(collector);
    }
  }

  inline SymmetryGroup::~SymmetryGroup() {}

  inline SymmetryGroup SymmetryGroup::operator=(const SymmetryGroup& sg) {
    if (this == &sg) return *this;
    _n = sg._n;
    _generators = sg._generators;
    _complete = sg._complete;
    _identity = sg._identity;
    symmetry_iterdata::operator=(sg);
    return *this;
  }

  inline SymmetryGroup SymmetryGroup::operator=(SymmetryGroup&& sg) {
    if (this == &sg) return *this;
    _n = sg._n;
    _generators = std::move(sg._generators);
    _complete = sg._complete;
    _identity = std::move(sg._identity);
    symmetry_iterdata::operator=(std::move(sg));
    return *this;
  }

  // compute the subgroup that fixes an IntegerSet (symmetry group must be completed first):
  inline SymmetryGroup SymmetryGroup::stabilizer(const IntegerSet& fixed,
						 const bool add_generators) const {
    if (fixed.empty()) {
      return *this;
    }
    SymmetryGroup result(_n);
    if (!_complete) {
      std::cerr << "SymmetryGroup SymmetryGroup::stabilizer(const IntegerSet& fixed):" << std::endl;
      std::cerr << "Error: stabilizer called for incomplete group - exiting" << std::endl;
      exit(1);
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      const Symmetry& g(*iter);
      if (g.fixes(fixed)) {
	result.push_back(g);
	// it is important to have all group elements in result._generators
	// for checking membership in stabilizer:
	if (add_generators) {
	  result._generators.insert(g);
	}
      }
    }
    result._complete = true;
    return result;
  }

  // compute the subgroup that fixes a simplicial complex (symmetry group must be completed first):
  inline SymmetryGroup SymmetryGroup::stabilizer(const SimplicialComplex& fixed,
						 const bool add_generators) const {
    if (fixed.empty()) {
      return *this;
    }
    SymmetryGroup result(_n);
    if (!_complete) {
      std::cerr << "SymmetryGroup SymmetryGroup::stabilizer(const SimplicialComplex& fixed):" << std::endl;
      std::cerr << "Error: stabilizer called for incomplete group - exiting" << std::endl;
      exit(1);
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      const Symmetry& g(*iter);
      if (g.fixes(fixed)) {
	result.push_back(g);
	// it is important to have all group elements in result._generators
	// for checking membership in stabilizer:
	if (add_generators) {
	  result._generators.insert(g);
	}
      }
    }
    result._complete = true;
    return result;
  }
  
  inline SymmetryGroup SymmetryGroup::stabilizer(const SimplicialComplex& fixed,
						 const Vector& hint,
						 const bool add_generators) const {
    if (fixed.empty()) {
      return *this;
    }
    SymmetryGroup result(_n);
    if (!_complete) {
      std::cerr << "SymmetryGroup SymmetryGroup::stabilizer(const SimplicialComplex& fixed):" << std::endl;
      std::cerr << "Error: stabilizer called for incomplete group - exiting" << std::endl;
      exit(1);
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      const Symmetry& g(*iter);
      if (g == _identity) {
	continue;
      }
      if (!g.fixes(hint)) {

	// certainly not in stabilizer:
	continue;
      }
      if (g.fixes(fixed)) {
	result.push_back(g);
	// it is important to have all group elements in result._generators
	// for checking membership in stabilizer:
	if (add_generators) {
	  result._generators.insert(g);
	}
      }
    }
    result._complete = true;
    return result;
  }


  // same stabilizers, but returning a low-level container pair with pointers to symmetries:
   inline symmetryptr_datapair SymmetryGroup::stabilizer_ptrs(const IntegerSet& fixed,
							      const bool add_generators) const {
    symmetryptr_datapair result;
    if (!_complete) {
      std::cerr << "SymmetryGroup SymmetryGroup::stabilizer_ptrs(const IntegerSet& fixed):" << std::endl;
      std::cerr << "Error: stabilizer called for incomplete group - exiting" << std::endl;
      exit(1);
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      const Symmetry& g(*iter);
      if (g.fixes(fixed)) {
	result.first.push_back(&g);
	// it is important to have all group elements in result._generators
	// for checking membership in stabilizer:
	if (add_generators) {
	  result.second.insert(&g);
	}
      }
    }
    return result;
  }

  inline symmetryptr_datapair SymmetryGroup::stabilizer_ptrs(const SimplicialComplex& fixed,
							     const bool add_generators) const {
    symmetryptr_datapair result;
    if (!_complete) {
      std::cerr << "SymmetryGroup SymmetryGroup::stabilizer_ptrs(const SimplicialComplex& fixed):" << std::endl;
      std::cerr << "Error: stabilizer called for incomplete group - exiting" << std::endl;
      exit(1);
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      const Symmetry& g(*iter);
      if (g.fixes(fixed)) {
	result.first.push_back(&g);
	// it is important to have all group elements in result._generators
	// for checking membership in stabilizer:
	if (add_generators) {
	  result.second.insert(&g);
	}
      }
    }
    return result;
  }
  
  inline symmetryptr_datapair SymmetryGroup::stabilizer_ptrs(const SimplicialComplex& fixed,
							     const Vector& hint,
							     const bool add_generators) const {
    symmetryptr_datapair result;
    if (!_complete) {
      std::cerr << "SymmetryGroup SymmetryGroup::stabilizer_ptrs(const SimplicialComplex& fixed):" << std::endl;
      std::cerr << "Error: stabilizer called for incomplete group - exiting" << std::endl;
      exit(1);
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      const Symmetry& g(*iter);
      if (g == _identity) {
	continue;
      }
      if (!g.fixes(hint)) {

	// certainly not in stabilizer:
	continue;
      }
      if (g.fixes(fixed)) {
	result.first.push_back(&g);
	// it is important to have all group elements in result._generators
	// for checking membership in stabilizer:
	if (add_generators) {
	  result.second.insert(&g);
	}
      }
    }
    return result;
  }

  // shortcut for the order of a stabilizer:
  inline size_type SymmetryGroup::stabilizer_card(const IntegerSet64& fixed) const {
    size_type result = 0UL;
    if (fixed.empty()) {
      return result;
    }
    if (!_complete) {
      std::cerr << "SymmetryGroup SymmetryGroup::stabilizer_card(const SimplicialComplex& fixed):" << std::endl;
      std::cerr << "Error: stabilizer called for incomplete group - exiting" << std::endl;
      exit(1);
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      const Symmetry& g(*iter);
      if (g.fixes(fixed)) {
	++result;
      }
    }
    return result;
  }

  inline size_type SymmetryGroup::stabilizer_card(const IntegerSet& fixed) const {
    size_type result = 0UL;
    if (fixed.empty()) {
      return result;
    }
    if (!_complete) {
      std::cerr << "SymmetryGroup SymmetryGroup::stabilizer_card(const SimplicialComplex& fixed):" << std::endl;
      std::cerr << "Error: stabilizer called for incomplete group - exiting" << std::endl;
      exit(1);
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      const Symmetry& g(*iter);
      if (g.fixes(fixed)) {
	++result;
      }
    }
    return result;
  }

  inline size_type SymmetryGroup::stabilizer_card(const SimplicialComplex& fixed) const {
    size_type result = 0UL;
    if (fixed.empty()) {
      return result;
    }
    if (!_complete) {
      std::cerr << "SymmetryGroup SymmetryGroup::stabilizer_card(const SimplicialComplex& fixed):" << std::endl;
      std::cerr << "Error: stabilizer called for incomplete group - exiting" << std::endl;
      exit(1);
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      const Symmetry& g(*iter);
      if (g.fixes(fixed)) {
	++result;
      }
    }
    return result;
  }

  
  // comparison:
  inline const bool SymmetryGroup::operator==(const SymmetryGroup& sg) const {
    if (_n != sg._n) {
      return false;
    }
    // we need groups with all elements to be able to compare:
    SymmetryGroup tmp1(*this);
    SymmetryGroup tmp2(sg);
    symmetry_collectordata collector1(_generators);
    symmetry_collectordata collector2(sg._generators);
    if (!_complete) {
      tmp1._closure(tmp1._generators, collector1);
    }
    else {
      tmp1._generate_collector(collector1);
    }
    if (!sg._complete) {
      tmp2._closure(tmp2._generators, collector2);
      tmp2._update_iterarray(collector2);
    }
    if (tmp1.size() != tmp2.size()) {
      return false;
    }
    for (SymmetryGroup::const_iterator iter = sg.begin();
	 iter != sg.end();
	 ++iter) {
      if (collector1.find(*iter) == collector1.end()) {
	return false;
      }
    }
    return true;
  }

  inline const bool SymmetryGroup::operator!=(const SymmetryGroup& sg) const {
    return !(*this == sg);
  }

  inline void SymmetryGroup::extend(const Symmetry& s) {
    symmetry_collectordata collector;
    symmetry_collectordata new_symmetry;
    _generators.insert(s);
    new_symmetry.insert(s);
    _generate_collector(collector);
    const size_type old_size(collector.size());
#ifdef INDEX_CHECK
    assert (s.n() == s.k());
    assert (s.n() == _n);
#endif
    collector.insert(s);
    if (collector.size() > old_size) {
      _complete = false;
      _closure(new_symmetry, collector);
      _update_iterarray(collector);
    }
  }

  inline std::ostream& SymmetryGroup::write(std::ostream& ost) const {
    ost << start_char;
    if (!empty()) {
      SymmetryGroup::const_iterator iter = begin();
      ost << *iter;
      while (++iter != end()) {
	ost << delim_char << *iter;
      }
    }
    ost << end_char;
    return ost;
  }

  inline std::ostream& SymmetryGroup::write_generators(std::ostream& ost) const {
    ost << start_char;
    if (!_generators.empty()) {
      symmetry_collectordata::const_iterator iter = _generators.begin();
      ost << *iter;
      while (++iter != _generators.end()) {
	ost << delim_char << *iter;
      }
    }
    ost << end_char;
    return ost;
  }

  inline std::ostream& operator<<(std::ostream& ost, const SymmetryGroup& sg) {
    return sg.write(ost);
  }

  inline std::istream& SymmetryGroup::read(std::istream& ist) {
    read_generators(ist);
    _complete = false;
    if (CommandlineOptions::debug()) {
      std::cerr << "read symmetry generators ";
      write_generators(std::cerr);
      std::cerr << std::endl;
    }
    symmetry_collectordata collector(_generators);
    _closure(_generators, collector);
    return ist;
  }

  inline std::istream& operator>>(std::istream& ist, SymmetryGroup& sg) {
    return sg.read(ist);
  }

}; // namespace topcom

#endif

// eof Symmetry.hh
