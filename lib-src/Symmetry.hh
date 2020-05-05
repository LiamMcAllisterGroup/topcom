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

#include <set>

#include "HashSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Permutation.hh"
#include "Matrix.hh"

#include "TriangNode.hh"
#include "Flip.hh"

class Symmetry : public Permutation {
public:
  inline Symmetry();
  inline explicit Symmetry(const parameter_type n);
  // initialize by permutation of length p.k() on numbers up to p.n()
  // interpreted as a cycle (cycle=true) or permutation:
  Symmetry(const Permutation& p, const bool cycle=true);
  inline Symmetry(const Symmetry& s);
  inline ~Symmetry();
  inline Symmetry& operator=(const Symmetry& s);
public:
  // actions on various data types:
  inline const parameter_type operator()(const parameter_type i)      const;
  inline const IntegerSet     operator()(const IntegerSet& is)        const;
  const SimplicialComplex     operator()(const SimplicialComplex& sc) const;
  const TriangNode            operator()(const TriangNode& tn)        const;
  const FlipRep               operator()(const FlipRep& fr)           const;
  const Permutation           operator()(const Permutation& p)        const;
  // tests:
  const bool maps(const SimplicialComplex&, const SimplicialComplex&) const;
  const bool fixes(const SimplicialComplex&) const;
  const bool maps(const TriangNode&, const TriangNode&) const;
  const bool fixes(const TriangNode&) const;
  // concatenation:
  Symmetry operator* (const Symmetry& s) const;
  // output transformation:
  Matrix PermutationMatrix() const;
  Matrix ReducedPermutationMatrix() const;
  // stream operations:
  friend std::istream&             operator>>(std::istream& ist, Symmetry& s);
};

inline Symmetry::Symmetry() : Permutation() {}

inline Symmetry::Symmetry(const parameter_type n) : Permutation(n,n) {}

inline Symmetry::Symmetry(const Symmetry& s) : Permutation(s) { 
  assert(n() == k()); 
}

inline Symmetry::~Symmetry() {}

inline Symmetry& Symmetry::operator=(const Symmetry& s) {
  if (this == &s) return *this;
  Permutation::operator=(s);
  return *this;
}

inline const parameter_type Symmetry::operator()(const parameter_type i) const {
  return (*this)[i];
}

inline const IntegerSet Symmetry::operator()(const IntegerSet& is) const {
  IntegerSet result;
  // this function is very time critical; thus, we save the end iterator:
  const IntegerSet::const_iterator end(is.end());
  for (Simplex::const_iterator iter = is.begin();
       iter != end;
       ++iter) {
    result += (*this)[*iter];
  }
  return result;
}

#ifndef STL_SYMMETRIES
typedef HashSet<Symmetry> symmetry_data;
#else
typedef std::set<Symmetry> symmetry_data;
inline std::ostream& operator<<(std::ostream& ost, const symmetry_data& sd) {
  for (symmetry_data::const_iterator iter = sd.begin();
       iter != sd.end();
       ++iter) {
    ost << *iter << std::endl;
  }
  return ost;
}
#endif

class SymmetryGroup : public symmetry_data {
private:
  parameter_type _n;
private:
  SymmetryGroup();
public:
  inline SymmetryGroup                (const parameter_type& n);
  inline SymmetryGroup                (const SymmetryGroup& sg);
  inline SymmetryGroup                (const parameter_type& n, const symmetry_data& sd);
  inline SymmetryGroup                (const SymmetryGroup& sg, const SimplicialComplex& fixed);
  inline ~SymmetryGroup               ();
  inline SymmetryGroup operator=      (const SymmetryGroup& sg);
  inline void          insert         (const Symmetry& s);
  const SimplicialComplex operator()  (const SimplicialComplex& fixed,
				       const Simplex& simp) const;
  inline const parameter_type n       () const { return _n; }

  inline std::ostream& write               (std::ostream& ost) const;
  inline friend std::ostream& operator>>   (std::ostream& ost, const SymmetryGroup& sg);
  std::istream& read_generators            (std::istream& ist);
  inline std::istream& read                (std::istream& ist);
  inline friend std::istream& operator<<   (std::istream& ist, SymmetryGroup& sg);
private:
  void _closure                       (const symmetry_data& sd);
};

inline SymmetryGroup::SymmetryGroup(const parameter_type& n) :
  symmetry_data(), _n(n) {
}

inline SymmetryGroup::SymmetryGroup(const SymmetryGroup& sg) : 
  symmetry_data(sg), _n(sg._n) {}

inline SymmetryGroup::SymmetryGroup(const parameter_type& n, const symmetry_data& sd) : 
  symmetry_data(sd), _n(n) {
//   symmetry_data::insert(Symmetry(_n));
  _closure(sd);
}

inline SymmetryGroup::SymmetryGroup(const SymmetryGroup& sg, const SimplicialComplex& fixed) :
//   symmetry_data(sg._n), _n(sg._n) {
  symmetry_data(), _n(sg._n) {
    for (SymmetryGroup::const_iterator iter = sg.begin();
       iter != sg.end();
       ++iter) {
    const Symmetry& g(*iter);
    if (g.fixes(fixed)) {
      symmetry_data::insert(g);
    }
  }
}

inline SymmetryGroup::~SymmetryGroup() {}

inline SymmetryGroup SymmetryGroup::operator=(const SymmetryGroup& sg) {
  if (this == &sg) return *this;
  symmetry_data::operator=(sg);
  return *this;
}

inline void SymmetryGroup::insert(const Symmetry& s) {
#ifdef INDEX_CHECK
  assert (s.n() == s.k());
  assert (s.n() == _n);
#endif
  symmetry_data::insert(s);
  _closure(*this);
}

inline std::ostream& SymmetryGroup::write(std::ostream& ost) const {
#ifndef STL_SYMMETRIES
  return symmetry_data::write(ost);
#else
  ost << '[';
  copy(begin(), end(), std::ostream_iterator<Symmetry>(ost,","));
  ost << ']';
#endif
}

inline std::ostream& operator<<(std::ostream& ost, const SymmetryGroup& sg) {
  return sg.write(ost);
}

inline std::istream& SymmetryGroup::read(std::istream& ist) {
  read_generators(ist);
  _closure(symmetry_data(*this));
  return ist;
}

inline std::istream& operator>>(std::istream& ist, SymmetryGroup& sg) {
  return sg.read(ist);
}

#endif

// eof Symmetry.hh
