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

#include "Pair.hh"
#include "HashSet.hh"
#include "IntegerSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "Circuits.hh"

typedef SimplicialComplex                     outflip_type;
typedef outflip_type                          inflip_type;

// typedef dependent_set_type                    fliprep_type;
typedef Circuit                               fliprep_type;

typedef Pair<outflip_type, inflip_type>       flip_type;

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
  // casts:
  inline operator bool() const { return (!first.is_empty()); }
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
};

class Flip : public flip_type {
public:
  // constructors:
  inline Flip() :
    flip_type()  {}
  inline Flip(const Flip& f) :
    flip_type(f) {}
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
  // casts:
  inline operator bool() const { return !(first.is_empty()); }
private:
  // internal algorithms:
  void _construct(const TriangNode&, const Circuit&);
  void _construct(const Chirotope& chiro, const TriangNode& tn, const dependent_set_type& ds);
};

#endif

// eof Flip.hh
