//////////////////////////////////////////////////////////////////////////
// ClassifiedExtensionSymmetries.cc
// produced: 22/03/2020 jr
// last change: 22/03/2020 jr
/////////////////////////////////////////////////////////////////////////

#include <unordered_set>

#include "ContainerIO.hh"
#include "PermutationPartialComparator.hh"
#include "ClassifiedExtensionSymmetries.hh"

namespace topcom {
  
  ces_symmetryptr_iterdata ClassifiedExtensionSymmetries::emptyset;

  // constructors:

  ClassifiedExtensionSymmetries::ClassifiedExtensionSymmetries(const mode_type          mode,
							       const int                no_of_workers,
							       const SimplicialComplex& simplices,
							       const SymmetryGroup&     sg) :
    _mode(mode),
    _n(sg.n()),
    _no_of_workers(no_of_workers),
    _simplicesptr(&simplices),
    _symmetriesptr(&sg),
    _worker_symmetryptrs(no_of_workers),
    _relevant_symmetryptrs(ces_relevant_symmetryptr_table_type()),
    _worker_relevant_symmetryptrs(no_of_workers, ces_relevant_symmetryptr_table_type()) {
  
    // collect relevant symmetries for label sets with min i and max j:
    if (CommandlineOptions::verbose()) {
      std::cerr << "... building relevant representatives ..." << std::endl;
    }
    size_type count(0UL);
    size_type report_frequency(CommandlineOptions::report_frequency() * CommandlineOptions::report_frequency());
    // initialize relevant symmetries with maximal simplex simp
    // strictly decreasing simp:
    for (SimplicialComplex::const_iterator simpiter = _simplicesptr->begin();
	 simpiter != _simplicesptr->end();
	 ++simpiter) {
      const Simplex& simp(*simpiter);
      for (SymmetryGroup::const_iterator symiter = sg.begin();
	   symiter != sg.end();
	   ++symiter) {
	const Symmetry* gptr(&(*symiter));

	if (mode == pointidx_mode) {
	  if (!gptr->decreases_simpidx(simp)) {
	    continue;
	  }
	}
	else if (mode == simpidx_mode) {
	  const parameter_type simpidx = SimplicialComplex::index_of_simplex(simp);
	  if ((*gptr)[simpidx] >= simpidx) {
	    continue;
	  }	  
	}
	else {
	  std::cerr << "ClassifiedExtensionSymmetries::ClassifiedExtensionSymmetries(const mode_type ...): unknown mode - exiting" << std::endl;
	  exit(1);
	}
	_relevant_symmetryptrs[simp].push_back(gptr);
      }
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "percentages of relevant symmetries:" << std::endl;
      write_relevant_symmetry_fractions(std::cerr);
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
      if (CommandlineOptions::verbose()) {
	std::cerr << "... distributing relevant representatives ..." << std::endl;
      }
      for (SimplicialComplex::const_iterator simpiter = _simplicesptr->begin();
	   simpiter != _simplicesptr->end();
	   ++simpiter) {
	const Simplex& simp(*simpiter);
	for (ces_symmetryptr_iterdata::const_iterator iter = _relevant_symmetryptrs[simp].begin();
	     iter != _relevant_symmetryptrs[simp].end();
	     ++iter) {
	  // distribute all symmetries in a round-robin fashion:
	  _worker_relevant_symmetryptrs[thread_no][simp].push_back(*iter);
	  thread_no = (thread_no + 1) % _no_of_workers;
	}
	for (int k = 0; k < _no_of_workers; ++k) {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "initializing worker " << k << " with " << _worker_relevant_symmetryptrs[k][simp].size()
		      << " symmetry pointers " << " lex-decreasing " << simp << " ..." << std::endl;
	  }
	}
      }
      if (CommandlineOptions::verbose()) {
	std::cerr << "... done" << std::endl;
      }
    }
  }

  // functionality:
  const bool ClassifiedExtensionSymmetries::lex_decreases_by_relevant_symmetries(const SimplicialComplex& sc,
										 const Simplex& max_simp) const {
    for (ces_symmetryptr_iterdata::const_iterator iter = relevant_symmetryptrs(max_simp).begin();
	 iter != relevant_symmetryptrs(max_simp).end();
	 ++iter) {
      const Symmetry& g(**iter);

      if (_mode == pointidx_mode) {
	if (g.lex_decreases(sc)) {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "found a permutation " << g << " lex-decreasing "
		      << sc << " to " << g.map(sc) << std::endl;
	  }
	  return true;
	}
      }
      else if (_mode == simpidx_mode) {
	if (g.lex_decreases(sc.index_set_pure())) {
	  if (CommandlineOptions::debug()) {
	    SimplicialComplex sc_image;
	    sc_image.replace_indexset(g.map(sc.index_set_pure()), sc.pure_rank());
	    std::cerr << "found a permutation " << g << " lex-decreasing "
		      << sc << " to " << sc_image << std::endl;
	  }
	  return true;
	}
      }
      else {
	std::cerr << "ClassifiedExtensionSymmetries::lex_decreases_by_relevant_symmetries(...): unknown mode - exiting" << std::endl;
	exit(1);
      }
    }
    if (CommandlineOptions::debug()) {
      std::cerr << "... no lex-decreasing symmetry detected" << std::endl;
    }
    return false;
  }

  std::ostream& ClassifiedExtensionSymmetries::write_relevant_symmetry_fractions(std::ostream& ost) const {
    for (SimplicialComplex::const_iterator simpiter = _simplicesptr->begin();
	 simpiter != _simplicesptr->end();
	 ++simpiter) {
      const Simplex& simp(*simpiter);
      ost << simp << "\t" << 100 * (relevant_symmetryptrs(simp).size() + 1) / (_symmetriesptr->size() + 1) << "%" << std::endl;
    }
    return ost;
  };

}; // namespace topcom

// eof ClassifiedExtensionSymmetries.cc
