////////////////////////////////////////////////////////////////////////////////
// 
// Admissibles.cc
//
//    produced: 19/04/98 jr
// last change: 19/04/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <string.h>
#include <thread>

#include "VertexFacetTable.hh"

#include "Permutation.hh"
#include "Admissibles.hh"

namespace topcom {
  const char    Admissibles::start_char = '[';
  const char    Admissibles::end_char   = ']';
  const char    Admissibles::delim_char = ',';
  const char*   Admissibles::map_chars = "->";


  void Admissibles::Worker::operator()(const SimplicialComplex& universe,
				       const VertexFacetTable&  vertexfacets,
				       const Circuits&          circuits) {
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "\t thread " << std::setw(2) << _ID << ": generating admissibles ..." << std::endl;
    }
    size_type count(0);
    for (SimplicialComplex::iterator sciter = _input.begin(); sciter != _input.end(); ++sciter) {
      const Simplex simp(*sciter);
      SimplicialComplex simp_admissibles(universe);
      for (Circuits::const_iterator iter = circuits.begin();
	   iter != circuits.end();
	   ++iter) {
#ifndef STL_CONTAINERS
	const Circuit& circuit(iter->data());
#else
	const Circuit& circuit(iter->second);
#endif
	if (circuit.first.subset(simp)) {
	  vertexfacets.remove_star(circuit.second, simp_admissibles);
	}
	if (circuit.second.subset(simp)) {
	  vertexfacets.remove_star(circuit.first, simp_admissibles);
	}
      }
      simp_admissibles -= simp;
      _output[simp] = simp_admissibles;
      if (CommandlineOptions::verbose() && (++count % CommandlineOptions::report_frequency() == 0)) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "\t thread " << std::setw(2) << _ID << ": " << count << " simplices processed so far." << std::endl;
      }
    }
    if (CommandlineOptions::verbose()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "\t ... thread " << _ID << " done" << std::endl;
    }
  }
  
  Admissibles::Admissibles(const Circuits& circuits,
			   const PointConfiguration& points,
			   const Chirotope& chiro,
			   const bool only_empty_simplices) : 
    admissibles_data() {
    if (CommandlineOptions::check_unimodular()) {
      if (points.empty()) {
	std::cerr << "Admissibles::Admissibles(const Circuits&, const PointConfiguration&, const Chirotope&, const bool):\n"
		  << " unimodularity only possible with points as input - exiting." << std::endl;
	exit(1);
      }
    }
    const parameter_type no(circuits.no());
    const parameter_type rank(circuits.rank());
    Field min_vol;
    bool have_first_vol = false;
    SimplicialComplex universe;
    Permutation perm(no, rank);
    if (CommandlineOptions::check_unimodular()) {
      // first, determine the minimal positive volume of a simplex:
      do {
	Simplex simp(perm);
	if (chiro(simp) != FieldConstants::ZERO) {
	  Field simp_vol(points.volume(simp));
	  if (!have_first_vol) {
	    min_vol = simp_vol;
	    have_first_vol = true;
	  }
	  else {
	    if (simp_vol < min_vol) {
	      min_vol = simp_vol;
	    }
	  }
	}
      } while (perm.lexnext());
      if (CommandlineOptions::verbose()) {
	std::cerr << "unimodularity check done with simplex volume " << min_vol << std::endl;
      }
    }
    perm = Permutation(no, rank);
    do {
      Simplex simp(perm);
      if (CommandlineOptions::check_unimodular() && (points.volume(simp) != min_vol)) {
	if (CommandlineOptions::debug()) {
	  std::cerr << "simplex " << simp << " is not unimodular: volume(simp) = " << points.volume(simp) << std::endl;
	}
	continue;
      }
      if (chiro[simp] != 0) {
	  bool is_empty(true);
	  if (only_empty_simplices) {
	    for (Circuits::const_iterator iter = circuits.begin();
		 iter != circuits.end();
		 ++iter) {
#ifndef STL_CONTAINERS
	      const Circuit circuit(iter->data());
#else
	      const Circuit circuit(iter->second);
#endif
	      if ( (simp.superset(circuit.first) && (circuit.second.card() == 1))
		   || (simp.superset(circuit.second) && (circuit.first.card() == 1)) ) {
		is_empty = false;
		break;
	      }
	    }
	  }
	  if (is_empty) {
	    universe += simp;
	  }
	}
      } while (perm.lexnext());
      if (CommandlineOptions::verbose()) {
	std::cerr << universe.card() << " valid simplices." << std::endl;
      }
      VertexFacetTable vertexfacets(universe);

      if (CommandlineOptions::parallel_enumeration()) {
	int local_no_of_threads = std::min<int>(CommandlineOptions::no_of_threads(), universe.card());
	std::vector<std::thread> local_threads(local_no_of_threads);
	std::vector<SimplicialComplex> thread_simplices(local_no_of_threads, SimplicialComplex());
	std::vector<admissibles_data> thread_results(local_no_of_threads, admissibles_data());
	std::vector<Worker> thread_workers;
    
	int thread_no = 0;

	// distribute simplices from the universe:
	for (SimplicialComplex::const_iterator iter = universe.begin();
	     iter != universe.end();
	     ++iter) {
      
	  // round-robin distribution:
	  thread_simplices[thread_no] += *iter;
	  thread_no = (thread_no + 1) % local_no_of_threads;
	}

	// generate workers:
	for (int i = 0; i < local_no_of_threads; ++i) {
	  thread_workers.emplace_back(i, thread_simplices[i]);
	}

	// generate threads:
	SimplicialComplex::start_multithreading();
	for (int i = 0; i < local_no_of_threads; ++i) {
	  local_threads[i] = std::thread(&Admissibles::Worker::operator(),
					 std::ref(thread_workers[i]),
					 std::ref(universe),
					 std::ref(vertexfacets),
					 std::ref(circuits));
	}
    
	// wait for threads to finish:
	for (int i = 0; i < local_no_of_threads; ++i) {
	  if (local_threads[i].joinable()) {
	    local_threads[i].join();
	  }
	}
	SimplicialComplex::stop_multithreading();

	// collect the results (not in the same order as the input!):
	for (int i = 0; i < local_no_of_threads; ++i) {
	  (*this).insert(thread_workers[i].output().begin(), thread_workers[i].output().end());
	}
      }
      else {
	size_type count(0);
	for (SimplicialComplex::iterator sciter = universe.begin(); sciter != universe.end(); ++sciter) {
	  const Simplex simp(*sciter);
	  SimplicialComplex simp_admissibles(universe);
	  for (Circuits::const_iterator iter = circuits.begin();
	       iter != circuits.end();
	       ++iter) {
#ifndef STL_CONTAINERS
	    const Circuit& circuit(iter->data());
#else
	    const Circuit& circuit(iter->second);
#endif
	    if (circuit.first.subset(simp)) {
	      vertexfacets.remove_star(circuit.second, simp_admissibles);
	    }
	    if (circuit.second.subset(simp)) {
	      vertexfacets.remove_star(circuit.first, simp_admissibles);
	    }
	  }
	  simp_admissibles -= simp;
	  (*this)[simp] = simp_admissibles;
	  if (CommandlineOptions::verbose() && (++count % CommandlineOptions::report_frequency() == 0)) {
	    std::cerr << count << " simplices processed so far." << std::endl;
	  }
	}
      }
    }
    // stream input/output:
    std::istream& Admissibles::read(std::istream& ist) {
      char dash;
      char arrow;
      char c;

      clear();
      ist >> std::ws >> c;
      if (c == start_char) {
	while (ist >> std::ws >> c) {
	  if (c == end_char) {
	    break;
	  }
	  if (c == delim_char) {
	    continue;
	  }
	  ist.putback(c);

	  // an entry starts here:
	  Simplex key_reader;
	  SimplicialComplex data_reader;
      
	  if (!(ist >> std::ws >> key_reader)) {
#ifdef READ_DEBUG
	    std::cerr << "std::istream& Admissibles::read(std::istream& ist): "
		      << "key simplex not found." << std::endl;
#endif
	    ist.clear(std::ios::failbit);
	    return ist;
	  }
	  if (!(ist >> std::ws >> dash >> arrow)) {
#ifdef READ_DEBUG
	    std::cerr << "std::istream& Admissibles::read(std::istream& ist): "
		      << '`' << map_chars << "' not found." << std::endl;
#endif
	    ist.clear(std::ios::failbit);
	    return ist;
	  }
	  if (!(ist >> std::ws >> data_reader)) {
#ifdef READ_DEBUG
	    std::cerr << "std::istream& Admissibles::read(std::istream& ist): "
		      << "data simplicial complex not found." << std::endl;
#endif
	    ist.clear(std::ios::failbit);
	    return ist;
	  }
	}
      }
      else {
#ifdef READ_DEBUG
	std::cerr << "std::istream& Admissibles::read(std::istream& ist): "
		  << "missing `" << start_char << "'." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
      ist.clear(std::ios::goodbit);
      return ist;
    }

    std::ostream& Admissibles::write(std::ostream& ost) const {
      size_type count = 0;

      ost << start_char;
      for (typename Admissibles::const_iterator iter = this->begin();
	   iter != this->end();
	   ++iter) {
#ifndef STL_CONTAINERS
	ost << iter->key() << map_chars << iter->data();
#else
	ost << iter->first << map_chars << iter->second;
#endif
	count++;
	if (count < size())
	  ost << delim_char;
      }
      ost << end_char;
      return ost;
    }
  
  }; // namespace topcom

// eof Admissibles.cc
