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

#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#ifndef STL_CONTAINERS
#include "PlainArray.hh"
namespace topcom {
  typedef PlainArray<FastSimplicialComplex>   vertex_facet_table_data;
};
#else
#include <vector>
namespace topcom {
  typedef std::vector<FastSimplicialComplex>  vertex_facet_table_data;
};
#endif

namespace topcom {

  class VertexFacetTable : public vertex_facet_table_data {
  private:
    FastSimplicialComplex _union_complex;
  public:
    inline VertexFacetTable();
    inline VertexFacetTable(const VertexFacetTable&);
    inline ~VertexFacetTable();
    // additional constructors:
    VertexFacetTable(const FastSimplicialComplex&);
    // additional accessors:
    inline const FastSimplicialComplex& facets(const parameter_type) const;
    // writable accessors:
    inline FastSimplicialComplex* facetsptr(const parameter_type);
    // basic operations:
    inline VertexFacetTable& operator+=(const FastSimplicialComplex&);
    inline VertexFacetTable& operator-=(const FastSimplicialComplex&);
    // additional functions:
    operator FastSimplicialComplex() const;
    FastSimplicialComplex star(const Simplex&) const;
    FastSimplicialComplex link(const Simplex&) const;
    void remove_star(const Simplex&, SimplicialComplex&) const;
    const bool contains(const Simplex&) const;
    const bool contains_face(const Simplex&, const parameter_type, const FastSimplicialComplex&) const;
    const bool contains_face(const Simplex&, const FastSimplicialComplex&) const;
    const bool contains_free_face(const Simplex&, const parameter_type, 
				  const FastSimplicialComplex&, Simplex&) const;
    inline const bool contains_free_face(const Simplex&, const FastSimplicialComplex&, Simplex&) const;
  };

  // constructors:
  inline VertexFacetTable::VertexFacetTable() :
    vertex_facet_table_data(),
    _union_complex() {}
  
  inline VertexFacetTable::VertexFacetTable(const VertexFacetTable& vft) :
    vertex_facet_table_data(vft),
    _union_complex(vft._union_complex) {}

  // destructor:
  inline VertexFacetTable::~VertexFacetTable() {}

  // additional accessors:
  inline const FastSimplicialComplex& VertexFacetTable::facets(const parameter_type n) const {
    return (*this)[n];
  }

  // writable accessors:
  inline FastSimplicialComplex* VertexFacetTable::facetsptr(const parameter_type n) {
    return &(*this)[n];
  }

  // basic operations:
  inline VertexFacetTable& VertexFacetTable::operator+=(const FastSimplicialComplex& sc) {
    const Simplex& support(sc.support());
    for (parameter_type i = 0; i < size(); ++i) {
      if (support.contains(i)) {
	(*this)[i] += sc;
      }
    }
    _union_complex += sc;
    return *this;
  } 

  inline VertexFacetTable& VertexFacetTable::operator-=(const FastSimplicialComplex& sc) {
    for (parameter_type i = 0; i < size(); ++i) {
      (*this)[i] -= sc;
    }
    _union_complex -= sc;
    return *this;
  }

  inline const bool VertexFacetTable::contains_face(const Simplex& simp, 
						    const FastSimplicialComplex& sub) const {
    const parameter_type card = simp.card();
    return contains_face(simp, card, sub);
  }

  inline const bool VertexFacetTable::contains_free_face(const Simplex& simp, 
							 const FastSimplicialComplex& sub,
							 Simplex& facet) const {
    const parameter_type card = simp.card();
    return contains_free_face(simp, card, sub, facet);
  }

}; // namespace topcom

#endif
