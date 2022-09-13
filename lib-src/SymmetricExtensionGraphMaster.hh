//////////////////////////////////////////////////////////////////////////
// SymmetricExtensionGraphMasterMaster.hh
// produced: 22/03/2020 jr
// last change: 22/03/2020 jr
/////////////////////////////////////////////////////////////////////////
#ifndef SYMMETRICEXTENSIONGRAPHMASTER_HH
#define SYMMETRICEXTENSIONGRAPHMASTER_HH

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
#include <atomic>
#include <condition_variable>

#include "Global.hh"
#include "CommandlineOptions.hh"
#include "Graphics.hh"

#include "LabelSet.hh"
#include "SimplicialComplex.hh"

#include "PointConfiguration.hh"
#include "Chirotope.hh"
#include "Symmetry.hh"
#include "ClassifiedExtensionSymmetries.hh"
#include "SwitchTable.hh"
#include "Admissibles.hh"
#include "Incidences.hh"
#include "Volumes.hh"
#include "PartialTriang.hh"
#include "SymmetricExtensionGraph.hh"
#include "RegularityCheck.hh"

namespace topcom {

  class SymmetricExtensionGraphMaster;

  class SymmetricExtensionGraphMaster {
  public:
    typedef typename SymmetricExtensionGraph::node_type node_type;
  private:
    class Worker {
    private:
      // context information:
      const int                            _workerID;
      const SymmetricExtensionGraphMaster* _callerptr;
      const node_type*                     _root_nodeptr;
      // results:
      size_type                            _totalcount;
      size_type                            _symcount;
      size_type                            _nodecount;
      size_type                            _deadendcount;
      size_type                            _earlydeadendcount;
      size_type&                           _mincard;
      SimplicialComplex&                   _mintriang;
      int                                  _maxiter_coversimptighten; 
      std::deque<node_type>                _open_nodes;
      // statistics:
      size_type                            _no_of_runs;
      // multi-threading:
      enum State { idle, hired, busy, done, stopped };
      State                                _state;
    public:
      mutable std::condition_variable      worker_condition;
    private:
      Worker();
    public:
      Worker(const int,
	     const SymmetricExtensionGraphMaster&);
      Worker(const Worker&);
    private:
      const Worker& operator=(const Worker&);
    public:
      // accessors:
      inline int       workerID()                 const { return _workerID; }
      inline size_type totalcount()               const { return _totalcount; }
      inline size_type symcount()                 const { return _symcount; }
      inline size_type nodecount()                const { return _nodecount; }
      inline size_type deadendcount()             const { return _deadendcount; }
      inline size_type earlydeadendcount()        const { return _earlydeadendcount; }
      inline size_type mincard()                  const { return _mincard; }
      inline const SimplicialComplex& mintriang()       const { return _mintriang; }
      inline size_type maxiter_coversimptighten() const { return _maxiter_coversimptighten; }
      // accessor to the no of runs:
      inline size_type no_of_runs()               const { return _no_of_runs; }
      // accessor to the non-const open nodes structure because we want to move elements from it to the caller:
      inline       std::deque<node_type>& open_nodes()        { return _open_nodes; }
      // accessors for multi-threading control:
      inline const State get_state()                    const {
	std::lock_guard<std::mutex> main_lock(_callerptr->_main_waitmutex);
	return _state;
      }
      inline bool is_idle()                       const { return (get_state() == State::idle); }
      inline bool is_hired()                      const { return (get_state() == State::hired); }
      inline bool is_busy()                       const { return (get_state() == State::busy); }
      inline bool is_done()                       const { return (get_state() == State::done); }
      inline bool is_stopped()                    const { return (get_state() == State::stopped); }
      // modifiers for multi-threading control:
      inline void set_state(const State state) {
	std::lock_guard<std::mutex> main_lock(_callerptr->_main_waitmutex);
	_state = state;
      }
      inline void set_idle()                            { set_state(State::idle); }
      inline void set_hired()                           { set_state(State::hired); }
      inline void set_busy()                            { set_state(State::busy); }
      inline void set_done()                            { set_state(State::done); }
      inline void set_stopped()                         { set_state(State::stopped); }
      inline bool wake_up()                       const;
      inline void stop_worker();
      inline void pass_work(const node_type*);
      inline void clear_results();
      inline void run();
      void        operator()();
    };
  private:
    const parameter_type                 _no;
    const parameter_type                 _rank;
    const PointConfiguration*            _pointsptr;
    const Chirotope*                     _chiroptr;
    const SymmetryGroup*                 _symmetriesptr;
    const SymmetryGroup*                 _simpidx_symmetriesptr;
    const ClassifiedExtensionSymmetries* _classified_symmetriesptr;
    const SwitchTable<lexmin_mode>*      _switch_tableptr;
    const Admissibles*                   _admtableptr;
    const Incidences*                    _inctableptr;
    const Volumes*                       _voltableptr;
    Field                                _volume;
    node_type*                           _rootptr;
    bool                                 _print_triangs;
    bool                                 _only_fine_triangs;
    bool                                 _find_minimal_triang;
  private:
    size_type                       _node_budget;
  private:
    size_type                       _totalcount;
    size_type                       _symcount;
    size_type                       _nodecount;
    size_type                       _deadendcount;
    size_type                       _earlydeadendcount;
    mutable size_type               _mincard;
    mutable SimplicialComplex       _mintriang;
    int                             _maxiter_coversimptighten;
    std::deque<node_type>           _open_nodes;
    size_type                       _current_workbuffersize;
  private:
    // multi-threading:
    int                             _no_of_threads;
    std::vector<std::thread>        _threads;
    std::vector<Worker>             _workers;
    mutable int                     _no_of_idle_threads;
    mutable int                     _no_of_pending_results;
    mutable std::mutex              _main_waitmutex;
    mutable std::condition_variable _main_condition;
    friend class SymmetricExtensionGraphMaster::Worker;

  private:
    SymmetricExtensionGraphMaster();
    SymmetricExtensionGraphMaster(const SymmetricExtensionGraphMaster&);
    SymmetricExtensionGraphMaster& operator=(const SymmetricExtensionGraphMaster&);
  public:
    // constructor:
    inline SymmetricExtensionGraphMaster(const parameter_type, 
					 const parameter_type,
					 const PointConfiguration&,
					 const Chirotope&,
					 const SymmetryGroup&,
					 const bool = false,
					 const bool = false,
					 const bool = false);
    // destructor:
    inline ~SymmetricExtensionGraphMaster();
    // accessors for multi-threading control:
    inline int get_no_of_idle_threads() const {
      std::lock_guard<std::mutex> main_lock(_main_waitmutex);
      return _no_of_idle_threads;
    }
    inline int get_no_of_pending_results() const {
      std::lock_guard<std::mutex> main_lock(_main_waitmutex);
      return _no_of_pending_results;
    }
    // modifiers for multi-threading control:
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
    inline size_type totalcount()               const { return _totalcount; }
    inline size_type symcount()                 const { return _symcount; }
    inline size_type nodecount()                const { return _nodecount; }
    inline size_type deadendcount()             const { return _deadendcount; }
    inline size_type earlydeadendcount()        const { return _earlydeadendcount; }
    inline size_type mincard()                  const { return _mincard; }
    inline const SimplicialComplex& mintriang() const { return _mintriang; }
    inline size_type maxiter_coversimptighten() const { return _maxiter_coversimptighten; }
    // multi-threading control:
    inline bool wake_up() const;
    // internal methods:
    inline bool _exists_idle_thread() const;
    inline bool _exists_pending_result() const;
    inline bool _all_threads_idle() const;
    inline void _collect_results();
    void _run();
    // collect init/term steps:
    void _init();
    void _init_workers();
    void _term();
    void _term_workers();
    // stream output/input:
  };

  inline SymmetricExtensionGraphMaster::SymmetricExtensionGraphMaster(const parameter_type no, 
								      const parameter_type rank,
								      const PointConfiguration& points,
								      const Chirotope& chiro,
								      const SymmetryGroup& symmetries,
								      const bool print_triangs,
								      const bool only_fine_triangs,
								      const bool find_minimal_triang) :
    _no(no),
    _rank(rank),
    _pointsptr(&points),
    _chiroptr(&chiro),
    _symmetriesptr(&symmetries),
    _simpidx_symmetriesptr(0),
    _classified_symmetriesptr(0),
    _switch_tableptr(0),
    _admtableptr(0),
    _inctableptr(0),
    _voltableptr(0),
    _volume(FieldConstants::ZERO),
    _rootptr(0),
    _print_triangs(print_triangs),
    _only_fine_triangs(only_fine_triangs),
    _find_minimal_triang(find_minimal_triang),
    _node_budget(1),
    _totalcount(0),
    _symcount(0),
    _nodecount(0),
    _deadendcount(0),
    _earlydeadendcount(0),
    _mincard(std::numeric_limits<size_type>::max()),
    _mintriang(),
    _maxiter_coversimptighten(0),
    _open_nodes(),
    _current_workbuffersize(0UL),
    _no_of_threads(1) {
    _init();
    _run();
  }

  inline SymmetricExtensionGraphMaster::~SymmetricExtensionGraphMaster() {
    _term();
  }

  // budgeting:
  inline void SymmetricExtensionGraphMaster::update_node_budget() {
    _node_budget = std::max<size_type>(1, _open_nodes.size() * _open_nodes.size() * CommandlineOptions::scale_nodebudget() / 100);
    if (_node_budget < CommandlineOptions::min_nodebudget()) {
      _node_budget = CommandlineOptions::min_nodebudget();
    }
    else if (_node_budget > CommandlineOptions::max_nodebudget()) {
      _node_budget = CommandlineOptions::max_nodebudget();
    }
  }

  // reporting:
  inline void SymmetricExtensionGraphMaster::report_progress(std::ostream& ost) const {
    static int count = CommandlineOptions::report_frequency();
    static size_type old_nodecount(0UL);
  
    count -= (this->_nodecount - old_nodecount);
    if (count < 0) {
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
		<< " | t idle: " << std::setw(4) << this->_no_of_idle_threads;
	  }
	  else {
	    ost << " | n open: " << std::setw(9) << _open_nodes.size()
		<< " | n budget: " << std::setw(9) << this->_node_budget
		<< " | t idle: " << std::setw(4) << this->_no_of_idle_threads;
	  }
	}
	std::cerr << std::endl;
      }
    }
    old_nodecount = this->_nodecount;
  }

  inline bool SymmetricExtensionGraphMaster::wake_up() const {

    // use shared data directly since lock is organized by the wait of the condition variable:
    return (_no_of_pending_results > 0) // need to collect results
      || (!_open_nodes.empty() && (_no_of_idle_threads > 0)) // can distribute work
      || (_open_nodes.empty() && (_no_of_idle_threads == _no_of_threads)); // need to terminate
  }

  inline bool SymmetricExtensionGraphMaster::_exists_idle_thread() const {
    return (get_no_of_idle_threads() > 0);
  }

  inline bool SymmetricExtensionGraphMaster::_exists_pending_result() const {
    return (get_no_of_pending_results() > 0);
  }

  inline bool SymmetricExtensionGraphMaster::_all_threads_idle() const {
    return (get_no_of_idle_threads() >= _no_of_threads);
  }

  // inlines for worker subclass:
  inline bool SymmetricExtensionGraphMaster::Worker::wake_up() const {

    // no lock here because the lock is held by the condition variable already:
    return ((_state == State::hired) || (_state == State::stopped));
  }

  inline void SymmetricExtensionGraphMaster::Worker::pass_work(const node_type* nodeptr) {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "worker " << _workerID << " is assigned to work on "
		<< *nodeptr << " ..." << std::endl;
    }
    _callerptr->decrease_no_of_idle_threads();
    _root_nodeptr               = nodeptr;
    set_hired();
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done" << std::endl;
    }
  }

  inline void SymmetricExtensionGraphMaster::Worker::run() {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "constructing new enumerator ..." << std::endl;
    }
    set_busy();
    SymmetricExtensionGraph seg(_workerID,
				_no_of_runs,
				_callerptr->_symcount,
				_callerptr->_no,
				_callerptr->_rank,
				_callerptr->_pointsptr,
				_callerptr->_chiroptr,
				_callerptr->_symmetriesptr,
				_callerptr->_simpidx_symmetriesptr,
				_callerptr->_classified_symmetriesptr,
				_callerptr->_switch_tableptr,
				_callerptr->_admtableptr,
				_callerptr->_inctableptr,
				_callerptr->_voltableptr,
				_root_nodeptr,
				_mincard,
				_mintriang,
				_open_nodes,
				_callerptr->_print_triangs,
				_callerptr->_only_fine_triangs,
				_callerptr->_find_minimal_triang,
				_callerptr->_node_budget,
				&_callerptr->_current_workbuffersize);
    _nodecount                = seg.nodecount();
    _totalcount               = seg.totalcount();
    _symcount                 = seg.symcount();
    _deadendcount             = seg.deadendcount();
    _earlydeadendcount        = seg.earlydeadendcount();
    if (seg.maxiter_coversimptighten() > _maxiter_coversimptighten) {
      _maxiter_coversimptighten = seg.maxiter_coversimptighten();
    }
    ++_no_of_runs;
    delete(_root_nodeptr);
    set_done();
    _callerptr->increase_no_of_pending_results();
    if (CommandlineOptions::debug()) {        
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... enumerator going out of scope" << std::endl;
    }
  }

  inline void SymmetricExtensionGraphMaster::Worker::clear_results() {
    _root_nodeptr           = 0;
    _symcount               = 0UL;
    _totalcount             = 0UL;
    _nodecount              = 0UL;
    _deadendcount           = 0UL;
    _earlydeadendcount      = 0UL;
    _open_nodes.clear();
    _callerptr->decrease_no_of_pending_results();
    set_idle();
    _callerptr->increase_no_of_idle_threads();
  }

  inline void SymmetricExtensionGraphMaster::Worker::stop_worker() {
    set_stopped();
  }

}; // namespace topcom

#endif

// eof SymmetricExtensionGraphMaster.hh
