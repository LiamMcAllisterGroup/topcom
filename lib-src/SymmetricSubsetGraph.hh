//////////////////////////////////////////////////////////////////////////
// SymmetricSubsetGraph.hh
// produced: 06/02/2020 jr
// last change: 30/03/2020 jr
/////////////////////////////////////////////////////////////////////////
#ifndef SYMMETRICSUBSETGRAPH_HH
#define SYMMETRICSUBSETGRAPH_HH

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <assert.h>
#include <vector>
#include <set>
#include <map>

#include <thread>
#include <atomic>

#include "LabelSet.hh"

#include "CommandlineOptions.hh"

#include "StrictStairCaseMatrix.hh"
#include "StrictStairCaseMatrixTrans.hh"
#include "PointConfiguration.hh"
#include "Chirotope.hh"
#include "Circuits.hh"
#include "Cocircuits.hh"
#include "Symmetry.hh"
#include "SwitchTable.hh"
#include "ClassifiedSubsetSymmetries.hh"
#include "SymmetricSubsetGraphNode.hh"

namespace topcom {

  typedef LabelSet                                            subset_type;

#ifndef STL_CONTAINERS
#include "HashSet.hh"
  typedef HashSet<subset_type>                                ssg_doneset_type;
  typedef PlainArray<size_type>                               ssg_progress_vector_type;
  typedef PlainArray<const Symmetry*>                         ssg_symmetryptr_iterdata;
#else
#include <unordered_set>
#include "ContainerIO.hh"
  typedef std::unordered_set<subset_type, Hash<subset_type> > ssg_doneset_type;
  typedef std::vector<int>                                    ssg_progress_vector_type;
  typedef std::vector<const Symmetry*>                        ssg_symmetryptr_iterdata;
#endif
  
  template <ssg_mode_type mode>
  class SymmetricSubsetGraph {
  public:
    // the following typedef is subject to change into a full-fledged node class:
    // typedef std::pair<LabelSet, NodeMatrixClass<mode> > old_node_type;
    // ... like the one here:
    typedef SymmetricSubsetGraphNode<mode>                node_type;
    typedef typename node_type::matrix_type               matrix_type;
  public:
    class SymmetryWorker {
    private:
      const int                   _workerID;
      const SymmetricSubsetGraph* _callerptr;
    private:
      SymmetryWorker();
    public:
      SymmetryWorker(const int, const SymmetricSubsetGraph&);
      void old_symmetry_class_incrementalcheck(const dependent_set_type&,
					       const parameter_type,
					       const parameter_type);
      void old_symmetry_class_fullcheck(const dependent_set_type&);
    };
    friend class SymmetricSubsetGraph::SymmetryWorker;
  private:
    const int                         _ID;
    const size_type                   _runID;
    const parameter_type              _no;
    const parameter_type              _rank;
    const PointConfiguration*         _pointsptr;
    const SymmetryGroup*              _symmetriesptr;
    const ClassifiedSubsetSymmetries* _classified_symmetriesptr;
    const SwitchTable<LabelSet,
		      colexmax_mode>* _switch_tableptr;
    const node_type*                  _root_nodeptr;
    const bool                        _print_objects;
    const bool                        _save_objects;
    // parameters determined by calling master:
    const size_type                   _node_budget;
    const size_type*                  _current_workbuffersizeptr;
  private:
    size_type                         _totalcount;
    size_type                         _symcount;
    size_type                         _nodecount;
    size_type                         _deadendcount;
    size_type                         _earlydeadendcount;
    // nodes that could not be explored inside
    // the budget; reference set and owned by caller:
    std::deque<node_type>&            _open_nodes;
    // this is a global result set used to collect
    // found cocircuits used for the template parameter "cocircuits":
    // it is owned by the caller,
    // and must therefore be protected by a static mutex:
    ssg_doneset_type&                 _result_set;
    static std::mutex                 _result_set_mutex;
    ssg_progress_vector_type          _progress_vector;
  private:
    size_type                         _orbitsize;
    subset_type                       _equivalent_subset;
  private:
    // multi-threading:
    int                               _no_of_threads;
    std::vector<std::thread>          _threads;

    // parallelize the check for minimality in orbit:
  
    // the following bool is shared among threads to signal
    // that the result in a thread has been obtained;
    // we can, however, get away without atomic<bool>
    // because all threads would only switch this
    // from false to true and never back;
    // so, race conditions are not an issue:
    mutable bool                    _is_old_symmetry_class;
    std::vector<SymmetryWorker>     _symmetry_workers;

  private:
    SymmetricSubsetGraph();
    SymmetricSubsetGraph(const SymmetricSubsetGraph&);
    SymmetricSubsetGraph& operator=(const SymmetricSubsetGraph&);
  public:
    // constructor:
    inline SymmetricSubsetGraph(const int,
				const size_type,
				const parameter_type, 
				const parameter_type,
				const PointConfiguration*,
				const SymmetryGroup*,
				const ClassifiedSubsetSymmetries*,
				const SwitchTable<LabelSet, colexmax_mode>*,
				const node_type*,
				// const ssg_circuits_node*,
				// const ssg_cocircuits_node*,
				std::deque<node_type>&,
				// std::deque<ssg_circuits_node>&,
				// std::deque<ssg_cocircuits_node>&,
				ssg_doneset_type&,
				const bool,
				const bool,
				const size_type,
				const size_type*);
    // destructor:
    inline ~SymmetricSubsetGraph();
    // accessors:
    inline const int       ID()                                           const { return _ID; }
    inline const size_type runID()                                        const { return _runID; }
    inline const std::deque<node_type>& open_nodes()                      const { return _open_nodes; }
    // results:
    inline const size_type totalcount()                                   const { return _totalcount; }
    inline const size_type symcount()                                     const { return _symcount; }
    inline const size_type nodecount()                                    const { return _nodecount; }
    inline const size_type deadendcount()                                 const { return _deadendcount; }
    inline const size_type earlydeadendcount()                            const { return _earlydeadendcount; }
    // reporting:
    inline void report_progress(std::ostream&) const;
    // functionality:
    inline const bool old_symmetry_class_incrementalcheck(const subset_type&);
    inline const bool old_symmetry_class_fullcheck       (const subset_type&);
  private:
    // internal methods:
    inline void _init();
    // void _init_symmetries_table();
    inline void _init_symmetry_workers();
    inline void _dfs(const node_type&,
		     const parameter_type,
		     parameter_type&);
    // internal helper functions:
    inline bool _semi_is_not_monotonically_extendable(const subset_type&,
						      const parameter_type,
						      const matrix_type&,
						      const parameter_type);
    // stream output/input:
  };

  // first announce the upcoming template specializations in order to allow
  // the implementations to be ordered arbitrarily:
  template <>
  inline void SymmetricSubsetGraph<circuits>::_dfs(const node_type&,
						   const parameter_type,
						   parameter_type&);
  template <>
  inline void SymmetricSubsetGraph<cocircuits_independent>::_dfs(const node_type&,
								 const parameter_type,
								 parameter_type&);
  template <>
  inline void SymmetricSubsetGraph<cocircuits>::_dfs(const node_type&,
						     const parameter_type,
						     parameter_type&);
  
  template <>
  inline bool SymmetricSubsetGraph<cocircuits>::_semi_is_not_monotonically_extendable(const subset_type&,
										      const parameter_type,
										      const matrix_type&,
										      const parameter_type);

  template <ssg_mode_type mode>
  std::mutex SymmetricSubsetGraph<mode>::_result_set_mutex;

  template <ssg_mode_type mode>
  inline SymmetricSubsetGraph<mode>::SymmetricSubsetGraph(const int                                           ID,
							  const size_type                                     runID,
							  const parameter_type                                no, 
							  const parameter_type                                rank,
							  const PointConfiguration*                           pointsptr,
							  const SymmetryGroup*                                symmetriesptr,
							  const ClassifiedSubsetSymmetries*                   classified_symmetriesptr,
							  const SwitchTable<LabelSet, colexmax_mode>*         switch_tabletptr,
							  const node_type*                                    root_nodeptr,
							  std::deque<node_type>&                              open_nodes,
							  ssg_doneset_type&                                   result_set,
							  const bool                                          print_objects,
							  const bool                                          save_objects,
							  const size_type                                     node_budget,
							  const size_type*                                    current_workbufferptr) :
    _ID(ID),
    _runID(runID),
    _no(no),
    _rank(rank),
    _pointsptr(pointsptr),
    _symmetriesptr(symmetriesptr),
    _classified_symmetriesptr(classified_symmetriesptr),
    _switch_tableptr(switch_tabletptr),
    _root_nodeptr(root_nodeptr),
    _result_set(result_set),
    _print_objects(print_objects),
    _save_objects(save_objects),
    _node_budget(node_budget),
    _current_workbuffersizeptr(current_workbufferptr),
    _totalcount(0),
    _symcount(0),
    _nodecount(0),
    _deadendcount(0),
    _earlydeadendcount(0),
    _open_nodes(open_nodes),
    _progress_vector(),
    _orbitsize(0),
    _is_old_symmetry_class(false),
    _no_of_threads(CommandlineOptions::no_of_threads()) {
    _init();
    ssg_progress_vector_type progress_vector(1UL, 0UL);
    parameter_type start(0);
    parameter_type depth(_root_nodeptr->subset().card());
    if (depth > 0) {
      _progress_vector.resize(depth - 1, -1); // set the nodes at fixed depths to -1
      const parameter_type min_elem(_root_nodeptr->subset().min_elem());
      start = _no - min_elem;
    }
    _dfs(*_root_nodeptr, start, depth);
  }

  template <ssg_mode_type mode>
  inline SymmetricSubsetGraph<mode>::~SymmetricSubsetGraph() {
  }

  // reporting:
  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraph<mode>::report_progress(std::ostream& ost) const {
    if (this->_nodecount % CommandlineOptions::report_frequency() == 0) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      ost << "+ worker "<< std::setw(4) << this->_ID
	  << " | n: " << std::setw(15) << this->_nodecount 
	  << " | ldead: " << std::setw(15) << this->_deadendcount
	  << " | edead: " << std::setw(15) << this->_earlydeadendcount;
      ost << " | sym: " << std::setw(12) << this->_symcount;
      if (!CommandlineOptions::skip_orbitcount()) {
	ost << " | tot: " << std::setw(15) << this->_totalcount;
      }
      ost << " | progress [%]: " << _progress_vector << std::endl;
    }
  }

  template <ssg_mode_type mode>
  inline SymmetricSubsetGraph<mode>::SymmetryWorker::SymmetryWorker(const int workerID,
									  const SymmetricSubsetGraph<mode>& ssg) :
    _workerID(workerID),
    _callerptr(&ssg) {
  }

  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraph<mode>::SymmetryWorker::old_symmetry_class_fullcheck(const subset_type& current_subset) {
    if (_callerptr->_is_old_symmetry_class) {
      // some other thread may have found that current_partial_triang is not new:
      return;
    }
    const css_symmetryptr_iterdata& relevant_symmetries(_callerptr->_classified_symmetriesptr->worker_symmetryptrs(_workerID));
    for (ssg_symmetryptr_iterdata::const_iterator iter = relevant_symmetries.begin();
	 iter != relevant_symmetries.end();
	 ++iter) {
      // some other thread may have found that current_partial_triang is not new:
      if (_callerptr->_is_old_symmetry_class) {
	return;
      }
      const Symmetry& g(**iter);
      if (g.colex_increases(current_subset)) {
	_callerptr->_is_old_symmetry_class = true;
	return;
      }
    } 
  }

  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraph<mode>::SymmetryWorker::old_symmetry_class_incrementalcheck(const subset_type&   current_subset,
												    const parameter_type max_elem,
												    const parameter_type new_elem) {
    if (_callerptr->_is_old_symmetry_class) {
      // some other thread may have found that current_partial_triang is not new:
      return;
    }
    const css_symmetryptr_iterdata& relevant_symmetries(_callerptr->_classified_symmetriesptr->worker_relevant_symmetryptrs(_workerID, new_elem, max_elem));
    for (css_symmetryptr_iterdata::const_iterator iter = relevant_symmetries.begin();
	 iter != relevant_symmetries.end();
	 ++iter) {
      // some other thread may have found that current_subset is not new:
      if (_callerptr->_is_old_symmetry_class) {
	return;
      }
      const Symmetry& g(**iter);
      if (g.colex_increases(current_subset)) {
	_callerptr->_is_old_symmetry_class = true;
	return;
      }
    } 
  }

  template <ssg_mode_type mode>
  inline const bool SymmetricSubsetGraph<mode>::old_symmetry_class_incrementalcheck(const subset_type& current_subset) {
#ifdef CLASSIFIED_DEBUG
    //////////////////////////////////////////////////////////////////////////////
    static size_type cnt_overtaking(0);
    static size_type cnt_compressing(0);
    static size_type cnt_exhaustive(0);
    //////////////////////////////////////////////////////////////////////////////
#endif
  
    // in this function, we assume that the minimal element of current_subset was added last 
    // and that current_subset minus {min_elem} cannot be colex-increased:
    const parameter_type min_elem(*current_subset.begin());
    const parameter_type max_elem(current_subset.max_elem());
  
    // use a preprocessed structure to accelerate colex-increasing check:
    if (CommandlineOptions::use_classified_symmetries() && _classified_symmetriesptr->colex_increases_by_overtaking(current_subset, max_elem, min_elem)) {
#ifdef CLASSIFIED_DEBUG
      //////////////////////////////////////////////////////////////////////////////
      {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "early success no " << std::setw(9) << ++cnt_overtaking << ": overtaking" << std::endl;
      }
      //////////////////////////////////////////////////////////////////////////////
#endif
      return true;
    }
    if (CommandlineOptions::use_classified_symmetries() && _classified_symmetriesptr->colex_increases_by_compressing(current_subset, max_elem, min_elem)) {
#ifdef CLASSIFIED_DEBUG
      //////////////////////////////////////////////////////////////////////////////
      {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "early success no " << std::setw(9) << ++cnt_compressing << ": compressing" << std::endl;
      }
      //////////////////////////////////////////////////////////////////////////////
#endif
      return true;
    }
  
    if (CommandlineOptions::parallel_symmetries() && !CommandlineOptions::use_switch_tables()) {
      _threads.clear();
      _is_old_symmetry_class = false;
      for (int i = 0; i < _no_of_threads; ++i) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "spawning thread " << i << " ..." << std::endl;
	}
	_threads.push_back(std::thread(&SymmetryWorker::old_symmetry_class_incrementalcheck,
				       &_symmetry_workers[i],
				       std::ref(current_subset),
				       std::ref(max_elem),
				       std::ref(min_elem)));
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... done" << std::endl;
	}
      }
      for (int i = 0; i < _no_of_threads; ++i) {
	_threads[i].join();
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "workers final result = " << _is_old_symmetry_class << std::endl;
      }
      return _is_old_symmetry_class;
    }
    else {
      if (CommandlineOptions::use_switch_tables()) {
      	if (_switch_tableptr->colex_increases(current_subset)) {
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "found a colex-increasing switch product." << std::endl;
	  }
	  return true;
	}
	else {
	  return false;
	}
      }
      else if (CommandlineOptions::use_classified_symmetries()) {
	if (_classified_symmetriesptr->colex_increases_by_relevant_symmetries(current_subset, max_elem, min_elem)) {
#ifdef CLASSIFIED_DEBUG
	  //////////////////////////////////////////////////////////////////////////////
	  {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "late success no " << std::setw(9) << ++cnt_exhaustive << ": exhaustive search on "
		      << 100 * (_classified_symmetriesptr->relevant_symmetryptrs(min_elem, max_elem).size()) / _symmetriesptr->size() << "% of elements" << std::endl;
	  }
	  //////////////////////////////////////////////////////////////////////////////
#endif
	  return true;
	}
	else {
#ifdef COMPUTATIONS_DEBUG
	  for (SymmetryGroup::const_iterator iter = _symmetriesptr->begin();
	       iter != _symmetriesptr->end();
	       ++iter) {
	    if (iter->colex_increases(current_subset)) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "contradicting result for subset " << current_subset << ":" << std::endl;
	      std::cerr << "symmetry " << *iter << " colex increases " << current_subset << std::endl;
	      std::cerr << "but is not contained in relevant symmetries: " << _classified_symmetriesptr->relevant_symmetryptrs(min_elem, max_elem) << std::endl;
	      std::cerr << "total min-elem increasing relevant symmetries: " << _classified_symmetriesptr->relevant_symmetryptrs(min_elem, _no - 1) << std::endl;
	      exit(1);
	    }
	  }
#endif
	  return false;
	}
      }
      else {
	for (SymmetryGroup::const_iterator iter = _symmetriesptr->begin();
	     iter != _symmetriesptr->end();
	     ++iter) {
	  if (iter->colex_increases(current_subset)) {
	    return true;
	  }
	}
	return false;
      }
    }
  }

  template <ssg_mode_type mode>
  inline const bool SymmetricSubsetGraph<mode>::old_symmetry_class_fullcheck(const subset_type& current_subset) {
    // in this function, we assume nothing about current_subset minus min_elem (needed for cocircuits):
    if (CommandlineOptions::parallel_symmetries() && !CommandlineOptions::use_switch_tables()) {
      _threads.clear();
      _is_old_symmetry_class = false;
      for (int i = 0; i < _no_of_threads; ++i) {
	if (CommandlineOptions::debug()) {
	  std::cerr << "spawning thread " << i << " ..." << std::endl;
	}
	_threads.push_back(std::thread(&SymmetryWorker::old_symmetry_class_fullcheck,
				       &_symmetry_workers[i],
				       std::ref(current_subset)));
	if (CommandlineOptions::debug()) {
	  std::cerr << "... done" << std::endl;
	}
      }
      for (int i = 0; i < _no_of_threads; ++i) {
	_threads[i].join();
      }
      if (CommandlineOptions::debug()) {
	std::cerr << "workers final result = " << _is_old_symmetry_class << std::endl;
      }
      return _is_old_symmetry_class;
    }
    else {
      if (CommandlineOptions::use_switch_tables()) {
      
	// experimental - only in effect at this point:
	if (_switch_tableptr->colex_increases(current_subset)) {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "found a colex-increasing switch product." << std::endl;
	  }
	  return true;
	}
	else {
	  return false;
	}
      }
      else {
	for (symmetry_iterdata::const_iterator iter = _symmetriesptr->begin();
	     iter != _symmetriesptr->end();
	     ++iter) {
	  const Symmetry& g(*iter);
	  if (g.colex_increases(current_subset)) {
	    return true;
	  }
	}
	return false;
      }
    }
  }

  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraph<mode>::_init() {
    if (CommandlineOptions::output_asy()) {
      Graphics::run_to_asy(_ID);
      Graphics::matrix_to_asy(_ID, _runID, _root_nodeptr->subset(), _root_nodeptr->matrix());
    }
    if (CommandlineOptions::parallel_symmetries()) {
      _init_symmetry_workers();
    }
  }

  template <ssg_mode_type mode>
  inline void SymmetricSubsetGraph<mode>::_init_symmetry_workers() {
    if (CommandlineOptions::debug()) {
      std::cerr << "initializing symmetry workers ..." << std::endl;
    }
    for (int i = 0; i < _no_of_threads; ++i) {
      _symmetry_workers.push_back(SymmetryWorker(i, *this));
    }
    if (CommandlineOptions::debug()) {    
      std::cerr << "... done" << std::endl;
    }
  }

  // if the template parameter is "circuits", we enumerate circuits:
  template <>
  inline void SymmetricSubsetGraph<circuits>::_dfs(const node_type&     current_node,
						   const parameter_type start,
						   parameter_type&      depth) {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "processing node " << this->_nodecount
		<< " corresponding to subset " << current_node.subset() << " by all possible one-element extensions:" << std::endl;
    }
    const size_type workload(_no - start);
    size_type doneload(0UL);
    ++depth;
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "upsizing progress vector to " << depth << " ..." << std::endl;
    }
    _progress_vector.resize(depth);

    // for asy output:
    size_type parent_node_ID = this->_nodecount;
    
    for (parameter_type cnt = start; cnt < _no; ++cnt) {
      ++doneload;
      const parameter_type i = _no - cnt - 1;
      _progress_vector.at(depth - 1) = (100 * doneload) / workload;
      ++this->_nodecount;
      subset_type next_subset(current_node.subset());
      next_subset += i;

      // for asy output:
      size_type child_node_ID = this->_nodecount;

      if (CommandlineOptions::output_asy()) {

	// the matrix is not needed yet, and it takes time to compute,
	// thus, it is not yet computed;
	// if graphics output is wanted, the problem is probably small,
	// so that computing it here just for the graphics should not harm:
	matrix_type next_matrix_for_asy = current_node.matrix();
	next_matrix_for_asy.augment(_pointsptr->col(i));
	Graphics::matrix_to_asy(_ID, _runID, next_subset, next_matrix_for_asy);
	Graphics::arc_to_asy(_ID, _runID, parent_node_ID, child_node_ID);
      }
      
      if (CommandlineOptions::verbose()) {
	report_progress(std::cerr);
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "processing point " << i << " ..." << std::endl;
      }

      // check for equivalent colex-greater subsets:
      bool is_new = true;
      critical_element_data_type new_critelem_data;
      size_type new_stabilizer_card;
      if (CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_switch_tables() || CommandlineOptions::use_naive_symmetries()) {
	
	// use the method of TOPCOM-1.0.0:
	is_new = !old_symmetry_class_incrementalcheck(next_subset);
      }
      else {
	
	// use the method of TOPCOM-1.0.1:
	is_new = current_node.child_is_colexmax(i, &new_critelem_data, &new_stabilizer_card);
      }

      if (!is_new) {

	// for asy output:
	if (CommandlineOptions::output_asy()) {
	  Graphics::notnew_to_asy(_ID, _runID, child_node_ID);
	}	
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_subset << " was found already: continue" << std::endl;
	}
	continue;
      }
      else {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_subset << " was not found yet: process" << std::endl;
	}
      }
      matrix_type next_matrix(current_node.matrix());
      next_matrix.augment(_pointsptr->col(i));
      if (CommandlineOptions::debug()) {
	Matrix raw_matrix(current_node.matrix());
	raw_matrix.augment(_pointsptr->col(i));
	std::cerr << "matrix with new column:" << std::endl;
	raw_matrix.pretty_print(std::cerr);
	std::cerr << std::endl;
	std::cerr << "new strict staircase matrix:" << std::endl;
	next_matrix.pretty_print(std::cerr);
	std::cerr << std::endl;
      }
      if (next_matrix.has_full_rank()) {
	if (i == 0) {

	  // no further element available for extending subset - we can stop here:

	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::deadend_to_asy(_ID, _runID, child_node_ID);
	  }	
	  continue;
	}
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "entering recursion with " << next_subset << std::endl;
	}
	bool continue_work(true);
	if (CommandlineOptions::workbuffercontrol()) {
	  if (*this->_current_workbuffersizeptr < CommandlineOptions::min_workbuffersize()) {
	    continue_work = false;
	  }
	}
	else {
	  if (this->_nodecount > _node_budget) {
	    continue_work = false;
	  }
	}
	node_type next_node(_symmetriesptr,
			    next_subset,
			    next_matrix,
			    new_critelem_data,
			    new_stabilizer_card);
	if (continue_work) {
	  _dfs(next_node, cnt + 1, depth);
	}
	else {
	  _open_nodes.push_back(next_node);
	}
      }
      else {
#ifdef SUPER_VERBOSE
	std::cerr << "dependency found of corank " << _rank - depth << std::endl;
#endif
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "circuit on " << next_subset
		    << " found with support size at most " << depth << std::endl;
	}
	
	// we find the coefficients of the dependency in the final column of the
	// transformation matrix associated with next:
	Circuit newcircuit;
	bool is_minimal = true;
	size_type j(0);
	for (LabelSet::const_iterator iter = next_subset.begin();
	     iter != next_subset.end();
	     ++iter) {
	  const Field& coefficient = next_matrix.transformation()(j, depth - 1);
	  ++j;
	  if (coefficient == FieldConstants::ZERO) {
	    is_minimal = false;
	    break;
	  }
	  if (coefficient > FieldConstants::ZERO) {
	    newcircuit.first += *iter;
	  }
	  else if (coefficient < FieldConstants::ZERO) {
	    newcircuit.second += *iter;
	  }
	}
	if (is_minimal) {
	  
	  // here we count one circuit corresponding to the minimal dependent set "newbasis":
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "found minimal dependent set " << next_subset << std::endl;
	  }
	  if (!CommandlineOptions::skip_orbitcount()) {
	    if (CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_naive_symmetries()) {
	      _orbitsize = (_symmetriesptr->size() + 1) / (_symmetriesptr->stabilizer_card(next_subset) + 1);
	    }
	    else if (CommandlineOptions::use_switch_tables() ) {
	      _orbitsize = _switch_tableptr->orbit_size(next_subset);
	    }
	    else {
	      _orbitsize = (_symmetriesptr->size() + 1) / (new_stabilizer_card + 1);
	    }
	  }
	  if (_print_objects) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cout << newcircuit << '\n';
	  }
	  if (_save_objects) {
	    std::lock_guard<std::mutex> lock(_result_set_mutex);
	    _result_set.insert(next_subset);
	  }
	  
	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::solution_to_asy(_ID, _runID, child_node_ID);
	  }	
	  
	  _totalcount += _orbitsize;
	  ++_symcount;
	}
	else {

	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::deadend_to_asy(_ID, _runID, child_node_ID);
	  }
	  
	  ++_deadendcount;
	}
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done." << std::endl;
    }
    --depth;
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "downsizing progress vector to " << depth << " ..." << std::endl;
    }
    _progress_vector.resize(depth);
  }

  // if the template parameter is "cocircuits", we enumerate cocircuits:
  template <>
  inline void SymmetricSubsetGraph<cocircuits_independent>::_dfs(const node_type&     current_node,
								 const parameter_type start,
								 parameter_type&      depth) {
    
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "processing node " << this->_nodecount
		<< " corresponding to subset " << current_node.subset() << " by all possible one-element extensions:" << std::endl;
    }
    const size_type workload(_no - start);
    size_type doneload(0UL);
    ++depth;
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "upsizing progress vector to " << depth << " ..." << std::endl;
    }
    
    // for asy output:
    size_type parent_node_ID = this->_nodecount;

    _progress_vector.resize(depth);
    // we traverse the possible extensions from right to left because
    // right-to-left lexorder is compatible with the
    // bit representations of LabelSets, while the left-to-right
    // lexorder is not:
    for (parameter_type cnt = start; cnt < _no - (_rank - depth - 1); ++cnt) {
      ++doneload;
      const parameter_type i = _no - cnt - 1;
      _progress_vector.at(depth - 1) = (100 * doneload) / workload;
      ++this->_nodecount;
      subset_type next_subset(current_node.subset() + i);

      // for asy output:
      size_type child_node_ID = this->_nodecount;

      if (CommandlineOptions::output_asy()) {

	// the matrix is not needed yet, and it takes time to compute,
	// thus, it is not yet computed;
	// if graphics output is wanted, the problem is probably small,
	// so that computing it here just for the graphics should not harm:
	matrix_type next_matrix_for_asy = current_node.matrix();
	next_matrix_for_asy.augment(_pointsptr->col(i));
	Graphics::matrix_to_asy(_ID, _runID, next_subset, next_matrix_for_asy);
	Graphics::arc_to_asy(_ID, _runID, parent_node_ID, child_node_ID);
      }
      
      if (CommandlineOptions::verbose()) {
	report_progress(std::cerr);
      }

      // check for non-full rank first because this is mostly faster than the symmetry check:
      matrix_type next_matrix(current_node.matrix());
      next_matrix.augment(_pointsptr->col(i));
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	Matrix raw_matrix(current_node.matrix());
	raw_matrix.augment(_pointsptr->col(i));
	std::cerr << "matrix with new column:" << std::endl;
	raw_matrix.pretty_print(std::cerr);
	std::cerr << std::endl;
	std::cerr << "new strict staircase matrix:" << std::endl;
	next_matrix.pretty_print(std::cerr);
	std::cerr << std::endl;
      }
      if (!next_matrix.has_full_rank()) {
	// cannot become a minimal spanning zero set of a cocircuit:
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_subset << " not independent - deadend" << std::endl;
	}
	
	// for asy output:
	if (CommandlineOptions::output_asy()) {
	  Graphics::earlydeadend_to_asy(_ID, _runID, child_node_ID);
	}
	
	++_earlydeadendcount;
	continue;
      }

      // check for equivalent colex-greater subsets:
      bool is_new = true;
      critical_element_data_type new_critelem_data;
      size_type new_stabilizer_card;
      if (CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_switch_tables() || CommandlineOptions::use_naive_symmetries()) {
	// use the method of TOPCOM-1.0.0:
	is_new = !old_symmetry_class_incrementalcheck(next_subset);
      }
      else {
	// use the method of TOPCOM-1.0.1:
	is_new = current_node.child_is_colexmax(i, &new_critelem_data, &new_stabilizer_card);
      }

      if (!is_new) {

	// for asy output:
	if (CommandlineOptions::output_asy()) {
	  Graphics::notnew_to_asy(_ID, _runID, child_node_ID);
	}	
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_subset << " was found already: continue" << std::endl;
	}
	continue;
      }
      else {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_subset << " was not found already: process" << std::endl;
	}
      }

      if (depth < _rank - 1) {
	// the set newbasis does not yet span a hyperplane:
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_subset << " not yet spanning - entering recursion ..." << std::endl;
	}
	bool continue_work(true);
	if (CommandlineOptions::workbuffercontrol()) {
	  if (*this->_current_workbuffersizeptr < CommandlineOptions::min_workbuffersize()) {
	    continue_work = false;
	  }
	}
	else {
	  if (this->_nodecount > _node_budget) {
	    continue_work = false;
	  }
	}
	node_type next_node(_symmetriesptr,
			    next_subset,
			    next_matrix,
			    new_critelem_data,
			    new_stabilizer_card);
	if (continue_work) {
	  _dfs(next_node, cnt + 1, depth);
	}
	else {
	  _open_nodes.push_back(next_node);
	}
      }
      else {
	// the set newbasis is independent and spans a hyperplane:
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_subset << " is spanning a hyperplane" << std::endl;
	  std::cerr << "cocircuit found with zeroset size at least " << next_subset.card()
		    << " in recursion depth " << depth << std::endl;
	}
	// Cocircuit checkcocircuit(Chirotope(*_pointsptr, false), next_subset);
	// we find the coefficients of the dependency in the final column of the
	// transformation matrix associated with next:
	Cocircuit newcocircuit;
	bool is_degenerate = false;
	subset_type result_subset(next_subset);
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "computing signature ..." << std::endl;
	}
	for (size_type col = 0; col < _no; ++col) {
	  if (result_subset.contains(col)) {
	    continue;
	  }
	  const Field coefficient = next_matrix.valuation(_pointsptr->col(col));
	  if (coefficient == FieldConstants::ZERO) {
	    // more points lie on the hyperplane spanned by newbasis:
	    is_degenerate = true;
	    result_subset += col;
	  }
	  if (coefficient > FieldConstants::ZERO) {
	    newcocircuit.first += col;
	  }
	  else if (coefficient < FieldConstants::ZERO) {
	    newcocircuit.second += col;
	  }
	}
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... done: cocircuit = " << newcocircuit << std::endl;
	}
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << result_subset << " now equals zero set of cocircuit" << std::endl;
	}
	// make a preliminary search in _result_set
	// in order to possibly save the expensive symmetry check:
	if (is_degenerate && (_result_set.find(result_subset) != _result_set.end())) {

	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::deadend_to_asy(_ID, _runID, child_node_ID);
	  }
	  ++_deadendcount;
	  continue;
	}
	if (is_degenerate && old_symmetry_class_fullcheck(result_subset)) {

	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::deadend_to_asy(_ID, _runID, child_node_ID);
	  }
	  ++_deadendcount;
	}
	else {
	  if (is_degenerate) {
	    // unfortunately, in the degenerate case we need to lock this section
	    // because of the shared _result_set:
	    // repeat the search, since _result_set may have
	    // received new elements in the meantime:
	    std::lock_guard<std::mutex> lock(_result_set_mutex);
	    if (_result_set.find(result_subset) != _result_set.end()) {

	      // for asy output:
	      if (CommandlineOptions::output_asy()) {
		Graphics::deadend_to_asy(_ID, _runID, child_node_ID);
	      }
	      ++_deadendcount;
	      continue;
	    }
	    else {
	      _result_set.insert(result_subset);
	    }
	  }
	  if (_print_objects) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cout << newcocircuit << '\n';
	  }
	  
	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::solution_to_asy(_ID, _runID, child_node_ID);
	  }	

	  if (!CommandlineOptions::skip_orbitcount()) {
	    if (CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_naive_symmetries()) {
	      _totalcount += (_symmetriesptr->size() + 1) / (_symmetriesptr->stabilizer_card(next_subset) + 1);
	    }
	    else if (CommandlineOptions::use_switch_tables() ) {
	      _totalcount += _switch_tableptr->orbit_size(next_subset);
	    }
	    else {
	      _totalcount += (_symmetriesptr->size() + 1) / (new_stabilizer_card + 1);
	    }
	  }
	  ++_symcount;
	}
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done." << std::endl;
    }
    --depth;
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "downsizing progress vector to " << depth << " ..." << std::endl;
    }
    _progress_vector.resize(depth);
  }


  // if the template parameter is "cocircuits", we enumerate cocircuits
  // without saving the found canonical representatives in result_set;
  // this requires to keep the complete zero-set of an affine subspace in the enumeration nodes:
  template <>
  inline void SymmetricSubsetGraph<cocircuits>::_dfs(const node_type&     current_node,
						     const parameter_type start,
						     parameter_type&      depth) {
    static const bool check_maximality_early = false;
    static const bool check_subset_not_monotonically_extendable = !CommandlineOptions::no_extension_check();
    static const bool check_subset_not_monotonically_extendable_early = CommandlineOptions::extension_check_first();
    
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "processing node " << this->_nodecount
		<< " corresponding to subset " << current_node.subset() << " by all possible one-element extensions:" << std::endl;
    }
    const parameter_type current_rank = current_node.matrix().rank();
    
    // for asy output:
    size_type parent_node_ID = this->_nodecount;

    // if the current subset has corank 1,
    // it is a candidate for a cocircuit,
    // and we can check for maximality either right away
    // or later:
    if (check_maximality_early) {
      if (current_rank == _rank - 1) {
      
	// the set current_subset spans a hyperplane, but is it maximal?
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << current_node.subset() << " is spanning a hyperplane" << std::endl;
	  std::cerr << "cocircuit found with zeroset size at least " << current_node.subset().card()
		    << " in recursion depth " << depth << std::endl;
	}
	
	// we find the signature of the cocircuit by the valuation form of the determinant
	// of the zero-set of the hyperplane spanned by current_node.subset():
	Cocircuit newcocircuit;
	bool is_maximal = true;
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "computing signature ..." << std::endl;
	}
	for (size_type col = 0; col < _no; ++col) {
	  if (current_node.subset().contains(col)) {
	    continue;
	  }
	  const Field value = current_node.matrix().valuation(_pointsptr->col(col));
	  if (value == FieldConstants::ZERO) {

	    // more points lie on the hyperplane spanned by newbasis:
	    is_maximal = false;
	    break;
	  }
	  if (value > FieldConstants::ZERO) {
	    newcocircuit.first += col;
	  }
	  else if (value < FieldConstants::ZERO) {
	    newcocircuit.second += col;
	  }
	}
	if (is_maximal) {

	  // no further points on the hyperplane, i.e., we have found a cocircuit:
	  if (_print_objects) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cout << newcocircuit << '\n';
	  }
	
	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::solution_to_asy(_ID, _runID, parent_node_ID);
	  }
	
	  if (!CommandlineOptions::skip_orbitcount()) {
	    if (CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_naive_symmetries()) {
	      _totalcount += (_symmetriesptr->size() + 1) / (_symmetriesptr->stabilizer_card(current_node.subset()) + 1);
	    }
	    else if (CommandlineOptions::use_switch_tables() ) {
	      _totalcount += _switch_tableptr->orbit_size(current_node.subset());
	    }
	    else {
	      _totalcount += (_symmetriesptr->size() + 1) / (current_node.stabilizer_card() + 1);
	    }
	  }
	  ++_symcount;
	  return;
	}
      }
    }

    const size_type workload(_no - start);
    size_type doneload(0UL);
    ++depth;
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "upsizing progress vector to " << depth << " ..." << std::endl;
    }
    _progress_vector.resize(depth);

    // check for monotonical maximality:
    bool is_monotonically_maximal = true;

    // we traverse the possible extensions from right to left because
    // right-to-left lexorder is compatible with the
    // bit representations of LabelSets, while the left-to-right
    // lexorder is not;
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "_rank        = " << _rank << std::endl;
      std::cerr << "current_rank = " << current_rank << std::endl;
      std::cerr << "start        = " << start << std::endl;
      std::cerr << "loop min     = " << start << std::endl;
      std::cerr << "loop max     = " << _no - (_rank - current_rank - 1) << std::endl;
    }

    // after the addition of the next point, the following is the minimal
    // number of points that is still missing in a hyperplane-spanning set:
    parameter_type max_cnt = _no;
    if (current_rank + 2 < _rank) {

      // rank is too small for spanning a hyperplane after the addition of one element;
      // leave room for a sufficiently large no of elements to the right
      // (substraction is ok even if parameter_type is unsigned,
      // since _no is larger than _rank and thus than any rank difference):
      max_cnt -= (_rank - 2 - current_rank);
    }
    for (parameter_type cnt = start; cnt < max_cnt; ++cnt) {
      ++doneload;
      const parameter_type i = _no - cnt - 1;
      _progress_vector.at(depth - 1) = (100 * doneload) / workload;
      ++this->_nodecount;
      subset_type next_subset(current_node.subset() + i);

      // for asy output:
      size_type child_node_ID = this->_nodecount;
      
      if (CommandlineOptions::verbose()) {
	report_progress(std::cerr);
      }

      // generate the data for the new node:
      matrix_type next_matrix(current_node.matrix());
      next_matrix.augment(_pointsptr->col(i));
      const parameter_type next_rank = next_matrix.rank();
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	Matrix raw_matrix(current_node.matrix());
	raw_matrix.augment(_pointsptr->col(i));
	std::cerr << "matrix with new column:" << std::endl;
	raw_matrix.pretty_print(std::cerr);
	std::cerr << std::endl;
	std::cerr << "new strict staircase matrix:" << std::endl;
	next_matrix.pretty_print(std::cerr);
	std::cerr << std::endl;
      }

      if (CommandlineOptions::output_asy()) {

	Graphics::matrix_to_asy(_ID, _runID, next_subset, next_matrix);
	Graphics::arc_to_asy(_ID, _runID, parent_node_ID, child_node_ID);
      }

      // check whether the rank of the subset is too high for a hyperplane already:
      if (next_rank >= _rank) {
	
	// for asy output:
	if (CommandlineOptions::output_asy()) {
	  Graphics::earlydeadend_to_asy(_ID, _runID, child_node_ID);
	}
	++_earlydeadendcount;
	continue;
      }
      else {

	// from now on we know that new_subset spans a proper affine subspace,
	// and the current subset is not monotonically maximal:
	is_monotonically_maximal = false;
      }
      if (check_subset_not_monotonically_extendable && check_subset_not_monotonically_extendable_early) {
	  
	// in this pruning step,
	// we check first whether we are in a node that cannot be monotonically
	// extended to the zero set of a cocircuit:
	if (SymmetricSubsetGraph<cocircuits>::_semi_is_not_monotonically_extendable(next_subset, i, next_matrix, next_rank)) {
	
	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::earlydeadend_to_asy(_ID, _runID, child_node_ID);
	  }
	  ++_earlydeadendcount;
	  continue;
	}
      }
      
      // check for equivalent colex-greater subsets:
      bool is_new = true;

      // generate a local version of critical element data
      // that is filled by a method of the current node:
      critical_element_data_type new_critelem_data;
      size_type new_stabilizer_card;
      if (CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_switch_tables() || CommandlineOptions::use_naive_symmetries()) {
	// use the method of TOPCOM-1.0.0:
	is_new = !old_symmetry_class_incrementalcheck(next_subset);
      }
      else {
	// use the method of TOPCOM-1.0.1:
	is_new = current_node.child_is_colexmax(i, &new_critelem_data, &new_stabilizer_card);
      }

      if (!is_new) {

	// for asy output:
	if (CommandlineOptions::output_asy()) {
	  Graphics::notnew_to_asy(_ID, _runID, child_node_ID);
	}	
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_subset << " was found already: continue" << std::endl;
	}
	continue;
      }
      else {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_subset << " was not found already: process" << std::endl;
	}
      }

      if (next_rank == current_rank) {
	
	// the new point is in the affine subspace spanned by the previous points
	// and is therefore contained in all hyperplanes that contain the
	// previous points;
	// thus, it must be added to the zero set of the cocircuit,
	// and we can break the traversal through all other additional points:

	// for asy output:
	if (CommandlineOptions::output_asy()) {
	  Graphics::veryearlydeadend_to_asy(_ID, _runID, child_node_ID);
	}
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << next_subset << " spans the same affine subspace as "
		    << current_node.subset() << ": add, recurse, break" << std::endl;
	}
	if (_no - cnt > _rank - 1 - next_rank) {
	  node_type next_node(_symmetriesptr,
			      next_subset,
			      next_matrix,
			      new_critelem_data,
			      new_stabilizer_card);
	  _dfs(next_node, cnt + 1, depth);
	}
	break;
      }
      else if (next_rank < _rank) {

	// here, the new point increases the rank, but not beyond corank 1:
	if (_no - cnt > _rank - 1 - next_rank) {
	  if (check_subset_not_monotonically_extendable && !check_subset_not_monotonically_extendable_early) {
	  
	    // in this pruning step,
	    // we check first whether we are in a node that cannot be monotonically
	    // extended to the zero set of a cocircuit:
	    if (SymmetricSubsetGraph<cocircuits>::_semi_is_not_monotonically_extendable(next_subset, i, next_matrix, next_rank)) {
	      
	      // for asy output:
	      if (CommandlineOptions::output_asy()) {
		Graphics::earlydeadend_to_asy(_ID, _runID, child_node_ID);
	      }
	      ++_earlydeadendcount;
	      continue;
	    }
	  }
	  
	  // enough points left to reach the "target" rank of _rank - 1:
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << next_subset << " can be completed to rank " << _rank << ": entering recursion ..." << std::endl;
	  }
	  bool continue_work(true);
	  if (CommandlineOptions::workbuffercontrol()) {
	    if (*this->_current_workbuffersizeptr < CommandlineOptions::min_workbuffersize()) {
	      continue_work = false;
	    }
	  }
	  else {
	    if (this->_nodecount > _node_budget) {
	      continue_work = false;
	    }
	  }
	  node_type next_node(_symmetriesptr,
			      next_subset,
			      next_matrix,
			      new_critelem_data,
			      new_stabilizer_card);
	  if (continue_work) {
	    _dfs(next_node, cnt + 1, depth);
	  }
	  else {
	    _open_nodes.push_back(next_node);
	  }
	}
      }
    }

    // if the current subset could not be extended by any point
    // without exceeding the rank beyond _rank - 1,
    // then it is monotonically maximal; if it has corank 1,
    // it is a candidate for a cocircuit;
    // if it has not been checked in the beginning of this method,
    // we have to do it now:

    if (!check_maximality_early) {
      if (is_monotonically_maximal) {
	if (current_rank != _rank - 1) {
	  
	  // for asy output:
	  if (CommandlineOptions::output_asy()) {
	    Graphics::deadend_to_asy(_ID, _runID, parent_node_ID);
	  }
	  ++_deadendcount;
	}
	else {
	  
	  // the set current_subset spans a hyperplane in a monotonically maximal way:
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << current_node.subset() << " is spanning a hyperplane" << std::endl;
	    std::cerr << "cocircuit found with zeroset size at least " << current_node.subset().card()
		      << " in recursion depth " << depth << std::endl;
	  }
	  
	  // we find the signature of the cocircuit by the valuation form of the determinant
	  // of the zero-set of the hyperplane spanned by current_node.subset():
	  Cocircuit  newcocircuit;
	  bool is_maximal = true;
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "computing signature ..." << std::endl;
	  }
	  for (size_type col = 0; col < _no; ++col) {
	    if (current_node.subset().contains(col)) {
	      continue;
	    }
	    const Field value = current_node.matrix().valuation(_pointsptr->col(col));
	    if (value == FieldConstants::ZERO) {
	      
	      // more points lie on the hyperplane spanned by newbasis:
	      is_maximal = false;
	      break;
	    }
	    if (value > FieldConstants::ZERO) {
	      newcocircuit.first += col;
	    }
	    else if (value < FieldConstants::ZERO) {
	      newcocircuit.second += col;
	    }
	  }
	  if (is_maximal) {
	    
	    // no further points on the hyperplane, i.e., we have found a cocircuit:
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "cocircuit!" << std::endl;
	    }

	    if (_print_objects) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cout << newcocircuit << '\n';
	    }
	    
	    // for asy output:
	    if (CommandlineOptions::output_asy()) {
	      Graphics::solution_to_asy(_ID, _runID, parent_node_ID);
	    }
	    
	    if (!CommandlineOptions::skip_orbitcount()) {
	      if (CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_naive_symmetries()) {
		_totalcount += (_symmetriesptr->size() + 1) / (_symmetriesptr->stabilizer_card(current_node.subset()) + 1);
	      }
	      else if (CommandlineOptions::use_switch_tables() ) {
		_totalcount += _switch_tableptr->orbit_size(current_node.subset());
	      }
	      else {
		_totalcount += (_symmetriesptr->size() + 1) / (current_node.stabilizer_card() + 1);
	      }
	    }
	    ++_symcount;
	  }
	  else {
	    if (CommandlineOptions::debug()) {
	      std::lock_guard<std::mutex> lock(IO_sync::mutex);
	      std::cerr << "not maximal" << std::endl;
	    }
	    
	    // for asy output:
	    if (CommandlineOptions::output_asy()) {
	      Graphics::deadend_to_asy(_ID, _runID, parent_node_ID);
	    }
	    ++_deadendcount;
	  }
	}
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... done." << std::endl;
    }
    --depth;
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "downsizing progress vector to " << depth << " ..." << std::endl;
    }
    _progress_vector.resize(depth);
  }
  
  template <>
  inline bool SymmetricSubsetGraph<cocircuits>::_semi_is_not_monotonically_extendable(const subset_type& next_subset,
										      const parameter_type next_min_elem,
										      const matrix_type& next_matrix,
										      const parameter_type next_rank) {
    if (next_rank > _rank - 1) {
      return true;
    }
    else if (next_rank == _rank - 1) {

      // the following checks whether the hyperplane spanned by next_subset has been
      // spanned by a colex-larger subset already
      // (special case of the check below requiring less memory allocations/deallocations):
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "checking for containment in a known hyperplane spanned in earlier branch ..." << std::endl;
      }
      
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "computing signature for non-elements already processed ..." << std::endl;
      }
      for (parameter_type col = next_min_elem + 1; col < _no; ++col) {
	if (next_subset.contains(col)) {
	  continue;
	}
	const Field value = next_matrix.valuation(_pointsptr->col(col));
	if (value == FieldConstants::ZERO) {
	  
	  // earlier points lie on the hyperplane spanned by next_subset:
	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "... found point of earlier branch on hyperplane - subset cannot be monotonically extendable" << std::endl;
	  }
	  return true;
	}
      }
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... found no point of earlier branch on hyperplane - subset might be monotonically extendable" << std::endl;
      }
      return false;
    }
    else { // next_rank < _rank - 1
      
      // the following checks whether the affine space spanned by next_subset has been
      // spanned by a colex-larger subset already:
      if (CommandlineOptions::debug()) {
      	std::lock_guard<std::mutex> lock(IO_sync::mutex);
      	std::cerr << "checking for containment in a known affine subspace spanned in earlier branch ..." << std::endl;
      }
      
      if (CommandlineOptions::debug()) {
      	std::lock_guard<std::mutex> lock(IO_sync::mutex);
      	std::cerr << "computing rank increase for non-elements already processed ..." << std::endl;
      }
      for (parameter_type col = next_min_elem + 1; col < _no; ++col) {
      	if (next_subset.contains(col)) {
      	  continue;
      	}
      	matrix_type check_rank_matrix = next_matrix;
      	check_rank_matrix.augment(_pointsptr->col(col));
      	if (check_rank_matrix.rank() == next_rank) {
	  
      	  // same affine subspace was discovered earlier;
      	  // thus, next_subset cannot be monotonically extended to a maximal subset:
      	  if (CommandlineOptions::debug()) {
      	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
      	    std::cerr << "... found earlier point in affine subspace - subset cannot be monotonically extendable" << std::endl;
      	  }
      	  return true;
      	}
      }
      if (CommandlineOptions::debug()) {
      	std::lock_guard<std::mutex> lock(IO_sync::mutex);
      	std::cerr << "... found no earlier point in affine subspace - subset might be monotonically extendable" << std::endl;
      }
      
      // this checks whether there is enough rank increase possible to the left of next_min_elem;      
      matrix_type rank_checker_matrix(next_matrix);
      for (parameter_type col = 0; col < next_min_elem; ++col) {
      	rank_checker_matrix.augment(_pointsptr->col(col));
	const parameter_type rank_checker_rank = rank_checker_matrix.rank();
	if (rank_checker_rank + next_min_elem - col < _rank - 1) {

	  // rank (_rank - 1) is already unreachable:
      	  if (CommandlineOptions::debug()) {
	    std::lock_guard<std::mutex> lock(IO_sync::mutex);
	    std::cerr << "not enough rank-increase left!" << std::endl;
	  }
	  return true;
	}
      	if (rank_checker_rank >= _rank - 1) {

	  // rank (_rank - 1) has been reached:
      	  return false;
      	}
      }

      // rank (_rank - 1) is unreachable with remaining columns:
      return true;
    }
  }


}; // namespace topcom

#endif

// eof SymmetricSubsetGraph.hh
