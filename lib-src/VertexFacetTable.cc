////////////////////////////////////////////////////////////////////////////////
// 
// VertexFacetTable.cc
//
//    produced: 16/06/98 jr
// last change: 16/06/98 jr
//
////////////////////////////////////////////////////////////////////////////////

#include "VertexFacetTable.hh"

namespace topcom {

  // additional constructors:
  VertexFacetTable::VertexFacetTable(const FastSimplicialComplex& sc) :
    vertex_facet_table_data(),
    _union_complex(sc) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "VertexFacetTable::VertexFacetTable(const FastSimplicialComplex&)" 
	      << std::endl;
#endif
    Simplex support(sc.support());
    size_type init_size = 0;
    for (Simplex::iterator iter = support.begin();
	 iter != support.end();
	 ++iter) {
      init_size = *iter;
    }
    ++init_size;
    resize(init_size);
    for (size_type i = 0; i < size(); ++i) {
      for (FastSimplicialComplex::iterator iter = sc.begin();
	   iter != sc.end();
	   ++iter)
	if (iter->contains(i))
	  (*this)[i] += *iter;
	
    }
  }

  // functions:
  VertexFacetTable::operator FastSimplicialComplex() const {
    FastSimplicialComplex result;
    for (size_type i = 0; i < size(); ++i) {
      result += (*this)[i];
    }
    return result;
  }

  FastSimplicialComplex VertexFacetTable::star(const Simplex& simp) const {
    FastSimplicialComplex result(_union_complex);
    for (Simplex::iterator iter = simp.begin(); iter != simp.end(); ++iter) {
      result *= (*this)[*iter];
    }
    return result;
  }

  void VertexFacetTable::remove_star(const Simplex&     simp,
				     SimplicialComplex& from_complex) const {
    FastSimplicialComplex star_complex(from_complex);
    for (Simplex::iterator iter = simp.begin(); iter != simp.end(); ++iter) {
      star_complex *= (*this)[*iter];
    }
    from_complex -= star_complex;
  }

  FastSimplicialComplex VertexFacetTable::link(const Simplex& simp) const {
    FastSimplicialComplex result(_union_complex);
    for (Simplex::iterator iter = simp.begin(); iter != simp.end(); ++iter) {
      result *= ((*this)[*iter] - simp);
    }
    return result;
  }

  const bool VertexFacetTable::contains_face(const Simplex& simp, 
					     const parameter_type card, 
					     const FastSimplicialComplex& sub) const {
    if (card + 1 > sub._maxcard)
      return false;

    if (sub.contains(simp))
      return false;

    parameter_type maxmincard = sub._mincard;
    parameter_type minmaxcard = sub._maxcard;
    if (card + 1 > maxmincard)
      maxmincard = card + 1;

    for (Simplex::iterator simp_iter = simp.begin();
	 simp_iter != simp.end();
	 ++simp_iter) {
      parameter_type vf_mincard = (*this)[*simp_iter]._mincard;
      parameter_type vf_maxcard = (*this)[*simp_iter]._maxcard;
      if (vf_mincard > maxmincard)
	maxmincard = vf_mincard;

      if (vf_maxcard < minmaxcard)
	minmaxcard = vf_maxcard;
	
    }
    const FastSimplicialComplex::IndexSet** setarray = new const FastSimplicialComplex::IndexSet*[card];
    size_type intersection_index = 0;
    for (parameter_type i = maxmincard; i < minmaxcard; ++i) {
      size_type count = 0;
      for (Simplex::iterator simp_iter = simp.begin();
	   simp_iter != simp.end();
	   ++simp_iter) {
	setarray[count++] = &(*this)[*(simp_iter)]._index_set[i];
      }
      if (sub._index_set[i].intersection_nonempty(setarray, card, intersection_index) != 0) {
	delete []setarray;
#ifdef COMPUTATIONS_DEBUG
	Simplex test_facet;
	if (!sub.contains_face(simp)) {
	  std::cerr << "internal error: " << simp << " not face in " << *this << std::endl;
	  std::cerr << "intersection_index: " << intersection_index << std::endl;
	  std::cerr << "maxmincard: " << maxmincard << std::endl;
	  std::cerr << "minmaxcard: " << minmaxcard << std::endl;
	  for (parameter_type j = maxmincard; j < minmaxcard; ++j) {
	    std::cout << "card = " << j << std::endl;
	    for (Simplex::iterator simp_iter = simp.begin(); 
		 simp_iter != simp.end(); 
		 ++simp_iter) {
	      std::cout << "(*this)[" << *simp_iter << "] = " 
			<< (*this)[*simp_iter]._index_set[j] << std::endl;

	    }
	  }
	  exit(1);
	}
#endif
	return true;
      }
    }
    delete []setarray;
    return false;
  }

  const bool VertexFacetTable::contains(const Simplex& simp) const {
    for (parameter_type i = 0; i < size(); ++i) {
      if ((*this)[i].contains(simp)) {
	return true;
      }
    }
    return false;
  }

  const bool VertexFacetTable::contains_free_face(const Simplex& simp,
						  const parameter_type card, 
						  const FastSimplicialComplex& sub,
						  Simplex& facet) const {
    if (card + 2 > sub._maxcard)
      return false;

    if (sub.contains(simp))
      return false;

    parameter_type maxmincard = sub._mincard;
    parameter_type minmaxcard = sub._maxcard;
    if (card + 1 > maxmincard) 
      maxmincard = card + 1;

    for (Simplex::iterator simp_iter = simp.begin();
	 simp_iter != simp.end();
	 ++simp_iter) {
      parameter_type vf_mincard = (*this)[*simp_iter]._mincard;
      parameter_type vf_maxcard = (*this)[*simp_iter]._maxcard;
      if (vf_mincard > maxmincard)
	maxmincard = vf_mincard;

      if (vf_maxcard < minmaxcard)
	minmaxcard = vf_maxcard;
	
    }
    const FastSimplicialComplex::IndexSet** setarray = new const FastSimplicialComplex::IndexSet*[card];
    int intersection_card = 0;
    size_type intersection_index = 0;
    for (size_type i = maxmincard; i < minmaxcard; ++i) {
      size_type count = 0;
      for (Simplex::iterator simp_iter = simp.begin();
	   simp_iter != simp.end();
	   ++simp_iter) {
	setarray[count++] = &(*this)[*simp_iter]._index_set[i];
      }
      int card_of_intersection = 
	sub._index_set[i].intersection_card(setarray, card, intersection_index);
      if (card_of_intersection == 1) {
	if (++intersection_card > 1) {
	  delete []setarray;
#ifdef COMPUTATIONS_DEBUG
	  if (sub.contains_free_face(simp, facet)) {
	    std::cerr << "internal error: " << simp << " free in " << *this << std::endl;
	    exit(1);
	  }
#endif
	  return false;
	}
	else {
	  facet = sub._index_table[i].get_obj(intersection_index);
	}
      }
      else if (card_of_intersection > 1) {
	delete []setarray;
#ifdef COMPUTATIONS_DEBUG
	if (sub.contains_free_face(simp, facet)) {
	  std::cerr << "internal error: " << simp << " free in " << *this << std::endl;
	  exit(1);
	}
#endif
	return false;
      }
    }
    if (intersection_card == 0) {
      delete []setarray;
#ifdef COMPUTATIONS_DEBUG
      if (sub.contains_free_face(simp, facet)) {
	std::cerr << "internal error: " << simp << " free in " << *this << std::endl;
	exit(1);
      }
#endif
      return false;
    }
#ifdef COMPUTATIONS_DEBUG
    Simplex test_facet;
    if (!sub.contains_free_face(simp, test_facet)) {
      std::cerr << "internal error: " << simp << " not free in " << *this << std::endl;
      std::cerr << "intersection_index: " << intersection_index << std::endl;
      std::cerr << "intersection_card: " << intersection_card << std::endl;
      std::cerr << "sub.mincard: " << sub.mincard() << std::endl;
      std::cerr << "sub.maxcard: " << sub.maxcard() << std::endl;
      for (size_type j = maxmincard; j < minmaxcard; ++j) {
	std::cout << "card = " << j << std::endl;
	for (Simplex::iterator simp_iter = simp.begin(); 
	     simp_iter != simp.end(); 
	     ++simp_iter) {
	  std::cout << "(*this)[" << *simp_iter << "] = " 
		    << (*this)[*simp_iter]._index_set[j] << std::endl;

	}
      }
      exit(1);
    }
#endif
    delete []setarray;
    return true;
  }

}; // namespace topcom

// eof VertexFacetTable.cc
