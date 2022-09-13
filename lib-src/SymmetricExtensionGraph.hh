//////////////////////////////////////////////////////////////////////////
// SymmetricExtensionGraph.hh
// produced: 15/01/2020 jr
// last change: 15/01/2020 jr
/////////////////////////////////////////////////////////////////////////
#ifndef SYMMETRICEXTENSIONGRAPH_HH
#define SYMMETRICEXTENSIONGRAPH_HH

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

#include <assert.h>
#include <limits>
#include <deque>
#include <vector>
#include <set>
#include <map>

#include <thread>
#include <atomic>
#include <condition_variable>

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
#include "SymmetricExtensionGraphNode.hh"

#ifndef STL_CONTAINERS
#include "HashSet.hh"
#include "PlainArray.hh"
namespace topcom {
  typedef HashSet<pt_complex_type>                                              seg_orbit_type;
  typedef HashSet<pt_complex_type>                                              seg_doneset_type;
  typedef PlainArray<int>                                                       seg_progress_vector_type;
  typedef PlainArray<SymmetricExtensionGraphNode>                               seg_nodes_container_type;
  typedef PlainHashMap<Simplex, SimplicialComplex>                              seg_cover_cache_type;
  typedef PlainHashMap<Simplex, SimplicialComplex>                              seg_coversimps_type;
  typedef PlainHashMap<Simplex, SimplicialComplex>                              seg_coveradmissibles_type;
  typedef PlainHashMap<Simplex, bool>                                           seg_coversimps_flag_type;
  typedef PlainHashMap<Simplex, bool>                                           seg_coveradmissibles_flag_type;
  typedef PlainArray<const Symmetry*>                                           seg_symmetryptr_iterdata;
  typedef PlainHashMap<Simplex, seg_symmetryptr_iterdata>                       seg_symmetries_table_type;
};
#else
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "ContainerIO.hh"
namespace topcom {
  typedef std::unordered_set<pt_complex_type, Hash<pt_complex_type> >           seg_orbit_type;
  typedef std::unordered_set<pt_complex_type, Hash<pt_complex_type> >           seg_doneset_type;
  typedef std::vector<int>                                                      seg_progress_vector_type;
  typedef std::vector<SymmetricExtensionGraphNode>                              seg_nodes_container_type;
  typedef std::unordered_map<Simplex, SimplicialComplex, Hash<Simplex> >        seg_cover_cache_type;
  typedef std::unordered_map<Simplex, SimplicialComplex, Hash<Simplex> >        seg_coversimps_type;
  typedef std::unordered_map<Simplex, SimplicialComplex, Hash<Simplex> >        seg_coveradmissibles_type;
  typedef std::unordered_map<Simplex, bool, Hash<Simplex> >                     seg_coversimps_flag_type;
  typedef std::unordered_map<Simplex, bool, Hash<Simplex> >                     seg_coveradmissibles_flag_type;
  typedef std::vector<const Symmetry*>                                          seg_symmetryptr_iterdata;
  typedef std::unordered_map<Simplex, seg_symmetryptr_iterdata, Hash<Simplex> > seg_symmetryptr_table_type;
};
#endif

namespace topcom {
  
  class SymmetricExtensionGraph {
  public:
    typedef SymmetricExtensionGraphNode node_type;
  private:
    class SymmetryWorker {
    private:
      const int                         _workerID;
      const SymmetricExtensionGraph*    _callerptr;
      pt_complex_type                   _partial_triang_to_check;
      Simplex                           _partial_triang_new_simplex;
      bool                              _work_received;
      bool                              _work_done;
      bool                              _worker_stopped;
    public:
      mutable std::condition_variable   _worker_condition;
    private:
      SymmetryWorker();
    public:
      SymmetryWorker(const int,
		     const SymmetricExtensionGraph&);
      SymmetryWorker(const SymmetryWorker&);
    private:
      const SymmetryWorker& operator=(const SymmetryWorker&);
    public:
      inline int  workerID() const;
      inline bool work_done() const;
      inline bool wake_up() const;
      inline void stop_worker();
      inline void pass_work(const pt_complex_type&, const Simplex&);
      void        check_lex_decreasing_symmetry();
      void operator()();
    };
  private:

    // sub-problem data:
    const int                            _ID;
    const size_type                      _runID;
    const size_type                      _current_symcount;
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
    const node_type*                     _rootptr;
    Field                                _volume;
    bool                                 _print_triangs;
    bool                                 _only_fine_triangs;
    bool                                 _find_minimal_triang;

    // parameters determined by calling master:
    const size_type                      _node_budget;
    const size_type*                     _current_workbuffersizeptr;
  private:

    // variables holding results:
    mutable std::ostringstream      _output_stream;
    size_type                       _totalcount;
    size_type                       _symcount;
    size_type                       _nodecount;
    size_type                       _deadendcount;
    size_type                       _earlydeadendcount;
    size_type&                      _mincard;
    SimplicialComplex&              _mintriang;
    static std::mutex               _min_mutex;

    // partial triangulations that could not be explored inside
    // the budget; reference set and owned by caller:
    std::deque<node_type>&          _open_nodes;

    // progress messaging:
    seg_progress_vector_type        _progress_vector;
  private:

    // variables holding intermediate results:
    size_type                       _orbitsize;
    seg_orbit_type                  _orbit;
    seg_orbit_type                  _regular_orbit;
    mutable pt_complex_type         _equivalent_partial_triang;
    mutable int                     _iter_coversimptighten;
    mutable int                     _maxiter_coversimptighten;
  private:

    // multi-threading for symmetry handling:
    int                             _no_of_threads;
    std::vector<std::thread>        _threads;

    // parallelize the check for minimality in orbit:
  
    // the following bool is shared among threads to signal
    // that the result in a thread has been obtained;
    // we can, however, get away without atomic<bool>
    // because all threads would only switch this
    // from false to true and never back;
    // so, race conditions are not an issue:
    mutable bool                            _is_old_symmetry_class;
    std::vector<SymmetryWorker>             _symmetry_workers;

    // we generate for each worker a symmetry table containing
    // for each simplex only those symmetries that
    // reduce the index of the simplex:
    // std::vector<seg_symmetryptr_table_type> _worker_symmetryptr_tables;
    mutable int                             _no_of_busy_threads;
    mutable std::mutex                      _main_mutex;
    mutable std::condition_variable         _main_condition;
    friend class SymmetryWorker;

  private:
    SymmetricExtensionGraph();
    SymmetricExtensionGraph(const SymmetricExtensionGraph&);
    SymmetricExtensionGraph& operator=(const SymmetricExtensionGraph&);
  public:
    // constructor:
    inline SymmetricExtensionGraph(const int,
				   const size_type,
				   const size_type,
				   const parameter_type, 
				   const parameter_type,
				   const PointConfiguration*,
				   const Chirotope*,
				   const SymmetryGroup*,
				   const SymmetryGroup*,
				   const ClassifiedExtensionSymmetries*,
				   const SwitchTable<lexmin_mode>*,
				   const Admissibles*,
				   const Incidences*,
				   const Volumes*,
				   const node_type*,
				   size_type&,
				   SimplicialComplex&,
				   std::deque<node_type>&,
				   const bool,
				   const bool,
				   const bool,
				   const size_type,
				   const size_type*);

    // destructor:
    inline ~SymmetricExtensionGraph();

    // accessors:
    inline int       ID()                       const { return _ID; }
    inline size_type runID()                    const { return _runID; }
    inline const std::deque<node_type>& open_nodes()  const { return _open_nodes; }

    // results:
    inline size_type totalcount()               const { return _totalcount; }
    inline size_type symcount()                 const { return _symcount; }
    inline size_type nodecount()                const { return _nodecount; }
    inline size_type deadendcount()             const { return _deadendcount; }
    inline size_type earlydeadendcount()        const { return _earlydeadendcount; }
    inline size_type mincard()                  const { return _mincard; }
    inline SimplicialComplex& mintriang()       const { return _mintriang; }

    // effecticity of the iterative reduction of possible cover simplices:
    inline const size_type maxiter_coversimptighten() const { return _maxiter_coversimptighten; }

    // flush results buffer:
    inline void output_results(std::ostream&, bool = false) const;

    // reporting:
    inline void report_progress(std::ostream&) const;

    // internal methods:
  private:
    inline bool _threads_done() const;
    bool _old_symmetry_class(const PartialTriang&,
			     const Simplex&);
    bool _update_coversimps(const PartialTriang&,
			    const Simplex&,
			    seg_coversimps_type&,
			    seg_coveradmissibles_type&,
			    seg_coversimps_flag_type&,
			    seg_coveradmissibles_flag_type&,
			    bool&) const;
    bool _update_coveradmissibles(const PartialTriang&,
				  const Simplex&,
				  seg_coversimps_type&,
				  seg_coveradmissibles_type&,
				  seg_coversimps_flag_type&,
				  seg_coveradmissibles_flag_type&) const;
    bool _some_possiblecoversimps_empty(const PartialTriang&,
					seg_coversimps_type&,
					seg_coveradmissibles_type&,
					seg_coversimps_flag_type&,
					seg_coveradmissibles_flag_type&) const;
    bool _some_possibleinitcoversimps_empty(const PartialTriang&,
					    seg_coversimps_type&,
					    seg_coveradmissibles_type&,
					    seg_coversimps_flag_type&,
					    seg_coveradmissibles_flag_type&) const;
    bool _noncoverable_freefacet_strong(const PartialTriang&) const;
    bool _noncoverable_freefacet_lex(const PartialTriang&) const;
    bool _noncoverable_freefacet_lean(const PartialTriang&) const;
    bool _noncoverable_freefacet(const PartialTriang&) const;
    bool _not_enough_volume(const PartialTriang&) const;
    size_type _min_no_of_missing_simplices(const PartialTriang&) const;
    void _init_symmetry_workers();
    void _init();
    void _dfs(const node_type&                 current_partial_triang,
	      size_type&                       depth);
    void _bfs(const seg_nodes_container_type&  current_triangs,
	      size_type&                       depth);
  };

  inline SymmetricExtensionGraph::SymmetricExtensionGraph(const int ID,
							  const size_type runID,
							  const size_type current_symcount,
							  const parameter_type no, 
							  const parameter_type rank,
							  const PointConfiguration* pointsptr,
							  const Chirotope* chiroptr,
							  const SymmetryGroup* symmetriesptr,
							  const SymmetryGroup* simpidx_symmetriesptr,
							  const ClassifiedExtensionSymmetries* classified_symmetriesptr,
							  const SwitchTable<lexmin_mode>* switching_tableptr,
							  const Admissibles* admtableptr,
							  const Incidences* inctableptr,
							  const Volumes* voltableptr,
							  const node_type* root_nodeptr,
							  size_type& mincard,
							  SimplicialComplex& mintriang,
							  std::deque<node_type>& open_nodes_collector,
							  const bool print_triangs,
							  const bool only_fine_triangs,
							  const bool find_minimal_triang,
							  const size_type node_budget,
							  const size_type* current_workbuffersizeptr) :
    _ID(ID),
    _runID(runID),
    _current_symcount(current_symcount),
    _no(no),
    _rank(rank),
    _pointsptr(pointsptr),
    _chiroptr(chiroptr),
    _symmetriesptr(symmetriesptr),
    _simpidx_symmetriesptr(simpidx_symmetriesptr),
    _classified_symmetriesptr(classified_symmetriesptr),
    _switch_tableptr(switching_tableptr),
    _admtableptr(admtableptr),
    _inctableptr(inctableptr),
    _voltableptr(voltableptr),
    _rootptr(root_nodeptr),
    _volume(pointsptr->volume()),
    _print_triangs(print_triangs),
    _only_fine_triangs(only_fine_triangs),
    _find_minimal_triang(find_minimal_triang),
    _node_budget(node_budget),
    _current_workbuffersizeptr(current_workbuffersizeptr),
    _output_stream(),
    _totalcount(0),
    _symcount(0),
    _nodecount(0),
    _deadendcount(0),
    _earlydeadendcount(0),
    _mincard(mincard),
    _mintriang(mintriang),
    _open_nodes(open_nodes_collector),
    _progress_vector(),
    _orbitsize(0),
    _orbit(),
    _regular_orbit(),
    _equivalent_partial_triang(),
    _iter_coversimptighten(0),
    _maxiter_coversimptighten(0),
    _is_old_symmetry_class(false),
    _no_of_threads(1) {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "SymmetricExtensionGraph::SymmetricExtensionGraph(...)" << std::endl;
#endif
    _init();
    size_type depth(_rootptr->partial_triang().card());
    if (depth > 0) {
      _progress_vector.resize(depth, -1); // set the nodes at fixed depths to -1
    }
    _dfs(*_rootptr, depth);
  }

  inline SymmetricExtensionGraph::~SymmetricExtensionGraph() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "SymmetricExtensionGraph::~SymmetricExtensionGraph()" << std::endl;
#endif
    if (_print_triangs) {
      output_results(std::cout, true);
    }
    if (CommandlineOptions::parallel_symmetries()) {
      for (int i = 0; i < _no_of_threads; ++i) {
	_symmetry_workers[i].stop_worker();
	_symmetry_workers[i]._worker_condition.notify_one();
	if (_threads[i].joinable()) {
	  _threads[i].join();
	}
      }
    }
  }

  // flush results buffer:
  inline void SymmetricExtensionGraph::output_results(std::ostream& ost, const bool force) const {
    if (force || (this->_symcount % 1000 == 0)) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      ost << _output_stream.str();
      // std::ostringstream().swap(_output_stream);
      _output_stream.str("");
    }   
  }
  
  // reporting:
  inline void SymmetricExtensionGraph::report_progress(std::ostream& ost) const {
    if (this->_nodecount % CommandlineOptions::report_frequency() == 0) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      ost << "+ worker "<< std::setw(4) << this->_ID
	  << " | n: " << std::setw(15) << this->_nodecount 
	  << " | ldead: " << std::setw(15) << this->_deadendcount
	  << " | edead: " << std::setw(15) << this->_earlydeadendcount
	  << " | sym: " << std::setw(12) << this->_symcount;
      if (!CommandlineOptions::skip_orbitcount()) {
	ost << " | tot: " << std::setw(15) << this->_totalcount;
      }
      ost << " | progress [%]: " << _progress_vector << std::endl;
    }
  }

  inline bool SymmetricExtensionGraph::_threads_done() const {
    return (_no_of_busy_threads == 0);
  }

  // inlines for subclass:
  inline int SymmetricExtensionGraph::SymmetryWorker::workerID() const {
    return _workerID;
  }

  inline bool SymmetricExtensionGraph::SymmetryWorker::wake_up() const {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "checking wake-up condition for thread "
		<< _workerID << " - value = "
		<< (_work_received || _worker_stopped) << std::endl;
    }
    return _work_received || _worker_stopped;
  }

  inline void SymmetricExtensionGraph::SymmetryWorker::pass_work(const pt_complex_type& pt, const Simplex& s) {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "worker " << _workerID << " is assigned to check " << pt
		<< " for old symmetry class ..." << std::endl;
    }
    _partial_triang_to_check    = pt;
    _partial_triang_new_simplex = s;
    _work_received              = true;
    _work_done                  = false;
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done" << std::endl;
    }
  }

  inline void SymmetricExtensionGraph::SymmetryWorker::stop_worker() {
    _worker_stopped = true;
  }

}; // namespace topcom

#endif

// eof SymmetricExtensionGraph.hh
