////////////////////////////////////////////////////////////////////////////////
// 
// SymmetricFlipGraph.cc
//
//    produced: 24/07/98 jr
// last change: 07/02/2020 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "SymmetricFlipGraph.hh"
#include "PlacingTriang.hh"

namespace topcom {
  SymmetricFlipGraph::SymmetryWorker::SymmetryWorker(SymmetryWorker&& sw) :
    _workerID(sw._workerID),
    _callerptr(sw._callerptr),
    _worker_symmetriesptr(sw._worker_symmetriesptr),
    _tn(sw._tn),
    _equivalent_tnode(sw._equivalent_tnode),
    _gkz(sw._gkz),
    _equivalent_gkz(sw._equivalent_gkz),
    _state(State::idle) {
    if (CommandlineOptions::debug()) {
      std::cerr << "generated a worker with symmetries " << *_worker_symmetriesptr << std::endl;
    }
  }

  SymmetricFlipGraph::SymmetryWorker::SymmetryWorker(const int                      workerID,
						     SymmetricFlipGraph&            sfg,
						     simpidx_symmetries_type&       syms) :
    _workerID(workerID),
    _callerptr(&sfg),
    _worker_symmetriesptr(&syms),
    _tn(-1, sfg._no, sfg._rank, SimplicialComplex()),
    _equivalent_tnode(-1, sfg._no, sfg._rank, SimplicialComplex()),
    _gkz(sfg._no),
    _equivalent_gkz(sfg._no),
    _state(State::idle) {
    if (CommandlineOptions::debug()) {
      std::cerr << "generated a worker with symmetries " << *_worker_symmetriesptr << std::endl;
    }
  }

  void SymmetricFlipGraph::SymmetryWorker::find_old_symmetry_class() {
    // some other thread may have found that current_partial_triang is not new:
    if (_callerptr->_location_of_old_symmetry_class != 0) {
      return;
    }
    if (_worker_symmetriesptr->empty()) {
      return;
    }
    
    for (simpidx_symmetries_type::iterator iter = _worker_symmetriesptr->begin();
	 iter != _worker_symmetriesptr->end();
	 ++iter) {
      
      // some other thread may have found that current_partial_triang is not new:
      if (_callerptr->_location_of_old_symmetry_class != 0) {
	return;
      }
      std::pair<Symmetry, Symmetry>& g_pair(*iter);
      // const Symmetry& g_elem(iter->first);
      // const Symmetry& g_simpidx(iter->second);

      // use GKZ vectors for preliminary check:
      if (CommandlineOptions::use_gkz() && _callerptr->_voltableptr) {
	g_pair.first.map_into(_gkz, _equivalent_gkz);
	if (_callerptr->_known_gkzs.find(_equivalent_gkz) == _callerptr->_known_gkzs.end()) {
	  continue;
	}
      }
      
      // map tnode depending on whether simplex-index representation is present:
      // if yes, map the triangulation's index set; if no, map all simplices
      // in the ordinary way:
      
      _callerptr->_map_into(g_pair, _tn, _equivalent_tnode);
    
      // old direct method is deprecated:
      // const TriangNode& equivalent_tnode(g.map(_tn));
    
      tnode_container_type::iterator find_previous_iter = _callerptr->_previous_triangs.find(_equivalent_tnode);
      if (find_previous_iter != _callerptr->_previous_triangs.end()) {
	std::lock_guard<std::mutex> pushresults_guard(_callerptr->_old_symmetry_class_pushresults_mutex);
	_callerptr->_orbitsize = 0;
	_callerptr->_find_iter = std::move(find_previous_iter);
	_callerptr->_transformation = std::move(g_pair.first);
	_callerptr->_representative = std::move(_equivalent_tnode);
	_callerptr->_location_of_old_symmetry_class = 1;
	return;
      }
      tnode_container_type::iterator find_new_iter = _callerptr->_new_triangs.find(_equivalent_tnode);
      if (find_new_iter != _callerptr->_new_triangs.end()) {
	std::lock_guard<std::mutex> pushresults_guard(_callerptr->_old_symmetry_class_pushresults_mutex);
	_callerptr->_orbitsize = 0;
	_callerptr->_find_iter = std::move(find_new_iter);
	_callerptr->_transformation = std::move(g_pair.first);
	_callerptr->_representative = std::move(_equivalent_tnode);
	_callerptr->_location_of_old_symmetry_class = 2;
	return;
      }
    }
    return;
  }

  void SymmetricFlipGraph::SymmetryWorker::build_orbit_with_searchpred() const {
    for (simpidx_symmetries_type::const_iterator iter = _worker_symmetriesptr->begin();
	 iter != _worker_symmetriesptr->end();
	 ++iter) {
      const std::pair<Symmetry, Symmetry>& g_pair(*iter);
      // const Symmetry& g_elem(iter->first);
      // const Symmetry& g_simpidx(iter->second);
      if (!_callerptr->_node_symmetryptrs.second.empty()
	  && (_callerptr->_node_symmetryptrs.second.find(&g_pair.first) != _callerptr->_node_symmetryptrs.second.end())) {
	// g is in stabilizer of tnode - nothing new:
	continue;
      }

      // generic map (via index set if present, via set if not):
      const TriangNode equivalent_tnode(std::move(_callerptr->_map(g_pair, _tn)));

      // deprecated:
      // const TriangNode& equivalent_tnode = g.map(tnode);

      // check whether we already have that image of tnode in one of the workers' orbit:
      bool is_new(true);
      for (int i = 0; i < _callerptr->_no_of_threads; ++i) {	
	if (_callerptr->_worker_orbits[i].find(equivalent_tnode) != _callerptr->_worker_orbits[i].end()) {
	  is_new = false;
	  break;
	}
      }
    
      if (is_new) {
	if ((*_callerptr->_search_pred_ptr).is_invariant() && (*_callerptr->_output_pred_ptr).is_invariant()) {
	  if (_callerptr->_rep_has_search_pred && (*_callerptr->_output_pred_ptr)(*_callerptr->_pointsptr,
										  *_callerptr->_chiroptr,
										  *_callerptr->_inctableptr,
										  _tn)) {
	  
	    // in this case, no check required because symmetries do not change 
	    // the search predicate:
	    _callerptr->_worker_orbits[_workerID].insert(std::move(equivalent_tnode));
	  }
	  else {
	  
	    // no further triangulation with search predicate in orbit:
	    break;
	  }
	}
	else {
	  // if not, check the search predicate (e.g., regularity):
	  // here we need to check the orbit element directly:
	  if ((*_callerptr->_search_pred_ptr)(*_callerptr->_pointsptr,
					      *_callerptr->_chiroptr,
					      *_callerptr->_inctableptr,
					      equivalent_tnode)
	      && (*_callerptr->_output_pred_ptr)(*_callerptr->_pointsptr,
						 *_callerptr->_chiroptr,
						 *_callerptr->_inctableptr,
						 equivalent_tnode)) {
	    _callerptr->_worker_orbits[_workerID].insert(std::move(equivalent_tnode));
	    if (_callerptr->_print_triangs) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cout << "Tworkerorbit[" << _tn.ID() << ","
			<< _workerID << ","
			<< _callerptr->_worker_orbits[_workerID].size() - 1 << "] := "
			<< equivalent_tnode << ";" << std::endl;
	    }
	  }
	}
      }
    }
  }

  // interface with caller:
  void SymmetricFlipGraph::SymmetryWorker::pass_work_for_find_old_symmetry_class(const TriangNode& tn,
										 const Vector&     gkz) {
    _tn = tn;
    _gkz = gkz;
    std::lock_guard<std::mutex> lock(_callerptr->_main_mutex);
    _state = State::hired_for_find_old_symmetry_class;
  }
  
  void SymmetricFlipGraph::SymmetryWorker::pass_work_for_build_orbit_with_searchpred(const TriangNode& tn) {
    _tn = tn;
    std::lock_guard<std::mutex> lock(_callerptr->_main_mutex);
    _state = State::hired_for_build_orbit_with_searchpred;
  }
    
  void SymmetricFlipGraph::SymmetryWorker::stop_worker() {
    _state = State::stopped;
    worker_condition.notify_one();
  }

  bool SymmetricFlipGraph::SymmetryWorker::_wake_up() const {
    return (_state != State::idle);
  }
  
  void SymmetricFlipGraph::SymmetryWorker::operator()() {

    static const bool local_debug = false;

    // perform the postponed generation of simpidx symmetries:
    if (CommandlineOptions::simpidx_symmetries()) {
      size_type cnt = 0;
      if (CommandlineOptions::verbose()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "inserting simpidx symmetries for worker " << _workerID << " ..." << std::endl;
      }
      for (simpidx_symmetries_type::iterator iter = _worker_symmetriesptr->begin();
	   iter != _worker_symmetriesptr->end();
	   ++iter) {
	iter->second = std::move(iter->first.simpidx_symmetry(_callerptr->_rank));
	if (CommandlineOptions::verbose() && (++cnt % CommandlineOptions::report_frequency() == 0)) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << cnt << " symmetries processed so far." << std::endl;
	}
      }
      if (CommandlineOptions::verbose()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done" << std::endl;
      }
    }
    
    while (_state != State::stopped) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "worker " << _workerID << " processing state " << _state
		  << " with " << _callerptr->_no_of_busy_threads << " done threads ..." << std::endl;
      }

      // wait until threre is work:
      while (_state == State::idle) {
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "worker " << _workerID << " waiting ..." << std::endl;
	}

	// wait for new work from main thread:
	std::unique_lock<std::mutex> worker_lock(_callerptr->_main_mutex);
	++_callerptr->_no_of_waiting_threads;
	worker_condition.wait(worker_lock, [this] { return _wake_up(); });
	--_callerptr->_no_of_waiting_threads;
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "worker " << _workerID << " waking up ..." << std::endl;
	}
      }

      // check the type of work:
      if (_state == State::hired_for_find_old_symmetry_class) {
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "worker " << _workerID << " checking for old symmetry class ..." << std::endl;
	}

	// do the corresponding work:
	find_old_symmetry_class();
	_state = State::done;
      }
      else if (_state == State::hired_for_build_orbit_with_searchpred) {
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "worker " << _workerID << " building orbit ..." << std::endl;
	}

	// do the corresponding work:
	build_orbit_with_searchpred();
	_state = State::done;
      }

      // report that work is done:
      if (_state == State::done) {
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "worker " << _workerID << " done ..." << std::endl;
	}
	std::lock_guard<std::mutex> main_lock(_callerptr->_main_mutex);
	--_callerptr->_no_of_busy_threads;
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "notifying main thread ..." << std::endl;
	}
	if (_callerptr->_threads_done()) {
	  _callerptr->_main_condition.notify_one();
	}
	_state = State::idle;
      }
    }
  }

  // end of SymmetryWorker methods.
  
  // auxiliary functions only for this module:
  TriangNode SymmetricFlipGraph::_map(const std::pair<Symmetry, Symmetry>& sympair,
				      const TriangNode& tnode) const {
    if (sympair.second.n() == 0) {
      // we have not built the simplex-index representation of the symmetry group
      // (usually because of memory consumption), thus, we have to use the
      // TOPCOM's slower legacy method:
      return sympair.first.map(tnode);
    }
    else {
      return _map_via_indexset(sympair.second, tnode);
    }
  }

  void SymmetricFlipGraph::_map_into(std::pair<Symmetry, Symmetry>& sympair,
				     const TriangNode& tnode,
				     TriangNode& result) const {
    if (sympair.second.n() == 0) {
      result.clear();
      sympair.first.map_into(tnode, result);
    }
    else {
      _map_into_via_indexset(sympair.second, tnode, result);
    }
  }

  TriangNode SymmetricFlipGraph::_map_via_indexset(const Symmetry& g_simpidx,
						   const TriangNode& tnode) const {
    TriangNode result(tnode.ID(), tnode.no(), tnode.rank(), SimplicialComplex());
    result.replace_indexset(std::move(g_simpidx.map(tnode.index_set_pure())));
    return result;
  }

  void SymmetricFlipGraph::_map_into_via_indexset(const Symmetry& g_simpidx,
						  const TriangNode& tnode,
						  TriangNode& result) const {
    const SimplicialComplex::IndexSet& tnode_indexset = tnode.index_set_pure();
    result.replace_indexset(std::move(g_simpidx.map(tnode_indexset)));
  }

  symmetryptr_datapair SymmetricFlipGraph::_stabilizer_ptrs(const TriangNode& tnode,
							    const Vector&     gkz) const {
    if (_simpidx_symmetries.empty()) {
      return symmetryptr_datapair();
    }
    else {
      if (_simpidx_symmetries.begin()->second.n() > 0) {
	return _stabilizer_ptrs_via_indexset(tnode, gkz);
      }
      else {
	if (_voltableptr && CommandlineOptions::use_gkz()) {

	  // in this case, we have a fingerprint:
	  return _symmetries.stabilizer_ptrs(tnode, gkz);
	}
	else {
	  return _symmetries.stabilizer_ptrs(tnode);
	}
      }
    }
  }

  symmetryptr_datapair SymmetricFlipGraph::_stabilizer_ptrs_via_indexset(const TriangNode& tnode,
									 const Vector&     gkz) const {
    symmetryptr_datapair result;
    for (simpidx_symmetries_type::const_iterator iter = _simpidx_symmetries.begin();
	 iter != _simpidx_symmetries.end();
	 ++iter) {
      if (_voltableptr && CommandlineOptions::use_gkz()) {
	if (!iter->first.fixes(gkz)) {
	  continue;
	}
      }
      if (iter->second.fixes(tnode.index_set_pure())) {
	result.first.push_back(&iter->first);
	result.second.insert(&iter->first);
      }
    }
    return result;
  }
  
  Vector SymmetricFlipGraph::_gkz(const TriangNode& tnode) const {
    Vector result(_no);
    if (!_voltableptr) {
      std::cerr << "SymmetricFlipGraph::_gkz(const TriangNode&: "
		<< "no volumes available for the computation of GKZ vectors - exiting" << std::endl;
      exit(1);
    }
    for (SimplicialComplex::const_iterator sciter = tnode.begin();
	 sciter != tnode.end();
	 ++sciter) {
      Field simpvol = _voltableptr->find(*sciter)->second;
      for (Simplex::const_iterator simpiter = sciter->begin();
	   simpiter != sciter->end();
	   ++simpiter) {
	result[*simpiter] += simpvol;
      }
    }
    return result;
  }

  bool SymmetricFlipGraph::_threads_done() const {
    return (_no_of_busy_threads == 0);
  }
  
  int SymmetricFlipGraph::_old_symmetry_class(const TriangNode& tnode,
					      const Vector&     gkz) {

    static const bool local_debug = false;
    
    // return values are:
    // 0: tnode represents a new symmetry class
    // -1: tnode is contained in previous triangulations
    // -2: tnode is contained in new triangulations
    // +1: an element of the orbit of tnode is in previous triangulations
    // +2: an element of the orbit of tnode is in new triangulations
    // updated data:
    // _representative: the element in the orbit of tnode that is in previous/new triangulations
    // _find_iter: an iterator in previous/new triangulations pointing to _representative
    // _transformation: a symmetry mapping tnode to _representative
    // _orbitsize: the size of the orbit of tnode
    // _rep_has_search_pred: whether or not tnode satisfies the search predicate
  
    // the case of non-simple BFS:
  
    // handle the identity individually:
    if ((_find_iter = _previous_triangs.find(tnode)) != _previous_triangs.end()) {
      _location_of_old_symmetry_class = 0;
      _orbitsize = 0;
      return -1;
    }
    if ((_find_iter = _new_triangs.find(tnode)) != _new_triangs.end()) {
      _location_of_old_symmetry_class = 0;
      _orbitsize = 0;
      return -2;
    }

    // if (false) {
    if (CommandlineOptions::parallel_symmetries()) {
      // multi-threaded version of non-simple BFS:
      // _threads.clear();
      SimplicialComplex::start_multithreading();

      // replaced the old generation of new threads by waiting threads:
      if (CommandlineOptions::debug() || local_debug) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "starting parallel symmetry check with "
		  << _no_of_waiting_threads << " waiting threads and "
		  << _no_of_busy_threads << " done threads ..." << std::endl;
      }
      {
	std::lock_guard<std::mutex> main_lock(_main_mutex);
	_location_of_old_symmetry_class = 0;
	_no_of_busy_threads = 0;
      }
      for (int i = 0; i < _no_of_threads; ++i) {
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "main hiring and notifying worker " << i << " for symmetry check ..." << std::endl;
	}
	_symmetry_workers[i].pass_work_for_find_old_symmetry_class(tnode, gkz);
	_symmetry_workers[i].worker_condition.notify_one();
	std::lock_guard<std::mutex> main_lock(_main_mutex);
	++_no_of_busy_threads;
      }
      
      while (!_threads_done()) {
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "main waiting for symmetry checks ..." << std::endl;
	}

	// wait for threads to finish work:
	std::unique_lock<std::mutex> main_lock(_main_mutex);
	_main_condition.wait(main_lock, [this] { return _threads_done(); });
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... main waking up" << std::endl;
	}
      }

      if (CommandlineOptions::debug() || local_debug) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... completed parallel symmetry check" << std::endl;
      }
      
      SimplicialComplex::stop_multithreading();
      if (CommandlineOptions::debug()  || local_debug) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done" << std::endl;
      }
      if (_location_of_old_symmetry_class != 0) {
	_orbit.clear();
	_orbitsize = 0;
	return _location_of_old_symmetry_class;
      }
    }
    else {
      // single-threaded version of non-simple BFS:
    
      // first check whether we already found an equivalent triangluation:
      for (simpidx_symmetries_type::iterator iter = _simpidx_symmetries.begin();
	   iter != _simpidx_symmetries.end();
	   ++iter) {
	std::pair<Symmetry, Symmetry>& g_pair(*iter);

	// bool gkz_result;
	if (CommandlineOptions::use_gkz() && _voltableptr) {
	
	  // first check is by the GKZ vector:
	  g_pair.first.map_into(gkz, _equivalent_gkz);
	  if (_known_gkzs.find(_equivalent_gkz) == _known_gkzs.end()) {
	    
	    // GKZ is new, thus, tnode is new:	  
	    continue;
	    // gkz_result = false;
	  }
	  else {
	    // gkz_result = true;
	  }
	}

	// if not, further investigations are necessary in case also non-regular
	// triangulations are traversed:
	
	// map tnode by mapping its index set by means of the alternative representation
	// of the symmetry; this way, not all simplices in tnode have to be mapped
	// individually:
	_map_into(g_pair, tnode, _representative);

	// the old direct method is deprecated:
	// _representative = g_elem.map(tnode);
	if ((_find_iter = _previous_triangs.find(_representative)) != _previous_triangs.end()) {
	  _transformation = g_pair.first;
	  _orbit.clear();
	  _orbitsize = 0;
	  // if (!gkz_result) {
	  //   std::cerr << g_pair.first << " maps GKZ " << gkz
	  // 	      << " of triangulation " << tnode
	  // 	      << " to unknown " << gkz_image
	  // 	      << " in " << _known_gkzs << std::endl;
	  //   std::cerr << _transformation << " maps "
	  // 	      << " triangulation " << tnode
	  // 	      << " to known triangulation " << _representative
	  // 	      << " with GKZ " << _gkz(_representative) 
	  // 	      << " in " << _previous_triangs << std::endl;
	  // }
	  return 1;
	}
	if ((_find_iter = _new_triangs.find(_representative)) != _new_triangs.end()) {
	  _transformation = g_pair.first;
	  _orbit.clear();
	  _orbitsize = 0;
	  // if (!gkz_result) {
	  //   std::cerr << g_pair.first << " maps GKZ " << gkz
	  // 	      << " of triangulation " << tnode
	  // 	      << " to unknown " << gkz_image 
	  // 	      << " in " << _new_gkzs << std::endl;
	  //   std::cerr << _transformation << " maps "
	  // 	      << " triangulation " << tnode
	  // 	      << " to known triangulation " << _representative
	  // 	      << " with GKZ " << _gkz(_representative) 
	  // 	      << " in " << _new_triangs << std::endl;
	  // }
	  return 2;
	}
      }
    }

    bool have_node_symmetryptrs = false;
    
    // now check the search predicate, e.g., regularity, for the 
    // discovered representative tnode of the new symmetry class:
    if ((*_search_pred_ptr)(*_pointsptr, *_chiroptr, *_inctableptr, tnode)) {
    
      // tnode has search predicate:
      _rep_has_search_pred = true;
      _orbit.insert(tnode);
      if ((*_output_pred_ptr)(*_pointsptr, *_chiroptr, *_inctableptr, tnode)) {
	(*_cout_triang_ptr)(_symcount, tnode);
      }

      // compute stabilizer, if possible using the index set representation:
      _node_symmetryptrs = _stabilizer_ptrs(tnode, gkz);
      have_node_symmetryptrs = true;
    
      if (CommandlineOptions::skip_orbitcount()) {
	_orbitsize = _orbit.size();
	_orbit.clear();
	return 0;
      }
      if ((*_search_pred_ptr).is_invariant() && (*_output_pred_ptr).is_invariant()) {

	// stabilizer formula:
	_orbitsize = (_symmetries.size() + 1) / (_node_symmetryptrs.first.size() + 1);
	_orbit.clear();
	return 0;
      }
    }
    else {
      _rep_has_search_pred = false;
      if ((*_search_pred_ptr).is_invariant()) {
 
	// tnode's complete orbit does not have search predicate
	_orbitsize = 0;
	return 0; // what we return does not matter anymore because _orbitsize is zero
      }
    }

    // compute stabilizer, if possible via index sets, if not yet done:
    if (!have_node_symmetryptrs) {
      _node_symmetryptrs = _stabilizer_ptrs(tnode, gkz);
    }
  
    if (CommandlineOptions::parallel_symmetries()) {

      // multi-threaded version of non-simple orbit-building:
      SimplicialComplex::start_multithreading();

      // replaced the old generation of new threads by waiting threads:
      if (CommandlineOptions::debug() || local_debug) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "starting parallel orbit building with "
		  << _no_of_waiting_threads << " waiting threads and "
		  << _no_of_busy_threads << " done threads ..." << std::endl;
      }
      {
	std::lock_guard<std::mutex> main_lock(_main_mutex);
	_no_of_busy_threads = 0;
	for (int i = 0; i < _no_of_threads; ++i) {
	  _worker_orbits[i].clear();
	}
      }
      for (int i = 0; i < _no_of_threads; ++i) {
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "main hiring and notifying worker " << i << " for orbit building ..." << std::endl;
	}
	_symmetry_workers[i].pass_work_for_build_orbit_with_searchpred(tnode);
	_symmetry_workers[i].worker_condition.notify_one();
	std::lock_guard<std::mutex> main_lock(_main_mutex);
	++_no_of_busy_threads;
      }
      while (!_threads_done()) {
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "main waiting for orbit building ..." << std::endl;
	}
	
	// wait for threads to finish work:
	std::unique_lock<std::mutex> main_lock(_main_mutex);
	_main_condition.wait(main_lock, [this] { return _threads_done(); });
	if (CommandlineOptions::debug() || local_debug) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... main waking up" << std::endl;
	}
      }
      
      if (CommandlineOptions::debug() || local_debug) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... completed parallel orbit building" << std::endl;
      }

      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done" << std::endl;
      }
      SimplicialComplex::stop_multithreading();

      // collect the results:
      for (int i = 0; i < _no_of_threads; ++i) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... main collecting results from worker " << i << " ..." << std::endl;
	}
	for (orbit_type::iterator iter = _worker_orbits[i].begin();
	     iter != _worker_orbits[i].end();
	     ++iter) {
	  _orbit.insert(std::move(*iter));
	}
      }
      _orbitsize = _orbit.size();
      _orbit.clear();
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "workers final result on _orbitsize = " << _orbitsize << std::endl;
      }
    }
    else {
      // single-threaded version of non-simple orbit-building:
    
      // next, we need to check the orbit of tnode for the search predicate (e.g., regularity):
      for (simpidx_symmetries_type::const_iterator iter = _simpidx_symmetries.begin();
	   iter != _simpidx_symmetries.end();
	   ++iter) {
	const std::pair<Symmetry, Symmetry>& g_pair(*iter);
	// const Symmetry& g_elem(iter->first);
	// const Symmetry& g_simpidx(iter->second);
	if (_node_symmetryptrs.second.find(&g_pair.first) != _node_symmetryptrs.second.end()) {
	  // g is in stabilizer of tnode - nothing new:
	  continue;
	}

	// map via index set:
	_representative = std::move(_map(g_pair, tnode));
      
	// deprecated:
	// _representative = g_elem.map(tnode);
      
	// check whether we already have that image of tnode in the orbit:
	if (_orbit.find(_representative) == _orbit.end()) {
	
	  // if not, check the search predicate (e.g., regularity):
	  if ((*_search_pred_ptr)(*_pointsptr, *_chiroptr, *_inctableptr, _representative)
	      && (*_output_pred_ptr)(*_pointsptr, *_chiroptr, *_inctableptr, _representative)) {
	    _orbit.insert(_representative);
	    if (this->_print_triangs) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cout << "Torbit[" << tnode.ID() << "," << this->_orbit.size() - 1 << "] := " << (SimplicialComplex)_representative << ";" << std::endl;
	    }
	  }
	}
      }
      _orbitsize = _orbit.size();
      _orbit.clear();
    }
    return 0;
  }

  void SymmetricFlipGraph::_mark_equivalent_flips(const TriangNode&                     tnode, 
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
    for (symmetryptr_iterdata::const_iterator iter = _node_symmetryptrs.first.begin();
	 iter != _node_symmetryptrs.first.end();
	 ++iter) {
      const Symmetry& g = **iter;
#ifndef STL_CONTAINERS
      find_iter->dataptr()->mark_flip(g.map(fliprep));
#else
      find_iter->second.mark_flip(g.map(fliprep));
#endif
    }
#ifdef CHECK_MARK
    std::cerr << "flip after marking:" << std::endl;
    std::cerr << find_iter->key() << "->" << find_iter->data() << std::endl;
#endif
  }

  void SymmetricFlipGraph::_mark_equivalent_flips(const TriangNode& tnode, 
						  TriangFlips&      tflips,
						  const FlipRep&    fliprep) {
#ifdef CHECK_MARK
    std::cerr << "flips before marking:" << std::endl;
    std::cerr << tflips << std::endl;
#endif
    // mark this flip:
    tflips.mark_flip(fliprep);
    // mark all equivalent flips:
    for (symmetryptr_iterdata::const_iterator iter = _node_symmetryptrs.first.begin();
	 iter != _node_symmetryptrs.first.end();
	 ++iter) {
      const Symmetry& g(**iter);
      tflips.mark_flip(g.map(fliprep));
    }
#ifdef CHECK_MARK
    std::cerr << "flips after marking:" << std::endl;
    std::cerr << tflips << std::endl;
#endif
  }

  void SymmetricFlipGraph::_process_newtriang(const TriangNode&     current_triang,
					      const TriangFlips&    current_flips,
					      const TriangNode&     next_triang,
					      const FlipRep&        current_fliprep) {
    if (CommandlineOptions::use_gkz() && _voltableptr) {
      _new_gkz = std::move(_gkz(next_triang));
    }
    const int where(_old_symmetry_class(next_triang, _new_gkz));
    if (where < 0) {
    
      // next_triang was found before: mark all equivalent flips in next_triang:
      const FlipRep& inversefliprep(current_fliprep.inverse());

      _node_symmetryptrs = _stabilizers.find(next_triang)->second;
    
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
      const FlipRep& inversefliprep(_transformation.map(current_fliprep.inverse()));

      _node_symmetryptrs = _stabilizers.find(_representative)->second;
      _mark_equivalent_flips(_representative, _find_iter, inversefliprep);
      return;
    }
    if (_orbitsize > 0) {
    
      // we found at least one new triangulation in orbit satisfying 
      // the search predicate; thus we must save the representative
      TriangFlips next_flips = TriangFlips(*_chiroptr,
					   current_triang, 
					   current_flips, 
					   next_triang, 
					   Flip(current_triang, current_fliprep),
					   _node_symmetryptrs,
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
      if ((*_output_pred_ptr)(*_pointsptr, *_chiroptr, *_inctableptr, next_triang)) {
	++_symcount;
	--_reportcount;
	_totalcount += _orbitsize;
	// (*_cout_triang_ptr)(_symcount, next_triang);
	if (CommandlineOptions::verbose() && (_reportcount == 0)) {
	  _reportcount = CommandlineOptions::report_frequency();
	  std::cerr << _symcount << " symmetry classes";
	  if (!CommandlineOptions::skip_orbitcount()) {
	    std::cerr << " | " << _totalcount << " total triangulations";
	  }
	  std::cerr << " --- " << _previous_triangs.size() + _new_triangs.size() 
		    << " currently stored." << std::endl;
	}
      }
#ifdef CHECK_NEW
      std::cerr << "Checking whether " << next_triang << " is new:" << std::endl;
      std::cerr << "\t _all_triangs: " << std::endl;
      std::cerr << "\t " << _all_triangs << std::endl;
      std::cerr << "\t current new triangs:" << std::endl;
      std::cerr << "\t " << _new_triangs << std::endl;
      std::cerr << "\t current previous triangs:" << std::endl;
      std::cerr << "\t " << _previous_triangs << std::endl;
      if (_all_triangs.find(next_triang) != _all_triangs.end()) {
	std::cerr << "\t Error in SymmetricFlipGraph: old triangulation " << std::endl;
	std::cerr << "\t " << next_triang << std::endl;
	std::cerr << "\t is not new - exiting" << std::endl;
	exit(1);
      }
      std::cerr << next_triang << " is new." << std::endl;
#endif
      _new_triangs.insert(std::pair<TriangNode, TriangFlips>(next_triang, next_flips));
      _stabilizers.insert(std::pair<TriangNode, symmetryptr_datapair>(next_triang, _node_symmetryptrs));
      if (CommandlineOptions::use_gkz() && _voltableptr) {
	// _new_gkzs.insert(_new_gkz);
	_known_gkzs.insert(_new_gkz);
      }
    }
  }

  void SymmetricFlipGraph::_process_flips(const TriangNode&     current_triang,
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
      if ((*iter).second) {
	continue;
      }
#ifdef SUPER_VERBOSE
#ifndef STL_FLIPS
      std::cerr << "flipping flip " << iter->key() 
	      << " = " << Flip(current_triang, iter->key()) << std::endl;      
#else
      std::cerr << "flipping flip " << iter->first 
	      << " = " << Flip(current_triang, iter->first) << std::endl;
#endif      
#endif
#ifndef STL_FLIPS
      const FlipRep current_fliprep(iter->key());
#else
      const FlipRep current_fliprep(iter->first);
#endif
      const Flip flip(current_triang, current_fliprep);
      const TriangNode next_triang(_symcount, current_triang, flip);
      _process_newtriang(current_triang, current_flips, next_triang, current_fliprep);
    }
  }

  void SymmetricFlipGraph::_bfs_step() {
    // go through all triangulations found so far:
    while (!_previous_triangs.empty()) {
      tnode_container_type::const_iterator iter(_previous_triangs.begin());
#ifndef STL_CONTAINERS
      const TriangNode current_triang(iter->key());
#else
      const TriangNode current_triang(iter->first);
#endif
#ifndef STL_CONTAINERS
      const TriangFlips current_flips(iter->data());
#else
      const TriangFlips current_flips(iter->second);
#endif
      _process_flips(current_triang, current_flips);
      _previous_triangs.erase(current_triang);
      _stabilizers.erase(current_triang);
      if (CommandlineOptions::use_gkz() && _voltableptr && CommandlineOptions::check_regular()) {

	// we can only erase old GKZ vectors in the regular case;
	// for non-regular triangulations, we need to keep all the GKZ vectors:
	_known_gkzs.erase(_gkz(current_triang));
      }
    }
#ifdef CHECK_NEW
    for (tnode_container_type::const_iterator iter = _new_triangs.begin();
	 iter != _new_triangs.end();
	 ++iter) {
#ifndef STL_CONTAINERS
      _all_triangs[iter->key()] = iter->data();
#else
      _all_triangs[iter->first] = iter->second;
#endif
    }
#endif
    if (CommandlineOptions::verbose()) {
      std::cerr << _new_triangs.size() << " new symmetry classes." << std::endl;
#ifdef WATCH_MAXCHAINLEN
      std::cerr << "length of maximal chain in hash table _new_triangs: " 
		<< _new_triangs.maxchainlen() << std::endl;
#endif
    }
  }

  void SymmetricFlipGraph::_bfs() {    
    while (!_previous_triangs.empty()) {
      _bfs_step();
#ifndef STL_CONTAINERS
      tnode_container_type tmp;
      _previous_triangs = _new_triangs;
      _new_triangs = tmp;
      if (CommandlineOptions::use_gkz() && _voltableptr) {
	// gkz_container_type tmp;
	// _known_gkzs = _new_gkzs;
	// _new_gkzs = tmp;
      }
#else
      _previous_triangs.swap(_new_triangs);
      // if (CommandlineOptions::use_gkz() && _voltableptr) {
      // 	_known_gkzs.swap(_new_gkzs);
      // }
#endif
 
      // dump status if requested:
      if (CommandlineOptions::dump_status()) {
	if  (this->_processed_count % CommandlineOptions::dump_frequency() == 0) {
	  std::ostringstream filename_str;
	  filename_str << CommandlineOptions::dump_file() << "." << _dump_no % CommandlineOptions::dump_rotations();
	  _dump_str.open(filename_str.str().c_str(), std::ios::out | std::ios::trunc);
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "dumping intermediate results into dump file "
		      << filename_str.str().c_str() << " ..." << std::endl;
	  }
	  write(_dump_str);
	  _dump_str.close();
	  ++_dump_no;
	  _processed_count = 0;
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "... done" << std::endl;
	  }
	}
	++_processed_count;
      }
    }
  }
  
  void SymmetricFlipGraph::_init_symmetry_workers() {
    int thread_no(0);
    for (simpidx_symmetries_type::const_iterator iter = _simpidx_symmetries.begin();
	 iter != _simpidx_symmetries.end();
	 ++iter) {
      // distribute all simpidx symmetries in a round-robin fashion:
      // generate a copy of each symmetry by assigning to a non-constant
      // temporary object for thread-safety:
      const std::pair<Symmetry, Symmetry> sym(*iter);
      _worker_symmetries[thread_no].push_back(sym);
      thread_no = (thread_no + 1) % _no_of_threads;
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      if (CommandlineOptions::debug()) {
	std::cerr << "initializing worker " << i << " with symmetries "
		  << _worker_symmetries[i] << " ..." << std::endl;
      }
      _symmetry_workers.emplace_back(i, *this, _worker_symmetries[i]);
      if (CommandlineOptions::debug()) {    
	std::cerr << "... done" << std::endl;
      }
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      if (CommandlineOptions::verbose()) {
	std::lock_guard<std::mutex> lock(_main_mutex);
	std::cerr << "starting thread " << i << " ..." << std::endl;
      }
      _threads.push_back(std::thread(&SymmetricFlipGraph::SymmetryWorker::operator(),
				     &_symmetry_workers[i]));

      // for the time being, allow the worker to wait:
      if (CommandlineOptions::verbose()) {    
	std::lock_guard<std::mutex> lock(_main_mutex);
	std::cerr << "... done" << std::endl;
      }
    }
  }
  
  void SymmetricFlipGraph::_init() {
    if (CommandlineOptions::check_regular()
	|| CommandlineOptions::check_nonregular()
	|| CommandlineOptions::check_sometimes()) {

      // compute boundary facets of seed:
      if (CommandlineOptions::verbose()) {
	std::cerr << "computing boundary triangulation from seed ..." << std::endl;
      }
      SimplicialComplex seed_boundary_triang;
      for (SimplicialComplex::const_iterator seediter = _seed.begin();
	   seediter != _seed.end();
	   ++seediter) {
	Simplex simp = *seediter;
	for (Simplex::const_iterator simpiter = simp.begin();
	     simpiter != simp.end();
	     ++simpiter) {
	  size_type simpelm = *simpiter;
	  simp -= simpelm; 
	  seed_boundary_triang ^= simp;
	  simp += simpelm; 
	}
      }
      if (CommandlineOptions::verbose()) {
	std::cerr << "... done." << std::endl;
      }

      // use that to determine facets by beneath-beyond:
      if (CommandlineOptions::verbose()) {
	std::cerr << "computing facets from boundary triangulation ..." << std::endl;
      }
      Facets* facetsptr = new Facets(*_chiroptr, seed_boundary_triang);
      if (CommandlineOptions::verbose()) {
	std::cerr << "... done." << std::endl;
      }
      if (CommandlineOptions::verbose()) {
	std::cerr << "computing incidences ..." << std::endl;
      }
      _inctableptr = new Incidences(*_chiroptr, *facetsptr);
      if (CommandlineOptions::verbose()) {
	std::cerr << "... done." << std::endl;
      }
      delete facetsptr;
    }

    // for non-parallel symmetry processing we generate the simpidx symmetries here:
    if (CommandlineOptions::verbose()) {
      std::cerr << "preparing simpidx symmetry pairs";
      if (CommandlineOptions::simpidx_symmetries() && !CommandlineOptions::parallel_symmetries()) {
	std::cerr << " with preprocessed simpidx symmetries ..." << std::endl;
      }
      else {
	std::cerr << " with empty simpidx symmetries ..." << std::endl;
      }
    }
    size_type cnt = 0;
    _simpidx_symmetries.reserve(_symmetries.size());
    for (SymmetryGroup::const_iterator iter = _symmetries.begin();
	 iter != _symmetries.end();
	 ++iter) {
      if (CommandlineOptions::simpidx_symmetries() && !CommandlineOptions::parallel_symmetries()) {
	_simpidx_symmetries.emplace_back(*iter, iter->simpidx_symmetry(_rank));
      }
      else {
	_simpidx_symmetries.emplace_back(*iter, Symmetry(0));
      }
      if (CommandlineOptions::verbose() && (++cnt % CommandlineOptions::report_frequency() == 0)) {
	std::cerr << cnt << " symmetries processed so far." << std::endl;
      }
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done" << std::endl;
    }

    if (CommandlineOptions::parallel_symmetries()) {
      _init_symmetry_workers();
    }
    
    LabelSet seed_support(_seed.support());
    if (_print_triangs) {
      _cout_triang_ptr = new cout_triang_type();
    }
    else {
      _cout_triang_ptr = new cout_triang_base_type();
    }
    if (CommandlineOptions::max_no_of_simplices() > 0) {
      _output_pred_ptr = new output_checkmaxno_type();
    }
    else if (CommandlineOptions::no_of_simplices() > 0) {
      _output_pred_ptr = new output_checkno_type();
    }
    else if (CommandlineOptions::check_nonregular()) {
      _output_pred_ptr = new output_checknonregularity_type();
    }
    else if (CommandlineOptions::check_unimodular()) {
      _output_pred_ptr = new output_checkunimodularity_type(*_pointsptr, *_chiroptr);
    }
    else {
      _output_pred_ptr = new output_pred_base_type();
    }
    if (CommandlineOptions::check_regular() && CommandlineOptions::dont_add_points()) {
      _search_pred_ptr = new search_seedsupportandregular_type(seed_support);
    }
    else if (CommandlineOptions::check_regular()) {
      _search_pred_ptr = new search_checkreg_type();
    }
    else if (CommandlineOptions::dont_add_points()) {
      _search_pred_ptr = new search_seedsupportonly_type(seed_support);
    }
    else if (CommandlineOptions::check_sometimes()) {
      _search_pred_ptr = new search_checksometimes_type();
    }
    else if (CommandlineOptions::reduce_points()) {
      _search_pred_ptr = new search_min_no_of_verts_type(seed_support.card());
    }
    else {
      _search_pred_ptr = new search_pred_base_type();
    }

    if (CommandlineOptions::read_status()) {
      std::ifstream read_str(CommandlineOptions::read_file());
      read(read_str);

      std::cerr << "SymmetricFlipGraph initialized from file " << CommandlineOptions::read_file() << std::endl;
      if (CommandlineOptions::debug()) {
	std::cerr << "data:" 
		  << std::endl
		  << *this
		  << std::endl;
      }
    }
    else {
      const TriangNode current_triang = TriangNode(_symcount, _no, _rank, _seed);
      if (CommandlineOptions::use_gkz() && _voltableptr) {
	_current_gkz = std::move(_gkz(current_triang));
      }
      _old_symmetry_class(current_triang, _current_gkz);
      if (_orbitsize > 0) {
	if ((*_output_pred_ptr)(*_pointsptr, *_chiroptr, *_inctableptr, current_triang)) {
	  _totalcount += _orbitsize;
	  ++_symcount;
	  --_reportcount;
	  // (*_cout_triang_ptr)(_symcount, current_triang);
	}
	const TriangFlips current_flips = TriangFlips(*_chiroptr,
						      current_triang, 
						      _node_symmetryptrs,
						      _only_fine_triangs);
	_previous_triangs.insert(std::pair<TriangNode, TriangFlips>(current_triang, current_flips));
#ifdef CHECK_NEW
	_all_triangs.insert(std::pair<TriangNode, TriangFlips>(current_triang, current_flips));	
#endif
	_stabilizers.insert(std::pair<TriangNode, symmetryptr_datapair>(current_triang, _node_symmetryptrs));
	if (CommandlineOptions::use_gkz() && _voltableptr) {
	  _known_gkzs.insert(_current_gkz);
	}
      }
    }
    _bfs();
  }


  // stream input:

  std::istream& SymmetricFlipGraph::read(std::istream& ist) {
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
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): error while reading _no; exiting" << std::endl;
	  exit(1);
	}
	if (_no != no_check) {
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): no of points in input differs from no of points in dump file; exiting" << std::endl;
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
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): error while reading _rank; exiting" << std::endl;
	  exit(1);
	}
	if (_rank != rank_check) {
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): rank of input differs from rank in dump file; exiting" << std::endl;
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
	    std::cerr << "SymmetricFlipGraph::read(std::istream& ist): error while reading _points; exiting" << std::endl;
	    exit(1);
	  }
	  if (*_pointsptr!= points_check) {
	    std::cerr << "SymmetricFlipGraph::read(std::istream& ist): points of input differ from points in dump file; exiting" << std::endl;
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
	  Chirotope chiro_check(*_pointsptr, false);
	
	  if (!(istrst >> chiro_check)) {
	    std::cerr << "SymmetricFlipGraph::read(std::istream& ist): error while reading _chiro; exiting" << std::endl;
	    exit(1);
	  }
	  if ((*_chiroptr) != chiro_check) {
	    std::cerr << "SymmetricFlipGraph::read(std::istream& ist): chirotope of input differs from chirotope in dump file; exiting" << std::endl;
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
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): error while reading _symmetries; exiting" << std::endl;
	  exit(1);
	}
	if (_symmetries != symmetries_check) {
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): symmetries of input differ from symmetries in dump file; exiting" << std::endl;
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
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): error while reading _previous_triangs; exiting" << std::endl;
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
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): error while reading _new_triangs; exiting" << std::endl;
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
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): error while reading _totalcount; exiting" << std::endl;
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
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): error while reading _symcount; exiting" << std::endl;
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
	  std::cerr << "SymmetricFlipGraph::read(std::istream& ist): error while reading _reportcount; exiting" << std::endl;
	  exit(1);
	}
	if (CommandlineOptions::debug()) {
	  std::cerr << "_reportcount initialized with " << _reportcount << std::endl;
	}
      }
    }
    std::cerr << "... done" << std::endl;
    return ist;
  }

}; // namespace topcom

// eof SymmetricFlipGraph.cc
