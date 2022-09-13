////////////////////////////////////////////////////////////////////////////////
// 
// ComputeCircuits.cc 
//
//    produced: 16/07/19 jr
// last change: 29/01/2020 jr
//
////////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include "CommandlineOptions.hh"
#include "LabelSet.hh"
#include "Circuits.hh"
#include "Permutation.hh"
#include "Symmetry.hh"
#include "SymmetricSubsetGraphMaster.hh"

#include "ComputeCircuits.hh"

#ifndef STL_CONTAINERS
#include "PlainHashSet.hh"
#include "HashSet.hh"
namespace topcom {
  typedef PlainHashSet<dependent_set_type> dependent_sets_type;
};
#else
#include <unordered_set>
namespace topcom {
  typedef std::unordered_set<dependent_set_type, Hash<dependent_set_type> > dependent_sets_type;
};
#endif

namespace topcom {

  int ComputeCircuits::run(const int flags) {

    const bool input_chiro     = flags & INPUT_CHIRO;
    const bool output_circuits = flags & OUTPUT_CIRCUITS;
    const bool preprocess      = flags & PREPROCESS;
    bool from_chiro            = flags & FROM_CHIRO;

    if (CommandlineOptions::verbose()) {
      std::cerr << std::endl;
      std::cerr << "------------------------------------------------------------\n";
      std::cerr << "------------------ " << PACKAGE << " VERSION " << VERSION << " -------------------\n";
      std::cerr << "Triangulations of Point Configurations and Oriented Matroids\n";
      std::cerr << "--------------------- by Joerg Rambau ----------------------\n";
      std::cerr << "------------------------------------------------------------\n";
      std::cerr << std::endl;
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << std::endl;
      std::cerr << "------------------------------------------------------------\n";
      std::cerr << " computing circuits of a point configuration up to symmetry \n";
      std::cerr << "------------------------------------------------------------\n";
      std::cerr << std::endl;
#ifdef STL_CONTAINERS
      std::cerr << "        -- using STL containers for hash tables --" << std::endl;
#endif
#ifdef STL_SYMMETRIES
      std::cerr << "        -- using STL containers for symmetries  --" << std::endl;
#endif
      std::cerr << std::endl;
    }
    Chirotope chiro;
    PointConfiguration points;
    if (input_chiro) {
      from_chiro = true; // no way to use the direct computation method here
      if (!chiro.read_string(std::cin)) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "error while reading chirotope." << std::endl;
	}
	return 1;
      }
      if (CommandlineOptions::verbose()) {
	std::cerr << "read chirotope with " << chiro.no()
		  << " elements in rank " << chiro.rank() << std::endl;
      }    
    }
    else {
      if (!points.read(std::cin)) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "error while reading point configuration." << std::endl;
	}
	return 1;
      }
      points.transform_to_full_rank();
      if ((points.no() < 2) || (points.rank() < 2)) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "no of points and rank must be at least two." << std::endl;
	}
	return 1;
      }
      if (points.rank() > points.no()) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "rank must not be larger than no of points." << std::endl;
	}
	return 1;
      }
      chiro = Chirotope(points, preprocess);
      if (CommandlineOptions::verbose()) {
	std::cerr << "read point configuration with " << points.no()
		  << " points in rank " << points.rank() << std::endl;
      }
    }
    size_type no(chiro.no());
    size_type rank(chiro.rank());
    if (no == rank) {
      if (CommandlineOptions::debug()) {
	std::cerr << 0 << " circuits in total." << std::endl;
      }
      std::cout << 0 << std::endl;
      return 0;
    }
    SymmetryGroup symmetries(no);
    if (!CommandlineOptions::ignore_symmetries()) {
      if (CommandlineOptions::use_switch_tables()) {
      // if (false) {
	if (symmetries.read_generators(std::cin)) {
	  std::cerr << "read " << symmetries.generators().size()
		    << " generators" << std::endl;
	  if (CommandlineOptions::debug()) {
	    std::cerr << "symmetries.generators():" << std::endl;
	    std::cerr << symmetries.generators() << std::endl;
	  }
	}
      }
      else {
	if (symmetries.read(std::cin)) {
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "read symmetry group with " << symmetries.generators().size()
		      << " generators of order " << symmetries.size() + 1 << std::endl;
	    if (CommandlineOptions::debug()) {
	      std::cerr << "symmetries:" << std::endl;
	      std::cerr << symmetries << std::endl;
	    }
	  }
	}
      }
    }
    else {
      if (CommandlineOptions::verbose()) {
	std::cerr << "no valid symmetry generators found." << std::endl;
      }
    }
    if (CommandlineOptions::preprocess_points()) {
      Symmetry transformation(points.no());
      points.preprocess(transformation);
      symmetries.transform(transformation);
    }
    size_type iterations(0);
    size_type count(0);
    size_type total_count(0);
    size_type node_count(0);
    if (from_chiro) {
      // effort estimation:
      size_type no_of_dependentsets(_binomial(no, rank+1));
      // counters:
    
      // use either as full storage or cache of already processed sets:
      const bool activate_cache(!CommandlineOptions::memopt() || (CommandlineOptions::localcache() > 0));
      dependent_sets_type support_sets;
      dependent_sets_type done_sets;
      if (CommandlineOptions::memopt()) {
	if (activate_cache) {
	  done_sets = dependent_sets_type(std::min<size_type>(CommandlineOptions::localcache(), no_of_dependentsets));
	}
      }
      else {
	done_sets = dependent_sets_type(no_of_dependentsets);
      }
    
      // use for storage of canonical representatives:
      dependent_sets_type canonical_support_sets;
      dependent_sets_type canonical_dependent_sets;
      dependent_sets_type orbit;
      Permutation dependent_perm(no, rank + 1);
    
      do {
	if (CommandlineOptions::verbose() && (++iterations % CommandlineOptions::report_frequency() == 0)) {
	  std::cerr << count << " symmetry classes --- "
		    << total_count << " total circuits ("
		    << iterations << " out of "
		    << no_of_dependentsets << " " << rank + 1 << "-sets investigated [cache load: "
		    << done_sets.size() << ", cache size: "
		    << done_sets.bucket_count() << "])"
		    << std::endl;
	}
	if (done_sets.size() == no_of_dependentsets) {
	  break;
	}
	dependent_set_type dependent_set(dependent_perm);
	if (CommandlineOptions::debug()) {
	  std::cerr << "investigating " << dependent_set << " ..." << std::endl;
	}
	if ((activate_cache) && (done_sets.find(dependent_set) != done_sets.end())) {
	  // already considered and stored/cached:
	  continue;
	}
	dependent_set_type canonical_dependent_set = dependent_set;
	bool old_dependent_set = false;
	for (SymmetryGroup::const_iterator iter = symmetries.begin();
	     iter != symmetries.end();
	     ++iter) {
	  // canonicalize the dependent set:
	  const Symmetry& g(*iter);

	  // if there is a symmetry mapping this dependent set to something lexicographically smaller,
	  // then we have had an equivalent dependent set already:
	  if (g.lex_decreases(dependent_set)) {
	    if (CommandlineOptions::debug()) {
	      std::cerr << g.map(dependent_set) << " has been investigated already." << std::endl;
	    }
	    old_dependent_set = true;
	    break;
	  }	  
	
	  // const dependent_set_type equivalent_dependent_set(g.map(dependent_set));
	  // if (done_sets.find(equivalent_dependent_set) != done_sets.end()) {
	  //   // symmetric variant of dependent_set has been stored/cached as done:
	  //   if (CommandlineOptions::debug()) {
	  //     std::cerr << equivalent_dependent_set << " has been investigated already." << std::endl;
	  //   }
	  //   old_dependent_set = true;
	  //   break;
	  // }
	  // else if (canonical_dependent_sets.find(equivalent_dependent_set) != canonical_dependent_sets.end()) {
	  //   // symmetric variant of dependent_set has already been investigated:
	  //   old_dependent_set = true;
	  //   break;
	  // }
	}
	if (old_dependent_set) {
	  // nothing new:
	  continue;
	}
	// else add the canonicalized dependent set to the set of already considered sets:
	canonical_dependent_sets.insert(canonical_dependent_set);
      
	// store/cache the dependent set as done:
	if (activate_cache) {
	  if (CommandlineOptions::memopt()) {
	    if (CommandlineOptions::localcache() > 0) {
	      if (CommandlineOptions::localcache() > done_sets.size()) {
		done_sets.insert(dependent_set);
		// done_sets.erase_random();
	      }
	    }
	  }
	  else {
	    done_sets.insert(dependent_set);
	  }
	}
	// store/cache all equivalent dependent sets:
	for (SymmetryGroup::const_iterator iter = symmetries.begin();
	     iter != symmetries.end();
	     ++iter) {
	  // canonicalize the support set:
	  const Symmetry& g(*iter);
	  const dependent_set_type equivalent_dependent_set(g.map(dependent_set));
	  if (activate_cache) {
	    if (CommandlineOptions::memopt()) {
	      if (CommandlineOptions::localcache() > 0) {
		if (CommandlineOptions::localcache() > done_sets.size()) {
		  done_sets.insert(equivalent_dependent_set);
		  // done_sets.erase_random();
		}
	      }
	    }
	    else {
	      done_sets.insert(equivalent_dependent_set);
	    }
	  }
	}
	Circuit circuit(chiro, dependent_set);
	if (!circuit.first.empty() || !circuit.second.empty()) {
	  dependent_set_type support_set(circuit.first);
	  if (CommandlineOptions::debug()) {
	    std::cerr << "computing circuit support for " << dependent_set << " ..." << std::endl;
	  }
	  support_set += circuit.second;
	  bool old_support_set = false;
	  dependent_set_type canonical_support_set = support_set;
	  if (support_sets.find(support_set) != support_sets.end()) {
	    // the circuit support is in the storage/cache:
	    old_support_set = true;
	  }
	  else if (canonical_support_sets.find(support_set) != canonical_support_sets.end()) {
	    // the circuit support was found already:
	    old_support_set = true;
	  }
	  else {
	    for (SymmetryGroup::const_iterator iter = symmetries.begin();
		 iter != symmetries.end();
		 ++iter) {
	      // canonicalize the support set:
	      const Symmetry& g(*iter);
	      const dependent_set_type equivalent_support_set(g.map(support_set));
	      if (support_sets.find(equivalent_support_set) != support_sets.end()) {
		// a symmetric image of the circuit support is in the storage
		old_support_set = true;
		break;
	      }
	      else if (canonical_support_sets.find(equivalent_support_set) != canonical_support_sets.end()) {
		// a symmetric image of the circuit support was found already
		old_support_set = true;
		break;
	      }
	    }
	  }
	  if (!old_support_set) {
	    if (output_circuits) {
	      std::cout << "C[" << count << "] := " << circuit << ";" << std::endl;
	    }
	    // add the canonicalized support set to the set of already found canonical support sets:
	    canonical_support_sets.insert(canonical_support_set);
	    // insert into support sets:
	    if (activate_cache) {
	      if (CommandlineOptions::memopt()) {
		if (CommandlineOptions::localcache() > 0) {
		  if (CommandlineOptions::localcache() > support_sets.size()) {
		    support_sets.insert(support_set);
		    // support_sets.erase_random();
		  }
		}
	      }
	      else {
		support_sets.insert(support_set);
	      }
	    }	  
	    // we now need to count the number of support sets in the orbit:
	    orbit.insert(support_set);
	    for (SymmetryGroup::const_iterator iter = symmetries.begin();
		 iter != symmetries.end();
		 ++iter) {
	      // store all equivalent support sets:
	      const Symmetry& g(*iter);
	      const dependent_set_type equivalent_support_set(g.map(support_set));
	      orbit.insert(equivalent_support_set);
	      // store/cache all equivalent support sets as found:
	      if (activate_cache) {
		if (CommandlineOptions::memopt()) {
		  if (CommandlineOptions::localcache() > 0) {
		    if (CommandlineOptions::localcache() > support_sets.size()) {
		      support_sets.insert(equivalent_support_set);
		      // support_sets.erase_random();
		    }
		  }
		}
		else {
		  support_sets.insert(equivalent_support_set);
		}
	      }
	    }
	    total_count += orbit.size();
	    count += 1;
	    orbit.clear();
	  }
	}
	if (CommandlineOptions::debug()) {
	  std::cerr << "... done." << std::endl;
	}
      } while (dependent_perm.lexnext());
      if (CommandlineOptions::verbose()) {
	std::cerr << count << " symmetry classes";
	if (!CommandlineOptions::skip_orbitcount()) {
	  std::cerr << " | " << total_count << " total circuits";
	}
	std::cerr << "." << std::endl;
      }
      if (!output_circuits) {
	std::cout << count << std::endl;
      }
      return 0;
    }
    else {
      // if we know the points, we can enumerate circuits in a smarter way:
      SymmetricSubsetGraphMaster<circuits> ssgm(points.no(),
						points.rank(),
						points,
						symmetries,
						output_circuits,
						false);
      const Integer uniform_number = binomial(Integer(points.no()), Integer(points.rank() + 1));
      if (CommandlineOptions::verbose()) {
	std::cerr << ssgm.symcount() << " symmetry classes";
	if (!CommandlineOptions::skip_orbitcount()) {
	  std::cerr << " | " << ssgm.totalcount() << " total circuits";
	}
	std::cerr << " using " << ssgm.nodecount() << " nodes";
	std::cerr << " | " << ssgm.deadendcount() << " branching deadends";
	std::cerr << " | " << ssgm.earlydeadendcount() << " early detected deadends";
	std::cerr << "." << std::endl;
	std::cerr << "(total number is " << ssgm.totalcount() * 100.0 / uniform_number.get_d()
		  << "% of uniform number = " << uniform_number
		  << ")";
	std::cerr << std::endl;
      }
      if (!output_circuits) {
	std::cout << ssgm.symcount() << std::endl;
      }
      return 0;
    }
  }

}; // namespace topcom

// eof
