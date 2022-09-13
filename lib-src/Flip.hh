
////////////////////////////////////////////////////////////////////////////////
// 
// Flip.hh 
//
//    produced: 18/06/98 jr
// last change: 18/06/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef FLIP_HH
#define FLIP_HH

#include <assert.h>

#include "LabelSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "Circuits.hh"

namespace topcom {
  typedef SimplicialComplex                     outflip_type;
  typedef outflip_type                          inflip_type;
  typedef Circuit                               fliprep_type;
};

#ifndef STL_CONTAINERS
#include "Pair.hh"
#include "HashSet.hh"
namespace topcom {
  typedef Pair<outflip_type, inflip_type>       flip_type;
};
#else
namespace topcom {
  typedef std::pair<outflip_type, inflip_type>  flip_type;
};
#endif

namespace topcom {

  class TriangNode;
  class Flip;
  class DownFlip;

  class FlipRep : public fliprep_type {
  public:
    // constructors:
    inline FlipRep() :
      fliprep_type()  {}
    inline FlipRep(const FlipRep& f) :
      fliprep_type(f) {}
    inline FlipRep(FlipRep&& f) :
      fliprep_type(std::move(f)) {}
    FlipRep (const Chirotope& chiro, const dependent_set_type&, const TriangNode&);
    inline FlipRep (const Circuit& c) :
      fliprep_type(c) {}
    // destructor:
    inline ~FlipRep() {}
    // assignment:
    inline FlipRep& operator=(const FlipRep& f) {
      if (this == &f) {
	return *this;
      }
      fliprep_type::operator=(f);
      return *this;
    }
    inline FlipRep& operator=(FlipRep&& f) {
      if (this == &f) {
	return *this;
      }
      fliprep_type::operator=(std::move(f));
      return *this;
    }
    // casts:
    inline operator bool() const { return (!first.empty() && !second.empty()); }
    // functions:
    inline FlipRep inverse() const {
      return fliprep_type::inverse();
    }
    inline bool kills_vertex() const {
      return (second.card() == 1);
    }
    inline bool adds_vertex() const {
      return (first.card() == 1);
    }
    inline bool is_balanced() const {
      return (first.card() == second.card());
    }
    inline const size_type keysize() const {
      return 2;
    }
    inline const size_type key(const size_type n) const {
      return n < first.keysize() ? first.key(n) : second.key(n - first.keysize());
    }
  };

  class Flip : public flip_type {
  public:
    // constructors:
    inline Flip() :
      flip_type()  {}
    inline Flip(const Flip& f) :
      flip_type(f) {}
    inline Flip(Flip&& f) :
      flip_type(std::move(f)) {}
    inline Flip(const TriangNode& tn, const FlipRep& fliprep) :
      flip_type()  { _construct(tn, fliprep); }
    inline Flip(const Chirotope& chiro, const TriangNode& tn, const dependent_set_type& depset) :
      flip_type()  { _construct(chiro, tn, depset); }
    // destructor:
    inline ~Flip() {}
    // assignment:
    inline Flip& operator=(const Flip& f) {
      if (this == &f) {
	return *this;
      }
      flip_type::operator=(f);
      return *this;
    }
    inline Flip& operator=(Flip&& f) {
      if (this == &f) {
	return *this;
      }
      flip_type::operator=(std::move(f));
      return *this;
    }
    // casts:
    inline operator bool() const { return !(first.empty()); }
  private:
    // internal algorithms:
    void _construct(const TriangNode&, const Circuit&);
    void _construct(const Chirotope& chiro, const TriangNode& tn, const dependent_set_type& ds);
  };

}; // namespace topcom

#endif

// eof Flip.hh
