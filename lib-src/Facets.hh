////////////////////////////////////////////////////////////////////////////////
// 
// Facets.hh 
//
//    produced: 12/03/98 jr
// last change: 12/03/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef FACETS_HH
#define FACETS_HH

#include <assert.h>
#include <set>

#include "LabelSet.hh"

#include "CommandlineOptions.hh"

#include "PointConfiguration.hh"
#include "Chirotope.hh"
#include "Cocircuits.hh"

namespace topcom {

  typedef LabelSet             facet_type;
  typedef std::set<facet_type> facets_data;

  class Facets : public facets_data {
  private:
    parameter_type _no;
    parameter_type _rank;
  public:
    // constructors:
  private:
    inline Facets();
  public:
    inline Facets(const parameter_type& no, const parameter_type& rank);
    inline Facets(const Facets&);
    Facets(const Cocircuits&);
    Facets(const Chirotope&, const SimplicialComplex& bd_triang);

    // destructor:
    inline ~Facets();

    // accessors:
    inline const parameter_type no() const;
    inline const parameter_type rank() const;

    // functions:
    inline bool contains(const facet_type&) const;
    inline bool contains_face(const facet_type&) const;
    
    // stream output/input:
    std::ostream& write(std::ostream&) const;
    std::istream& read(std::istream&);

    friend inline std::ostream& operator<<(std::ostream& ost, const Facets& f) {
      return f.write(ost);
    }
    friend inline std::istream& operator>>(std::istream& ist, Facets& f) {
      return f.read(ist);
    }
  };

  // constructors:
  inline Facets::Facets(const parameter_type& no, const parameter_type& rank) : facets_data(), _no(no), _rank(rank) {}
  inline Facets::Facets(const Facets& facets) : 
    facets_data(facets), _no(facets._no), _rank(facets._rank) {}

  // destructor:
  inline Facets::~Facets() {}

  // accessors:
  inline const parameter_type Facets::no()   const { return _no; }
  inline const parameter_type Facets::rank() const { return _rank; }

  // functions:
  inline bool Facets::contains(const facet_type& facet) const {
    return (find(facet) != end());
  }
  inline bool Facets::contains_face(const facet_type& facet) const {
    for (const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      if (iter->superset(facet)) {
	return true;
      }
    }
    return false;
  }

}; // namespace topcom

#endif

// eof Facets.hh
