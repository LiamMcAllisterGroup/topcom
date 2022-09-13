////////////////////////////////////////////////////////////////////////////////
// 
// ComputeCocircuits.cc 
//
//    produced: 29/01/2020 jr
// last change: 29/01/2020 jr
//
////////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include "CommandlineOptions.hh"
#include "LabelSet.hh"
#include "Cocircuits.hh"
#include "Permutation.hh"
#include "Symmetry.hh"
#include "SymmetricSubsetGraphMaster.hh"

#include "ComputeCocircuits.hh"

namespace topcom {

  int ComputeCocircuits::run(const int flags) {

    const bool input_chiro       = flags & INPUT_CHIRO;
    const bool output_cocircuits = flags & OUTPUT_COCIRCUITS;
    const bool preprocess        = flags & PREPROCESS;
    bool from_chiro              = flags & FROM_CHIRO;

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
      std::cerr << "computing cocircuits of a point configuration up to symmetry\n";
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
      if (CommandlineOptions::verbose()) {
	std::cerr << "read point configuration with " << points.no()
		  << " points in rank " << points.rank() << std::endl;
      }    
      chiro = Chirotope(points, preprocess);
    }
    size_type no(chiro.no());
    size_type rank(chiro.rank());
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

      // prefix for output:
      if (output_cocircuits) {
	std::cout << chiro.no() << ',' << chiro.rank() << ":\n{\n";
      }
      
      // effort estimation:
      size_type no_of_spanningsets(_binomial(no, rank - 1));
      // counters:
    
      // use either as full storage or cache of already processed sets:
      const bool activate_cache(!CommandlineOptions::memopt() || (CommandlineOptions::localcache() > 0));
      spanning_sets_type support_sets;
      spanning_sets_type done_sets;
      if (CommandlineOptions::memopt()) {
	if (activate_cache) {
	  done_sets = spanning_sets_type(std::min<size_type>(CommandlineOptions::localcache(), no_of_spanningsets));
	}
      }
      else {
	done_sets = spanning_sets_type(no_of_spanningsets);
      }
    
      // use for storage of canonical representatives:
      spanning_sets_type canonical_support_sets;
      spanning_sets_type canonical_spanning_sets;
      spanning_sets_type orbit;
      Permutation dependent_perm(no, rank - 1);

      do {
	if (CommandlineOptions::verbose() && (++iterations % CommandlineOptions::report_frequency() == 0)) {
	  std::cerr << count << " symmetry classes --- "
		    << total_count << " total cocircuits ("
		    << iterations << " out of "
		    << no_of_spanningsets << " " << rank - 1 << "-sets investigated [cache load: "
		    << done_sets.size() << ", cache size: "
		    << done_sets.bucket_count() << "])"
		    << std::endl;
	}
	if (done_sets.size() == no_of_spanningsets) {
	  break;
	}
	spanning_set_type spanning_set(dependent_perm);
	if (CommandlineOptions::debug()) {
	  std::cerr << "investigating " << spanning_set << " ..." << std::endl;
	}
	if ((activate_cache) && (done_sets.find(spanning_set) != done_sets.end())) {
	  // already considered and stored/cached:
	  continue;
	}
	spanning_set_type canonical_spanning_set = spanning_set;
	bool old_spanning_set = false;
	for (SymmetryGroup::const_iterator iter = symmetries.begin();
	     iter != symmetries.end();
	     ++iter) {
	  // canonicalize the dependent set:
	  const Symmetry& g(*iter);
	  const spanning_set_type equivalent_spanning_set(g.map(spanning_set));
	  if (done_sets.find(equivalent_spanning_set) != done_sets.end()) {
	    // symmetric variant of spanning_set has been stored/cached as done:
	    if (CommandlineOptions::debug()) {
	      std::cerr << equivalent_spanning_set << " has been investigated already." << std::endl;
	    }
	    old_spanning_set = true;
	    break;
	  }
	  else if (canonical_spanning_sets.find(equivalent_spanning_set) != canonical_spanning_sets.end()) {
	    // symmetric variant of spanning_set has already been investigated:
	    old_spanning_set = true;
	    break;
	  }
	}
	if (old_spanning_set) {
	  // nothing new:
	  continue;
	}
	// else add the canonicalized dependent set to the set of already considered sets:
	canonical_spanning_sets.insert(canonical_spanning_set);
      
	// store/cache the dependent set as done:
	if (activate_cache) {
	  if (CommandlineOptions::memopt()) {
	    if (CommandlineOptions::localcache() > 0) {
	      if (CommandlineOptions::localcache() > done_sets.size()) {
		done_sets.insert(spanning_set);
		// done_sets.erase_random();
	      }
	    }
	  }
	  else {
	    done_sets.insert(spanning_set);
	  }
	}
	// store/cache all equivalent dependent sets:
	for (SymmetryGroup::const_iterator iter = symmetries.begin();
	     iter != symmetries.end();
	     ++iter) {
	  
	  // canonicalize the support set:
	  const Symmetry& g(*iter);
	  const spanning_set_type equivalent_spanning_set(g.map(spanning_set));
	  if (activate_cache) {
	    if (CommandlineOptions::memopt()) {
	      if (CommandlineOptions::localcache() > 0) {
		if (CommandlineOptions::localcache() > done_sets.size()) {
		  done_sets.insert(equivalent_spanning_set);
		  // done_sets.erase_random();
		}
	      }
	    }
	    else {
	      done_sets.insert(equivalent_spanning_set);
	    }
	  }
	}
	Cocircuit cocircuit(chiro, spanning_set);
	if (!cocircuit.first.empty() || !cocircuit.second.empty()) {
	  if (CommandlineOptions::debug()) {
	    std::cerr << "computing cocircuit support (i.e., 0-set) for " << spanning_set << " ..." << std::endl;
	  }
	  spanning_set_type support_set(0, no);
	  support_set -= cocircuit.first;
	  support_set -= cocircuit.second;
	  bool old_support_set = false;
	  spanning_set_type canonical_support_set = support_set;
	  if (support_sets.find(support_set) != support_sets.end()) {
	    
	    // the cocircuit support is in the storage/cache:
	    old_support_set = true;
	  }
	  else if (canonical_support_sets.find(support_set) != canonical_support_sets.end()) {
	    
	    // the cocircuit support was found already:
	    old_support_set = true;
	  }
	  else {
	    for (SymmetryGroup::const_iterator iter = symmetries.begin();
		 iter != symmetries.end();
		 ++iter) {
	      
	      // canonicalize the support set:
	      const Symmetry& g(*iter);
	      const spanning_set_type equivalent_support_set(g.map(support_set));
	      if (support_sets.find(equivalent_support_set) != support_sets.end()) {
		
		// a symmetric image of the cocircuit support is in the storage
		old_support_set = true;
		break;
	      }
	      else if (canonical_support_sets.find(equivalent_support_set) != canonical_support_sets.end()) {
		// a symmetric image of the cocircuit support was found already
		old_support_set = true;
		break;
	      }
	    }
	  }
	  if (!old_support_set) {
	    if (output_cocircuits) {
	      std::cout << cocircuit << '\n';
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
	      const spanning_set_type equivalent_support_set(g.map(support_set));
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
	std::cerr << count << " symmetry classes --- " << total_count << " total cocircuits." << std::endl;
      }
      if (!output_cocircuits) {
	std::cout << count << std::endl;
      }
      else {
	std::cout << "}" << std::endl;
      }
      return 0;
    }
    else {
      // if we know the points, we can enumerate cocircuits in a smarter way:
      SymmetricSubsetGraphMaster<cocircuits> ssgm(points.no(),
						  points.rank(),
						  points,
						  symmetries,
						  output_cocircuits,
						  true); 
      const Integer uniform_number = binomial(Integer(points.no()), Integer(points.rank() - 1));
      if (CommandlineOptions::verbose()) {
	std::cerr << ssgm.symcount() << " symmetry classes";
	if (!CommandlineOptions::skip_orbitcount()) {
	  std::cerr << " | " << ssgm.totalcount() << " total cocircuits";
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
      if (!output_cocircuits) {
	std::cout << ssgm.symcount() << std::endl;
      }
      return 0;
    }
  }

}; // namespace topcom
	
// eof
