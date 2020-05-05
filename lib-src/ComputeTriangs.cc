////////////////////////////////////////////////////////////////////////////////
// 
// ComputeTriangs.cc
//
//    produced: 25 Nov 1999 jr
// last change: 25 nov 1999 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include "HashMap.hh"
#include "HashSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Permutation.hh"
#include "PointConfiguration.hh"
#include "Chirotope.hh"
#include "Circuits.hh"
#include "Facets.hh"
#include "Admissibles.hh"
#include "InteriorFacets.hh"
#include "PartialTriang.hh"
#include "CheckTriang.hh"
#include "Flip.hh"
#include "MarkedFlips.hh"
#include "TriangNode.hh"
#include "TriangFlips.hh"
#include "Symmetry.hh"
#include "FineTriang.hh"
#include "SymmetricBFS.hh"

#include "ComputeTriangs.hh"

void __dfs(const size_type no,
	   const size_type rank,
	   PartialTriang&  current_partial_triang,
	   size_type*      partial_count_ptr,
	   size_type*      count_ptr,
	   const bool      output_triangs) {
  SimplicialComplex forbidden;
  for (SimplicialComplex::const_iterator sciter 
	 = current_partial_triang.admissibles().begin();
       sciter != current_partial_triang.admissibles().end(); 
       ++sciter) {
    const Simplex new_simp(*sciter);
    forbidden += new_simp;
    PartialTriang next_partial_triang(PartialTriang(current_partial_triang, new_simp, forbidden));
    // new partial triang:
    ++(*partial_count_ptr);
    if (next_partial_triang.freefacets().is_empty()) {
      // complete triang:
      // output the triangulation:
      if (output_triangs) {
	std::cout << next_partial_triang << std::endl;
      }
      ++(*count_ptr);
      if (CommandlineOptions::verbose()) {
	if (*count_ptr % 10 == 0) {
	  std::cerr << *count_ptr << " triangulations so far." << std::endl;
	}
      }
    }
    else if (next_partial_triang.admissibles().is_empty()) {
      // dead end:
    }
    else {
      // partial triang:
      __dfs(no, rank, next_partial_triang, partial_count_ptr, count_ptr, output_triangs);
    }
  }
}

const size_type __no_of_all_triangs(const size_type       no,
				    const size_type       rank, 
				    const Admissibles&    admtable, 
				    const InteriorFacets& intfacets,
				    const bool            output_triangs) {
  PartialTriang root(no, rank, admtable, intfacets);
  if (CommandlineOptions::debug()) {
    std::cerr << "Root triangulation: " << root << std::endl;
    std::cerr << "Admissibles Table:  " << admtable << std::endl;
    std::cerr << "InteriorFacets:     " << intfacets << std::endl;
  }
  size_type count(0);
  size_type partial_count(0);
  size_type no_of_simplices(root.admissibles().card());
  size_type count_start_simplices(0);
  SimplicialComplex forbidden;
  for (SimplicialComplex::const_iterator sciter = root.admissibles().begin();
       sciter != root.admissibles().end();
       ++sciter) {
    const Simplex start_simp(*sciter);
    forbidden += start_simp;
    PartialTriang next_partial_triang(root, start_simp, forbidden);
    if (next_partial_triang.freefacets().is_empty()) {
      // complete triang:
      // output the triangulation:
      if (output_triangs) {
	std::cout << next_partial_triang << std::endl;
      }
      ++count;
      if (CommandlineOptions::verbose() && (count % 10 == 0)) {
	std::cerr << count << " triangulations so far." << std::endl;
      }
    }
    __dfs(no, rank, next_partial_triang, &partial_count, &count, output_triangs);
    if (CommandlineOptions::verbose()) {
      std::cerr << "done " << ++count_start_simplices 
	   << " start simplices out of " << no_of_simplices 
	   << '.' << std::endl;
    }
  }
  if (CommandlineOptions::verbose()) {
    std::cerr << partial_count << " partial triangulations visited in total." << std::endl;
  }
  return count;
}

const size_type __no_of_triangs(const parameter_type      no,
				const parameter_type      rank,
				const PointConfiguration& points,
 				const Chirotope&          chiro,
				const SymmetryGroup&      symmetries,
				const SimplicialComplex&  seed,
				const SymmetryGroup&      seed_symmetries,
				const bool                output_triangs,
				const bool                fine_only) {
  SymmetricBFS bfs(no, rank, points, chiro, 
		   symmetries, seed, seed_symmetries, 
		   output_triangs, fine_only);
  if (CommandlineOptions::verbose()) {
    std::cerr << bfs.symcount() << " symmetry classes." << std::endl;
    std::cerr << bfs.totalcount() << " triangulations in total." << std::endl;
  }
  return bfs.symcount();
}

int ComputeTriangs::run(const int flags) {

  const bool input_chiro    = flags & INPUT_CHIRO;
  const bool compute_all    = flags & COMPUTE_ALL;
  const bool fine_only      = flags & FINE_ONLY;
  const bool output_triangs = flags & OUTPUT_TRIANGS;
  const bool preprocess     = flags & PREPROCESS;
  const bool findflips      = flags & FINDFLIPS;
  const bool unimodular     = flags & UNIMODULAR;

  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "------------------------------------------------------------\n";
    std::cerr << "------------------ " << PACKAGE << " VERSION " << VERSION << " -------------------\n";
    std::cerr << "Triangulations of Point Configurations and Oriented Matroids\n";
    std::cerr << "--------------------- by Joerg Rambau ----------------------\n";
    std::cerr << "------------------------------------------------------------\n";
    std::cerr << std::endl;
  }
  if (input_chiro && CommandlineOptions::check_regular()) {
    std::cerr << "regularity check not possible if only chirotope is given; exiting." << std::endl;
    exit(1);
  }

  if (compute_all) {
    PointConfiguration points;
    Chirotope chiro;
    if (input_chiro) {
      if (chiro.read_string(std::cin)) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "read chirotope." << std::endl;
	}
      }
      else {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "error while reading chirotope." << std::endl;
	}
	return 1;
      }
    }  
    else {
      if (!points.read(std::cin)) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "error while reading point configuration." << std::endl;
	}
	return 1;
      }
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
      }
//     if (CommandlineOptions::verbose()) {
//       std::cerr << "computing dual chirotope ..." << std::endl;
//     }
//     Chirotope* dualptr = new Chirotope(chiro.dual());
//     if (CommandlineOptions::verbose()) {
//       std::cerr << "... done." << std::endl;
//     }
    size_type no(chiro.no());
    size_type rank(chiro.rank());
    if (CommandlineOptions::verbose()) {
      std::cerr << "computing circuits ..." << std::endl;
    }
    Circuits* circuitsptr = new Circuits(chiro);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
      std::cerr << "computing admissibles ..." << std::endl;
    }
    Admissibles admissibles(*circuitsptr, chiro, fine_only);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
    delete circuitsptr;
    if (CommandlineOptions::verbose()) {
      std::cerr << "computing cocircuits ..." << std::endl;
    }
    Cocircuits* cocircuitsptr = new Cocircuits(chiro);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
//     delete dualptr;
    if (CommandlineOptions::verbose()) {
      std::cerr << "computing facets ..." << std::endl;
    }
    Facets* facetsptr = new Facets(*cocircuitsptr);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
    delete cocircuitsptr;
    if (CommandlineOptions::verbose()) {
      std::cerr << "computing interior facets ..." << std::endl;
    }
    InteriorFacets interiorfacets = InteriorFacets(*facetsptr);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
    delete facetsptr;
    if (CommandlineOptions::verbose()) {
      std::cerr << "counting triangulations ..." << std::endl;
    }
    const size_type N = __no_of_all_triangs(no, rank, admissibles, interiorfacets, output_triangs);
    if (CommandlineOptions::verbose()) {
      std::cerr << N << " triangulations in total." << std::endl;
      std::cerr << "... done." << std::endl;
    }
    if (!output_triangs) {
      std::cout << N << std::endl;
    }
    return 0;
  }
  else {
    Chirotope chiro;
    PointConfiguration points;
    if (input_chiro) {
      // polymake input:
//       if (CommandlineOptions::polymakeobj()) {
// 	CommandlineOptions::polymakeobj().read("CHIROTOPE", 0);
// 	chiro.read_string(CommandlineOptions:polymakeobj());
//       }
//       else {
	if (!chiro.read_string(std::cin)) {
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "error while reading chirotope." << std::endl;
	  }
	  return 1;
	}
//       }
    }
    else {
      if (!points.read(std::cin)) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "error while reading point configuration." << std::endl;
	}
	return 1;
      }
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
    }
    size_type no(chiro.no());
    size_type rank(chiro.rank());
    SymmetryGroup symmetries(no);
    if (!CommandlineOptions::ignore_symmetries()) {
      if (symmetries.read(std::cin)) {
	if (CommandlineOptions::verbose()) {
#ifndef STL_SYMMETRIES
	  std::cerr << symmetries.load() << " symmetries in total." << std::endl;
#else
	  std::cerr << symmetries.size() << " symmetries in total." << std::endl;
#endif
	  if (CommandlineOptions::debug()) {
	    std::cerr << "symmetries:" << std::endl;
	    std::cerr << symmetries << std::endl;
	  }
	}
      }
    }
    else {
      if (CommandlineOptions::verbose()) {
	std::cerr << "no valid symmetry generators found." << std::endl;
      }
    }
    SimplicialComplex seed;
    if (!seed.read(std::cin)) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "no valid seed triangulation found" << std::endl;
	std::cerr << "computing seed triangulation via placing and pushing ..." 
	     << std::endl;
      }
      if (fine_only) {
	seed = FineTriang(chiro);
      }
      else {
	seed = PlacingTriang(chiro);
      }
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "Computing symmetries of seed ..." << std::endl;
    }
    const SymmetryGroup seed_symmetries(symmetries, seed);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
#ifndef STL_SYMMETRIES
      std::cerr << seed_symmetries.load() << " symmetries in total in seed." << std::endl;
#else
      std::cerr << seed_symmetries.size() << " symmetries in total in seed." << std::endl;
#endif
      if (CommandlineOptions::debug()) {
	std::cerr << "symmetries:" << std::endl;
	std::cerr << symmetries << std::endl;
      }
    }
    if (CommandlineOptions::check()) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "checking seed triangulation ..." << std::endl;
      }
      if (!(CheckTriang(seed, seed_symmetries, chiro, symmetries, fine_only))()) {
	if (CommandlineOptions::verbose()) {
	  std::cerr << "seed triangulation " << std::endl
	       << seed << std::endl
	       << "not valid." << std::endl;
	}
	return(1);
      }
      if (CommandlineOptions::verbose()) {
	std::cerr << "... done." << std::endl;;
      }
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "seed: " << seed << std::endl;
      std::cerr << "containing " << seed.card() << " simplices" << std::endl;
      std::cerr << "using the following " << seed.support().card() << " vertices: " 
		<< seed.support() << std::endl;
      std::cerr << "... done." << std::endl;
    }
    if (findflips) {
      if (CommandlineOptions::verbose()) {
	std::cerr << "count all flips of seed ..." << std::endl;
      }
      const TriangNode tn(0, no, rank, seed);
      const TriangFlips tf(chiro, tn, seed_symmetries, fine_only);
      if (CommandlineOptions::verbose()) {
// 	std::cerr << tf.flips().load() << " flips in total." << std::endl;
	std::cerr << tf.flips().size() << " flips in total." << std::endl;
	std::cerr << "... done." << std::endl;
      }
      if (output_triangs) {
	std::cout << tf << std::endl;
      }
      else {
// 	std::cout << tf.flips().load() << std::endl;
	std::cout << tf.flips().size() << std::endl;
      }
      return 0;
    }
    if (CommandlineOptions::verbose()) {
      std::cerr << "count all symmetry classes of triangulations ..." << std::endl;
    }
    const size_type N = __no_of_triangs(no, rank, points, chiro, 
					symmetries, seed, seed_symmetries,
					output_triangs, fine_only);
    if (CommandlineOptions::verbose()) {
      std::cerr << N << " symmetry classes of triangulations in total." << std::endl;
      std::cerr << "... done." << std::endl;
    }
    if (!output_triangs) {
      std::cout << N << std::endl;
    }
    return 0;
  }
}

// eof ComputeTriangs.cc
