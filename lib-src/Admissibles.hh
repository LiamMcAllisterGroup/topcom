////////////////////////////////////////////////////////////////////////////////
// 
// Admissibles.hh 
//
//    produced: 19/04/98 jr
// last change: 19/04/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef ADMISSIBLES_HH
#define ADMISSIBLES_HH

#include <assert.h>

#include "HashMap.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "Circuits.hh"

typedef HashMap<Simplex, SimplicialComplex> admissibles_data;

class Admissibles : public admissibles_data {
public:
  // constructors:
  inline Admissibles();
  inline Admissibles(const Admissibles&);
  Admissibles(const Circuits&, const Chirotope&, const bool = false);
  // destructor:
  inline ~Admissibles();
};

// constructors:
inline Admissibles::Admissibles() : admissibles_data() {}
inline Admissibles::Admissibles(const Admissibles& admissibles) : admissibles_data(admissibles) {}
// destructor:
inline Admissibles::~Admissibles() {}

#endif

// eof Admissibles.hh
