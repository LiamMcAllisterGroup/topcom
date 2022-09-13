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

#include <vector>
#include <assert.h>

#include "Global.hh"

#include "LabelSet.hh"
#include "HashKey.hh"

#include "CommandlineOptions.hh"

namespace topcom {

  // typedef PlainArray<parameter_type> permutation_data;
  // no real reason to use a container with reference counting on elements:
  typedef std::vector<parameter_type> permutation_data;

  class Permutation : public permutation_data {
  public:
    static const char start_char;
    static const char end_char;
    static const char delim_char;
  protected:
    parameter_type _n;
    parameter_type _k;
  public:

    // constructors:
    inline Permutation();
    inline Permutation(const Permutation&);
    inline Permutation(const Permutation&&);
    inline Permutation(const LabelSet&);
    inline Permutation(const parameter_type, const parameter_type);
    inline Permutation(const parameter_type, const parameter_type, const LabelSet&);
    inline Permutation(const parameter_type, const parameter_type, const permutation_data&);

    // destructor:
    inline ~Permutation();

    // assignment:
    inline Permutation& operator=(const Permutation&);
    inline Permutation& operator=(const Permutation&&);

    // accessors:
    inline const parameter_type n() const;
    inline const parameter_type k() const;

    // indexing:
    inline       parameter_type& operator()(const parameter_type);
    inline const parameter_type& operator()(const parameter_type) const;

    // functions:
    Permutation& push_back(const parameter_type);
    Permutation& push_back(const Permutation&);
    const int sign() const;
    const int sign(const parameter_type) const;
    const int sort();
    Permutation complement() const;
    Permutation deletion(const parameter_type m) const;
    Permutation reverse() const;
    bool lexnext();

    // keys for containers:
    const size_type keysize() const;
    const size_type key(const size_type n) const;

    // stream output/input:
    std::ostream& write(std::ostream& ost) const;
    std::istream& read(std::istream& ist);
    friend std::ostream& operator<<(std::ostream& ost, const Permutation& p);
    friend std::istream& operator>>(std::istream& ist, Permutation& p);
  };

  // constructors:
  inline Permutation::Permutation() : permutation_data(), _n(0), _k(0) {}

  inline Permutation::Permutation(const Permutation& perm) : 
    permutation_data(perm), _n(perm._n), _k(perm._k) {}

  inline Permutation::Permutation(const Permutation&& perm) : 
    permutation_data(std::move(perm)), _n(perm._n), _k(perm._k) {}

  inline Permutation::Permutation(const LabelSet& s) : 
    permutation_data(), _n(0), _k(0) {
    for (LabelSet::iterator iter = s.begin(); iter != s.end(); ++iter) {
      push_back(*iter);
    }
  }

  inline Permutation::Permutation(const parameter_type n, const parameter_type k) : 
    permutation_data(k), _n(n), _k(k) {
#ifdef INDEX_CHECK
    assert(_n >= _k);
#endif
    for (parameter_type i = 0; i < _k; ++i) {
      (*this)(i) = i;
    }
  }

  inline Permutation::Permutation(const parameter_type n, 
				  const parameter_type k, 
				  const LabelSet& s) : 
    permutation_data(k), _n(n), _k(k) {
    parameter_type i(0);
    for (LabelSet::iterator iter = s.begin(); iter != s.end(); ++iter) {
      assert(*iter <= _n);
      assert(i < k);
      (*this)(i) = *iter;
      ++i;
    }
  }

  inline Permutation::Permutation(const parameter_type n, 
				  const parameter_type k, 
				  const permutation_data& v) : 
    permutation_data(v), _n(n), _k(k) {}

  // destructor:
  inline Permutation::~Permutation() {}

  // assignment:
  inline Permutation& Permutation::operator=(const Permutation& p) {
    if (&p == this) {
      return *this;
    }
    permutation_data::operator=(p);
    _n = p._n;
    _k = p._k;
    return *this;
  }

  inline Permutation& Permutation::operator=(const Permutation&& p) {
    if (&p == this) {
      return *this;
    }
    permutation_data::operator=(std::move(p));
    _n = p._n;
    _k = p._k;
    return *this;
  }

  // indexing:
  inline parameter_type& Permutation::operator()(const parameter_type i) {
    return this->at(i);
  }
  
  inline const parameter_type& Permutation::operator()(const parameter_type i) const {
    return this->at(i);
  }

  // accessors:
  inline const parameter_type Permutation::n() const { return _n; }
  inline const parameter_type Permutation::k() const { return _k; }

  // keys for containers:
  inline const size_type Permutation::keysize() const {
#ifdef HASH_DEBUG
    std::cerr << "Permutation:keysize() was called - return value = " << size() << std::endl;
#endif
    return permutation_data::size();
  }

  inline const size_type Permutation::key(const size_type n) const {
    return (size_type)((*this)((parameter_type)n));
  }

}; // namespace topcom

#endif

// eof Permutation.hh
