//////////////////////////////////////////////////////////////////////////
// SymmetricSubsetGraphMasterMaster.hh
// produced: 29/03/2020 jr
// last change: 29/03/2020 jr
/////////////////////////////////////////////////////////////////////////
#ifndef SYMMETRICSUBSETGRAPHMASTER_HH
#define SYMMETRICSUBSETGRAPHMASTER_HH

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <assert.h>
#include <deque>
#include <vector>
#include <set>
#include <map>

#include <thread>
#include <mutex>
// #include <shared_mutex>
#include <atomic>
#include <condition_variable>

#include "Global.hh"
#include "CommandlineOptions.hh"
#include "Statistics.hh"
#include "Graphics.hh"
#include "StrictStairCaseMatrixTrans.hh"
#include "LabelSet.hh"
#include "PointConfiguration.hh"
#include "Chirotope.hh"
#include "Symmetry.hh"
#include "ClassifiedSubsetSymmetries.hh"
#include "SymmetricSubsetGraph.hh"

namespace topcom {

  template <ssg_mode_type mode>
  class SymmetricSubsetGraphMaster;

  template <ssg_mode_type mode>
  class SymmetricSubsetGraphMaster {
  public:
    typedef typename SymmetricSubsetGraph<mode>::node_type node_type;
  private:
    class Worker {
    private:
      // context information:
      const int                            _workerID;
      const SymmetricSubsetGraphMaster*    _callerptr;
      const node_type*                     _root_nodeptr;
      // results:
      size_type                            _totalcount;
      size_type                            _symcount;
      size_type                            _nodecount;
      size_type                            _deadendcount;
      size_type                            _earlydeadendcount;
      std::deque<node_type>                _open_nodes;
      // statistics:
      size_type                            _no_of_runs;
      // multi-threading:
      enum State { idle, hired, busy, done, stopped };
      State                                _state;
      bool                                 _worker_stopped;
    public:
      mutable std::condition_variable      worker_condition;
    private:
      Worker();
    public:
      Worker(const int,
	     const SymmetricSubsetGraphMaster&);
      Worker(const Worker&);
    private:
      const Worker& operator=(const Worker&);
    public:
      // accessors:
      inline const int                    workerID()          const { return _workerID; }
      inline const size_type              totalcount()        const { return _totalcount; }
      inline const size_type              symcount()          const { return _symcount; }
      inline const size_type              nodecount()         const { return _nodecount; }
      inline const size_type              deadendcount()      const { return _deadendcount; }
      inline const size_type              earlydeadendcount() const { return _earlydeadendcount; }
      // accessor to the no of runs:
      inline const size_type              no_of_runs()        const { return _no_of_runs; }
      inline const std::deque<node_type>& open_nodes()        const { return _open_nodes; }
      // accessors for multi-threading control:
      inline const State                  get_state()         const {
	std::lock_guard<std::mutex> main_lock(_callerptr->_main_waitmutex);
	return _state;
      }
      inline const bool                   is_idle()           const { return (get_state() == State::idle); }
      inline const bool                   is_hired()          const { return (get_state() == State::hired); }
      inline const bool                   is_busy()           const { return (get_state() == State::busy); }
      inline const bool                   is_done()           const { return (get_state() == State::done); }
      inline const bool                   is_stopped()        const { return (get_state() == State::stopped); }
      // modifiers for multi-threading control:
      inline void                         set_state(const State state) {
	std::lock_guard<std::mutex> main_lock(_callerptr->_main_waitmutex);
	_state = state;
      }
      inline void                         set_idle()                { set_state(State::idle); }
      inline void                         set_hired()               { set_state(State::hired); }
      inline void                         set_busy()                { set_state(State::busy); }
      inline void                         set_done()                { set_state(State::done); }
      inline void                         set_stopped()             { set_state(State::stopped); }
      inline const bool                   wake_up()           const;
      inline void                         stop_worker();
      inline void                         pass_work(const node_type*);
      inline void                         clear_results();
      inline void                         run();
      void                                operator()();
    };
  private:
    const parameter_type                 _no;
    const parameter_type                 _rank;
    const PointConfiguration*            _pointsptr;
    const Chirotope*                     _chiroptr;
    const SymmetryGroup*                 _symmetriesptr;
    const ClassifiedSubsetSymmetries*    _classified_symmetriesptr;
    const SwitchTable<LabelSet,
		      colexmax_mode>*    _switch_tableptr;
    node_type*                           _root_nodeptr;
    bool                                 _print_objects;
    bool                                 _save_objects;
  private:
    size_type                            _node_budget;
  private:
    size_type                            _totalcount;
    size_type                            _symcount;
    size_type                            _nodecount;
    size_type                            _deadendcount;
    size_type                            _earlydeadendcount;
    std::deque<node_type>                _open_nodes;
    size_type                            _current_workbuffersize;
    // this container is for cocircuit enumeration via independent bases:
    mutable ssg_doneset_type             _result_set;
  private:
    // multi-threading:
    int                                  _no_of_threads;
    std::vector<std::thread>             _threads;
    std::vector<Worker>                  _workers;
    mutable int                          _no_of_idle_threads;
    mutable int                          _no_of_pending_results;
    // mutable std::shared_mutex            _main_datamutex;
    mutable std::mutex                   _main_waitmutex;
    mutable std::condition_variable      _main_condition;
    friend class SymmetricSubsetGraphMaster::Worker;

  private:
    SymmetricSubsetGraphMaster();
    SymmetricSubsetGraphMaster(const SymmetricSubsetGraphMaster&);
    SymmetricSubsetGraphMaster& operator=(const SymmetricSubsetGraphMaster&);
  public:
    // constructor:
    inline SymmetricSubsetGraphMaster(const parameter_type, 
				      const parameter_type,
				      const PointConfiguration&,
				      const SymmetryGroup&,
				      const bool,
				      const bool);
    // destructor:
    inline ~SymmetricSubsetGraphMaster();
    // accessors:
    inline const int get_no_of_idle_threads() const {
      std::lock_guard<std::mutex> main_lock(_main_waitmutex);
      return _no_of_idle_threads;
    }
    inline const int get_no_of_pending_results() const {
      std::lock_guard<std::mutex> main_lock(_main_waitmutex);
      return _no_of_pending_results;
    }
    // modifiers:
    inline void set_no_of_idle_threads(const int i) const {
      std::lock_guard<std::mutex> main_lock(_main_waitmutex);
      _no_of_idle_threads = i;
    }
    inline void set_no_of_pending_results(const int i) const {
      std::lock_guard<std::mutex> main_lock(_main_waitmutex);
      _no_of_pending_results = i;
    }
    inline void increase_no_of_idle_threads() const {
      std::lock_guard<std::mutex> main_lock(_main_waitmutex);
      ++_no_of_idle_threads;
    }
    inline void increase_no_of_pending_results() const {
      std::lock_guard<std::mutex> main_lock(_main_waitmutex);
      ++_no_of_pending_results;
    }
    inline void decrease_no_of_idle_threads() const {
      std::lock_guard<std::mutex> main_lock(_main_waitmutex);
      --_no_of_idle_threads;
    }
    inline void decrease_no_of_pending_results() const {
      std::lock_guard<std::mutex> main_lock(_main_waitmutex);
      --_no_of_pending_results;
    }
    // adapting to workload:
    inline void update_node_budget();
    // reporting:
    inline void report_progress(std::ostream&) const;
    // results:
    inline const size_type totalcount()               const { return _totalcount; }
    inline const size_type symcount()                 const { return _symcount; }
    inline const size_type nodecount()                const { return _nodecount; }
    inline const size_type deadendcount()             const { return _deadendcount; }
    inline const size_type earlydeadendcount()        const { return _earlydeadendcount; }
    inline const ssg_doneset_type resultset()         const { return _result_set; }
    // multi-threading control:
    inline const bool wake_up() const;
    // internal methods:
    inline const bool _exists_idle_thread() const;
    inline const bool _exists_pending_result() const;
    inline const bool _all_threads_idle() const;
    inline void _collect_results();
    void _run();
    void _init();
    void _init_workers();
    // stream output/input:
  };

  // inlines for worker subclass:
  template <ssg_mode_type mode>
  SymmetricSubsetGraphMaster<mode>::Worker::Worker(const int                               workerID,
						   const SymmetricSubsetGraphMaster<mode>& ssgm) :
    _workerID(workerID),
    _callerptr(&ssgm),
    _root_nodeptr(0),
    _symcount(0UL),
    _totalcount(0UL),
    _nodecount(0UL),
    _deadendcount(0UL),
    _earlydeadendcount(0UL),
    _no_of_runs(0),
    _open_nodes(),
    _state(idle),
    _worker_stopped(false) {
  }

  template <ssg_mode_type mode>
  SymmetricSubsetGraphMaster<mode>::Worker::Worker(const Worker& sw) :
    _workerID(sw._workerID),
    _callerptr(sw._callerptr),
    _root_nodeptr(sw._root_nodeptr),
    _symcount(sw._symcount),
    _totalcount(sw._totalcount),
    _nodecount(sw._nodecount),
    _deadendcount(sw._deadendcount),
    _earlydeadendcount(sw._earlydeadendcount),
    _no_of_runs(sw._no_of_runs),
    _open_nodes(sw._open_nodes),
    _state(idle),
    _worker_stopped(false) {
  }


  // shared data: _worker_stopped (read)
  // callers: all worker threads
  template <ssg_mode_type mode>
  inline const bool SymmetricSubsetGraphMaster<mode>::Worker::wake_up() const {

    // since this function is called inside the condition variable predicate,
    // we need to access the shared data lock-free (the lock is already held by the wait):
    return ((_state == State::hired) || (_state == State::stopped));
  }

  // shared data: _state, _callerptr->_no_of_idle_threads (write)
  // callers: master thread only
  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraphMaster<mode>::Worker::pass_work(const SymmetricSubsetGraphMaster<mode>::node_type* nodeptr) {
    _callerptr->decrease_no_of_idle_threads();
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "worker " << _workerID << " is assigned to work on "
		<< *nodeptr << " ..." << std::endl;
    }
    _root_nodeptr = nodeptr;
    set_hired();
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done" << std::endl;
    }
  }

  // shared data: _state, _callerptr->_no_of_pending_results (write)
  // shared const data: most of _callerptr->...
  // callers: all worker threads
  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraphMaster<mode>::Worker::run() {
    set_busy();
    SymmetricSubsetGraph<mode> ssg(_workerID,
				   _no_of_runs,
				   _callerptr->_no,
				   _callerptr->_rank,
				   _callerptr->_pointsptr,
				   _callerptr->_symmetriesptr,
				   _callerptr->_classified_symmetriesptr,
				   _callerptr->_switch_tableptr,
				   _root_nodeptr,
				   _open_nodes,
				   _callerptr->_result_set,
				   _callerptr->_print_objects,
				   _callerptr->_save_objects,
				   _callerptr->_node_budget,
				   &_callerptr->_current_workbuffersize);
    _nodecount                = ssg.nodecount();
    _totalcount               = ssg.totalcount();
    _symcount                 = ssg.symcount();
    _deadendcount             = ssg.deadendcount();
    _earlydeadendcount        = ssg.earlydeadendcount();
#ifdef COMPUTATIONS_DEBUG
    std::cerr << "... copying remaning work into worker open queue ..." << std::endl;
#endif
#ifdef COMPUTATIONS_DEBUG
    std::cerr << "... done" << std::endl;
#endif
    ++_no_of_runs;
    set_done();
    _callerptr->increase_no_of_pending_results();
  }

  // shared data: _state, _callerptr->_no_of_pending_results, _callerptr->_no_of_idle_threads (write)
  // callers: master thread only
  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraphMaster<mode>::Worker::clear_results() {
    if (_root_nodeptr) {
      delete(_root_nodeptr);
    }
    _root_nodeptr            = 0;
    _symcount                = 0UL;
    _totalcount              = 0UL;
    _nodecount               = 0UL;
    _deadendcount            = 0UL;
    _earlydeadendcount       = 0UL;
    _open_nodes.clear();
    _callerptr->decrease_no_of_pending_results();
    set_idle();
    _callerptr->increase_no_of_idle_threads();
  }
  
  // shared data: _worker_stopped (write)
  // callers: main thread only
  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraphMaster<mode>::Worker::stop_worker() {
    set_stopped();
  }

  // shared data: _worker_stopped (read)
  // callers: all worker threads
  template <ssg_mode_type mode>
  void SymmetricSubsetGraphMaster<mode>::Worker::operator()() {
    
    // main loop of worker thread:
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "worker " << _workerID << " is spawned" << std::endl;
    }

    // do not notify many times in a row:
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
	while (!wake_up()) {
	  std::unique_lock<std::mutex> main_lock(_callerptr->_main_waitmutex);
	  worker_condition.wait(main_lock, [this] { return wake_up(); });
	}
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

  // inlines for the main class:
  template <ssg_mode_type mode>
  inline SymmetricSubsetGraphMaster<mode>::SymmetricSubsetGraphMaster(const parameter_type no, 
								      const parameter_type rank,
								      const PointConfiguration& points,
								      const SymmetryGroup& symmetries,
								      const bool print_objects,
								      const bool save_objects) :
    _no(no),
    _rank(rank),
    _pointsptr(&points),
    _symmetriesptr(&symmetries),
    _classified_symmetriesptr(0),
    _switch_tableptr(0),
    _root_nodeptr(0),
    _print_objects(print_objects),
    _save_objects(save_objects),
    _node_budget(1),
    _totalcount(0),
    _symcount(0),
    _nodecount(0),
    _deadendcount(0),
    _earlydeadendcount(0),
    _open_nodes(),
    _current_workbuffersize(0UL),
    _result_set(),
    _no_of_threads(1) {
    _init();
    _run();
  }

  template <ssg_mode_type mode>
  inline SymmetricSubsetGraphMaster<mode>::~SymmetricSubsetGraphMaster() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "SymmetricSubsetGraphMaster<mode>::~SymmetricSubsetGraphMaster():" << std::endl;
    std::cerr << "destructing owned pointers ..." << std::endl;
#endif
    if (CommandlineOptions::parallel_enumeration()) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "work completed - master is stopping all workers ..." << std::endl;
      }
      for (int i = 0; i < _no_of_threads; ++i) {
	_workers[i].stop_worker();
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "master is waking up all workers for termination ..." << std::endl;
      }
      for (int i = 0; i < _no_of_threads; ++i) {
	_workers[i].worker_condition.notify_one();
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "master is waiting for threads to join ..." << std::endl;
      }
      for (int i = 0; i < _no_of_threads; ++i) {
	if (_threads[i].joinable()) {
	  _threads[i].join();
	}
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done" << std::endl;
      }
    }

    // finish the output, if necessary:
    if (_print_objects) {
      std::cout << "}" << std::endl;
    }

    
    // finish the asymptote file, if necessary:
    if (CommandlineOptions::output_asy()) {
      Graphics::nodepos_to_asy();
      Graphics::define_draw_node_for_matrix_to_asy();
      Graphics::drawings_to_asy();
      Graphics::term_asy();
    }

    // finish the statistics file, if necessary:
    if (CommandlineOptions::output_stats()) {
      Statistics::term();
    }

    // destroy all objects that have been generated by this class:
    if (_root_nodeptr) {
      delete _root_nodeptr;
    }
    if (_switch_tableptr) {
      delete _switch_tableptr;
    }
    if (_classified_symmetriesptr) {
      delete _classified_symmetriesptr;
    }
    std::cerr << "... done" << std::endl;
  }

  // budgeting:
  // shared data: none
  // callers: master thread only
  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraphMaster<mode>::update_node_budget() {
    _node_budget = std::max<size_type>(1, _open_nodes.size() * CommandlineOptions::scale_nodebudget() / 100);
    if (_node_budget < CommandlineOptions::min_nodebudget()) {
      _node_budget = CommandlineOptions::min_nodebudget();
    }
    else if (_node_budget > CommandlineOptions::max_nodebudget()) {
      _node_budget = CommandlineOptions::max_nodebudget();
    }
  }

  // reporting:
  // shared data: none
  // callers: master thread only
  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraphMaster<mode>::report_progress(std::ostream& ost) const {
    static int count = CommandlineOptions::report_frequency();
    static size_type old_nodecount(0UL);
  
    count -= (this->_nodecount - old_nodecount);
    if (count < 0) {
      const size_type no_of_open_nodes(_open_nodes.size());
      count = CommandlineOptions::report_frequency();
      {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	ost << "*** master ***| n: " << std::setw(15) << this->_nodecount 
	    << " | ldead: " << std::setw(15) << this->_deadendcount
	    << " | edead: " << std::setw(15) << this->_earlydeadendcount
	    << " | sym: " << std::setw(12) << this->_symcount;
	if (!CommandlineOptions::skip_orbitcount()) {
	  ost << " | tot: " << std::setw(15) << this->_totalcount;
	}
	if (CommandlineOptions::parallel_enumeration()) {
	  if (CommandlineOptions::workbuffercontrol()) {
	    ost << " | n open: " << std::setw(6) << _open_nodes.size()
		<< " | min workbuffer: " << std::setw(6) << CommandlineOptions::min_workbuffersize()
		<< " | t idle: " << std::setw(4) << this->get_no_of_idle_threads();
	  }
	  else {
	    ost << " | n open: " << std::setw(9) << _open_nodes.size()
		<< " | n budget: " << std::setw(9) << this->_node_budget
		<< " | t idle: " << std::setw(4) << this->get_no_of_idle_threads();
	  }
	}
	std::cerr << std::endl;
      }
    }
    old_nodecount = this->_nodecount;
  }

  // shared data: none
  // callers: master thread only
  template <ssg_mode_type mode>
  inline const bool SymmetricSubsetGraphMaster<mode>::wake_up() const {

    // since this function is called inside the condition variable predicate,
    // we need to access the shared data lock-free (the lock is already held by the wait):
    return (_no_of_pending_results > 0) // need to collect results
      || (!_open_nodes.empty() && (_no_of_idle_threads > 0)) // can distribute work
      || (_open_nodes.empty() && (_no_of_idle_threads == _no_of_threads)); // need to terminate
  }

  // shared data: _no_of_idle_threads
  // caller: master thread only
  template <ssg_mode_type mode>
  inline const bool SymmetricSubsetGraphMaster<mode>::_exists_idle_thread() const {
    return (get_no_of_idle_threads() > 0);
  }

  // shared data: _no_of_pending_results
  // caller: master thread only
  template <ssg_mode_type mode>
  inline const bool SymmetricSubsetGraphMaster<mode>::_exists_pending_result() const {
    return (get_no_of_pending_results() > 0);
  }

  // shared data: _no_of_idle_threads
  // caller: master thread only
  template <ssg_mode_type mode>
  inline const bool SymmetricSubsetGraphMaster<mode>::_all_threads_idle() const {
    return (get_no_of_idle_threads() >= _no_of_threads);
  }

  // shared data: _no_of_pending_results
  // callers: master thread only
  template <ssg_mode_type mode>
  void SymmetricSubsetGraphMaster<mode>::_collect_results() {
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
#ifdef COMPUTATIONS_DEBUG
	std::cerr << "... copying remaning work into queue ..." << std::endl;
#endif
	std::copy(_workers[i].open_nodes().begin(),
		  _workers[i].open_nodes().end(),
		  std::back_inserter<std::deque<node_type> >(_open_nodes));
	_current_workbuffersize = _open_nodes.size();
#ifdef COMPUTATIONS_DEBUG
	std::cerr << "... done" << std::endl;
#endif
	_workers[i].clear_results();
      }
    }
    update_node_budget();
    if (CommandlineOptions::verbose()) {
      report_progress(std::cerr);
    }
  }

  // shared data: _no_of_idle_threads, _no_of_pending_results
  // callers: master thread only
  template <ssg_mode_type mode>
  void SymmetricSubsetGraphMaster<mode>::_run() {
    if (CommandlineOptions::parallel_enumeration()) {
      
      // main loop of the master thread:
      set_no_of_idle_threads(_no_of_threads);
      set_no_of_pending_results(0);
      _open_nodes.push_back(*_root_nodeptr);
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
	      const node_type* next_nodeptr = new node_type(_open_nodes.front());
	      _open_nodes.pop_front();
	      _workers[i].pass_work(next_nodeptr);
	      if (_open_nodes.empty()) {
		break;
	      }
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
	  while (!wake_up()) {
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "master is waiting for results || work & idle_thread || termination ..." << std::endl;
	    }
	    std::unique_lock<std::mutex> main_lock(_main_waitmutex);
	    _main_condition.wait(main_lock, [this] { return wake_up(); } );
	  }
	}
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "work completed - master is terminating itself" << std::endl;
      }
    }
    else {
      
      // infinite node budget - one instance of SymmetricSubsetGraph
      // does the whole enumeration in one go for dfs:
      _node_budget = std::numeric_limits<size_type>::max();
      _open_nodes.push_back(*_root_nodeptr);
      while (!_open_nodes.empty()) {
	const node_type& next_node(_open_nodes.front());
	SymmetricSubsetGraph<mode> ssg(0,
				       0,
				       _no,
				       _rank,
				       _pointsptr,
				       _symmetriesptr,
				       _classified_symmetriesptr,
				       _switch_tableptr,
				       &next_node,
				       _open_nodes,
				       _result_set,
				       _print_objects,
				       _save_objects,
				       _node_budget,
				       &_current_workbuffersize);
	_nodecount                += ssg.nodecount();
	_totalcount               += ssg.totalcount();
	_symcount                 += ssg.symcount();
	_deadendcount             += ssg.deadendcount();
	_earlydeadendcount        += ssg.earlydeadendcount();
	_open_nodes.pop_front();
	_current_workbuffersize = _open_nodes.size();
	update_node_budget();
	if (CommandlineOptions::verbose()) {
	  report_progress(std::cerr);
	}
      }
    }
  }

  template <ssg_mode_type mode>
  void SymmetricSubsetGraphMaster<mode>::_init() {
    if (CommandlineOptions::use_classified_symmetries()) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "computing classified symmetries ..." << std::endl;
      }
      if (CommandlineOptions::parallel_symmetries()) {
	_classified_symmetriesptr = new ClassifiedSubsetSymmetries(std::min<int>(_no, CommandlineOptions::no_of_symtables()),
								   std::max<int>(CommandlineOptions::no_of_threads(), 1),
								   *_symmetriesptr);
      }
      else {
	_classified_symmetriesptr = new ClassifiedSubsetSymmetries(CommandlineOptions::no_of_symtables(),
								   std::min<int>(_no, CommandlineOptions::no_of_symtables()),
								   *_symmetriesptr);
      }
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
    if (CommandlineOptions::use_switch_tables()) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "computing switch tables ..." << std::endl;
      }
      // _switch_tableptr = new SwitchTable<colexmax_mode>(*_symmetriesptr);
      _switch_tableptr = new SwitchTable<LabelSet, colexmax_mode>(_symmetriesptr->n(), _symmetriesptr->generators());
      if (CommandlineOptions::verbose()) {
	std::cerr << "... done." << std::endl;
      }
    }
    
    // initialize the statistics file, if requested:
    if (CommandlineOptions::output_stats()) {
      Statistics::init();
    }
    
    // initialize the asymptote file, if requested and possible:
    if (CommandlineOptions::output_asy()) {
      Graphics::init_asy(CommandlineOptions::asy_file());
      Graphics::typedef_for_matrix_to_asy();
      Graphics::header_to_asy(_no, _rank);
      if (_rank == 3) {

	// in this case, we can make a drawing of the point configuration:
	Graphics::vertices_to_asy(Vertices(Circuits(Chirotope(*_pointsptr))));
	Graphics::points_to_asy(*_pointsptr);
      }
    }

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
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "generating root node with some symmetry-group preprocessing ..." << std::endl;
    }
    _root_nodeptr = new node_type(_symmetriesptr);
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      if (mode == circuits) {
	std::cerr << "exploring all symmetry classes of circuits via extension ..." << std::endl;
      }
      else {
	std::cerr << "exploring all symmetry classes of cocircuits via extension ..." << std::endl;
      }
    }
    if (_print_objects) {
      std::cout << _no << "," << _rank << ":\n{\n";
    }
  }

  template <ssg_mode_type mode>
  void SymmetricSubsetGraphMaster<mode>::_init_workers() {
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
      _threads.push_back(std::thread(&SymmetricSubsetGraphMaster<mode>::Worker::operator(),
				     std::ref(_workers[i])));
      if (CommandlineOptions::verbose()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... done" << std::endl;
      }
    }
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done" << std::endl;
    }
  }

}; // namespace topcom

#endif

// eof SymmetricSubsetGraphMaster.hh
