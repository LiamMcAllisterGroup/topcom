//////////////////////////////////////////////////////////////////////////
//
// SymmetricExtensionGraphNode.cc
// produced: 03/12/2020 jr
// last change: 03/12/2020 jr
//
/////////////////////////////////////////////////////////////////////////

#include "SymmetricExtensionGraphNode.hh"

namespace topcom {

  // static members:
  symmetry_table_type              SymmetricExtensionGraphNode::_symmetry_images_by_element;
  thread_local symmetry_cache_type SymmetricExtensionGraphNode::_symmetry_images_by_element_cache;

  // construct the root node with a given root partial triangulation
  // using the symmetry group to initialize the critical element table:

  SymmetricExtensionGraphNode::SymmetricExtensionGraphNode(const SymmetryGroup*  sgptr,
							   const PartialTriang&& partial_triang) :
    _symmetriesptr(sgptr),
    _partial_triang(std::move(partial_triang)),
    _critsimpidx_table() {
    try {
      _critsimpidx_table.resize(_symmetriesptr->size(), std::numeric_limits<size_type>::max());
    }
    catch (...) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "SymmetricExtensionGraphNode::SymmetricExtensionGraphNode(const SymmetryGroup&, const PartialTriang&): "
		<< "allocation of " << _symmetriesptr->size() << " int elements failed - exiting"
		<< std::endl;
      exit(1);
    }

    for (size_type symidx = 0; symidx < _symmetriesptr->size(); ++symidx) {
      const Symmetry& g = (*_symmetriesptr)[symidx];
      if (CommandlineOptions::simpidx_symmetries()) {
	_critsimpidx_table.push_back(critical_simpidx_lean(partial_triang, g, symidx));
      }
      else {
	_critsimpidx_table.push_back(critical_simpidx(partial_triang, g));
      }
    }
  }

  // functions:

  // the following is the core function:
  // it checks whether the child node of this node by extending subset by a new element
  // is lex-minimal; it is assumed that the new element is larger than all the existing
  // elements of subset; the critical element table is updated during the checking process
  // and returned if the extended subset is lex minimal:
  bool SymmetricExtensionGraphNode::child_is_lexmin(const Simplex&               new_simp,
						    critical_simpidx_table_type* new_critsimpidx_tableptr,
						    size_type*                   new_stabilizer_cardptr) const {
    const bool local_debug = false;

    *new_stabilizer_cardptr = 0UL;
    
    //////////////////////////////////////////////////////////////////////////////
    // disable the technique by answering the question without any smartness:
    //////////////////////////////////////////////////////////////////////////////
    // try {
    //   new_critsimpidx_tableptr->reserve(_critsimpidx_table.size());
    // }
    // catch (...) {
    //   std::cerr << "std::pair<bool, CriticalSimpidxTable> SymmetricExtensionGraphNode::child_is_lexmin(const Simplex&, CriticalSimpidxTable*) const: "
    // 	      << "allocation of " << _critsimpidx_table.size() << " int elements failed - exiting"
    // 	      << std::endl;
    //   exit(1);
    // }
    // for (size_type idx = 0; idx < _symmetriesptr->size(); ++idx) {
    //   // if ((*_symmetriesptr)[idx].lex_decreases((_partial_triang + new_simp).index_set(_partial_triang.rank()))) {
    //   // return false;
    //   // }
    //   new_critsimpidx_tableptr->push_back(std::numeric_limits<parameter_type>::max());
    // }
    // return true;
    //////////////////////////////////////////////////////////////////////////////
    // end disable
    //////////////////////////////////////////////////////////////////////////////

    if (local_debug || CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "SymmetricExtensionGraphNode<cocircuits>::child_is_lexmin(const Simplex& new_simp):" << std::endl;
      std::cerr << "checking extension of partial triangulation " << _partial_triang << " by new_element " << new_simp << " ..." << std::endl;
    }
    // first, we compute the extended partial triangulation
    // (without all the expensive auxiliary data in PartialTriang):
    SimplicialComplex new_partial_triang(_partial_triang);
    new_partial_triang += new_simp;
    const parameter_type rank = new_partial_triang.rank();
    const size_type new_simpidx = SimplicialComplex::index_of_simplex(new_simp, rank);

    // generate a table to save the updated critical elements:
    try {
      // new_critsimpidx_tableptr->insert(new_critsimpidx_tableptr->begin(), _critsimpidx_table.begin(), _critsimpidx_table.end());
      new_critsimpidx_tableptr->reserve(_symmetriesptr->size());
    }
    catch (...) {
      std::cerr << "std::pair<bool, CriticalSimpidxTable> SymmetricExtensionGraphNode::child_is_lexmin(const Simplex&, CriticalSimpidxTable*) const: "
		<< "allocation of " << _critsimpidx_table.size() << " int elements failed - exiting"
		<< std::endl;
      exit(1);
    }
    const std::vector<size_type>& img_of_elm_vec = _symmetry_images_by_element[new_simpidx];
    for (size_type symidx = 0; symidx < _symmetriesptr->size(); ++symidx) {
      const Symmetry& g = (*_symmetriesptr)[symidx];
      const size_type& critsimpidx = _critsimpidx_table[symidx];

      // here we take advantage of the special representation of the group:
      // no simplex has to be mapped, just read the image index from the array
      // representing the permutation on simplex indices:
      const size_type& new_simpidx_image = img_of_elm_vec[symidx];
      // const parameter_type& new_simpidx_image = g[new_simpidx];

      if (critsimpidx == std::numeric_limits<size_type>::max()) {

	// in case g(S) = S, the new critical element is the new element itself:
	if (new_simpidx_image < new_simpidx) {
	  return false;
	}
	if (new_simpidx_image > new_simpidx) {
	  // (*new_critsimpidx_tableptr)[symidx] = new_simpidx;
	  new_critsimpidx_tableptr->emplace_back(new_simpidx);
	  continue;
	}

	// the current symmetry is in the stabilizer of the new partial triangulation,
	// which therefore is not lex-decreased:
	new_critsimpidx_tableptr->emplace_back(std::numeric_limits<size_type>::max());
	++(*new_stabilizer_cardptr);
	continue;
      }

      // the order on simplices in the IndexTable has to be used:
      if (new_simpidx_image == critsimpidx) {

	// this case is the complicated case:
	// the image g(new_simpidx) of the new element under the symmetry g
	// is equal to the critical simplex index,
	// thus, there is a new critical simplex index for g w.r.t. partial_triang union new_simp,
	// and we have to compute the new critical element from scratch:
	const size_type& new_critsimpidx = critical_simpidx(new_partial_triang, g);

	if (new_critsimpidx == std::numeric_limits<size_type>::max()) {

	  // in this case, the new partial triangulation is fixed, thus it is lex minimal,
	  // and the critical-element table needs an update:
	  new_critsimpidx_tableptr->emplace_back(std::numeric_limits<size_type>::max());
	  ++(*new_stabilizer_cardptr);
	  continue;
	}
	else {
	  if (new_partial_triang.index_set_pure().contains(new_critsimpidx)) {
	    
	    // in this case, the critical simplex is in the new partial triang,
	    // thus it is lex minimal with new critical simplex,
	    // and the critical-element table needs an update:
	    new_critsimpidx_tableptr->emplace_back(new_critsimpidx);
	    continue;
	  }
	  
	  // in this case, neither the new subset is fixed nor the new critical element is
	  // in the image of the new subset, thus the new subset is not lex-minimal:
	  return false;
	}
      }
      if (new_simpidx_image > critsimpidx) {

	// the image g(new_simpidx) of the new simplex index under the symmetry g
	// is strictly larger than the critical simplex index,
	// thus, partial_triang union new_simp is lex-smaller than g(partial_triang union new_simp),
	// the critical simplex remains unchanged,
	// and we continue with the next symmetry:
	new_critsimpidx_tableptr->emplace_back(critsimpidx);
	continue;
      }

      // the image g(new_simpidx) of the new simplex index under the symmetry g
      // is strictly smaller than the critical simplex index,
      // thus, g(partial_triang union new_simp) is lex-smaller than partial_triang union new_simp,
      // we do not need updated critical simplex indices,
      // we return false, and the table built so far is irrelevant:
      
      return false;
    }

    // we have not found any colex-increasing symmetry;
    // in that case, all symmetries have been scanned,
    // and therefore all critical elements have been updated:
    return true;
  }
  bool SymmetricExtensionGraphNode::child_is_lexmin_lean(const Simplex&               new_simp,
							 critical_simpidx_table_type* new_critsimpidx_tableptr,
							 size_type*                   new_stabilizer_cardptr) const {
    const bool local_debug = false;
    *new_stabilizer_cardptr = 0UL;

    if (local_debug || CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "SymmetricExtensionGraphNode<cocircuits>::child_is_lexmin(const Simplex& new_simp):" << std::endl;
      std::cerr << "checking extension of partial triangulation " << _partial_triang << " by new_element " << new_simp << " ..." << std::endl;
    }
    
    // first, we compute the extended partial triangulation
    // (without all the expensive auxiliary data in PartialTriang):
    const parameter_type rank = _partial_triang.rank();
    const SimplicialComplex new_partial_triang(_partial_triang + new_simp);
    const size_type new_simpidx = SimplicialComplex::index_of_simplex(new_simp, rank);

    // generate a table to save the updated critical elements:
    try {
      new_critsimpidx_tableptr->reserve(_symmetriesptr->size());
    }
    catch (...) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "std::pair<bool, CriticalSimpidxTable> SymmetricExtensionGraphNode::child_is_lexmin(const Simplex&, CriticalSimpidxTable*) const: "
		<< "allocation of " << _critsimpidx_table.size() << " int elements failed - exiting"
		<< std::endl;
      exit(1);
    }

    // in the lean version, the symmetries in the node are the original symmetries on points:
    for (size_type symidx = 0; symidx < _symmetriesptr->size(); ++symidx) {
      const Symmetry& g = (*_symmetriesptr)[symidx];
      const size_type critsimpidx = _critsimpidx_table[symidx];
      size_type new_simpidx_image;
      if (CommandlineOptions::memopt()) {
	if (CommandlineOptions::localcache() == 0) {

	  // choose this branch if cache administration does not pay off:
	  new_simpidx_image = SimplicialComplex::index_of_simplex(g.map(new_simp), rank);
	}
	else {
	  const IndexPair index_pair(new_simpidx, symidx);
	  const size_type hash_value = Hash<IndexPair>()(index_pair);
	  const size_type cache_idx  = hash_value % _symmetry_images_by_element_cache.size();
	  symmetry_cache_entry_type& cache_entry_reference = _symmetry_images_by_element_cache[cache_idx];
	  if (cache_entry_reference.first == index_pair) {
	    new_simpidx_image = cache_entry_reference.second;
	  }
	  else {
	    new_simpidx_image = SimplicialComplex::index_of_simplex(g.map(new_simp), rank);
	    cache_entry_reference = std::move(symmetry_cache_entry_type(index_pair, new_simpidx_image));
	  }
	}
      }
      else {
	if (_symmetry_images_by_element[new_simpidx][symidx] == std::numeric_limits<size_type>::max()) {
	  
	  // bring requested value into the table:
	  _symmetry_images_by_element[new_simpidx][symidx] = SimplicialComplex::index_of_simplex(g.map(new_simp), rank);
	}

	// retrieve requested value from table:
	new_simpidx_image = _symmetry_images_by_element[new_simpidx][symidx];
      }
      if (critsimpidx == std::numeric_limits<size_type>::max()) {
	if (new_simpidx_image < new_simpidx) {
	  return false;
	}
	if (new_simpidx_image > new_simpidx) {
	  new_critsimpidx_tableptr->emplace_back(new_simpidx);
	  continue;
	}
	new_critsimpidx_tableptr->emplace_back(std::numeric_limits<size_type>::max());
	++(*new_stabilizer_cardptr);
	continue;
      }
      if (new_simpidx_image == critsimpidx) {
	const size_type new_critsimpidx = critical_simpidx_lean(new_partial_triang, g, symidx);
	if (new_critsimpidx == std::numeric_limits<size_type>::max()) {
	  new_critsimpidx_tableptr->emplace_back(std::numeric_limits<size_type>::max());
	  ++(*new_stabilizer_cardptr);
	  continue;
	}
	else {
	  if (new_partial_triang.index_set_pure().contains(new_critsimpidx)) {
	    new_critsimpidx_tableptr->emplace_back(new_critsimpidx);
	    continue;
	  }
	  return false;
	}
      }
      if (new_simpidx_image > critsimpidx) {
	new_critsimpidx_tableptr->emplace_back(critsimpidx);
	continue;
      }
      return false;
    }
    return true;
  }


  // auxiliary function to compute critical element from scratch for a symmetry:
  size_type SymmetricExtensionGraphNode::critical_simpidx(const SimplicialComplex& sc,
							  const Symmetry& g) const {
    const SimplicialComplex::IndexSet symdiff_idxset(sc.index_set_pure() ^ g.map(sc.index_set_pure()));
    if (symdiff_idxset.empty()) {
      return std::numeric_limits<size_type>::max(); // an encoding for infinity
    }
    else {
      return symdiff_idxset.min_elem();
    }
  }

  // auxiliary function to compute critical element from scratch for a symmetry:
  size_type SymmetricExtensionGraphNode::critical_simpidx_lean(const SimplicialComplex& sc,
							       const Symmetry& g,
							       const size_type symidx) const {
    // return (sc.index_set_pure() ^ g.map(sc).index_set_pure()).min_elem();

    if (CommandlineOptions::memopt()) {
      const SimplicialComplex::IndexSet symdiff_idxset(sc.index_set_pure() ^ g.map(sc).index_set_pure());
      if (symdiff_idxset.empty()) {
	return std::numeric_limits<size_type>::max(); // an encoding for infinity
      }
      else {
	return symdiff_idxset.min_elem();
      }
    }
    else {

      // use the cache to speed-up the map function:
      SimplicialComplex::IndexSet sc_image_indexset;
      for (SimplicialComplex::IndexSet::const_iterator isiter = sc.index_set_pure().begin();
	   isiter != sc.index_set_pure().end();
	   ++isiter) {
	
	// we know at this point that the images of
	// simplex indices for all simplices in the partial triangulation
	// have been cached already:
	sc_image_indexset += _symmetry_images_by_element[*isiter][symidx];
      }
      const SimplicialComplex::IndexSet symdiff_idxset(sc_image_indexset ^ sc.index_set_pure());
      if (symdiff_idxset.empty()) {
	return std::numeric_limits<size_type>::max();
      }
      else {
	return symdiff_idxset.min_elem();
      }
    }
  }

}; // namespace topcom

// eof SymmetricExtensionGraphNode.cc
