////////////////////////////////////////////////////////////////////////////////
// 
// VertexFacetTable.hh 
//
//    produced: 27/02/98 jr
// last change: 27/02/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef VERTEXFACETTABLE_HH
#define VERTEXFACETTABLE_HH

//#include <bool.h>

#include "PlainArray.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

typedef PlainArray<FastSimplicialComplex>   _VertexFacetTable;

class VertexFacetTable : public _VertexFacetTable {
public:
  // additional constructors:
  VertexFacetTable(const FastSimplicialComplex&);
  // additional accessors:
  inline const FastSimplicialComplex& facets(const size_type) const;
  // writable accessors:
  inline FastSimplicialComplex* facetsptr(const size_type);
  // basic operations:
  inline VertexFacetTable& operator+=(const FastSimplicialComplex&);
  inline VertexFacetTable& operator-=(const FastSimplicialComplex&);
  // additional functions:
  operator FastSimplicialComplex() const;
  FastSimplicialComplex star(const Simplex&) const;
  FastSimplicialComplex link(const Simplex&) const;
  const bool contains(const Simplex&) const;
  const bool contains_face(const Simplex&, const size_type, const FastSimplicialComplex&) const;
  const bool contains_face(const Simplex&, const FastSimplicialComplex&) const;
  const bool contains_free_face(const Simplex&, const size_type, 
				const FastSimplicialComplex&, Simplex&) const;
  inline const bool contains_free_face(const Simplex&, const FastSimplicialComplex&, Simplex&) const;
};

// additional accessors:
inline const FastSimplicialComplex& VertexFacetTable::facets(const size_type n) const {
  return (*this)[n];
}

// writable accessors:
inline FastSimplicialComplex* VertexFacetTable::facetsptr(const size_type n) {
  return &(*this)[n];
}

// basic operations:
inline VertexFacetTable& VertexFacetTable::operator+=(const FastSimplicialComplex& sc) {
  const FastSimplicialComplex& support(sc.support());
  for (size_type i = 0; i < maxindex(); ++i) {
    if (support.contains(i)) {
      (*this)[i] += sc;
    }
  }
  return *this;
} 

inline VertexFacetTable& VertexFacetTable::operator-=(const FastSimplicialComplex& sc) {
  for (size_type i = 0; i < maxindex(); ++i) {
    (*this)[i] -= sc;
  }
  return *this;
}

inline const bool VertexFacetTable::contains_face(const Simplex& simp, 
						  const FastSimplicialComplex& sub) const {
  const size_type card = simp.card();
  return contains_face(simp, card, sub);
}

inline const bool VertexFacetTable::contains_free_face(const Simplex& simp, 
						       const FastSimplicialComplex& sub,
						       Simplex& facet) const {
  const size_type card = simp.card();
  return contains_free_face(simp, card, sub, facet);
}

#endif
