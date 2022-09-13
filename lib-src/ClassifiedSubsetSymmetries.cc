//////////////////////////////////////////////////////////////////////////
// ClassifiedSubsetSymmetries.cc
// produced: 17/03/2020 jr
// last change: 17/03/2020 jr
/////////////////////////////////////////////////////////////////////////

#include <iomanip>
#include <unordered_set>

#include "ContainerIO.hh"
#include "PermutationPartialComparator.hh"
#include "ClassifiedSubsetSymmetries.hh"

namespace topcom {

  // constructors:

  ClassifiedSubsetSymmetries::ClassifiedSubsetSymmetries(const parameter_type no_of_tables,
							 const int            no_of_workers,
							 const SymmetryGroup& sg) :
    _n(sg.n()),
    _no_of_tables(no_of_tables),
    _no_of_workers(no_of_workers),
    _symmetriesptr(&sg),
    _worker_symmetryptrs(no_of_workers),
    _relevant_symmetryptrs(css_relevant_symmetryptr_table_type(sg.n(),
							       std::vector<css_symmetryptr_iterdata>(no_of_tables,
												     css_symmetryptr_iterdata()))),
    _worker_relevant_symmetryptrs(no_of_workers, css_relevant_symmetryptr_table_type(sg.n(),
										     std::vector<css_symmetryptr_iterdata>(no_of_tables,
															   css_symmetryptr_iterdata()))),
    _overtaking_symmetry_exists(css_overtaking_symmetry_existence_type(sg.n(),
								       std::vector<bool>(sg.n(), false))),
    _compressing_symmetry_exists(css_compressing_symmetry_existence_type(sg.n(),
									 std::vector<std::vector<bool> >(sg.n(),
													 std::vector<bool>(sg.n(), false)))),
    _index_maxelem_table(no_of_tables),
    _maxelem_index_table(sg.n()) {
  
    for (int i = 0; i < _no_of_tables; ++i) {
      _index_maxelem_table[i] = max_elem(i);
    }
    for (int k = 0; k < _n; ++k) {
      _maxelem_index_table[k] = relevant_index(k);
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "building classified symmetries with " << _no_of_tables << " tables for each min elem ..." << std::endl;
    }
    if (CommandlineOptions::parallel_symmetries()) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "distributing symmetries over " << _no_of_workers << " workers ..." << std::endl;
      }
      int thread_no(0);
      for (symmetry_iterdata::const_iterator iter = _symmetriesptr->begin();
	   iter != _symmetriesptr->end();
	   ++iter) {
	// distribute all symmetries in a round-robin fashion:
	_worker_symmetryptrs[thread_no].push_back(&(*iter));
	thread_no = (thread_no + 1) % _no_of_workers;
      }
    }
  
    // collect relevant symmetries for label sets with min i and max j:
    if (CommandlineOptions::verbose()) {
      std::cerr << "... building relevant representatives ..." << std::endl;
    }

    size_type count(0UL);
    size_type report_frequency(CommandlineOptions::report_frequency() * CommandlineOptions::report_frequency());
    // initialize relevant symmetries with min i and max _n - 1
    // strictly increasing i:
    for (parameter_type i = 0; i < _n; ++i) {
      css_symmetryptr_collectordata essential_perms;
      for (SymmetryGroup::const_iterator iter = sg.begin();
	   iter != sg.end();
	   ++iter) {
	const Symmetry* gptr(&(*iter));
	if ((*gptr)[i] <= i) {
	  continue;
	}
	const PermutationPartialComparator p(*gptr, i, _n - 1);
	if (essential_perms.find(p) != essential_perms.end()) {
#ifdef COMPUTATIONS_DEBUG
	  std::cerr << *gptr << " not " << i << "-relevant" << std::endl;
#endif
	  continue;
	}
	_relevant_symmetryptrs[i][0].push_back(gptr);
#ifdef HASH_DEBUG
	if (i > 63) {
	  std::cerr << "hash function for p = " << p << " is " << essential_perms.hash_function()(p) << std::endl;
	}
#endif
	essential_perms.insert(p);
	if (CommandlineOptions::verbose() && (++count % report_frequency == 0)) {
	  std::cerr << count << " relevant symmetries assigned so far (current [min, max] = ["
		    << i << ", " << _n - 1 << "])" << std::endl;
	}
      }
    }
    // initialize relevant symmetries with min i and max corresponding to table with index j
    // using the result for i and _n - 1:
    for (parameter_type i = 0; i < _n; ++i) {
      for (parameter_type j = 1; j < _no_of_tables; ++j) {
	const parameter_type max = _index_maxelem_table[j];
	if (max < i) {
	  break;
	}
	css_symmetryptr_collectordata essential_perms;
	for (css_symmetryptr_iterdata::const_iterator iter = _relevant_symmetryptrs[i][j - 1].begin();
	     iter != _relevant_symmetryptrs[i][j - 1].end();
	     ++iter) {
	  const Symmetry* gptr(*iter);
	  const PermutationPartialComparator p(*gptr, i, max);
	  if (essential_perms.find(p) != essential_perms.end()) {
	    continue;
	  }
	  _relevant_symmetryptrs[i][j].push_back(gptr);
	  essential_perms.insert(p);
	  if (CommandlineOptions::verbose() && (++count % report_frequency == 0)) {
	    std::cerr << count << " relevant symmetries assigned so far (current [min, max] = ["
		      << i << ", " << max << "])" << std::endl;
	  }
	}
      }
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "percentages of relevant symmetries:" << std::endl;
      write_relevant_symmetry_fractions(std::cerr);
    }
    if (CommandlineOptions::parallel_symmetries()) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "... distributing relevant representatives ..." << std::endl;
      }
      int thread_no(0);
      for (size_type i = 0; i < _n; ++i) {
	for (size_type j = 0; j < _no_of_tables; ++j) {
	  const parameter_type max = _index_maxelem_table[j];
	  if (max < i) {
	    break;
	  }
	  for (css_symmetryptr_iterdata::const_iterator iter = _relevant_symmetryptrs[i][j].begin();
	       iter != _relevant_symmetryptrs[i][j].end();
	       ++iter) {
	    // distribute all symmetries in a round-robin fashion:
	    _worker_relevant_symmetryptrs[thread_no][i][j].push_back(*iter);
	    thread_no = (thread_no + 1) % _no_of_workers;
	  }
	  for (int k = 0; k < _no_of_workers; ++k) {
	    if (CommandlineOptions::debug()) {
	      std::cerr << "initializing worker " << k << " with " << _worker_relevant_symmetryptrs[k][i][j].size()
			<< " symmetry pointers " << " between " << i << " and " << max << " ..." << std::endl;
	    }
	  }
	}
      }
    }

    // collect overtaking symmetries shifting i beyond j:
    if (CommandlineOptions::verbose()) {
      std::cerr << "... building overtaking symmetries ..." << std::endl;
    }  
    size_type count_overtaking(0UL);
    for (parameter_type i = 0; i < _n - 1; ++i) {
      for (parameter_type j = i + 1; j < _n; ++j) {
	for (SymmetryGroup::const_iterator iter = sg.begin();
	     iter != sg.end();
	     ++iter) {
	
	  // extract relevant symmetries between i (included) and j (included):
	  const Symmetry& g(*iter);

	  // extract existence of overtaking symmetries
	  // with g[i] > j;
	  if (!_overtaking_symmetry_exists[i][j]) {
	    if (g[i] > j) {
	      _overtaking_symmetry_exists[i][j] = true;
	      ++count_overtaking;
	    }
	  }
	}
      }
    }

    // collect compressing symmetries shifting i beyond j while fixing everything starting at k:
    if (CommandlineOptions::verbose()) {
      std::cerr << "... building compressing symmetries ..." << std::endl;
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "number of overtaking elements: " << count_overtaking << std::endl;
    }

    // as an auxiliary structure, collect the
    // point-wise stabilizing symmetries starting at an element:
    if (CommandlineOptions::verbose()) {
      std::cerr << "... building point-wise stabilizers ..." << std::endl;
    }
    std::vector<css_symmetryptr_iterdata> pointwise_stabilizers(_n);
    for (SymmetryGroup::const_iterator iter = sg.begin();
	 iter != sg.end();
	 ++iter) {
      const Symmetry& g(*iter);
      if (g[_n - 1] == _n - 1) {
	pointwise_stabilizers[_n - 1].push_back(&g);
      }
    }
    for (parameter_type i = 1; i < _n - 1; ++i) {
      const parameter_type elem(_n - i - 1);
      for (css_symmetryptr_iterdata::const_iterator iter = pointwise_stabilizers[elem + 1].begin();
	   iter != pointwise_stabilizers[elem + 1].end();
	   ++iter) {
	const Symmetry& g(**iter);
	if (g[elem] == elem) {
	  pointwise_stabilizers[elem].push_back(&g);
	}
      }      
    }
    size_type count_compressing(0UL);
    for (parameter_type k = 3; k < _n; ++k) {
      for (css_symmetryptr_iterdata::const_iterator iter = pointwise_stabilizers[k].begin();
	   iter != pointwise_stabilizers[k].end();
	   ++iter) {
	for (parameter_type i = 0; i < k - 2; ++i) {
	  for (parameter_type j = i + 1; j < k - 1; ++j) {
	  
	    // extract existence of compressing symmetries
	    // with g[i] > j and g[l] = l for all l >= k:
	    const Symmetry& g(**iter);
	    if (g[i] > j) {
	      _compressing_symmetry_exists[i][j][k] = true;
	      ++count_compressing;
	    }
	  }
	}
      }
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "number of compressing elements: " << count_compressing << std::endl;
      std::cerr << "... done" << std::endl;
    }
  }

  // functionality:
  const bool ClassifiedSubsetSymmetries::colex_increases_by_compressing(const LabelSet& ls,
									const parameter_type max_elem,
									const parameter_type new_elem) const {
    LabelSet::const_iterator iter1 = ls.begin();
    ++iter1;
    LabelSet::const_iterator iter2 = iter1;
    if (iter2 != ls.end()) {
      ++iter2;
    }
    while (iter2 != ls.end()) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "checking for existence of a compression ..." << std::endl;
      }
      if (compressing_symmetry_exists(new_elem, *iter1, *iter2)) {
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "... premature colex-increasing based on compression detected" << std::endl;
	}
	return true;
      }
      iter1 = iter2;
      ++iter2;
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... no premature colex-increasing based on compression detected" << std::endl;
    }
    return false;
  }

  const bool ClassifiedSubsetSymmetries::colex_increases_by_overtaking(const LabelSet& ls,
								       const parameter_type max_elem,
								       const parameter_type new_elem) const {
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "checking for existence of overtaking ..." << std::endl;
    }
    if (overtaking_symmetry_exists(new_elem, max_elem)) {
      if (CommandlineOptions::debug()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "... premature colex-increasing based on overtaking detected" << std::endl;
      }
      return true;
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... no premature colex-increasing based on overtaking detected" << std::endl;
    }
    return false;
  }

  const bool ClassifiedSubsetSymmetries::colex_increases_by_relevant_symmetries(const LabelSet& ls,
										const parameter_type max_elem,
										const parameter_type new_elem) const {
#ifdef CLASSIFIED_DEBUG
    //////////////////////////////////////////////////////////////////////////////
    static size_type cnt_calls(0);
    static size_type avg_searchlen(0);
    static size_type avg_card(0);

    ++cnt_calls;
    size_type cur_searchlen(0);
    size_type cur_card(relevant_symmetryptrs(new_elem, max_elem).size());
    size_type cur_single_symtable_card(relevant_symmetryptrs(new_elem, _n - 1).size());
    avg_card = ((cnt_calls - 1) * avg_card + cur_card) / cnt_calls;
    {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
      std::cerr << "no compressing/overtaking symmetries exist: exhaustive search required:" << std::endl;
      std::cerr << "label set under investigation           \t: " << ls << std::endl;
      std::cerr << "average search card   so far            \t: " << std::setw(9) << avg_card << std::endl;
      std::cerr << "average search length so far            \t: " << std::setw(9) << avg_searchlen << std::endl;
      std::cerr << "current search card          (max = " << std::setw(3) << max_elem << ")\t: " << std::setw(9) << cur_card << std::endl;
      std::cerr << "current single-symtable card (max = " << std::setw(3) << _n - 1 <<   ")\t: " << std::setw(9) << cur_single_symtable_card << std::endl;
      std::cerr << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////
#endif
    const css_symmetryptr_iterdata& local_symmetryptrs(relevant_symmetryptrs(new_elem, max_elem));
    for (css_symmetryptr_iterdata::const_iterator iter = local_symmetryptrs.begin();
	 iter != local_symmetryptrs.end();
	 ++iter) {
      // for (symmetry_iterdata::const_iterator iter = _symmetriesptr->begin();
      //      iter != _symmetriesptr->end();
      //      ++iter) {
#ifdef CLASSIFIED_DEBUG
      //////////////////////////////////////////////////////////////////////////////
      ++cur_searchlen;
      //////////////////////////////////////////////////////////////////////////////
#endif
      const Symmetry& g(**iter);
      // const Symmetry& g(*iter);
      if (g[new_elem] > max_elem) {
#ifdef CLASSIFIED_DEBUG
	//////////////////////////////////////////////////////////////////////////////
	avg_searchlen = ((cnt_calls - 1) * avg_searchlen + cur_searchlen) / cnt_calls;
	{
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "true: average search length " << std::setw(9) << avg_searchlen
		    << "/" << avg_card << std::endl;
	}
	//////////////////////////////////////////////////////////////////////////////
#endif
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "found a permutation " << g << " shifting " << new_elem << " beyond the max of "
		    << ls << std::endl;
	}
	return true;
      }
      if (g.colex_increases(ls)) {
#ifdef CLASSIFIED_DEBUG
	//////////////////////////////////////////////////////////////////////////////
	avg_searchlen = ((cnt_calls - 1) * avg_searchlen + cur_searchlen) / cnt_calls;
	{
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "true: average search length " << std::setw(9) << avg_searchlen
		    << "/" << avg_card << std::endl;
	}
	//////////////////////////////////////////////////////////////////////////////
#endif
	if (CommandlineOptions::debug()) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "found a permutation " << g << " colex-increasing "
		    << ls << std::endl;
	}
	return true;
      }
    }
    if (CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "... no colex-increasing symmetry detected" << std::endl;
    }
#ifdef CLASSIFIED_DEBUG
    //////////////////////////////////////////////////////////////////////////////
    avg_searchlen = ((cnt_calls - 1) * avg_searchlen + cur_searchlen) / cnt_calls;
    {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "false: average search length " << std::setw(9) << avg_searchlen << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////
#endif
    return false;
  }

  std::ostream& ClassifiedSubsetSymmetries::write_relevant_symmetry_fractions(std::ostream& ost) const {
    ost << "  \\max ";
    for (parameter_type j = 0; j < _no_of_tables; ++j) {
      const parameter_type max_elem = ((_no_of_tables - j) * (_n - 1)) / _no_of_tables;
      ost << "\t" << max_elem;
    }
    ost << std::endl << "min\\   " << std::endl;
    for (parameter_type i = 0; i < _n; ++i) {
      ost << i;
      for (parameter_type j = 0; j < _no_of_tables; ++j) {
	const parameter_type max_elem = ((_no_of_tables - j) * (_n - 1)) / _no_of_tables;
	if (max_elem < i) {
	  break;
	}
	ost << "\t" << 100 * (_relevant_symmetryptrs[i][j].size() + 1) / (_symmetriesptr->size() + 1) << "%";
      }
      ost << std::endl;
    }
    return ost;
  };

}; // namespace topcom

// eof ClassifiedSubsetSymmetries.cc
