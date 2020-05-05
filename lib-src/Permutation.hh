////////////////////////////////////////////////////////////////////////////////
// 
// Permutation.hh 
//
//    produced: 13/03/98 jr
// last change: 13/03/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PERMUTATION_HH
#define PERMUTATION_HH

#include <assert.h>

#include "PlainArray.hh"
#include "IntegerSet.hh"

#include "CommandlineOptions.hh"


typedef PlainArray<parameter_type> permutation_data;

class Permutation : public permutation_data {
private:
  parameter_type _n;
  parameter_type _k;
public:
  // constructors:
  inline Permutation();
  inline Permutation(const Permutation&);
  inline Permutation(const IntegerSet&);
  inline Permutation(const parameter_type, const parameter_type);
  inline Permutation(const parameter_type, const parameter_type, const IntegerSet&);
  // destructor:
  inline ~Permutation();
  // accessors:
  inline const parameter_type n() const;
  inline const parameter_type k() const;
  // functions:
  Permutation& append(const parameter_type);
  Permutation& append(const Permutation&);
  const int sign() const;
  const int sign(const parameter_type) const;
  const int sort();
  Permutation complement() const;
  Permutation deletion(const parameter_type m) const;
  Permutation reverse() const;
  bool lexnext();
  // stream output/input:
  friend std::ostream& operator<<(std::ostream& ost, const Permutation& p);
  friend std::istream& operator>>(std::istream& ist, Permutation& p);
};

// constructors:
inline Permutation::Permutation() : permutation_data(), _n(0), _k(0) {}
inline Permutation::Permutation(const Permutation& perm) : 
  permutation_data(perm), _n(perm._n), _k(perm._k) {}
inline Permutation::Permutation(const IntegerSet& s) : 
  permutation_data(), _n(0), _k(0) {
  for (IntegerSet::iterator iter = s.begin(); iter != s.end(); ++iter) {
    append(*iter);
  }
}
inline Permutation::Permutation(const parameter_type n, const parameter_type k) : 
  permutation_data(k), _n(n), _k(k) {
#ifdef INDEX_CHECK
  assert(_n > _k);
#endif
  for (parameter_type i = 0; i < _k; ++i) {
    (*this)[i] = i;
  }
}
inline Permutation::Permutation(const parameter_type n, 
				const parameter_type k, 
				const IntegerSet& s) : 
  permutation_data(k), _n(n), _k(k) {
  parameter_type i(0);
  for (IntegerSet::iterator iter = s.begin(); iter != s.end(); ++iter) {
    assert(*iter <= _n);
    assert(i < k);
    (*this)[i] = *iter;
    ++i;
  }
}

// destructor:
inline Permutation::~Permutation() {}

// accessors:
inline const parameter_type Permutation::n() const { return _n; }
inline const parameter_type Permutation::k() const { return _k; }

#endif

// eof Permutation.hh
