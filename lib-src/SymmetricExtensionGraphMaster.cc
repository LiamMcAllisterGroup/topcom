////////////////////////////////////////////////////////////////////////////////
// 
// SymmetricExtensionGraphMaster.cc
//
//    produced: 22/03/2020 jr
// last change: 22/03/2020 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include <limits>
#include <sstream>
#include <thread>
#include <functional>
#include <atomic>

#include "SymmetricExtensionGraphMaster.hh"

namespace topcom {

  SymmetricExtensionGraphMaster::Worker::Worker(const int                            workerID,
						const SymmetricExtensionGraphMaster& segm) :
    _workerID(workerID),
    _callerptr(&segm),
    _root_nodeptr(0),
    _symcount(0UL),
    _totalcount(0UL),
    _nodecount(0UL),
    _deadendcount(0UL),
    _earlydeadendcount(0UL),
    _mincard(segm._mincard),
    _mintriang(segm._mintriang),
    _maxiter_coversimptighten(0),
    _no_of_runs(0),
    _open_nodes(),
    _state(idle) {
  }

  SymmetricExtensionGraphMaster::Worker::Worker(const Worker& sw) :
    _workerID(sw._workerID),
    _callerptr(sw._callerptr),
    _root_nodeptr(sw._root_nodeptr),
    _symcount(sw._symcount),
    _totalcount(sw._totalcount),
    _nodecount(sw._nodecount),
    _deadendcount(sw._deadendcount),
    _earlydeadendcount(sw._earlydeadendcount),
    _mincard(sw._mincard),
    _mintriang(sw._mintriang),
    _maxiter_coversimptighten(0),
    _no_of_runs(sw._no_of_runs),
    _open_nodes(sw._open_nodes),
    _state(idle) {
  }

  void SymmetricExtensionGraphMaster::Worker::operator()() {
    
    // main loop of worker thread:
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "worker " << _workerID << " is spawned" << std::endl;
    }

    // the thread_local caches must be initialized in each thread:
    SymmetricExtensionGraphNode::init_simpidx_cache(_callerptr->_symmetriesptr);
    bool master_notified = false;
    while (!is_stopped()) {
      // the actual work is done here:
      if (is_hired()) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "worker " << _workerID << " doing work ..." << std::endl;
	}
	run();
	master_notified = false;
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... worker " << _workerID << " done" << std::endl;
	}
      }
      else if (is_stopped()) {
	_state = State::stopped;
      }
      else if (is_done() && !master_notified) {

	// actual work done:
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... worker " << _workerID << " done - notifying master" << std::endl;
	}
	_callerptr->_main_condition.notify_one();
	master_notified = true;
      }

      // no work - wait:
      else if (is_idle()) {
	std::unique_lock<std::mutex> main_lock(_callerptr->_main_waitmutex);
	worker_condition.wait(main_lock, [this] { return wake_up(); });
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "worker " << _workerID << " waking up ..." << std::endl;
	}
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "worker " << _workerID << " terminating" << std::endl;
    }
  }

  void SymmetricExtensionGraphMaster::_collect_results() {
    if (!_exists_pending_result()) {
      return;
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      if (_workers[i].is_done()) {
	_nodecount                += _workers[i].nodecount();
	_totalcount               += _workers[i].totalcount();
	_symcount                 += _workers[i].symcount();
	_deadendcount             += _workers[i].deadendcount();
	_earlydeadendcount        += _workers[i].earlydeadendcount();
	if (_workers[i].maxiter_coversimptighten() > _maxiter_coversimptighten) {
	  _maxiter_coversimptighten = _workers[i].maxiter_coversimptighten();
	}
#ifdef COMPUTATIONS_DEBUG
	{
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... moving remaning work into queue ..." << std::endl;
	}
#endif
	std::move(_workers[i].open_nodes().begin(),
		  _workers[i].open_nodes().end(),
		  std::back_inserter<std::deque<node_type> >(_open_nodes));
#ifdef COMPUTATIONS_DEBUG
	{
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... done" << std::endl;
	}
#endif
	_workers[i].clear_results();
      }
    }
    _current_workbuffersize = _open_nodes.size();
    update_node_budget();
    if (CommandlineOptions::verbose()) {
      report_progress(std::cerr);
    }
  }

  void SymmetricExtensionGraphMaster::_run() {
    if (CommandlineOptions::parallel_enumeration()) {
      // main loop of the master thread:
      SimplicialComplex::start_multithreading();
      set_no_of_idle_threads(_no_of_threads);
      set_no_of_pending_results(0);
      _open_nodes.push_back(*_rootptr);
      while (
	     !_open_nodes.empty() // need to distribute work
	     || !_all_threads_idle() // need to wait for results
	     || _exists_pending_result() // need to collect results
	     ) {
	_collect_results();
	if (_exists_idle_thread() && !_open_nodes.empty()) {

	  // distribute work:
	  for (int i = 0; i < _no_of_threads; ++i) {
	    if (_workers[i].is_idle()) {
	      const node_type* next_nodeptr = new node_type(std::move(_open_nodes.front()));
	      _open_nodes.pop_front();
	      _workers[i].pass_work(next_nodeptr);
	    }
	    if (_open_nodes.empty()) {
	      break;
	    }
	  }

	  // notify all workers with assigned work:
	  for (int i = 0; i < _no_of_threads; ++i) {
	    if (_workers[i].is_hired()) {
	      _workers[i].worker_condition.notify_one();
	    }
	  }
	}
	else {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "master is waiting for results || work & idle_thread || termination ..." << std::endl;
	  }
	  std::unique_lock<std::mutex> main_lock(_main_waitmutex);
	  _main_condition.wait(main_lock, [this] { return wake_up(); } );
	}
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "work completed - master is terminating itself" << std::endl;
      }
      SimplicialComplex::stop_multithreading();
    }
    else {

      // infinite node budget - one instance of SymmetricExtensionGraph
      // does the whole enumeration in one go:
      _node_budget = std::numeric_limits<size_type>::max();
      _open_nodes.push_back(*_rootptr);
      while (!_open_nodes.empty()) {
	const node_type next_node(std::move(_open_nodes.front()));
	SymmetricExtensionGraph seg(0,
				    0,
				    _symcount,
				    _no,
				    _rank,
				    _pointsptr,
				    _chiroptr,
				    _symmetriesptr,
				    _simpidx_symmetriesptr,
				    _classified_symmetriesptr,
				    _switch_tableptr,
				    _admtableptr,
				    _inctableptr,
				    _voltableptr,
				    &next_node,
				    _mincard,
				    _mintriang,
				    _open_nodes,
				    _print_triangs,
				    _only_fine_triangs,
				    _find_minimal_triang,
				    _node_budget,
				    &_current_workbuffersize);
	_nodecount                += seg.nodecount();
	_totalcount               += seg.totalcount();
	_symcount                 += seg.symcount();
	_deadendcount             += seg.deadendcount();
	_earlydeadendcount        += seg.earlydeadendcount();
	if (seg.maxiter_coversimptighten() > _maxiter_coversimptighten) {
	  _maxiter_coversimptighten = seg.maxiter_coversimptighten();
	}
	_open_nodes.pop_front();   
	update_node_budget();
	if (CommandlineOptions::verbose()) {
	  report_progress(std::cerr);
	}
      }
    }
  }

  void SymmetricExtensionGraphMaster::_init() {
    if (!_pointsptr->empty() && CommandlineOptions::use_volumes()) {
      
      // only in these cases, the volume table can be computed:
      if (CommandlineOptions::verbose()) {
	std::cerr << "computing volumes table ..." << std::endl;
      }
      _voltableptr = new Volumes(*_pointsptr, _only_fine_triangs);
      _volume = _pointsptr->volume();
      if (CommandlineOptions::verbose()) {
	std::cerr << "... done." << std::endl;
      }
    }
    if (_voltableptr) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "preprocessing simplex table sorted lexicographically for all full-dimensional simplices up to rank "
		  << std::min<parameter_type>(_no, _rank + 1) << " ..." << std::endl;
      }
      SimplicialComplex::reset_index_table();
      SimplicialComplex::preprocess_index_table(_no, 0, std::min<parameter_type>(_no, _rank + 1), *_voltableptr, true);
    }
    else {
      if (CommandlineOptions::use_random_order()) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "preprocessing simplex table sorted randomly for all simplices up to rank "
		    << _rank + 1 << " ..." << std::endl;
	}
      }
      else {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "preprocessing simplex table sorted lexicographically for all full-dimensional simplices up to rank "
		    << std::min<parameter_type>(_no, _rank + 1) << " ..." << std::endl;
	}
      }
      SimplicialComplex::reset_index_table();
      SimplicialComplex::preprocess_index_table(_no, 0, std::min<parameter_type>(_no, _rank + 1), *_chiroptr, true);
    }
    if (!CommandlineOptions::use_volumes()) {

      // volumes were only used to sort preprocessing simplex table; dump the volumes now:
      if (_voltableptr) {
	delete _voltableptr;
	_voltableptr = 0;
      }
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
      std::cerr << "computing circuits ..." << std::endl;
    }
    Circuits* circuitsptr = new Circuits(*_chiroptr);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
      std::cerr << "computing admissibles ..." << std::endl;
    }
    _admtableptr = new Admissibles(*circuitsptr, *_pointsptr, *_chiroptr, _only_fine_triangs);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
    Vertices* verticesptr = 0;
    if (CommandlineOptions::output_asy() && (_rank == 3)) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "computing vertices for graphics output ..." << std::endl;
      }
      verticesptr = new Vertices(*circuitsptr);
      if (CommandlineOptions::verbose()) {
	std::cerr << "... done." << std::endl;
      }
    }
    delete circuitsptr;
    if (CommandlineOptions::simpidx_symmetries()) {
      
      // preprocess the permutations describing the action of the symmetry group
      // on rank-subsets indexed in SimplicialComplex:
      if (CommandlineOptions::verbose()) {
	std::cerr << "computing simplex-index symmetries ..." << std::endl;
      }
      if (CommandlineOptions::use_switch_tables()) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "computing symmetry generators in terms of simplex indices:" << std::endl;
	}
	symmetry_collectordata simpidx_generators;
	for (symmetry_collectordata::const_iterator geniter = _symmetriesptr->generators().begin();
	     geniter != _symmetriesptr->generators().end();
	     ++geniter) {
	  simpidx_generators.insert(geniter->simpidx_symmetry(_rank));
	}
	if (CommandlineOptions::verbose()) {
	  std::cerr << "switch tables directly from generators ..." << std::endl;
	}
	_switch_tableptr = new SwitchTable<lexmin_mode>(SimplicialComplex::no_of_simplices(_rank), simpidx_generators);
	if (CommandlineOptions::verbose()) {
	  std::cerr << "... done." << std::endl;
	}
      }
      else {
	_simpidx_symmetriesptr = new SymmetryGroup(_symmetriesptr->simpidx_symmetries(_rank));
	if (CommandlineOptions::debug()) {
	  std::cerr << "symmetries in terms of simplex indices:" << std::endl;
	  std::cerr << *_simpidx_symmetriesptr << std::endl;
	}
	if (CommandlineOptions::verbose()) {
	  std::cerr << "... done." << std::endl;
	}
	if (CommandlineOptions::use_classified_symmetries()) {
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "computing classified symmetries ..." << std::endl;
	  }
	  if (CommandlineOptions::parallel_symmetries()) {
	    _classified_symmetriesptr = new ClassifiedExtensionSymmetries(ClassifiedExtensionSymmetries::simpidx_mode,
									  CommandlineOptions::no_of_threads(),
									  _admtableptr->simplices(),
									  *_simpidx_symmetriesptr);
	  }
	  else {
	    _classified_symmetriesptr = new ClassifiedExtensionSymmetries(ClassifiedExtensionSymmetries::simpidx_mode,
									  1,
									  _admtableptr->simplices(),
									  *_simpidx_symmetriesptr);
	  }
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "... done." << std::endl;
	  }
	}
      }
    }
    else {
      if (CommandlineOptions::verbose()) {
	std::cerr << "no preprocessing of simplex-index symmetries and switch tables ..." << std::endl;
      }
      if (CommandlineOptions::use_classified_symmetries()) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "computing classified symmetries ..." << std::endl;
	}
	if (CommandlineOptions::parallel_symmetries()) {
	  _classified_symmetriesptr = new ClassifiedExtensionSymmetries(ClassifiedExtensionSymmetries::pointidx_mode,
									CommandlineOptions::no_of_threads(),
									_admtableptr->simplices(),
									*_symmetriesptr);
	}
	else {
	  _classified_symmetriesptr = new ClassifiedExtensionSymmetries(ClassifiedExtensionSymmetries::pointidx_mode,
									1,
									_admtableptr->simplices(),
									*_symmetriesptr);
	}
	if (CommandlineOptions::verbose()) {
	  std::cerr << "... done." << std::endl;
	}
      }
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "computing positive cocircuits ..." << std::endl;
    }
    Cocircuits* cocircuitsptr = new Cocircuits(*_chiroptr, true);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "computing facets ..." << std::endl;
    }
    Facets* facetsptr = new Facets(*cocircuitsptr);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
    delete cocircuitsptr;
    if (CommandlineOptions::verbose()) {
      std::cerr << "computing incidences ..." << std::endl;
    }
    _inctableptr = new Incidences(*_chiroptr, *facetsptr);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }

    // initialize the statistics file, if requested:
    if (CommandlineOptions::output_stats()) {
      Statistics::init();
    }
    
    // initialize the asymptote file, if requested:
    if (CommandlineOptions::output_asy()) {
      Graphics::init_asy(CommandlineOptions::asy_file());
      Graphics::typedef_for_partialtriang_to_asy();
      Graphics::header_to_asy(_no, _rank);
      if (_rank == 3) {
	Graphics::vertices_to_asy(*verticesptr);
	Graphics::points_to_asy(*_pointsptr);
	Graphics::nodesize_for_pointsgraphics_to_asy();
      }
      delete verticesptr;
    }
    delete facetsptr;

    // by now, we have seen all simplices that might be occurring during the exploration:
    // SimplicialComplex::set_preprocessed(true);
    if (CommandlineOptions::parallel_enumeration()) {
      _no_of_threads = CommandlineOptions::no_of_threads();
      _init_workers();
    }
    else {

      // we need to generate a single worker for asymptote:
      if (CommandlineOptions::output_asy()) {
	Graphics::worker_to_asy();
      }
    }

    // from here on we need to block IO:
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "generating root node with some preprocessing of symmetries ..." << std::endl;
    }
    const PartialTriang root_partial_triang(_no, _rank, *_admtableptr, *_inctableptr, _voltableptr);
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "exploring all symmetry classes of triangulations by extension ..." << std::endl;
    }

    // generate the root node
    if (!_simpidx_symmetriesptr) {
      // here, we use a representation of the symmetry group as a
      // permutation group on the vertices of simplicial complexes:
      _rootptr = new node_type(_symmetriesptr, std::move(root_partial_triang));

      // initialize the simplex index tables according to command line options:
      SymmetricExtensionGraphNode::init_simpidx_table(_symmetriesptr, SimplicialComplex::no_of_simplices(_rank));
    }
    else {
      // here, we use a representation of the symmetry group as a
      // permutation group on simplex index sets representing simplicial complexes:
      _rootptr = new node_type(_simpidx_symmetriesptr, std::move(root_partial_triang));

      // initialize the simplex index tables according to command line options:
      SymmetricExtensionGraphNode::init_simpidx_table(_simpidx_symmetriesptr, SimplicialComplex::no_of_simplices(_rank));
    }

    
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "SymmetricExtensionGraphMaster:" << std::endl;
      std::cerr << "root triangulation: " << *_rootptr << std::endl;
      std::cerr << "no                : " << _rootptr->partial_triang().no() << std::endl;
      std::cerr << "rank              : " << _rootptr->partial_triang().rank() << std::endl;
      std::cerr << "freeintfacets     : " << _rootptr->partial_triang().freeintfacets() << std::endl;
      std::cerr << "admissibles       : " << _rootptr->partial_triang().admissibles() << std::endl;
      std::cerr << "admissibles table : " << *(_rootptr->partial_triang().admtableptr()) << std::endl;
      std::cerr << "incidences        : " << *(_rootptr->partial_triang().inctableptr()) << std::endl;
      if (_rootptr->partial_triang().voltableptr()) {
	std::cerr << "volumes           : " << *(_rootptr->partial_triang().voltableptr()) << std::endl;
      }
    }
  }

  void SymmetricExtensionGraphMaster::_init_workers() {
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "init distributed workers ..." << std::endl;
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      if (CommandlineOptions::verbose()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "initializing worker " << i << " ..." << std::endl;
      }
      _workers.emplace_back(i, *this);
      if (CommandlineOptions::output_asy()) {
	Graphics::worker_to_asy();
      }

      if (CommandlineOptions::debug()) {    
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done" << std::endl;
      }
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      if (CommandlineOptions::verbose()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "starting worker thread " << i 
		  << " ..." << std::endl;
      }
      _threads.push_back(std::thread(&SymmetricExtensionGraphMaster::Worker::operator(),
				     std::ref(_workers[i])));
      if (CommandlineOptions::verbose()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done" << std::endl;
      }
    }

    // from now on we need to block IO:
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done" << std::endl;
    }
  }

  void SymmetricExtensionGraphMaster::_term() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "SymmetricExtensionGraphMaster::~SymmetricExtensionGraphMaster():" << std::endl;
    std::cerr << "destructing owned pointers ..." << std::endl;
#endif
    if (CommandlineOptions::parallel_enumeration()) {
      _term_workers();
    }

    // finish the asymptote file, if necessary:
    if (CommandlineOptions::output_asy()) {
      Graphics::nodepos_to_asy();
      Graphics::define_draw_node_for_partialtriang_to_asy();
      Graphics::drawings_to_asy();
      Graphics::term_asy();
    }

    // finish the statistics file, if necessary:
    if (CommandlineOptions::output_stats()) {
      Statistics::term();
    }

    // destroy all objects that have been generated by this class:
    if (_rootptr) {
      delete _rootptr;
    }
    if (_inctableptr) {
      delete _inctableptr;
    }
    if (_voltableptr) {
      delete _voltableptr;
    }
    if (_classified_symmetriesptr) {
      delete _classified_symmetriesptr;
    }
    if (_admtableptr) {
      delete _admtableptr;
    }
    if (_simpidx_symmetriesptr) {
      delete _simpidx_symmetriesptr;
    }
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "... done" << std::endl;
#endif
  }

  void SymmetricExtensionGraphMaster::_term_workers() {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "work completed - master is stopping all workers ..." << std::endl;
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      _workers[i].stop_worker();
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "work completed - master is waking up all workers for termination ..." << std::endl;
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      _workers[i].worker_condition.notify_one();
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... joining threads" << std::endl;
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      if (_threads[i].joinable()) {
	_threads[i].join();
      }
    }
  }

}; // namespace topcom

// eof SymmetricExtensionGraphMaster.cc
  
