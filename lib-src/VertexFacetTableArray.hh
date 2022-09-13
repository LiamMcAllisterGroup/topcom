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

#include "PlainArray.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

namespace topcom {

  typedef PlainArray<SimplicialComplex>   _VertexFacetTable;

  class VertexFacetTable : public _VertexFacetTable {
  private:
    VertexFacetTable();
    VertexFacetTable(const VertexFacetTable&);
    VertexFacetTable& operator=(const VertexFacetTable&);
  public:
    VertexFacetTable(const SimplicialComplex& sc);
    const SimplicialComplex& facets(const size_type n) const;
    SimplicialComplex* facetsptr(const size_type n);
    friend std::ostream& operator<<(std::ostream& ost, const VertexFacetTable& vft);
  };

  inline VertexFacetTable::VertexFacetTable
  (const SimplicialComplex& sc) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "VertexFacetTable::VertexFacetTable(const SimplicialComplex&)" 
	      << std::endl;
#endif
    Simplex support(sc.support());
    size_type init_size = 0;
    for (Simplex::iterator iter = support.begin();
	 iter != support.end();
	 ++iter)
      init_size = *iter;

    init_size++;
    resize(init_size);
    for (size_type i = 0; i < size(); ++i) {
      for (SimplicialComplex::iterator iter = sc.begin();
	   iter != sc.end();
	   ++iter)
	if (iter->contains(i))
	  (*this)[i] += *iter;

    }
  }
  inline SimplicialComplex* VertexFacetTable::facetsptr(const size_type n) {
    return &(*this)[n];
  }

  inline std::ostream& operator<<(std::ostream& ost, 
				  const VertexFacetTable& vft) {
    ost << "[";
    for (size_type i = 0; i < vft.size(); ++i)
      ost << "[" << i << ":" << vft[i] << "]";
      
    ost << "]";
    return ost;
  }

}; // namespace topcom

#endif
// eof VertexFacetTableArray.hh
