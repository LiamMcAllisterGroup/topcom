////////////////////////////////////////////////////////////////////////////////
// 
// PermutationPartialComparator.hh 
//
//    produced: 15/03/2020 jr
// last change: 15/03/2020 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PERMUTATIONPARTIALCOMPARATOR_HH
#define PERMUTATIONPARTIALCOMPARATOR_HH

#include "Permutation.hh"

namespace topcom {

  class PermutationPartialComparator {
  protected:
    const Permutation* _permptr;
    parameter_type     _min;
    parameter_type     _max;
  public:
    // constructors:
    inline PermutationPartialComparator();
    inline PermutationPartialComparator(const PermutationPartialComparator&);
    inline PermutationPartialComparator(const Permutation&, const parameter_type, const parameter_type);
    // destructor:
    inline ~PermutationPartialComparator();
    // assignment:
    inline PermutationPartialComparator& operator=(const PermutationPartialComparator&);
    // accessors:
    inline const parameter_type min() const;
    inline const parameter_type max() const;
    // comparison:
    inline const bool operator==(const PermutationPartialComparator&) const;
    inline const bool operator!=(const PermutationPartialComparator&) const;
    // keys for containers:
    inline const size_type keysize() const;
    inline const size_type key(const size_type n) const;
    // stream output/input:
    inline std::ostream& write(std::ostream& ost) const;
    inline std::istream& read(std::istream& ist);
    inline friend std::ostream& operator<<(std::ostream& ost, const PermutationPartialComparator& p);
    inline friend std::istream& operator>>(std::istream& ist, PermutationPartialComparator& p);
  };

  // // specialization of Hash (for debugging):
  // template<>
  // class Hash<PermutationPartialComparator> {
  // public:
  //   inline size_type operator()(const PermutationPartialComparator& key) const {
  //     size_type result(0UL);
  //     size_type factor(1UL);
  //     const size_type keysize = key.keysize();
  //     for (size_type i = 0; i < keysize; ++i) {
  //       result = (result << 5UL) - result;
  // #ifdef HASH_DEBUG
  //       if (key.min() > 63) {
  // 	std::cerr << "result after shift = " << result << std::endl;
  //       }
  // #endif
  //       result += key.key(i);
  // #ifdef HASH_DEBUG
  //       if (key.min() > 63) {
  // 	std::cerr << "result after add   = " << result << std::endl;
  //       }
  // #endif
  //     }
  //     return result;
  //   }
  // };

  // constructors:
  inline PermutationPartialComparator::PermutationPartialComparator() : _permptr(0), _min(0), _max(-1) {}
  inline PermutationPartialComparator::PermutationPartialComparator(const PermutationPartialComparator& ppc) : 
    _permptr(ppc._permptr), _min(ppc._min), _max(ppc._max) {}
  inline PermutationPartialComparator::PermutationPartialComparator(const Permutation& perm, const parameter_type min, const parameter_type max) : 
    _permptr(&perm), _min(min), _max(max) {}

  // destructor:
  inline PermutationPartialComparator::~PermutationPartialComparator() {}

  // assignment:
  inline PermutationPartialComparator& PermutationPartialComparator::operator=(const PermutationPartialComparator& ppc) {
    if (&ppc == this) {
      return *this;
    }
    _permptr = ppc._permptr;
    _min     = ppc._min;
    _max     = ppc._max;
    return *this;
  }

  // accessors:
  inline const parameter_type PermutationPartialComparator::min() const {
    return _min;
  }

  inline const parameter_type PermutationPartialComparator::max() const {
    return _max;
  }

  // comparison:
  inline const bool PermutationPartialComparator::operator==(const PermutationPartialComparator& ppc) const {
    if ((_max != ppc._max) || (_min != ppc._min)) {
      return false;
    }
    for (parameter_type i = _min; i <= _max; ++i) {
      const parameter_type p1 = (*_permptr)[i];
      const parameter_type p2 = (*(ppc._permptr))[i];
      if (p1 == p2) {
	continue;
      }
      else if ((p1 < _min) && (p2 < _min)) {
	continue;
      }
      else if ((p1 > _max) && (p2 > _max)) {
	continue;
      }
      else {
	return false;
      }
    }
    return true;
  }

  inline const bool PermutationPartialComparator::operator!=(const PermutationPartialComparator& ppc) const {
    return !operator==(ppc);
  }

  // keys for containers:
  inline const size_type PermutationPartialComparator::keysize() const {
#ifdef HASH_DEBUG
    if (_min > 63) {
      std::cerr << "Permutation:keysize() was called - return value = " << _max - _min + 1 << std::endl;
    }
#endif
    return (size_type)(_max - _min + 1);
  }

  inline const size_type PermutationPartialComparator::key(const size_type n) const {
    const parameter_type value = (*_permptr)[(parameter_type)n + _min];
    if (value < _min) {
#ifdef HASH_DEBUG
      if (_min > 63) {
	std::cerr << "Permutation:key(" << n << ") was called - return value = "
		  << 0 << std::endl;
      }
#endif
      return (size_type)0;
    }
    else if (value > _max) {
#ifdef HASH_DEBUG
      if (_min > 63) {
	std::cerr << "Permutation:key(" << n << ") was called - return value = "
		  << _permptr->n() - 1 << std::endl;
      }
#endif
      return (size_type)(_permptr->n() - 1);
    }
    else {
#ifdef HASH_DEBUG
      if (_min > 63) {
	std::cerr << "Permutation:key(" << n << ") was called - return value = "
		  << value << std::endl;
      }
#endif
      return (size_type)value;
    }
  }

  // stream output/input:
  inline std::ostream& PermutationPartialComparator::write(std::ostream& ost) const {
    ost << "[" << _min << "=>" << *_permptr << "<=" << _max << "]";
    return ost;
  }

  inline std::istream& PermutationPartialComparator::read(std::istream& ist) {
    std::cerr << "PermutationPartialComparator::read(std::istream& ist): reading unsupported - exiting" << std::endl;
    exit(1);
  }

  inline std::ostream& operator<<(std::ostream& ost, const PermutationPartialComparator& p) {
    return p.write(ost);
  }

  inline std::istream& operator>>(std::istream& ist, PermutationPartialComparator& p) {
    return p.read(ist);
  }

}; // namespace topcom

#endif

// eof PermutationPartialComparator.hh
