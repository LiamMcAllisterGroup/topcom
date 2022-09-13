////////////////////////////////////////////////////////////////////////////////
// 
// Symmetry.cc
//
//    produced: 07/07/98 jr
// last change: 07/07/98 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <thread>

#include <unordered_map>
#include "HashKey.hh"
#include "ContainerIO.hh"

#include "Symmetry.hh"
#include "CommandlineOptions.hh"

namespace topcom {

  // Symmetry:

  const char Symmetry::elem_delim_char = '\t';

  Symmetry::Symmetry(const Permutation& p, const bool cycle) :
    Permutation(p.n(), p.n())
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
    ,
    _fixed(IntegerSet(0, p.n())),
    _nonincreasing(IntegerSet(0, p.n())) 
#endif
  {
    const parameter_type np = p.n();
    const parameter_type kp = p.k();
    if (kp == 0) {
      return;
    }
    if (cycle) {
      if (CommandlineOptions::debug()) {
	std::cerr << "cycle to convert: " << p << std::endl;
      }
      if (kp > 1) {
	parameter_type i = 0;
	const parameter_type first = p(0);
	parameter_type preimage = first;
	while (++i < kp) {
	  assert(preimage < np);
	  parameter_type image = p(i);
	  assert(image < np);
	  (*this)(preimage) = image;
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS	
	  _fixed -= preimage;
	  if (preimage < image) {
	    _nonincreasing -= preimage;
	  }
#endif
	  preimage = image;
	}
	(*this)(preimage) = first;
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS	
	_fixed -= preimage;
	if (preimage < first) {
	  _nonincreasing -= preimage;
	}
#endif
      }
    }
    else {
      if (CommandlineOptions::debug()) {
	std::cerr << "permutation to use: " << p << std::endl;
      }
      if (kp < np) {
	std::cerr << "error while initializing symmetry from partial permutation" << std::endl;
	exit(1);
      }
      else {
	for (parameter_type i = 0; i < np; ++i) {
	  if (p(i) != i) {
	    (*this)(i) = p(i);
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS	
	    _fixed -= i;
	    if (i < p(i)) {
	      _nonincreasing -= i;
	    }
#endif
	  }
	}
      }
    }
  }

  IntegerSet Symmetry::map(const IntegerSet& is) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    // IntegerSet result(is);
    IntegerSet result;
    for (IntegerSet::const_iterator iter = is.begin();
	 iter != is.end();
	 ++iter) {
      // result ^= *iter;
      // result ^= (*this)(*iter);
      result += (*this)(*iter);
    }
    return result;
  }

  IntegerSet64 Symmetry::map(const IntegerSet64& is) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    // IntegerSet64 result(is);
    IntegerSet64 result;
    for (IntegerSet64::const_iterator iter = is.begin();
	 iter != is.end();
	 ++iter) {
      // result ^= *iter;
      // result ^= (*this)(*iter);
      result += (*this)(*iter);
    }
    return result;
  }

  FastSimplicialComplex Symmetry::map(const FastSimplicialComplex& sc) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    // FastSimplicialComplex result(sc);
    FastSimplicialComplex result;
    for (FastSimplicialComplex::const_iterator iter = sc.begin();
	 iter != sc.end();
	 ++iter) {
      // result ^= *iter;
      // result ^= this->map(*iter);
      result += this->map(*iter);
    }
    return result;
  }

  SparseSimplicialComplex Symmetry::map(const SparseSimplicialComplex& sc) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    // SparseSimplicialComplex result(sc);
    SparseSimplicialComplex result;
    for (SparseSimplicialComplex::const_iterator iter = sc.begin();
	 iter != sc.end();
	 ++iter) {
      // result ^= *iter;
      // result ^= this->map(*iter);
      result += this->map(*iter);
    }
    return result;
  }

  TriangNode Symmetry::map(const TriangNode& tn) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    // TriangNode result(tn);
    TriangNode result;
    for (SimplicialComplex::const_iterator iter = tn.begin();
	 iter != tn.end();
	 ++iter) {
      // result ^= *iter;
      // result ^= this->map(*iter);
      result += this->map(*iter);
    }
    return result;
  }
 
  FlipRep Symmetry::map(const FlipRep& fr) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    FlipRep result;
    result.first = map(fr.first);
    result.second = map(fr.second);
    return result;
  }
 
  Permutation Symmetry::map(const Permutation& p) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    assert(n() == p.n());
    // assert(k() == p.k());
    Permutation result(p);
    for (parameter_type i = 0; i < result.k(); ++i) {
      result(i) = (*this)(p(i));
    }
    return result;  
  }

  Vector Symmetry::map(const Vector& v) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    Vector result(v);
    for (std::size_t i = 0; i < result.dim(); ++i) {
      result((*this)(i)) = v(i);
    }
    return result;  
  }

  std::vector<parameter_type> Symmetry::map(const std::vector<parameter_type>& v) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    std::vector<parameter_type> result(v);
    for (std::size_t i = 0; i < result.size(); ++i) {
      result.at((*this)(i)) = v.at(i);
    }
    return result;  
  }

  std::vector<size_type> Symmetry::map(const std::vector<size_type>& v) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    std::vector<size_type> result(v);
    for (std::size_t i = 0; i < result.size(); ++i) {
      result.at((*this)(i)) = v.at(i);
    }
    return result;  
  }
  
  void Symmetry::map_into(const IntegerSet64& is,
			  IntegerSet64& result) const {
    for (IntegerSet64::const_iterator iter = is.begin();
	 iter != is.end();
	 ++iter) {
      result += (*this)(*iter);
    }
  }

  void Symmetry::map_into(const IntegerSet& is,
			  IntegerSet& result) const {
    for (IntegerSet::const_iterator iter = is.begin();
	 iter != is.end();
	 ++iter) {
      result += (*this)(*iter);
    }
  }

  void Symmetry::map_into(const TriangNode& tn,
			  TriangNode& result) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    for (SimplicialComplex::const_iterator iter = tn.begin();
	 iter != tn.end();
	 ++iter) {
      result += this->map(*iter);
    }
  }

  void Symmetry::map_into(const Vector& v, Vector& result) const {
#ifdef STATISTICS
    Statistics::new_map_call();
#endif
    if (result.dim() != v.dim()) {
      std::cerr << "Symmetry::map_into(const Vector&, Vector&): "
		<< "dimension of result vector incompatible with symmetry - exiting"
		<< std::endl;
      exit(1);
    }
    for (parameter_type i = 0; i < v.dim(); ++i) {
      result((*this)(i)) = v(i);
    }
  }

  // tests:
  bool Symmetry::is_identity() const {
    for (parameter_type i = 0; i < _n; ++i) {
      if ((*this)(i) != i) {
	return false;
      } 
    }
    return true;
  }
  
  bool Symmetry::maps(const IntegerSet64& ls1, const IntegerSet64& ls2) const {
    for (IntegerSet64::const_iterator iter = ls1.begin();
	 iter != ls1.end();
	 ++iter) {
      if (!ls2.contains(this->map(*iter))) {
	return false;
      }
    }
    return true;
  }

  bool Symmetry::fixes(const IntegerSet64& ls) const {
    if (ls.empty()) {
      return true;
    }
    return maps(ls, ls);
  }

  bool Symmetry::maps(const IntegerSet& ls1, const IntegerSet& ls2) const {
    for (IntegerSet::const_iterator iter = ls1.begin();
	 iter != ls1.end();
	 ++iter) {
      if (!ls2.contains(this->map(*iter))) {
	return false;
      }
    }
    return true;
  }

  bool Symmetry::fixes(const IntegerSet& ls) const {
    if (ls.empty()) {
      return true;
    }
    return maps(ls, ls);
  }

  bool Symmetry::maps(const SimplicialComplex& sc1, const SimplicialComplex& sc2) const {
    for (SimplicialComplex::const_iterator iter = sc1.begin();
	 iter != sc1.end();
	 ++iter) {
      if (!sc2.contains(this->map(*iter))) {
	return false;
      }
    }
    return true;
  }

  bool Symmetry::fixes(const SimplicialComplex& sc) const {
    if (sc.empty()) {
      return true;
    }
    return maps(sc, sc);
  }

  bool Symmetry::maps(const Vector& v1, const Vector& v2) const {
    if (v1.size() != v2.size()) {
      return false;
    }
    for (parameter_type i = 0; i < v1.size(); ++i) {
      if (v1((*this)(i)) != v2(i)) {
	return false;
      }
    }
    return true;
  }

  bool Symmetry::fixes(const Vector& v) const {
    if (v.empty()) {
      return true;
    }
    return maps(v, v);
  }
  
  bool Symmetry::maps(const TriangNode& tn1, const TriangNode& tn2) const {
    for (SimplicialComplex::const_iterator iter = tn1.begin();
	 iter != tn1.end();
	 ++iter) {
      if (!tn2.contains(this->map(*iter))) {
	return false;
      }
    }
    return true;
  }

  bool Symmetry::fixes(const TriangNode& tn) const {
    return maps(tn, tn);
  }

  bool Symmetry::lex_decreases(const IntegerSet& is) const {
    if (is.empty()) {
      return false;
    }
    return map(is).lexsmaller(is);
  }
  
  bool Symmetry::lex_decreases(const IntegerSet64& is) const {
    if (is.empty()) {
      return false;
    }
    return map(is).lexsmaller(is);
  }

  bool Symmetry::colex_increases(const IntegerSet& is) const {
    if (is.empty()) {
      return false;
    }
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
    if (_fixed.superset(is)) {
      return false;
    }
    if (_nonincreasing.superset(is)) {
#ifdef COMPUTATIONS_DEBUG
      std::cerr << "set of nonincreasing elements " << _nonincreasing << " for permutation "
		<< *this << " is a superset of " << is << ": colex_increases = false"
		<< std::endl;
#endif
      return false;
    }
#endif
#ifdef COMPUTATIONS_DEBUG
    const IntegerSet image(map(is));
    if (image.lexgreater(is)) {
      std::cerr << image << " >_colex " << is << std::endl;
      return true;
    }
    else {
      std::cerr << image << " not >_colex " << is << std::endl;
    }
#endif
    return map(is).colexgreater(is);
  }
  
  bool Symmetry::colex_increases(const IntegerSet64& is) const {
    if (is.empty()) {
      return false;
    }
    return map(is).colexgreater(is);
  }

  bool Symmetry::lex_decreases(const SimplicialComplex& sc, const size_type card) const {
    if (sc.empty()) {
      return false;
    }
    return map(sc).lexsmaller(sc, card);
  }

  bool Symmetry::colex_increases(const SimplicialComplex& sc, const size_type card) const {
    if (sc.empty()) {
      return false;
    }
    return map(sc).colexgreater(sc, card);
  }

  bool Symmetry::lex_decreases(const SimplicialComplex& sc) const {
    if (sc.empty()) {
      return false;
    }
    return map(sc).lexsmaller(sc);
  }

  bool Symmetry::decreases_simpidx(const Simplex& s) const {
    if (s.empty()) {
      return false;
    }
    const parameter_type s_card = s.card();
    const parameter_type s_idx = SimplicialComplex::index_of_simplex(s, s_card);
    const Simplex s_image(map(s));
    const parameter_type s_image_idx = SimplicialComplex::index_of_simplex(s_image, s_card);
    return (s_image_idx < s_idx);
  }

  Symmetry Symmetry::reverse() const {
    Symmetry result(n());
    for (parameter_type i = 0; i < n(); ++i) {
      result(i) = (*this)(n() - i - 1);
    }
    return result;
  }

  Symmetry& Symmetry::push_back(const parameter_type i) {
    Permutation::push_back(i);
    return *this;
  }

  Symmetry& Symmetry::transpose(const parameter_type i, const parameter_type j) {
    const parameter_type tmp((*this)(i));
    (*this)(i) = (*this)(j);
    (*this)(j) = tmp;
    return *this;
  }

  Symmetry& Symmetry::left_multiply(const Symmetry& s) {
    static Symmetry tmp;

    tmp = *this;
    for (parameter_type i = 0; i < n(); ++i) {
      (*this)(i) = s(tmp(i));
    }
    return *this;
  }

  Field Symmetry::lex_index() const {
    Field result = FieldConstants::ZERO;
    Field radix = FieldConstants::ONE;
    parameter_type pos = 2;
    for (int p = k() - 2; p >= 0; --p) {
      int no_of_inv = 0;
      for (int q = p + 1; q < k(); ++q) {
	if ((*this)(p) > (*this)(q)) {
	  ++no_of_inv;
	}
      }
      result += (no_of_inv * radix);
      radix *= pos;
      ++pos;
    }
    return result;
  }
  
  Symmetry Symmetry::inverse() const {
    Symmetry result(n());
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
    result._nonincreasing.clear();
    result._fixed.clear();
#endif
    for (parameter_type i = 0; i < n(); ++i) {
      result((*this)(i)) = i;
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
      if (i <= (*this)(i)) {
	result._nonincreasing += (*this)(i);
      }
#endif
    }
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS	
    result._fixed = _fixed;
#endif
    return result;
  }

  // the following determines the symmetry that maps the index of
  // all rank-simplices to the index of the simplex's image,
  // where indices are taken from the static simplex dictionary of SimplicialComplex:
  Symmetry Symmetry::simpidx_symmetry(const parameter_type rank) const {
    // first, retrieve the number of rank-simplices in the table:
    const size_type no_of_simplices = SimplicialComplex::no_of_simplices(rank);
    assert(no_of_simplices < std::numeric_limits<parameter_type>::max());
    
    // allocate a suitable symmetry:
    Symmetry result(no_of_simplices);
    for (parameter_type i = 0; i < no_of_simplices; ++i) {
      const Simplex& simp = SimplicialComplex::simplex_of_index(i, rank);
      result(i) = SimplicialComplex::index_of_simplex(this->map(simp), rank);
    }
    return result;
  }

  Symmetry Symmetry::operator*(const Symmetry& s) const {
    Symmetry result(*this);
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS	
    result._fixed.clear();
    result._nonincreasing.clear();
#endif
    for (parameter_type i = 0; i < result.n(); ++i) {
      result(i) = (*this)(s(i));
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS
      if (result(i) == i) {
	result._fixed += i;
      }
      else if (result(i) <= i) {
	result._nonincreasing += i;
      }
#endif
    }
    return result;
  }

  Matrix Symmetry::PermutationMatrix() const {
    const parameter_type n = this->n();
    Matrix result(n, n, FieldConstants::ZERO);
    for (parameter_type i = 0; i < n; ++i) {
      result((*this)(i), i) = FieldConstants::ONE;
    }
    return result;
  }

  Matrix Symmetry::ReducedPermutationMatrix() const {
    const parameter_type n = this->n();
    Matrix result(n-1, n-1, FieldConstants::ZERO);
    for (parameter_type i = 0; i < n-1; ++i) {
      const parameter_type j((*this)(i));
      if (j < n-1) {
	result(j, i) = FieldConstants::ONE;
      }
    }
    return result;
  }

  std::ostream& Symmetry::pretty_print(std::ostream& ost) const {
    for (Symmetry::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      ost << *iter << elem_delim_char;
    }
    return ost;
  }

  std::istream& operator>>(std::istream& ist, Symmetry& s) {
#ifdef SUPER_VERBOSE
    std::cerr << "reading symmetry ...";
#endif
    ist >> (Permutation&)s;
    if (s.n() != s.k()) {
      ist.clear(std::ios::failbit);
      return ist;
    }
#ifdef EXPLOIT_SYMMETRY_FIXEDPOINTS	
    s._nonincreasing = LabelSet(0, s.n());
    for (parameter_type i = 0; i < s.n(); ++i) {
      if (s(i) != i) {
	s._fixed -= i;
	if (i < s(i)) {
	  s._nonincreasing -= i;
	}
      }
    }
#endif
#ifdef SUPER_VERBOSE
    std::cerr << "n = " << s.n() << ", k = " << s.k() << "... done." << std::endl;
#endif
    return ist;
  }

  // SymmetryGroup:

  const char SymmetryGroup::start_char = ContainerChars::list_start_char;
  const char SymmetryGroup::end_char   = ContainerChars::list_end_char;
  const char SymmetryGroup::delim_char = ContainerChars::delim_char;
  std::mutex SymmetryGroup::_mutex;

  const SimplicialComplex SymmetryGroup::map(const SimplicialComplex& fixed, 
					     const Simplex&           simp) const {
    SimplicialComplex result;
    result += simp;
    for (const_iterator iter = begin(); iter != end(); ++iter) {
      const Symmetry& sym(*iter);
      if (!sym.fixes(fixed)) {
	continue;
      }
      result += sym.map(simp);
    }
    return result;
  }

  // permutation action on a SymmetryGroup in place:
  SymmetryGroup& SymmetryGroup::closure() {
    symmetry_collectordata collector;
    _closure(_generators, collector);
    return *this;
  }

  SymmetryGroup& SymmetryGroup::transform(const Symmetry& transformation) {
    for (SymmetryGroup::iterator iter = begin();
	 iter != end();
	 ++iter) {
      Symmetry& sym(*iter);
      sym = transformation * sym * transformation.inverse();
    }
    for (symmetry_collectordata::iterator iter = _generators.begin();
	 iter != _generators.end();
	 ++iter) {
      const Symmetry& sym(*iter);    
      const Symmetry transformed_sym = transformation * sym * transformation.inverse();
      if (sym != transformed_sym) {
	_generators.erase(sym);
	_generators.insert(transformed_sym);
      }
    }
    return *this;
  }

  SymmetryGroup SymmetryGroup::simpidx_symmetries(const parameter_type rank) const {
    if (CommandlineOptions::parallel_enumeration() || CommandlineOptions::parallel_symmetries()) {
      return parallel_simpidx_symmetries(rank);
    }
    const size_type no_of_simplices = SimplicialComplex::no_of_simplices(rank);
    assert(no_of_simplices < std::numeric_limits<parameter_type>::max());
    SymmetryGroup result((parameter_type)no_of_simplices);
    result.reserve(size());
    int cnt = CommandlineOptions::report_frequency() * 1000 / no_of_simplices;
    if (CommandlineOptions::verbose()) {
      std::cerr << "generating representations of symmetry group on simplex indices ..." << std::endl;
    }
    for (SymmetryGroup::const_iterator iter = this->begin();
	 iter != this->end();
	 ++iter) {
      result.emplace_back(std::move(iter->simpidx_symmetry(rank)));
      if (CommandlineOptions::verbose()) {
	if (--cnt < 0) {
	  cnt = CommandlineOptions::report_frequency() * 1000 / no_of_simplices;
	  std::cerr << result.size()
		    << " out of " << size()
		    << " symmetries generated so far ..." << std::endl;
	}
      }
    }
    result._complete = _complete;
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done" << std::endl;
    }
    return result;
  }

  void SymmetryGroup::Worker::operator()(const parameter_type rank, SymmetryGroup& result) {
    const parameter_type no_of_simplices = SimplicialComplex::no_of_simplices(rank);
    // _output.reserve(_input.size());
    int report_point = CommandlineOptions::report_frequency() * 1000 / no_of_simplices;
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "\t thread " << std::setw(2) << _ID << ": generating representations of symmetry group on simplex indices ..." << std::endl;
    }
    for (symmetry_iterdata::const_iterator iter = _input.begin();
	 iter != _input.end();
	 ++iter) {
      Symmetry new_sym(iter->simpidx_symmetry(rank));
      {
	std::lock_guard<std::mutex> lock(SymmetryGroup::_mutex);
	result.emplace_back(std::move(new_sym));
      }    
      // _output.emplace_back(std::move(iter->simpidx_symmetry(rank)));
      if (CommandlineOptions::verbose()) {
	if (result.size() % report_point == 0) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "\t thread " << std::setw(2) << _ID << ": " << result.size()
		    << " out of " << result.capacity()
		    << " symmetries generated so far ..." << std::endl;
	}
      }
    }
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "\t ... thread " << _ID << " done" << std::endl;
    }
  }

  SymmetryGroup SymmetryGroup::parallel_simpidx_symmetries(const parameter_type rank) const {
    const size_type no_of_simplices = SimplicialComplex::no_of_simplices(rank);
    if (no_of_simplices >= std::numeric_limits<parameter_type>::max()) {
      std::cerr << "SymmetryGroup::parallel_simpidx_symmetries(const parameter_type rank) const: "
		<< "too many simplices for simplex-index symmetries - exiting" << std::endl;
      exit(1);
    }
    SymmetryGroup result((parameter_type)no_of_simplices);
    result.reserve(size());
  
    int local_no_of_threads = std::min<int>(CommandlineOptions::no_of_threads(), size());
    std::vector<std::thread> local_threads(local_no_of_threads);
    std::vector<SymmetryGroup> thread_symmetries(local_no_of_threads, SymmetryGroup(_n));
    std::vector<Worker> thread_workers;

    int thread_no = 0;

    // distribute symmetries:
    for (SymmetryGroup::const_iterator iter = this->begin();
	 iter != this->end();
	 ++iter) {

      // round-robin distribution:
      thread_symmetries[thread_no].push_back(*iter);
      thread_no = (thread_no + 1) % local_no_of_threads;
    }

    // generate workers:
    for (int i = 0; i < local_no_of_threads; ++i) {
      thread_workers.emplace_back(i, thread_symmetries[i]);
    }

    // generate threads:
    SimplicialComplex::start_multithreading();
    for (int i = 0; i < local_no_of_threads; ++i) {
      local_threads[i] = std::thread(&SymmetryGroup::Worker::operator(),
				     std::ref(thread_workers[i]),
				     std::ref(rank),
				     std::ref(result));
    }

    // wait for threads to finish:
    for (int i = 0; i < local_no_of_threads; ++i) {
      if (local_threads[i].joinable()) {
	local_threads[i].join();
      }
    }
    SimplicialComplex::stop_multithreading();

    // collect the results (not in the same order as the input!):
    for (int i = 0; i < local_no_of_threads; ++i) {
      for (symmetry_iterdata::const_iterator iter = thread_workers[i].output().begin();
	   iter != thread_workers[i].output().end();
	   ++iter) {
	result.emplace_back(std::move(*iter));
      }
    }

    if (result.size() != size()) {
      std::cerr << "SymmetryGroup SymmetryGroup::parallel_simpidx_symmetries(const parameter_type) const: "
		<< "size of simpidx representation " << result.size()
		<< " does not match size " << size()
		<< " of symmetries - exiting." << std::endl;
      exit(1);
    }
    result._complete = _complete;
    return result;
  }

  std::pair<PointConfiguration, SymmetryGroup> SymmetryGroup::permutation_polytope(const bool add_center) const {
    if (CommandlineOptions::verbose()) {
      std::cerr << "generating permutation polytope ..." << std::endl;
    }
    PointConfiguration points;
    std::unordered_map<Symmetry, size_type, Hash<Symmetry> > symmetry_index_map;
    std::vector<Symmetry>                                    symmetry_of_column;
    // first add the identity, because it is not stored in the SymmetryGroup:
    const Symmetry identity(_n);
    Point id_point(identity.ReducedPermutationMatrix().StackOfAllColumns());
    symmetry_index_map[identity] = points.no();
    symmetry_of_column.push_back(identity);
    points.push_back(id_point);
    if (CommandlineOptions::verbose()) {
      std::cerr << "\t generating vertex coordinates for all symmetries ..." << std::endl;
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      Point new_point(iter->ReducedPermutationMatrix().StackOfAllColumns());
      symmetry_index_map[*iter] = points.no();
      symmetry_of_column.push_back(*iter);
      points.push_back(new_point);
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "\t ... done" << std::endl;
    }
    size_type coldim(points.no());
    size_type rowdim(points.rank());

    symmetry_collectordata symmetry_generators;
    // generate symmetries for the permutation polytope
    // from left and from right multiplication;
    // in non-abelian groups this may yield
    // different symmetries:

    // left multiplication:
    if (CommandlineOptions::verbose()) {
      std::cerr << "\t generating symmetry group from left and right multiplication ..." << std::endl;
    }
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      Symmetry new_symmetry(coldim);
      const Symmetry& g(*iter);
      for (size_type preimage_idx = 0; preimage_idx < coldim; ++preimage_idx) {
	// read out the symmetry corresponding to column i:
	const Symmetry& symmetry_of_preimage(symmetry_of_column[preimage_idx]);
	const Symmetry symmetry_of_image(g * symmetry_of_preimage);
	std::unordered_map<Symmetry, size_type, Hash<Symmetry> >::const_iterator finder = symmetry_index_map.find(symmetry_of_image);
	if (finder == symmetry_index_map.end()) {
	  std::cerr << "SymmetryGroup::permutation_polytope(): error -- point of image symmetry "
		    << symmetry_of_image << " is not contained in the permutations matrices of points "
		    << symmetry_index_map << " -- exiting." << std::endl;
	  exit(1);
	}
	const size_type image_idx(finder->second);
	new_symmetry(preimage_idx) = image_idx;
      }
      if (add_center) {
	new_symmetry.push_back(coldim);
      }
      symmetry_generators.insert(new_symmetry);
    }
    // right multiplication:
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      Symmetry new_symmetry(coldim);
      const Symmetry& g(*iter);
      for (size_type preimage_idx = 0; preimage_idx < coldim; ++preimage_idx) {
	// read out the symmetry corresponding to column i:
	const Symmetry& symmetry_of_preimage(symmetry_of_column[preimage_idx]);
	const Symmetry symmetry_of_image(symmetry_of_preimage * g);
	std::unordered_map<Symmetry, size_type, Hash<Symmetry> >::const_iterator finder = symmetry_index_map.find(symmetry_of_image);
	if (finder == symmetry_index_map.end()) {
	  std::cerr << "SymmetryGroup::permutation_polytope(): error -- point of image symmetry "
		    << symmetry_of_image << " is not contained in the permutations matrices of points "
		    << symmetry_index_map << " -- exiting." << std::endl;
	  exit(1);
	}
	const size_type image_idx(finder->second);
	new_symmetry(preimage_idx) = image_idx;
      }
      if (add_center) {
	// add the next possible value to each symmetry for the center point:
	new_symmetry.push_back(coldim);
      }
      symmetry_generators.insert(new_symmetry);
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "\t ... done" << std::endl;
    }
    if (add_center) {
      Point center_point(id_point);
      for (size_type i = 0; i < points.no(); ++i) {
	center_point.add(points.col(i));
      }
      center_point.add(id_point.scale(-1));
      center_point.scale(FieldConstants::ONE / Field(points.no()));
      points.push_back(center_point);
    }
    symmetry_collectordata collector;
    SymmetryGroup symmetries(points.no(), symmetry_generators);
    points.homogenize();
    points.transform_to_full_rank();
    return std::pair<PointConfiguration, SymmetryGroup>(points, symmetries);
  }

  symmetry_collectordata SymmetryGroup::rowperm_symmetry_generators(const PointConfiguration& points) const {

    /// careful: can only be used for configurations with no duplicate points:
    const size_type coldim = points.coldim();
    const size_type rowdim = points.rowdim();
    symmetry_collectordata result;
    std::unordered_map<Vector, size_type, Hash<Vector> > column_index_map;

    // fill the table mapping columns to column indices:
    for (size_type col = 0; col < coldim; ++col) {
      column_index_map[points.col(col)] = col;
    }

    // loop over all transpositions of rows:
    for (const_iterator iter = begin();
	 iter != end();
	 ++iter) {

      // check whether the permutation of rows
      // induces a symmetry of columns and, if yes,
      // compute the resulting column symmetry:
      Symmetry new_symmetry(coldim);
      bool is_colsym = true;

      // check whether the points stay fixed under the permutation and
      // construct the corresponding column permutation:
      for (size_type preimage_idx = 0;
	   preimage_idx < coldim;
	   ++preimage_idx) {
      
	// get the column vector to compute the image vector:
	Vector image_vec = iter->map(points.col(preimage_idx));
      
	// find the index of the image:
	std::unordered_map<Vector, size_type, Hash<Vector> >::const_iterator finder = column_index_map.find(image_vec);
	if (finder == column_index_map.end()) {

	  // the permutation does not induce a symmetry on columns:
	  is_colsym = false;
	  break;
	}
	const size_type image_idx = column_index_map[image_vec];
      
	// set the corresponding symmetry entry:
	new_symmetry(preimage_idx) = image_idx;	
      }
      if (is_colsym) {
	result.insert(new_symmetry);
      }
    }
    return result;
  }

  SymmetryGroup SymmetryGroup::rowperm_symmetry_group(const PointConfiguration& points) const {
    SymmetryGroup result(points.coldim());
    result._generators = rowperm_symmetry_generators(points);
    symmetry_collectordata collector(result._generators);
    result._closure(result._generators, collector);
    result._update_iterarray(collector);
    return result;
  }

  // stream input/output:
  std::ostream& SymmetryGroup::pretty_print_generators(std::ostream& ost) const {
    _identity.pretty_print(ost);
    ost << std::endl;
    ost << std::endl;
    for (symmetry_collectordata::const_iterator iter = _generators.begin();
	 iter != _generators.end();
	 ++iter) {
      iter->pretty_print(ost);
      ost << std::endl;
    }
    return ost;
  }

  std::ostream& SymmetryGroup::pretty_print(std::ostream& ost) const {
    _identity.pretty_print(ost);
    ost << std::endl;
    ost << std::endl;
    for (SymmetryGroup::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      iter->pretty_print(ost);
      ost << std::endl;
    }
    return ost;
  }

  std::istream& SymmetryGroup::read_generators(std::istream& ist) {
    Symmetry s(_n);
    char c;
  
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
	if (ist >> s) {
	  if (s.n() == _n) {
	    _generators.insert(s);
	    _complete = false;
	  }
	  else {
#ifdef READ_DEBUG
	    std::cerr << "SymmetryGroup::read(std::istream&):"
		      << "Symmetry " << s << " not valid." << std::endl;
#endif	  
	    ist.clear(std::ios::failbit);
	    return ist;
	  }
	}
	else {
#ifdef READ_DEBUG
	  std::cerr << "SymmetryGroup::read(std::istream&):"
		    << "cannot find Symmetry." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "SymmetryGroup::read(std::istream&):"
		<< "missing `" << start_char << "'." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  void SymmetryGroup::_closure(const symmetry_collectordata& sd, symmetry_collectordata& collector) {

    static size_type count_products(0);
  
    // build containers for the symmetries multiplied and found in one phase;
    // in the first phase these are the symmetries currently in the data structure
    // and the empty container, resp.:
    symmetry_collectordata old_symmetries(sd);
    symmetry_collectordata new_symmetries;

    if (CommandlineOptions::verbose()) {
#ifndef STL_SYMMETRIES
      std::cerr << "computing closure of " << old_symmetries.size() << " symmetries with " 
		<< sd.size() <<" symmetry generators ..." << std::endl;
#else
      std::cerr << "computing closure of " << old_symmetries.size() << " symmetries with " 
		<< sd.size() <<" symmetry generators ..." << std::endl;
#endif
    }

    // indicate whether in a phase new symmetries have been found:
    bool new_elem(true);

    while (new_elem) {

      // no new symmetry so far in this phase:
      new_elem = false;

      // multiply every element
      for (symmetry_collectordata::const_iterator iter1 = old_symmetries.begin();
	   iter1 != old_symmetries.end();
	   ++iter1) {
	const Symmetry& g(*iter1);
#ifdef CHECK_SYMMETRIES
	std::cout << "found symmetry " << g << std::endl;
#endif
	for (symmetry_collectordata::const_iterator iter2 = sd.begin();
	     iter2 != sd.end();
	     ++iter2) {
	  const Symmetry& h(*iter2);
	  const Symmetry gh(g * h);
       
	  ++count_products;

	  if (CommandlineOptions::verbose()
	      && (count_products % (CommandlineOptions::report_frequency() * CommandlineOptions::report_frequency()) == 0)) {
	    std::cerr << "evaluated " << count_products << " products so far --- "
		      << "found " << new_symmetries.size() << " new symmetries." << std::endl;
	  }
	  if (gh == _identity) {
	    
	    // ignore the identity:
	    continue;
	  }
	  if (collector.find(gh) == collector.end()) {
	    new_elem = true;
#ifdef SUPER_VERBOSE
	    std::cerr << "new symmetry: " << gh << " with index " << gh.lex_index() << std::endl;
#endif
	    collector.insert(gh);
	    new_symmetries.insert(gh);
	  }
	}
      }
	
      if (CommandlineOptions::verbose()) {
	if (new_elem) {
	  std::cerr << collector.size() << " symmetries so far." << std::endl;
	}
      }
      
      // swap old and new symmetries:
#ifndef STL_SYMMETRIES
      old_symmetries = new_symmetries;
#else
      old_symmetries.swap(new_symmetries);
#endif    
      new_symmetries.clear();
    }
    collector.erase(_identity);
    _update_iterarray(collector);
    _complete = true;
    if (CommandlineOptions::verbose()) {
      std::cerr << "... found " << collector.size() << " symmetries without identity." << std::endl;
    }
  }

  void SymmetryGroup::_update_iterarray(const symmetry_collectordata& collector) {
    symmetry_iterdata::clear();
    symmetry_iterdata::reserve(collector.size());
    for (symmetry_collectordata::const_iterator iter = collector.begin();
	 iter != collector.end();
	 ++iter) {
      this->push_back(*iter);
    }
  }

  void SymmetryGroup::_generate_collector(symmetry_collectordata& collector) {
    collector.clear();
    collector.reserve(this->size());
    for (symmetry_iterdata::const_iterator iter = this->begin();
	 iter != this->end();
	 ++iter) {
      collector.insert(*iter);
    }
  }

}; // namespace topcom

// eof Symmetry.cc
