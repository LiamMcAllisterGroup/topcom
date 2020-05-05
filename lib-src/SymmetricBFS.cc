////////////////////////////////////////////////////////////////////////////////
// 
// SymmetricBFS.cc
//
//    produced: 24/07/98 jr
// last change: 24/07/98 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "SymmetricBFS.hh"

void SymmetricBFS::_mark_equivalent_flips(const TriangNode&                     tnode, 
					  const tnode_container_type::iterator  find_iter,
					  const FlipRep&                        fliprep) {
#ifdef CHECK_MARK
  std::cerr << "flip before marking:" << std::endl;
  std::cerr << find_iter->key() << "->" << find_iter->data() << std::endl;
#endif
  // mark this flip:
#ifndef STL_CONTAINERS
  find_iter->dataptr()->mark_flip(fliprep);
#else
  find_iter->second.mark_flip(fliprep);
#endif
  // mark all equivalent flips:
  for (SymmetryGroup::const_iterator iter = _node_symmetries.begin();
       iter != _node_symmetries.end();
       ++iter) {
    const Symmetry& g = *iter;
#ifndef STL_CONTAINERS
    find_iter->dataptr()->mark_flip(g(fliprep));
#else
    find_iter->second.mark_flip(g(fliprep));
#endif
  }
#ifdef CHECK_MARK
  std::cerr << "flip after marking:" << std::endl;
  std::cerr << find_iter->key() << "->" << find_iter->data() << std::endl;
#endif
}

void SymmetricBFS::_mark_equivalent_flips(const TriangNode& tnode, 
					  TriangFlips&      tflips,
					  const FlipRep&    fliprep) {
#ifdef CHECK_MARK
  std::cerr << "flips before marking:" << std::endl;
  std::cerr << tflips << std::endl;
#endif
  // mark this flip:
  tflips.mark_flip(fliprep);
  // mark all equivalent flips:
  for (SymmetryGroup::const_iterator iter = _node_symmetries.begin();
       iter != _node_symmetries.end();
       ++iter) {
#ifndef STL_SYMMETRIES
    const Symmetry& g(iter->key());
#else
    const Symmetry& g(*iter);
#endif
    tflips.mark_flip(g(fliprep));
  }
#ifdef CHECK_MARK
  std::cerr << "flips after marking:" << std::endl;
  std::cerr << tflips << std::endl;
#endif
}

const int SymmetricBFS::_old_symmetry_class(const TriangNode& tnode) {
  if (CommandlineOptions::simple()) {
#ifndef STL_CONTAINERS
    if (_all_triangs.member(tnode)) {
#else
    if (_all_triangs.find(tnode) != _all_triangs.end()) {
#endif
      _orbitsize = 0;
      return -1;
    }

    // first check whether tnode represents an old symmetry class:
    for (SymmetryGroup::const_iterator iter = _symmetries.begin();
	 iter != _symmetries.end();
	 ++iter) {
#ifndef STL_SYMMETRIES
      const Symmetry& g(iter->key());
#else
      const Symmetry& g(*iter);
#endif
      _representative = g(tnode);

      triang_container_type::const_iterator t_iter = _all_triangs.find(_representative);
// #ifndef STL_CONTAINERS
//       if (!_orbit.member(_representative)) {
// 	if (_all_triangs.member(_representative)) {
// #else
      if (_orbit.find(_representative) == _orbit.end()) {
	if (t_iter != _all_triangs.end()) {
// #endif	  
	  _orbit.clear();
	  _orbitsize = 0;
	  return 1;
	}
      }
    }

    // next, check for the search predicate (e.g., regularity) for tnode:
    if ((*_search_pred_ptr)(*_pointsptr, *_chiroptr, tnode)) {
      
      // tnode has search predicate:
      _rep_has_search_pred = true;
      _orbit.insert(tnode);
    }
    else {
      _rep_has_search_pred = false;
      if (CommandlineOptions::symmetries_are_affine()) {

	// tnode's complete orbit does not have search predicate:
	_orbitsize = 0;
	return 0; // what we return does not matter anymore because _orbitsize is zero
      }
    }

    // now check the orbit of tnode for the search predicate (e.g., regularity):
    for (SymmetryGroup::const_iterator iter = _symmetries.begin();
	 iter != _symmetries.end();
	 ++iter) {
#ifndef STL_SYMMETRIES
      const Symmetry& g(iter->key());
#else
      const Symmetry& g(*iter);
#endif
      _representative = g(tnode);

#ifndef STL_CONTAINERS
      if (!_orbit.member(_representative)) {
#else
	if (_orbit.find(_representative) == _orbit.end()) {
#endif
	if (_rep_has_search_pred && CommandlineOptions::symmetries_are_affine()) {
	  _orbit.insert(_representative);
	}
	else {
	  if ((*_search_pred_ptr)(*_pointsptr, *_chiroptr, _representative)) {
	    _orbit.insert(_representative);
	  }
	}
      }
    }
  }
  else {

    // handle the identity individually:
    if ((_find_iter = _previous_triangs.find(tnode)) != _previous_triangs.end()) {
      _orbitsize = 0;
      return -1;
    }
    if ((_find_iter = _new_triangs.find(tnode)) != _new_triangs.end()) {
      _orbitsize = 0;
      return -2;
    }

    // first check whether we already found an equivalent triangluation:
    for (SymmetryGroup::const_iterator iter = _symmetries.begin();
	 iter != _symmetries.end();
	 ++iter) {
#ifndef STL_SYMMETRIES
      const Symmetry& g(iter->key());
#else
      const Symmetry& g(*iter);
#endif
      _representative = g(tnode);
#ifndef STL_CONTAINERS
      if (!_orbit.member(_representative)) {      
#else
      if (_orbit.find(_representative) == _orbit.end()) {
#endif
	if ((_find_iter = _previous_triangs.find(_representative)) != _previous_triangs.end()) {
#ifndef STL_SYMMETRIES
	  _transformation = iter->key();
#else
	  _transformation = *iter;
#endif
	  _orbit.clear();
	  _orbitsize = 0;
	  return 1;
	}
	if ((_find_iter = _new_triangs.find(_representative)) != _new_triangs.end()) {
#ifndef STL_SYMMETRIES
	  _transformation = iter->key();
#else
	  _transformation = *iter;
#endif
	  _orbit.clear();
	  _orbitsize = 0;
	  return 2;
	}
      }
    }
    
    // now check the search predicate (e.g., regularity, for the 
    // discovered representative tnode of the new sysmmetry class:
    if ((*_search_pred_ptr)(*_pointsptr, *_chiroptr, tnode)) {
      
      // tnode has search predicate:
      _rep_has_search_pred = true;
      _orbit.insert(tnode);
    }
    else {
      _rep_has_search_pred = false;
      if (CommandlineOptions::symmetries_are_affine()) {
	
	// tnode's complete orbit does not have search predicate:
	_orbitsize = 0;
	return 0; // what we return does not matter anymore because _orbitsize is zero
      }
    }

    // next, we need to check the orbit of tnode for the search predicate (e.g., regularity):
    for (SymmetryGroup::const_iterator iter = _symmetries.begin();
	 iter != _symmetries.end();
	 ++iter) {
#ifndef STL_SYMMETRIES
      const Symmetry& g(iter->key());
#else
      const Symmetry& g(*iter);
#endif
      _representative = g(tnode);

      // check whether we already have that image of tnode in the orbit:
#ifndef STL_CONTAINERS
      if (!_orbit.member(_representative)) {
#else
      if (_orbit.find(_representative) == _orbit.end()) {
#endif

	// if not, check the search predicate (e.g., regularity):
	if (_rep_has_search_pred && CommandlineOptions::symmetries_are_affine()) {

	  // in this case, no check required because symmetries do not change 
          // the search predicate:
	  _orbit.insert(_representative);
	}
	else {

	  // here we need to check the orbit element directly:
	  if ((*_search_pred_ptr)(*_pointsptr, *_chiroptr, _representative)) {
	    _orbit.insert(_representative);
	  }
	}
      }
    }
  }
#ifndef STL_CONTAINERS
  _orbitsize = _orbit.load();
#else
  _orbitsize = _orbit.size();
#endif
  _orbit.clear();
  return 0;
}

void SymmetricBFS::_process_newtriang(const TriangNode&     current_triang,
				      const TriangFlips&    current_flips,
				      const TriangNode&     next_triang,
				      const FlipRep&        current_fliprep) {
  const int where(_old_symmetry_class(next_triang));
  if (CommandlineOptions::simple()) {
    if (where != 0) {
      return;
    }
    if (_orbitsize > 0) {
      if ((*_output_pred_ptr)(*_pointsptr, *_chiroptr, next_triang)) {
	++_symcount;
	--_reportcount;
	_totalcount += _orbitsize;
	(*_cout_triang_ptr)(_symcount, next_triang);
	if (CommandlineOptions::verbose() && (_reportcount == 0)) {
	  _reportcount = CommandlineOptions::report_frequency();
	  std::cerr << _symcount << " symmetry classes --- "
		    << _totalcount << " total triangulations --- "
#ifndef STL_CONTAINERS
 		    << _all_triangs.load() + _new_triangs.load() << " stored." << std::endl;
#else
		    << _all_triangs.size() + _new_triangs.size() << " stored." << std::endl;
#endif
	}
      }
      TriangFlips next_flips;
      _new_triangs[next_triang] = next_flips;
      _all_triangs.insert(next_triang);
    }
  }
  else {
    if (where < 0) {

      // next_triang was found before: mark all equivalent flips in next_triang:
      const FlipRep& inversefliprep(current_fliprep.inverse());
      _node_symmetries = SymmetryGroup(_symmetries, next_triang);
      _mark_equivalent_flips(next_triang, _find_iter, inversefliprep);
      if (CommandlineOptions::output_flips()) {
	// output flip to found triangulation pointed to by _find_iter:
#ifndef STL_CONTAINERS
	size_type next_ID(_find_iter->key().ID());
#else
	size_type next_ID(_find_iter->first.ID());
#endif
	std::cout << "flip[" 
		  << _flipcount << "]:=" 
		  << '{' 
		  << current_triang.ID() 
		  << ',' 
		  << next_ID
		  << "};" 
		  << " // to known triang, supported by "
		  << current_fliprep
		  << std::endl;
	++_flipcount;
      }
      return;
    }
    if (where > 0) {

      // a triangulation equivalent to next_triang was found before: mark all equivalent flips in representative:
      const FlipRep& inversefliprep(_transformation(current_fliprep.inverse()));
      _node_symmetries = SymmetryGroup(_symmetries, _representative);
      _mark_equivalent_flips(_representative, _find_iter, inversefliprep);
      return;
    }
    if (_orbitsize > 0) {

      // we found at least one new triangulation in orbit satisfying 
      // the search predicate; thus we must save the representative
      _node_symmetries = SymmetryGroup(_symmetries, next_triang);
      TriangFlips next_flips = TriangFlips(*_chiroptr,
					   current_triang, 
					   current_flips, 
					   next_triang, 
					   Flip(current_triang, current_fliprep),
					   _symmetries,
					   _node_symmetries,
					   _only_fine_triangs);
      _mark_equivalent_flips(next_triang, next_flips, current_fliprep.inverse());
      if (CommandlineOptions::output_flips()) {
	// output flip to next_triang:
	std::cout << "flip[" 
		  << _flipcount << "]:=" 
		  << '{' 
		  << current_triang.ID() 
		  << ',' 
		  << next_triang.ID() 
		  << "};" 
		  << " // supported by "
		  << current_fliprep
		  << std::endl;
	++_flipcount;
      }
      if ((*_output_pred_ptr)(*_pointsptr, *_chiroptr, next_triang)) {
	++_symcount;
	--_reportcount;
	_totalcount += _orbitsize;
	(*_cout_triang_ptr)(_symcount, next_triang);
	if (CommandlineOptions::verbose() && (_reportcount == 0)) {
	  _reportcount = CommandlineOptions::report_frequency();
	  std::cerr << _symcount << " symmetry classes --- "
		    << _totalcount << " total triangulations --- "
#ifndef STL_CONTAINERS
 		    << _previous_triangs.load() + _new_triangs.load() 
#else
		    << _previous_triangs.size() + _new_triangs.size() 
#endif
		    << " currently stored." << std::endl;
	}
      }
#ifdef CHECK_NEW
      if (all_triangs.find(next_triang) != all_triangs.end()) {
	std::cerr << "Error in SymmetricBFS: old triangulation:" << std::endl;
	std::cerr << next_triang << std::endl;
	std::cerr << "reinserted into" << std::endl;
	std::cerr << all_triangs << std::endl;
	std::cerr << "current new triangs:" << std::endl;
	std::cerr << _new_triangs << std::endl;
	std::cerr << "current previous triangs:" << std::endl;
	std::cerr << _previous_triangs << std::endl;
	exit(1);
      }
#endif
#ifdef SUPER_VERBOSE
      std::cerr << next_triang << " is new." << std::endl;
#endif
      _new_triangs[next_triang] = next_flips;
    }
  }
}

void SymmetricBFS::_process_flips(const TriangNode&     current_triang,
				  const TriangFlips&    current_flips) {
#ifdef SUPER_VERBOSE
  std::cerr << "current_triang" << std::endl;
  std::cerr << current_triang << std::endl;
  std::cerr << "current_flips" << std::endl;
  std::cerr << current_flips << std::endl;
#endif
  for (MarkedFlips::const_iterator iter = current_flips.flips().begin();
       iter !=current_flips.flips().end(); 
       ++iter) {
#ifndef STL_FLIPS
    if (iter->data()) {
#else
    if ((*iter).second) {
#endif
      continue;
    }
#ifdef SUPER_VERBOSE
    std::cerr << "flipping flip " << iter->key() 
	      << " = " << Flip(current_triang, iter->key()) << std::endl;
#endif
#ifndef STL_FLIPS
    const FlipRep current_fliprep(iter->key());
#else
    const FlipRep current_fliprep((*iter).first);
#endif
    const Flip flip(current_triang, current_fliprep);
    const TriangNode next_triang(_symcount, current_triang, flip);
    _process_newtriang(current_triang, current_flips, next_triang, current_fliprep);
  }
}

void SymmetricBFS::_bfs_step() {
  // go through all triangulations found so far:
#ifndef STL_CONTAINERS
  while (!_previous_triangs.is_empty()) {
#else
  while (!_previous_triangs.empty()) {
#endif
    tnode_container_type::const_iterator iter(_previous_triangs.begin());
#ifndef STL_CONTAINERS
    const TriangNode current_triang(iter->key());
#else
    const TriangNode current_triang(iter->first);
#endif
    if (CommandlineOptions::simple()) {
      const TriangFlips current_flips(*_chiroptr, current_triang, _symmetries, _only_fine_triangs);
      _process_flips(current_triang, current_flips);
    }
    else {
#ifndef STL_CONTAINERS
      const TriangFlips current_flips(iter->data());
#else
      const TriangFlips current_flips(iter->second);
#endif
      _process_flips(current_triang, current_flips);
    }
    _previous_triangs.erase(current_triang);
  }
#ifdef CHECK_NEW
  for (tnode_container_type::const_iterator iter = _new_triangs.begin();
       iter != _new_triangs.end();
       ++iter) {
#ifndef STL_CONTAINERS
    all_triangs[iter->key()] = iter->data();
#else
    all_triangs[iter->first] = iter->second;
#endif
  }
#endif
  if (CommandlineOptions::verbose()) {
#ifndef STL_CONTAINERS
    std::cerr << _new_triangs.load() << " new symmetry classes." << std::endl;
#else
    std::cerr << _new_triangs.size() << " new symmetry classes." << std::endl;
#endif
#ifdef WATCH_MAXCHAINLEN
    std::cerr << "length of maximal chain in hash table _new_triangs: " 
	 << _new_triangs.maxchainlen() << std::endl;
#endif
  }
}

void SymmetricBFS::_bfs() {
#ifndef STL_CONTAINERS
  while (!_previous_triangs.is_empty()) {
#else
  while (!_previous_triangs.empty()) {
#endif
    _bfs_step();
#ifndef STL_CONTAINERS
    tnode_container_type tmp;
    _previous_triangs = _new_triangs;
    _new_triangs = tmp;
#else
    _previous_triangs.swap(_new_triangs);
#endif
 
    // dump status if requested:
    if (CommandlineOptions::dump_status()) {
      if  (this->_processed_count % CommandlineOptions::dump_frequency() == 0) {
	std::ostringstream filename_str;
	filename_str << CommandlineOptions::dump_file() << "." << _dump_no % CommandlineOptions::dump_rotations();
	_dump_str.open(filename_str.str().c_str(), std::ios::out | std::ios::trunc);
	write(_dump_str);
	_dump_str.close();
	++_dump_no;
	_processed_count = 0;
      }
      ++_processed_count;
    }
  }
}

// stream input:

std::istream& SymmetricBFS::read(std::istream& ist) {
  std::string dump_line;

  while ((std::getline(ist, dump_line))) {
    std::string::size_type lastPos = dump_line.find_first_not_of(" ", 0);
    std::string::size_type pos     = dump_line.find_first_of(" ", lastPos);
    std::string keyword            = dump_line.substr(lastPos, pos - lastPos);

    // first, some data is parsed that makes sure that the dumped computational results were
    // obtained with the "right" data:
    if (keyword == "_no") {
      lastPos = dump_line.find_first_not_of(" ", pos);
      std::string value = dump_line.substr(lastPos, dump_line.length());
      std::istringstream istrst (value, std::ios::in);
      parameter_type no_check;
      
      if (!(istrst >> no_check)) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): error while reading _no; exiting" << std::endl;
	exit(1);
      }
      if (_no != no_check) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): no of points in input differs from no of points in dump file; exiting" << std::endl;
	exit(1);
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "no of points in input conincides with no of points in dump file: okay" << std::endl;
      }
    }
    if (keyword == "_rank") {
      lastPos = dump_line.find_first_not_of(" ", pos);
      std::string value = dump_line.substr(lastPos, dump_line.length());
      std::istringstream istrst (value, std::ios::in);
      parameter_type rank_check;

      if (!(istrst >> rank_check)) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): error while reading _rank; exiting" << std::endl;
	exit(1);
      }
      if (_rank != rank_check) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): rank of input differs from rank in dump file; exiting" << std::endl;
	exit(1);
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "rank of input conincides with rank of dump file: okay" << std::endl;
      }
    }
    if (_pointsptr) {
      if (keyword == "_points") {
	lastPos = dump_line.find_first_not_of(" ", pos);
	std::string value = dump_line.substr(lastPos, dump_line.length());
	std::istringstream istrst (value, std::ios::in);
	PointConfiguration points_check;
	
	if (!(istrst >> points_check)) {
	  std::cerr << "SymmetricBFS::read(std::istream& ist): error while reading _points; exiting" << std::endl;
	  exit(1);
	}
	if (*_pointsptr!= points_check) {
	  std::cerr << "SymmetricBFS::read(std::istream& ist): points of input differ from points in dump file; exiting" << std::endl;
	  exit(1);
	}
	if (CommandlineOptions::debug()) {
	  std::cerr << "points of input conincide with points in dump file: okay" << std::endl;
	}
      }
    }
    if (_chiroptr) {
      if (keyword == "_chiro") {
	lastPos = dump_line.find_first_not_of(" ", pos);
	std::string value = dump_line.substr(lastPos, dump_line.length());
	std::istringstream istrst (value, std::ios::in);
	Chirotope chiro_check;
	
	if (!(istrst >> chiro_check)) {
	  std::cerr << "SymmetricBFS::read(std::istream& ist): error while reading _chiro; exiting" << std::endl;
	  exit(1);
	}
	if ((*_chiroptr) != chiro_check) {
	  std::cerr << "SymmetricBFS::read(std::istream& ist): chirotope of input differs from chirotope in dump file; exiting" << std::endl;
	  exit(1);
	}
	if (CommandlineOptions::debug()) {
	  std::cerr << "chirotope of input conincides with chirotope of dump file: okay" << std::endl;
	}
      }
    }
    if (keyword == "_symmetries") {
      lastPos = dump_line.find_first_not_of(" ", pos);
      std::string value = dump_line.substr(lastPos, dump_line.length());
      std::istringstream istrst (value, std::ios::in);
      SymmetryGroup symmetries_check(_no);
      
      if (!(istrst >> symmetries_check)) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): error while reading _symmetries; exiting" << std::endl;
	exit(1);
      }
      if (_symmetries != symmetries_check) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): symmetries of input differ from symmetries in dump file; exiting" << std::endl;
	exit(1);
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "symmetries of input conincide with _symmetries in dump file: okay" << std::endl;
      }
    }
    // finally, parse the partial computational result from the dump file:
    if (keyword == "_previous_triangs") {
      lastPos = dump_line.find_first_not_of(" ", pos);
      std::string value = dump_line.substr(lastPos, dump_line.length());
      std::istringstream istrst (value, std::ios::in);
      if (!(istrst >> _previous_triangs)) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): error while reading _previous_triangs; exiting" << std::endl;
	exit(1);
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "_previous_triangs initialized with " << _previous_triangs << std::endl;
      }
    }
    if (keyword == "_new_triangs") {
      lastPos = dump_line.find_first_not_of(" ", pos);
      std::string value = dump_line.substr(lastPos, dump_line.length());
      std::istringstream istrst (value, std::ios::in);
      if (!(istrst >> _new_triangs)) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): error while reading _previous_triangs; exiting" << std::endl;
	exit(1);
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "_new_triangs initialized with " << _new_triangs << std::endl;
      }
    }
    if (keyword == "_totalcount") {
      lastPos = dump_line.find_first_not_of(" ", pos);
      std::string value = dump_line.substr(lastPos, dump_line.length());      
      std::istringstream istrst (value, std::ios::in);      
      if (!(istrst >> _totalcount)) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): error while reading _totalcount; exiting" << std::endl;
	exit(1);
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "_totalcount initialized with " << _totalcount << std::endl;
      }
    }
    if (keyword == "_symcount") {
      lastPos = dump_line.find_first_not_of(" ", pos);
      std::string value = dump_line.substr(lastPos, dump_line.length());
      std::istringstream istrst (value, std::ios::in);
      if (!(istrst >> _symcount)) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): error while reading _symcount; exiting" << std::endl;
	exit(1);
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "_symcount initialized with " << _symcount << std::endl;
      }
    }
    if (keyword == "_reportcount") {
      lastPos = dump_line.find_first_not_of(" ", pos);
      std::string value = dump_line.substr(lastPos, dump_line.length());
      std::istringstream istrst (value, std::ios::in);
      if (!(istrst >> _reportcount)) {
	std::cerr << "SymmetricBFS::read(std::istream& ist): error while reading _reportcount; exiting" << std::endl;
	exit(1);
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "_reportcount initialized with " << _reportcount << std::endl;
      }
    }
  }
  return ist;
}

// eof SymmetricBFS.cc
