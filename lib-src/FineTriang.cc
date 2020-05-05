//////////////////////////////////////////////////////////////////////////
// FineTriang.cc
// produced: 01 Oct 1999 jr
// last change: 01 Oct 1999 jr
/////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "Circuits.hh"

#include "FineTriang.hh"

void FineTriang::_flip_in() {
  IntegerSet not_used(IntegerSet(0, _chiroptr->no()));
  not_used -= support();
  while (!not_used.is_empty()) {
    const size_type flip_vertex(*not_used.begin());
    _flip_in(not_used, flip_vertex);
    not_used -= flip_vertex;
  }
}

void FineTriang::_flip_in(IntegerSet& not_used, const size_type i) {
  // search for all simplices that geometrically contain i:
  for (SimplicialComplex::const_iterator iter1 = begin();
       iter1 != end();
       ++iter1) {
    dependent_set_type dependent_set = *iter1;
    assert (!dependent_set.contains(i));
    dependent_set += i;
    const Circuit circuit(*_chiroptr, dependent_set);
    const SimplicialComplex upper = circuit.upper_facets();
    const SimplicialComplex lower = circuit.lower_facets();
    const IntegerSet usupport = upper.support();
    const IntegerSet lsupport = lower.support();
    if (usupport.contains(i) && !lsupport.contains(i)) {
      assert(!lower.is_empty());
      const SimplicialComplex common_link(link(*lower.begin()));
      *this -= lower.join(common_link);
      *this += upper.join(common_link);
      break;
    }
    if (!usupport.contains(i) && lsupport.contains(i)) {
      assert(!upper.is_empty());
      const SimplicialComplex common_link(link(*upper.begin()));
      *this -= upper.join(common_link);
      *this += lower.join(common_link);
      break;
    }
  }
}
  
// eof FineTriang.cc
