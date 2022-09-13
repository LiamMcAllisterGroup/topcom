////////////////////////////////////////////////////////////////////////////////
// 
// SymmetricExtensionGraph.cc
//
//    produced: 12/01/2020 jr
// last change: 12/01/2020 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <iomanip>
#include <thread>
#include <functional>
#include <atomic>

#include "SymmetricExtensionGraph.hh"
#include "RegularityCheck.hh"

namespace topcom {
  std::mutex SymmetricExtensionGraph::_min_mutex;

  SymmetricExtensionGraph::SymmetryWorker::SymmetryWorker(const int                         workerID,
							  const SymmetricExtensionGraph&    seg) :
    _workerID(workerID),
    _callerptr(&seg),
    _partial_triang_to_check(),
    _partial_triang_new_simplex(),
    _work_received(false),
    _work_done(true),
    _worker_stopped(false) {
  }

  SymmetricExtensionGraph::SymmetryWorker::SymmetryWorker(const SymmetryWorker& sw) :
    _workerID(sw._workerID),
    _callerptr(sw._callerptr),
    _partial_triang_to_check(sw._partial_triang_to_check),
    _partial_triang_new_simplex(sw._partial_triang_new_simplex),
    _work_received(false),
    _work_done(true),
    _worker_stopped(false) {
  }

  void SymmetricExtensionGraph::SymmetryWorker::operator()() {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "worker " << _workerID << " is spawned" << std::endl;
    }
    while (!_worker_stopped) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "worker " << _workerID << " waiting ..." << std::endl;
      }
      while (!wake_up()) {
	std::unique_lock<std::mutex> main_lock(_callerptr->_main_mutex);
	_worker_condition.wait(main_lock, [this] { return wake_up(); });
      }

      // wake-up because of new work?
      if (_work_received && !_worker_stopped) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "worker " << _workerID << " working ..." << std::endl;
	}
	// the actual work is done here:
	check_lex_decreasing_symmetry();
	// actual work done.
    
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... worker " << _workerID << " done - notifying master" << std::endl;
	}
	{
	  _work_done = true;
	  _work_received = false;
	  std::lock_guard<std::mutex> main_lock(_callerptr->_main_mutex);
	  --_callerptr->_no_of_busy_threads;
	  if (_callerptr->_threads_done()) {
	    _callerptr->_main_condition.notify_one();
	  }
	}
      }
    }
  }

  void SymmetricExtensionGraph::SymmetryWorker::check_lex_decreasing_symmetry() {

    // some other thread may have found that current_partial_triang is not new:
    if (_callerptr->_is_old_symmetry_class) {
      return;
    }
    if (_callerptr->_classified_symmetriesptr->worker_symmetryptrs(_workerID).empty()) {
      return;
    }
    const Simplex& min_simp(*_partial_triang_to_check.begin());
    const seg_symmetryptr_iterdata& relevant_symmetryptrs(_callerptr->_classified_symmetriesptr->worker_relevant_symmetryptrs(_workerID, _partial_triang_new_simplex));
    for (seg_symmetryptr_iterdata::const_iterator iter = relevant_symmetryptrs.begin();
	 iter != relevant_symmetryptrs.end();
	 ++iter) {
      // some other thread may have found that current_partial_triang is not new:
      if (_callerptr->_is_old_symmetry_class) {
	return;
      }
      const Symmetry& g(**iter);
      
      // if (g.map(_partial_triang_new_simplex).lexsmaller(min_simp)) {
      // 	// now this thread has found that current_partial_triang is not new:
      // 	_callerptr->_is_old_symmetry_class = true;
      // 	return;
      // }

      const size_type min_simp_idx = SimplicialComplex::index_of_simplex(min_simp, _callerptr->_rank);
      const size_type g_new_simp_idx = SimplicialComplex::index_of_simplex(g.map(_partial_triang_new_simplex), _callerptr->_rank);
      if (g_new_simp_idx < min_simp_idx) {
	_callerptr->_is_old_symmetry_class = true;
	return;
      }
      
      if (g.lex_decreases(_partial_triang_to_check, _callerptr->_rank)) {
	// now this thread has found that current_partial_triang is not new:
	_callerptr->_is_old_symmetry_class = true;
	return;
      }
    }
  }

  bool SymmetricExtensionGraph::_old_symmetry_class(const PartialTriang& current_partial_triang,
						    const Simplex&       new_simp) {
    if (CommandlineOptions::parallel_symmetries()) {
      {
	std::lock_guard<std::mutex> main_lock(_main_mutex);
	_is_old_symmetry_class = false;
	_no_of_busy_threads = 0;
      }
      for (int i = 0; i < _no_of_threads; ++i) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "activating thread " << i << " ..." << std::endl;
	}

	_symmetry_workers[i].pass_work(current_partial_triang, new_simp);
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... done" << std::endl;
	}
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "master is notifying all workers ..." << std::endl;
      }
      SimplicialComplex::start_multithreading();
      for (int i = 0; i < _no_of_threads; ++i) {
	_symmetry_workers[i]._worker_condition.notify_one();
	std::lock_guard<std::mutex> main_lock(_main_mutex);
	++_no_of_busy_threads;
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "master is locking main mutex ..." << std::endl;
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done" << std::endl;
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "master is waiting for threads to finish ..." << std::endl;
      }
      while (!_threads_done()) {
	std::unique_lock<std::mutex> main_lock(_main_mutex);
	_main_condition.wait(main_lock, [this] { return _threads_done(); } );
      }

      SimplicialComplex::stop_multithreading();
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "workers final result = " << _is_old_symmetry_class << std::endl;
      }
      return _is_old_symmetry_class;
    }
    else {
      if (CommandlineOptions::use_switch_tables()) {
      	return _switch_tableptr->lex_decreases(current_partial_triang.index_set_pure());
      }
      else if (CommandlineOptions::use_classified_symmetries()) {
	return _classified_symmetriesptr->lex_decreases_by_relevant_symmetries(current_partial_triang, new_simp);
      }
      else {

	// in this case, we use the naive traversal through all symmetries:
	if (_simpidx_symmetriesptr) {
	  for (SymmetryGroup::const_iterator iter = _simpidx_symmetriesptr->begin();
	       iter != _simpidx_symmetriesptr->end();
	       ++iter) {
	    if (iter->lex_decreases(current_partial_triang.index_set_pure())) {
	      return true;
	    }
	  }
	  return false;	
	}
	else {
	  for (SymmetryGroup::const_iterator iter = _symmetriesptr->begin();
	       iter != _symmetriesptr->end();
	       ++iter) {
	    if (iter->lex_decreases(current_partial_triang)) {
	      return true;
	    }
	  }
	  return false;	
	}
      }
    }
  }

  bool SymmetricExtensionGraph::_update_coversimps(const PartialTriang&            current_partial_triang,
						   const Simplex&                  freefacet,
						   seg_coversimps_type&            coversimps,
						   seg_coveradmissibles_type&      coveradmissibles,
						   seg_coversimps_flag_type&       these_coversimps_changed,
						   seg_coveradmissibles_flag_type& these_coveradmissibles_changed,
						   bool&                           no_extension_possible) const {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "updating coversimps of freefacet "
		<< freefacet << " ..." << std::endl;
    }
    SimplicialComplex common_otherfacets_coveradmissibles(current_partial_triang.admissibles());
    SimplicialComplex otherfreefacet_conflictfree; // default construction before loop saves destructions in each loop
    for (SimplicialComplex::const_iterator offiter = current_partial_triang.freeintfacets().begin();
	 offiter != current_partial_triang.freeintfacets().end();
	 ++offiter) {
      if (*offiter == freefacet) {
	continue;
      }
      const Simplex& other_freefacet(*offiter);
      otherfreefacet_conflictfree = coveradmissibles[other_freefacet]; // assignment instead of construction in loop saves destruction
      otherfreefacet_conflictfree += coversimps[other_freefacet];
      common_otherfacets_coveradmissibles *= otherfreefacet_conflictfree;
      // common_otherfacets_coveradmissibles *= (coveradmissibles[other_freefacet] + coversimps[other_freefacet]);
      if (common_otherfacets_coveradmissibles.empty()) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "no common coveradmissibles of other free interior facets: no extension possible." << std::endl;
	}
	if (_iter_coversimptighten > _maxiter_coversimptighten) {
	  _maxiter_coversimptighten = _iter_coversimptighten;
	}
	no_extension_possible = true;
	return true;
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "intersection of coveradmissibles of other free interior facets is "
		<< common_otherfacets_coveradmissibles << std::endl;
    }
    if (common_otherfacets_coveradmissibles.superset(coversimps[freefacet])) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "coversimps " << coversimps[freefacet]
		  << " of freefacet " << freefacet << " not tightened" << std::endl;
      }
      return false;
    }
    coversimps[freefacet] *= common_otherfacets_coveradmissibles;
    if (coversimps[freefacet].empty()) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "no coversimp of " << freefacet
		  << " is in coveradmissibles of all other free interior facets: no extension possible." << std::endl;
      }
      if (_iter_coversimptighten > _maxiter_coversimptighten) {
	_maxiter_coversimptighten = _iter_coversimptighten;
      }
      no_extension_possible = true;
      return true;
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "continue with updated coversimps of " << freefacet << std::endl;
    }
    return true;
  }

  bool SymmetricExtensionGraph::_update_coveradmissibles(const PartialTriang&            current_partial_triang,
							 const Simplex&                  freefacet,
							 seg_coversimps_type&            coversimps,
							 seg_coveradmissibles_type&      coveradmissibles,
							 seg_coversimps_flag_type&       these_coversimps_changed,
							 seg_coveradmissibles_flag_type& these_coveradmissibles_changed) const {
    these_coveradmissibles_changed[freefacet] = false;
    if (!these_coversimps_changed[freefacet]) {
      return false;
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "updating coveradmissibles of free interior facet " << freefacet << " ..." << std::endl;
    }
    coveradmissibles[freefacet].clear();
    for (SimplicialComplex::const_iterator coveriter = coversimps[freefacet].begin();
	 coveriter != coversimps[freefacet].end();
	 ++coveriter) {
#ifndef STL_CONTAINERS
      coveradmissibles[freefacet] += current_partial_triang.admtableptr()->operator[](*coveriter);
#else      
      coveradmissibles[freefacet] += current_partial_triang.admtableptr()->find(*coveriter)->second;
#endif
    }
    return true;
  }

  bool SymmetricExtensionGraph::_some_possiblecoversimps_empty(const PartialTriang&            current_partial_triang,
							       seg_coversimps_type&            coversimps,
							       seg_coveradmissibles_type&      coveradmissibles,
							       seg_coversimps_flag_type&       these_coversimps_changed,
							       seg_coveradmissibles_flag_type& these_coveradmissibles_changed) const {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "try to tighten coversimplices of free interior facets "
		<< current_partial_triang.freeintfacets() << " ..." << std::endl;
    }
    bool some_coversimps_changed(true);
    _iter_coversimptighten = 0;
    while (some_coversimps_changed) {
      ++_iter_coversimptighten;
      for (SimplicialComplex::const_iterator ffiter = current_partial_triang.freeintfacets().begin();
	   ffiter != current_partial_triang.freeintfacets().end();
	   ++ffiter) {
	const Simplex& freefacet(*ffiter);
	these_coveradmissibles_changed[freefacet] =_update_coveradmissibles(current_partial_triang,
									    freefacet,
									    coversimps,
									    coveradmissibles,
									    these_coversimps_changed,
									    these_coveradmissibles_changed);
      }
      for (facets_data::const_iterator ffiter = current_partial_triang.freeconffacets().begin();
	   ffiter != current_partial_triang.freeconffacets().end();
	   ++ffiter) {
	const Simplex& freefacet(*ffiter);
	these_coveradmissibles_changed[freefacet] =_update_coveradmissibles(current_partial_triang,
									    freefacet,
									    coversimps,
									    coveradmissibles,
									    these_coversimps_changed,
									    these_coveradmissibles_changed);
      }
      some_coversimps_changed = false;
      bool no_extension_possible = false;
      for (SimplicialComplex::const_iterator ffiter = current_partial_triang.freeintfacets().begin();
	   ffiter != current_partial_triang.freeintfacets().end();
	   ++ffiter) {
	const Simplex& freefacet(*ffiter);
	these_coversimps_changed[freefacet] = _update_coversimps(current_partial_triang,
								 freefacet,
								 coversimps,
								 coveradmissibles,
								 these_coversimps_changed,
								 these_coveradmissibles_changed,
								 no_extension_possible);
	if (no_extension_possible) {
	  return true;
	}
	if (these_coversimps_changed[freefacet]) {
	  some_coversimps_changed = true;
	}
      }
      for (facets_data::const_iterator ffiter = current_partial_triang.freeconffacets().begin();
	   ffiter != current_partial_triang.freeconffacets().end();
	   ++ffiter) {
	const Simplex& freefacet(*ffiter);
	these_coversimps_changed[freefacet] = _update_coversimps(current_partial_triang,
								 freefacet,
								 coversimps,
								 coveradmissibles,
								 these_coversimps_changed,
								 these_coveradmissibles_changed,
								 no_extension_possible);
	if (no_extension_possible) {
	  return true;
	}
	if (these_coversimps_changed[freefacet]) {
	  some_coversimps_changed = true;
	}
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... no further tightening possible" << std::endl;
    }
    return false;
  }

  bool SymmetricExtensionGraph::_some_possibleinitcoversimps_empty(const PartialTriang&            current_partial_triang,
								   seg_coversimps_type&            coversimps,
								   seg_coveradmissibles_type&      coveradmissibles,
								   seg_coversimps_flag_type&       these_coversimps_changed,
								   seg_coveradmissibles_flag_type& these_coveradmissibles_changed) const {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "generating all coversimplices of free interior facets for current_partial_triang "
		<< current_partial_triang << " ..." << std::endl;
    }
    for (SimplicialComplex::const_iterator ffiter = current_partial_triang.freeintfacets().begin();
	 ffiter != current_partial_triang.freeintfacets().end();
	 ++ffiter) {
      const Simplex& freeintfacet(*ffiter);
      const seg_coversimps_type::iterator coversimp_iter = coversimps.insert(std::pair<Simplex, SimplicialComplex>(freeintfacet,
														   current_partial_triang.inctableptr()->intcofacets(freeintfacet)
														   * current_partial_triang.admissibles()
														   )
									     ).first;
      if (coversimp_iter->second.empty()) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "coversimplices of free interior facet " << freeintfacet
		    << " empty: no extension possible" << std::endl;
	}
	return true;
      }
      these_coversimps_changed[freeintfacet] = true;
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "generating all coversimplices of free configuration facets "
		<< current_partial_triang.freeconffacets()
		<< " for current_partial_triang "
		<< current_partial_triang << " ..." << std::endl;
    }
    for (facets_data::const_iterator ffiter = current_partial_triang.freeconffacets().begin();
	 ffiter != current_partial_triang.freeconffacets().end();
	 ++ffiter) {
      const Simplex& freeconffacet(*ffiter);
      const seg_coversimps_type::iterator coversimp_iter = coversimps.insert(std::pair<Simplex, SimplicialComplex>(freeconffacet,
														   current_partial_triang.inctableptr()->confcofacets(freeconffacet)
														   * current_partial_triang.admissibles()
														   )
									     ).first;
      if (coversimp_iter->second.empty()) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "coversimplices of free configuration facet " << freeconffacet
		    << " empty: no extension possible" << std::endl;
	}
	return true;
      }
      these_coversimps_changed[freeconffacet] = true;
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done - no obstruction so far" << std::endl;
    }
    return false;
  }

  bool SymmetricExtensionGraph::_noncoverable_freefacet_lex(const PartialTriang& current_partial_triang) const {

    // we have to check beforehand that neither admissibles nor freefacets are empty:
    const Simplex min_coverable = current_partial_triang.admissibles().begin()->lexmin_subset(_rank - 1);
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << *current_partial_triang.freeintfacets().begin() << " < " << min_coverable << "?" << std::endl;
    }
    if (current_partial_triang.freeintfacets().begin()->lexsmaller(min_coverable)) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << *current_partial_triang.freeintfacets().begin() << " < " << min_coverable << std::endl;
      }
      return true;
    }
    else {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << *current_partial_triang.freeintfacets().begin() << " not < " << min_coverable << std::endl;
      }
      return false;
    }
  }

  bool SymmetricExtensionGraph::_noncoverable_freefacet_lean(const PartialTriang& current_partial_triang) const {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "lean search for a non coverable interior facet in "
		<< current_partial_triang << " ..." << std::endl;
    }
    for (SimplicialComplex::const_iterator ffiter = current_partial_triang.freeintfacets().begin();
	 ffiter != current_partial_triang.freeintfacets().end();
	 ++ffiter) {
      const Simplex& freeintfacet(*ffiter);
      if ((current_partial_triang.inctableptr()->intcofacets(freeintfacet) * current_partial_triang.admissibles()).empty()) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "coversimplices of free interior facet " << freeintfacet
		    << " empty: no extension possible" << std::endl;
	}
	return true;
      }
    }
    for (facets_data::const_iterator ffiter = current_partial_triang.freeconffacets().begin();
	 ffiter != current_partial_triang.freeconffacets().end();
	 ++ffiter) {
      const Simplex& freeconffacet(*ffiter);
      if ((current_partial_triang.inctableptr()->confcofacets(freeconffacet) * current_partial_triang.admissibles()).empty()) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "coversimplices of free configuration facet " << freeconffacet
		    << " empty: no extension possible" << std::endl;
	}
	return true;
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done - no obstruction so far" << std::endl;
    }
    return false;
  }

  bool SymmetricExtensionGraph::_noncoverable_freefacet_strong(const PartialTriang& current_partial_triang) const {
    bool result(false);
    seg_coversimps_type coversimps;
    seg_coveradmissibles_type coveradmissibles;
    seg_coversimps_flag_type these_coversimps_changed;
    seg_coveradmissibles_flag_type these_coveradmissibles_changed;

    if (_some_possibleinitcoversimps_empty(current_partial_triang,
					   coversimps,
					   coveradmissibles,
					   these_coversimps_changed,
					   these_coveradmissibles_changed)) {
      return true;
    }
    if (_some_possiblecoversimps_empty(current_partial_triang,
				       coversimps,
				       coveradmissibles,
				       these_coversimps_changed,
				       these_coveradmissibles_changed)) {
      if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "*** strong obstruction search successful - no extension possible ***" << std::endl;
      }
      return true;
    }
    return false;
  }
  
  bool SymmetricExtensionGraph::_noncoverable_freefacet(const PartialTriang& current_partial_triang) const {
    if (CommandlineOptions::no_extension_check()) {
      return false;
    }
    if (CommandlineOptions::full_extension_check()) {
      return this->_noncoverable_freefacet_strong(current_partial_triang);
    }
    else if (CommandlineOptions::use_volume_order() || CommandlineOptions::use_random_order()) {
      return this->_noncoverable_freefacet_lean(current_partial_triang);
    }
    else {
      return this->_noncoverable_freefacet_lex(current_partial_triang);
    }
  }
  
  bool SymmetricExtensionGraph::_not_enough_volume(const PartialTriang& current_partial_triang) const {
    if (CommandlineOptions::symmetries_are_isometric() || _symmetriesptr->empty()) {
      Field missing_volume = _volume - current_partial_triang.covered_volume();
      SimplicialComplex::const_iterator iter = current_partial_triang.admissibles().begin();
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "still " << current_partial_triang.admissibles().card() << " simplices available"
		  << " to cover an uncovered volume of " << missing_volume << " ..." << std::endl;
      }
      while (missing_volume > FieldConstants::ZERO) {
	if (iter == current_partial_triang.admissibles().end()) {
	  return true;
	}
	missing_volume -= current_partial_triang.voltableptr()->find(*iter)->second;
	++iter;
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "still " << current_partial_triang.admissibles().card() << " simplices available"
		    << " to cover an uncovered volume of " << missing_volume << " ..." << std::endl;
	}
      }
      return false;
    }
    else {
      return false;
    }
  }
  
  size_type SymmetricExtensionGraph::_min_no_of_missing_simplices(const PartialTriang& current_partial_triang) const {
    const size_type card_bound =  (current_partial_triang.freeintfacets().card() + current_partial_triang.freeconffacets().size() + _rank - 1) / _rank;
    if (CommandlineOptions::use_volume_order() && CommandlineOptions::use_volumes()) {

      // in this case, we can rely on current_partial_triang.admissibles() to be sorted by volume:
      Field missing_volume = _volume;
      missing_volume -= current_partial_triang.covered_volume();
      size_type result = 0UL;
      SimplicialComplex::const_iterator iter = current_partial_triang.admissibles().begin();
      while (missing_volume > FieldConstants::ZERO) {
	if (iter == current_partial_triang.admissibles().end()) {
	  return std::numeric_limits<size_type>::max();
	}
	missing_volume -= current_partial_triang.voltableptr()->find(*iter)->second;
	++result;
	++iter;
      }
      return std::max<size_type>(result, card_bound);
    }
    else {
      return card_bound;
    }
  }

  void SymmetricExtensionGraph::_init() {
    if (CommandlineOptions::output_asy()) {
      Graphics::run_to_asy(_ID);
      Graphics::partialtriang_to_asy(_ID, _runID, _rootptr->partial_triang().index_set_pure(), _rootptr->partial_triang());
    }
    if (CommandlineOptions::parallel_symmetries()) {
      _no_of_threads = CommandlineOptions::no_of_threads();
      _init_symmetry_workers();
    }

    // from here on we need to block IO:
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "exploring all symmetry classes of triangulations by extension ..." << std::endl;
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "SymmetricExtensionGraph:" << std::endl;
      std::cerr << "root triangulation: " << _rootptr->partial_triang() << std::endl;
      std::cerr << "freeintfacets     : " << _rootptr->partial_triang().freeintfacets() << std::endl;
      std::cerr << "admissibles       : " << _rootptr->partial_triang().admissibles() << std::endl;
      std::cerr << "admissibles table : " << *(_rootptr->partial_triang().admtableptr()) << std::endl;
      std::cerr << "incidences        : " << *(_rootptr->partial_triang().inctableptr()) << std::endl;
      if (_rootptr->partial_triang().voltableptr()) {
	std::cerr << "volumes           : " << *(_rootptr->partial_triang().voltableptr()) << std::endl;
      }
    }
  }

  void SymmetricExtensionGraph::_init_symmetry_workers() {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "init distributed symmetry workers ..." << std::endl;
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "initializing symmetry worker " << i << " ..." << std::endl;
      }
      _symmetry_workers.emplace_back(i, *this);
      if (CommandlineOptions::debug()) {    
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done" << std::endl;
      }
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "starting symmetry worker thread " << i 
		  << " ..." << std::endl;
      }
      _threads.push_back(std::thread(&SymmetricExtensionGraph::SymmetryWorker::operator(),
				     std::ref(_symmetry_workers[i])));
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done" << std::endl;
      }
    }

    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done" << std::endl;
    }
  }

  void SymmetricExtensionGraph::_bfs(const seg_nodes_container_type&  current_nodes,
				     size_type&                       depth) {
#if !defined USE_LEXORDER || defined USE_LEXORDER_DEBUG
    _doneset.clear();
#endif
    SimplicialComplex forbidden;
    // seg_triang_container_type new_partial_triangs;
    ++depth;
    _progress_vector.resize(depth);
    size_type doneload(0UL);
    const size_type workload(current_nodes.size());
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "upsizing progress vector to " << depth << " ..." << std::endl;
    }
    for (seg_nodes_container_type::const_iterator iter = current_nodes.begin();
	 iter != current_nodes.end();
	 ++iter) {
      ++doneload;
      _progress_vector.at(depth - 1) = (100 * doneload) / workload;

      const node_type current_node(*iter);
      const PartialTriang& current_partial_triang(current_node.partial_triang());
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "processing partial triang " << current_partial_triang << " ..." << std::endl;
	std::cerr << "still possible extension simplices " << current_partial_triang.admissibles() << std::endl;
      }

      // loop over all admissible extensions:
      for (SimplicialComplex::const_iterator simpiter = current_partial_triang.admissibles().begin();
	   simpiter != current_partial_triang.admissibles().end();
	   ++simpiter) {
	++this->_nodecount;
	if (CommandlineOptions::verbose()) {
	  report_progress(std::cerr);
	}
	const Simplex& new_simp(*simpiter);
	forbidden += new_simp;
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "adding " << new_simp << " to " << current_partial_triang << " ..." << std::endl;
	}
	PartialTriang next_partial_triang(current_partial_triang, new_simp, forbidden);
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "resulting in " << next_partial_triang << std::endl;
	  std::cerr << next_partial_triang << std::endl;
	  std::cerr << "next free facets" << std::endl;
	  std::cerr << next_partial_triang.freeintfacets() << std::endl;
	}
	// since the following check is faster than checking for an old symmetry class,
	// we prepend it here - check whether we are in a dead end:
	if (!next_partial_triang.freeintfacets().empty()) {

	  // not yet a triangulation - perform the extendability checks:
	  if (next_partial_triang.admissibles().empty()) {
	    
	    // we are in a dead end:
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "reached maximal non-triangulation:" << std::endl;
	      std::cerr << next_partial_triang << std::endl;
	    }
	    ++this->_deadendcount;
	    continue;
	  }
	  
	  // the following check is faster than to check for an equivalent partial triangulation:
	  // check whether the partial triangulation can possibly be extended:
	  if (this->_noncoverable_freefacet(next_partial_triang)) {
	    
	    //early detected dead end:
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "no full-triangulation extension possible of " << next_partial_triang <<  std::endl;
	    }
	    ++this->_earlydeadendcount;
	    continue;
	  }
	  
	  // another fast check:
	  // check whether there is still enough volume in the possible admissible simplices:
	  if (CommandlineOptions::use_volumes()) {
	    if (this->_not_enough_volume(next_partial_triang)) {
	      // early detected dead end:
	      if (CommandlineOptions::debug()) {
		std::lock_guard<std::mutex> lock(IO_sync::mutex);
		std::cerr << "volume obstruction hit - no extension possible of " << next_partial_triang <<  std::endl;
	      }
	      ++this->_earlydeadendcount;
	      continue;
	    }
	  }
	}

	// now the more expensive symmetry check (necessary also for triangulations):
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "checking for old symmetry class ..." << std::endl;
	}
	bool is_new = true;
	critical_simpidx_table_type new_critsimp_table;
	size_type new_stabilizer_card = 0UL;
	if (CommandlineOptions::use_classified_symmetries()) {
	  // use the method of TOPCOM-1.0.0:
	  is_new = !_old_symmetry_class(next_partial_triang, new_simp);
	}
	else {
	  // use the method of TOPCOM-1.0.1 and above:
	  if (!_simpidx_symmetriesptr) {
	    is_new = current_node.child_is_lexmin_lean(new_simp, &new_critsimp_table, &new_stabilizer_card);
	  }
	  else {
	    is_new = current_node.child_is_lexmin(new_simp, &new_critsimp_table, &new_stabilizer_card);
	  }
	}
      
	if (!is_new) {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << next_partial_triang << " was found already: continue" << std::endl;
	  }
	  continue;
	}
	else {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << next_partial_triang << " was not found yet: process" << std::endl;
	  }
	}
      
	// next partial triang is new:
	if (next_partial_triang.freeintfacets().empty()) {
	  // next partial triang is a complete triang of minimal cardinality:
	  // check regularity if required and output the triangulation:
	  if (CommandlineOptions::check_regular()) {
	    if (CommandlineOptions::symmetries_are_affine()) {
	      // here we need to check only the found representative for regularity:
	      RegularityCheck regchecker(*_pointsptr, *_chiroptr, *_inctableptr, next_partial_triang);
	      if (regchecker.is_regular()) {
		_mintriang = next_partial_triang;
		_mincard = _mintriang.card();
		if (this->_print_triangs) {
		  // std::lock_guard<std::mutex> lock(IO_sync::mutex);
		  // std::cout << "T_min[" << this->_ID
		  // 	    << "] := " << _mintriang << ";" << std::endl;
		  _output_stream << "T_min[" << this->_ID
				 << "] := " << _mintriang << ";\n";
		  if (CommandlineOptions::output_heights()) {
		    _output_stream << "h_min[" << this->_ID
				   << "] := " << regchecker.heights() << ";\n";
		  }
		  output_results(std::cout);
		}
		if (CommandlineOptions::verbose()) {
		  std::lock_guard<std::mutex> lock(IO_sync::mutex);
		  std::cerr << "worker " << this->_ID << " found minimal triangulation "
			    << _mintriang << " with " << _mincard << " simplices" << std::endl;
		}
		++this->_symcount;
		return;
	      }
	    }
	    else {
	      // here we need to check the complete orbit (except the user wants to skip this):
	      RegularityCheck regchecker(*_pointsptr, *_chiroptr, *_inctableptr, next_partial_triang);
	      if (regchecker.is_regular()) {
		_mintriang = next_partial_triang;
		_mincard = _mintriang.card();
		if (this->_print_triangs) {
		  // std::lock_guard<std::mutex> lock(IO_sync::mutex);
		  // std::cout << "T_min[" << this->_ID 
		  // 	    << "] := " << _mintriang << ";" << std::endl;
		  _output_stream << "T_min[" << this->_ID 
				 << "] := " << _mintriang << ";\n";
		  if (CommandlineOptions::output_heights()) {
		    _output_stream << "h_min[" << this->_ID
				   << "] := " << regchecker.heights() << ";\n";
		  }
		  output_results(std::cout);
		}
		if (CommandlineOptions::verbose()) {
		  std::lock_guard<std::mutex> lock(IO_sync::mutex);
		  std::cerr << "worker " << this->_ID << " found minimal triangulation "
			    << _mintriang << " with " << _mincard << " simplices" << std::endl;
		}
		++this->_symcount;
		return;
	      }
	      if (!CommandlineOptions::skip_orbitcount()) {
		for (SymmetryGroup::const_iterator symiter = _symmetriesptr->begin();
		     symiter != _symmetriesptr->end();
		     ++symiter) {
		  const Symmetry& g(*symiter);
		  const pt_complex_type _equivalent_partial_triang(g.map(next_partial_triang));
		  if (_orbit.find(_equivalent_partial_triang) == _orbit.end()) {
		    RegularityCheck regchecker(*_pointsptr, *_chiroptr, *_inctableptr, _equivalent_partial_triang);
		    if (regchecker.is_regular()) {
		      _mintriang = _equivalent_partial_triang;
		      _mincard = _mintriang.card();
		      if (this->_print_triangs) {
			// std::lock_guard<std::mutex> lock(IO_sync::mutex);
			// std::cout << "T_min[" << this->_ID 
			// 	  << "] := " << _mintriang << ";" << std::endl;
			_output_stream << "T_min[" << this->_ID 
				       << "] := " << _mintriang << ";\n";
			if (CommandlineOptions::output_heights()) {
			  _output_stream << "h_min[" << this->_ID
					 << "] := " << regchecker.heights() << ";\n";
			}
			output_results(std::cout);
		      }
		      if (CommandlineOptions::verbose()) {
			std::lock_guard<std::mutex> lock(IO_sync::mutex);
			std::cerr << "worker " << this->_ID << " found minimal triangulation "
				  << _mintriang << " with " << _mincard << " simplices" << std::endl;
		      }
		      ++this->_symcount;
		      return;
		    }
		  }
		}
	      }
	    }
	  }
	  else {
	    if (this->_print_triangs) {

	      // output the triangulation:
	      // std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      // std::cout << "T[" << this->_ID << "] := " << next_partial_triang << ";" << std::endl;								
	      _output_stream << "T[" << this->_ID << "] := " << next_partial_triang << ";\n";
              output_results(std::cout);
	    }
	    _mintriang = next_partial_triang;
	    _mincard = _mintriang.card();
	    if (CommandlineOptions::verbose()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "worker " << this->_ID << " found minimal triangulation "
			<< _mintriang << " with " << _mincard << " simplices" << std::endl;
	    }
	    ++_symcount;
	    ++_totalcount;
	    return;
	  }
	}
	else if (next_partial_triang.admissibles().empty()) {

	  // dead end because we ran out of admissible simplices to extend the partial triangulation:
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "reached maximal non-triangulation:" << std::endl;
	    std::cerr << next_partial_triang << std::endl;
	  }
	  ++this->_deadendcount;
	}
	else {
	
	  // partial triang - move node into queue for later processing:
	  _open_nodes.emplace_back(current_node, std::move(next_partial_triang), std::move(new_critsimp_table));
	}
      }
      forbidden.clear();
    }
  }

  void SymmetricExtensionGraph::_dfs(const node_type& current_node,
				     size_type&       depth) {

    SimplicialComplex forbidden;
    const PartialTriang& current_partial_triang(current_node.partial_triang());
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "extending the following partial triangulation:" << std::endl;
      std::cerr << "partial triangulation: " << current_partial_triang << std::endl;
      std::cerr << "freeintfacets        : " << current_partial_triang.freeintfacets() << std::endl;
      std::cerr << "admissibles          : " << current_partial_triang.admissibles() << std::endl;
      std::cerr << "admissibles table    : " << *current_partial_triang.admtableptr() << std::endl;
      std::cerr << "incidences           : " << *current_partial_triang.inctableptr() << std::endl;
      if (_rootptr->partial_triang().voltableptr()) {
	std::cerr << "volumes              : " << *current_partial_triang.voltableptr() << std::endl;
      }
    }
  
    const size_type workload(current_partial_triang.admissibles().card());  
    size_type doneload(0UL);
    ++depth;
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "upsizing progress vector to " << depth << " ..." << std::endl;
    }
    _progress_vector.resize(depth);

    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "D|A|I|C -> "
		<< std::setw(4) << depth << "|"
		<< std::setw(4) << workload << "|"
		<< std::setw(4) << current_partial_triang.freeintfacets().card() << "|"
		<< std::setw(4) << current_partial_triang.freeconffacets().size()
		<< std::endl;
    }

    // for asy output:
    size_type parent_node_ID = this->_nodecount;

    // loop over all admissible extensions:
    for (SimplicialComplex::const_iterator simpiter = current_partial_triang.admissibles().begin();
	 simpiter != current_partial_triang.admissibles().end();
	 ++simpiter) {
      ++doneload;
      _progress_vector.at(depth - 1) = (100 * doneload) / workload;
      ++this->_nodecount;
      const Simplex& new_simp(*simpiter);

      // for asy output:
      size_type child_node_ID = this->_nodecount;

      if (CommandlineOptions::output_asy()) {
	SimplicialComplex next_partial_triang(current_partial_triang + new_simp);
	Graphics::partialtriang_to_asy(_ID, _runID, next_partial_triang.index_set_pure(), next_partial_triang);
	Graphics::arc_to_asy(_ID, _runID, parent_node_ID, child_node_ID);
      }

      if (CommandlineOptions::verbose()) {
	report_progress(std::cerr);
      }

      if (!CommandlineOptions::no_extension_check()
	  // && !CommandlineOptions::full_extension_check()
	  && !CommandlineOptions::use_volume_order()
	  && !CommandlineOptions::use_random_order()) {

	// in this lex-order case, we can check whether the new simplex is able to cover the minimal uncovered facets;
	// if not, neither this new simplex nor the upcoming ones will cover it:
	const Simplex min_coverable = new_simp.lexmin_subset(_rank - 1);
	if (!current_partial_triang.freeintfacets().empty() && current_partial_triang.freeintfacets().begin()->lexsmaller(min_coverable)) {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << *current_partial_triang.freeintfacets().begin() << " < " << min_coverable << std::endl;
	  }

	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::veryearlydeadend_to_asy(_ID, _runID, child_node_ID);
	  }

	  ++this->_earlydeadendcount;
	  break;
	}
      }
      forbidden += new_simp;
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "adding " << new_simp << " to " << current_partial_triang << " ..." << std::endl;
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "node " << this->_nodecount << ": checking new partial triangulation:" << std::endl;
	std::cerr << "current free facets" << std::endl;
	std::cerr << current_partial_triang.freeintfacets() << std::endl;
      }

      // construct the extended partial triangulation:
      PartialTriang next_partial_triang(current_partial_triang, new_simp, forbidden);
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "resulting in " << next_partial_triang << std::endl;
	std::cerr << next_partial_triang << std::endl;
	std::cerr << "next free facets" << std::endl;
	std::cerr << next_partial_triang.freeintfacets() << std::endl;
      }
    
      // check lower bound for minimality if required:
      if (this->_find_minimal_triang) {
	size_type min_no_of_simplices = depth + this->_min_no_of_missing_simplices(next_partial_triang);
	std::lock_guard<std::mutex> min_lock(_min_mutex);
	if (min_no_of_simplices >= this->_mincard) {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "current minimal cardinality is " << this->_mincard << std::endl;
	    std::cerr << "minimal no. of simplices is    " << min_no_of_simplices << " => prune" << std::endl;
	  }
	  continue;
	}
      }

      // check target no of simplices if required:
      size_type target_no_of_simplices = 0;
      if (CommandlineOptions::max_no_of_simplices() > 0) {
	target_no_of_simplices = CommandlineOptions::max_no_of_simplices();
      }
      if (CommandlineOptions::no_of_simplices() > 0) {
	target_no_of_simplices = CommandlineOptions::no_of_simplices();
      }
      if (target_no_of_simplices > 0) {
	size_type min_no_of_simplices = depth + this->_min_no_of_missing_simplices(next_partial_triang);
	if (min_no_of_simplices > target_no_of_simplices) {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "current no. of simplices = " << depth << std::endl;
	    std::cerr << "minimal number of missing simplices = " << min_no_of_simplices << std::endl;
	    std::cerr << "=> target no. of simplices " << target_no_of_simplices << " unreachable => prune" << std::endl;
	  }
	  continue;
	}
      }
      
      // since the following check is faster than checking for an old symmetry class,
      // we prepend it here - check whether we are in a dead end:
      if (!next_partial_triang.freeintfacets().empty()) {

	// not yet a triangulation - perform the extendability checks:
	if (next_partial_triang.admissibles().empty()) {
	  
	  // we are in a dead end:
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);

	    std::cerr << "freeintfacets: " << next_partial_triang.freeintfacets() << std::endl;
	    std::cerr << "admissibles  : " << next_partial_triang.admissibles() << std::endl;
	    std::cerr << "reached maximal non-triangulation:" << std::endl;
	    std::cerr << next_partial_triang << std::endl;
	  }

	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::deadend_to_asy(_ID, _runID, child_node_ID);
	  }
	  
	  ++this->_deadendcount;
	  continue;
	}
	
	// the following checks are faster than checking for a lex-smaller partial triangulation,
	// thus, they are performed first:
	// check whether we can extend current_partial_triang at all free facets:
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "cover obstruction check for " << next_partial_triang <<  std::endl;
	}
	
	// the following check is faster than to check for an equivalent partial triangulation:
	// check whether the partial triangulation can possibly be extended:
	if (this->_noncoverable_freefacet(next_partial_triang)) {
	  
	  // early detected dead end:
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "cover obstruction hit - no extension possible of " << next_partial_triang <<  std::endl;
	  }

	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::earlydeadend_to_asy(_ID, _runID, child_node_ID);
	  }
	  
	  ++this->_earlydeadendcount;
	  continue;
	}

	// another fast check:
	// check whether there is still enough volume in the possibly extending simplices:
	if (CommandlineOptions::use_volumes() && !this->_find_minimal_triang) {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "volume obstruction check for " << next_partial_triang <<  std::endl;
	  }
	  if (this->_not_enough_volume(next_partial_triang)) {
	    
	    // early detected dead end:
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "volume obstruction hit - no extension possible of " << next_partial_triang <<  std::endl;
	    }

	    // for asy output:
	    if (CommandlineOptions::output_asy()) {
	      Graphics::missingvolume_to_asy(_ID, _runID, child_node_ID);
	    }
	  
	    ++this->_earlydeadendcount;
	    continue;
	  }
	}
      }
      
      // now we must check whether we really have found something new (also for triangulations):
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "checking for old symmetry class ..." << std::endl;
      }
      bool is_new = true;
      critical_simpidx_table_type new_critsimp_table;
      size_type new_stabilizer_card = 0UL;
      if (CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_switch_tables() || CommandlineOptions::use_naive_symmetries()) {

	// use the method of TOPCOM-1.0.0:
	is_new = !_old_symmetry_class(next_partial_triang, new_simp);
      }
      else {

	// use the method of TOPCOM-1.0.1 and above:
	if (!_simpidx_symmetriesptr) {
	  is_new = current_node.child_is_lexmin_lean(new_simp, &new_critsimp_table, &new_stabilizer_card);
	}
	else {
	  is_new = current_node.child_is_lexmin(new_simp, &new_critsimp_table, &new_stabilizer_card);
	}
      }
      
      if (!is_new) {

	// for asy output:
	if (CommandlineOptions::output_asy()) {
	  Graphics::notnew_to_asy(_ID, _runID, child_node_ID);
	}	
	if (CommandlineOptions::debug()) {	  
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_partial_triang << " was found already: continue" << std::endl;
	}
	continue;
      }
      else {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_partial_triang << " was not found yet: process" << std::endl;
	}
      }

      // next partial triang is new - check for completeness:
      if (next_partial_triang.freeintfacets().empty()) {
	
	// we found a complete triangulation - check regularity if required and output the triangulation:

	// for asy output:
	if (CommandlineOptions::output_asy()) {
	  Graphics::solution_to_asy(_ID, _runID, child_node_ID);
	}	
	
	// check lower bound for minimality again using the lower bound computed above:
	if (this->_find_minimal_triang) {
	  std::lock_guard<std::mutex> min_lock(_min_mutex);
	  if (depth >= this->_mincard) {
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "current minimal cardinality is " << this->_mincard << std::endl;
	      std::cerr << "current no. of simplices is    " << depth << " => ignore" << std::endl;
	    }
	    continue;
	  }
	}

	// check target no of simplices if required:
	if (((CommandlineOptions::no_of_simplices() > 0) && (depth != CommandlineOptions::no_of_simplices()))
	    || ((CommandlineOptions::max_no_of_simplices() > 0) && (depth > CommandlineOptions::max_no_of_simplices()))) {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "current no. of simplices = " << depth << std::endl;
	      std::cerr << "=> target no. of simplices " << CommandlineOptions::no_of_simplices() << " does not match => ignore" << std::endl;
	  }
	  continue;
	}

	if (CommandlineOptions::check_regular()) {
	  if (CommandlineOptions::symmetries_are_affine()) {
	    // here we need to check only the found representative for regularity:
	    RegularityCheck repregchecker(*_pointsptr, *_chiroptr, *_inctableptr, next_partial_triang);
	    if (repregchecker.is_regular()) {
	      ++this->_symcount;
	      {
		std::lock_guard<std::mutex> min_lock(_min_mutex);
		if (depth < this->_mincard) {
		  this->_mintriang = next_partial_triang;
		  this->_mincard = depth;
		}
	      }
	      if (this->_print_triangs) {
		// std::lock_guard<std::mutex> lock(IO_sync::mutex);
		// std::cout << "T[" << this->_ID << "," << this->_symcount + this->_current_symcount - 1
		// 	  << "] := " << next_partial_triang << ";" << std::endl;
		_output_stream << "T[" << this->_ID
			       << "," << this->_runID
			       << "," << this->_symcount
			       << "] := " << next_partial_triang << ";\n";
		if (CommandlineOptions::output_heights()) {
		  _output_stream << "h[" << this->_ID
				 << "," << this->_runID
				 << "," << this->_symcount
				 << "] := " << repregchecker.heights() << ";\n";
		}
		output_results(std::cout);
	      }
	      if (!CommandlineOptions::skip_orbitcount()) {

		// use the stabilizer formula:
		if (CommandlineOptions::use_switch_tables()) {
		  this->_orbitsize = _switch_tableptr->orbit_size(next_partial_triang.index_set_pure());
		}
		else if (CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_naive_symmetries()) {
		  if (!_simpidx_symmetriesptr) {
		    this->_orbitsize =
		      (this->_symmetriesptr->size() + 1)
		      / (this->_symmetriesptr->stabilizer_card(next_partial_triang) + 1);
		  }
		  else {
		    this->_orbitsize =
		      (this->_simpidx_symmetriesptr->size() + 1)
		      / (this->_simpidx_symmetriesptr->stabilizer_card(next_partial_triang.index_set_pure()) + 1);
		  }
		}
		else {
		  this->_orbitsize =
		    (this->_symmetriesptr->size() + 1)
		    / (new_stabilizer_card + 1);
		}
	      }
	    }
	  }
	  else {
	    
	    // here we need to check the complete orbit (except the user wants to skip this):
	    _orbit.insert(next_partial_triang);
	    RegularityCheck repregchecker(*_pointsptr, *_chiroptr, *_inctableptr, next_partial_triang);
	    static int count_reg = 0;
	    if (repregchecker.is_regular()) {
	      _regular_orbit.insert(next_partial_triang);
	      {
		std::lock_guard<std::mutex> min_lock(_min_mutex);
		if (depth < this->_mincard) {
		  this->_mintriang = next_partial_triang;
		  this->_mincard = depth;
		}
	      }
	      if (this->_print_triangs) {
		// std::lock_guard<std::mutex> lock(IO_sync::mutex);
		// std::cout << "T[" << this->_ID
		// 	  << "," << this->_symcount + this->_current_symcount
		// 	  << "] := " << next_partial_triang << ";" << std::endl;
		_output_stream << "T[" << this->_ID
			       << "," << this->_runID
			       << "," << this->_symcount
			       << "] := " << next_partial_triang << ";\n";
		if (CommandlineOptions::output_heights()) {
		  _output_stream << "h[" << this->_ID
				 << "," << this->_runID
				 << "," << this->_symcount
				 << "] := " << repregchecker.heights() << ";\n";
		}
		output_results(std::cout);
	      }
	    }
	    else {
	      if (CommandlineOptions::debug()) {
		std::lock_guard<std::mutex> lock(IO_sync::mutex);
		std::cerr << "non regular: " << next_partial_triang << std::endl;
	      }
	    }
	    if (!CommandlineOptions::skip_orbitcount()) {
	      for (SymmetryGroup::const_iterator symiter = _symmetriesptr->begin();
		   symiter != _symmetriesptr->end();
		   ++symiter) {
		const Symmetry& g(*symiter);
		const pt_complex_type _equivalent_partial_triang(g.map(next_partial_triang));
		if (_orbit.find(_equivalent_partial_triang) == _orbit.end()) {
		  
		  // not yet processed element in orbit:
		  _orbit.insert(_equivalent_partial_triang);
		  RegularityCheck orbregchecker(*_pointsptr, *_chiroptr, *_inctableptr, _equivalent_partial_triang);
		  if (orbregchecker.is_regular()) {
		    this->_regular_orbit.insert(_equivalent_partial_triang);
		    {
		      std::lock_guard<std::mutex> min_lock(_min_mutex);
		      if (depth < this->_mincard) {
			this->_mintriang = _equivalent_partial_triang;
			this->_mincard = depth;
		      }
		    }
		    if (this->_print_triangs) {
		      // std::lock_guard<std::mutex> lock(IO_sync::mutex);
		      // std::cout << "Torbit[" << this->_ID
		      // 		<< "," << this->_symcount + this->_current_symcount
		      // 		<< "," << this->_regular_orbit.size() - 1
		      // 		<< "] := " << _equivalent_partial_triang << ";" << std::endl;
		      _output_stream << "Torbit[" << this->_ID
				     << "," << this->_runID
				     << "," << this->_symcount
				     << "," << this->_regular_orbit.size() - 1
				     << "] := " << _equivalent_partial_triang << ";\n";
		      if (CommandlineOptions::output_heights()) {
			_output_stream << "horbit[" << this->_ID
				       << "," << this->_runID
				       << "," << this->_symcount
				       << "," << this->_regular_orbit.size() - 1
				       << "] := " << orbregchecker.heights() << ";\n";
		      }
		      output_results(std::cout);
		    }
		  }
		  else {
		    if (CommandlineOptions::debug()) {
		      std::lock_guard<std::mutex> lock(IO_sync::mutex);
		      std::cerr << "non regular: " << _equivalent_partial_triang << std::endl;
		    }
		  }
		}
	      }
	    }
	    this->_orbitsize = this->_regular_orbit.size();
	    if (this->_orbitsize > 0) {
	      ++this->_symcount;
	    }
	  }

	  // we count a symmetry class whenever one orbit element is regular:
	  this->_totalcount += this->_orbitsize;
	  this->_orbit.clear();
	  this->_regular_orbit.clear();
	}
	else {
	  if (this->_find_minimal_triang) {
	    std::lock_guard<std::mutex> min_lock(_min_mutex);	    
	    if (depth < this->_mincard) {
	      if (CommandlineOptions::debug()) {
		std::lock_guard<std::mutex> lock(IO_sync::mutex);
		std::cerr << "found smaller triangulation " << next_partial_triang
			  << " with " << depth << " simplices" << std::endl;
	      }
	      this->_mintriang = next_partial_triang;
	      this->_mincard = depth;
	    }
	  }
	  if (this->_print_triangs) {
	    // std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    // std::cout << "T[" << this->_ID << "," << this->_symcount + this->_current_symcount
	    // 	      << "] := " << next_partial_triang << ";" << std::endl;
	    _output_stream << "T[" << this->_ID
			   << "," << this->_runID
			   << "," << this->_symcount
			   << "] := " << next_partial_triang << ";\n";
	    output_results(std::cout);
	  }
	  if (!CommandlineOptions::skip_orbitcount()) {

	    // use the stabilizer formula:
	    if (CommandlineOptions::use_switch_tables()) {
	      this->_orbitsize = _switch_tableptr->orbit_size(next_partial_triang.index_set_pure());
	    }
	    else if (CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_naive_symmetries()) {
	      if (!_simpidx_symmetriesptr) {
		this->_orbitsize =
		  (this->_symmetriesptr->size() + 1)
		  / (this->_symmetriesptr->stabilizer_card(next_partial_triang) + 1);
	      }
	      else {
		this->_orbitsize =
		  (this->_simpidx_symmetriesptr->size() + 1)
		  / (this->_simpidx_symmetriesptr->stabilizer_card(next_partial_triang.index_set_pure()) + 1);
	      }
	    }
	    else {
	      this->_orbitsize =
		(this->_symmetriesptr->size() + 1)
		/ (new_stabilizer_card + 1);
	    }
	  }
	  // increase the count for symmetry classes:
	  ++this->_symcount;
	  this->_totalcount += this->_orbitsize;
	}
	this->_orbit.clear();
	this->_orbitsize = 0;
      }
      else {

	// depending on the choice of the user, interrupt when
	// * there are not enough nodes in the workbuffer
	// * the node budget has been used up:

	bool continue_work(true);
	if (CommandlineOptions::workbuffercontrol()) {
	
	  // break if not enough work on hold:
	  if (*this->_current_workbuffersizeptr < CommandlineOptions::min_workbuffersize()) {
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "work buffer drained down to size " << *this->_current_workbuffersizeptr
			<< " - stopping enumeration with "
			<< _open_nodes.size()
			<< " unprocessed nodes" << std::endl;
	    }
	    continue_work = false;
	  }
	}
	else {
	
	  // enter the recursion only if there is still node budget left:
	  if (this->_nodecount > _node_budget) {
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "node budget used up in depth " << depth
			<< " - stopping enumeration with "
			<< _open_nodes.size()
			<< " unprocessed nodes" << std::endl;
	    }
	    continue_work = false;
	  }
	}
	if (continue_work) {	
	  this->_dfs(node_type(current_node, std::move(next_partial_triang), std::move(new_critsimp_table)), depth);
	}
	else {
	  _open_nodes.emplace_back(current_node, std::move(next_partial_triang), std::move(new_critsimp_table));
	}
      }
    }
    --depth;
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "downsizing progress vector to " << depth << " ..." << std::endl;
    }
    _progress_vector.resize(depth);
  }
}; // namespace topcom

// eof SymmetricExtensionGraph.cc
