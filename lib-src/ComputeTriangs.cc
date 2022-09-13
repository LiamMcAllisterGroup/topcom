////////////////////////////////////////////////////////////////////////////////
// 
// ComputeTriangs.cc
//
//    produced: 25 Nov 1999 jr
// last change: 25 nov 1999 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Permutation.hh"
#include "PointConfiguration.hh"
#include "Volumes.hh"
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
#include "SymmetricFlipGraph.hh"
#include "SymmetricExtensionGraphMaster.hh"
#include "RegularityCheck.hh"

#include "ComputeTriangs.hh"

namespace topcom {

  const size_type __min_triang_card(const parameter_type      no,
				    const parameter_type      rank,
				    const PointConfiguration& points,
				    const Chirotope&          chiro,
				    const SymmetryGroup&      symmetries,
				    const bool                output_triangs,
				    const bool                only_fine_triangs
				    ) {
    SymmetricExtensionGraphMaster segm(no, rank, points, chiro, symmetries, output_triangs, only_fine_triangs, true);
    if (CommandlineOptions::verbose()) {
      std::cerr << segm.nodecount() << " partial triangulations visited in total." << std::endl;
      std::cerr << segm.deadendcount() << " branching deadends." << std::endl;
      std::cerr << segm.earlydeadendcount() << " early detected deadends." << std::endl;
      std::cerr << segm.mintriang() << " is a minimal triangulation." <<  std::endl;
      std::cerr << segm.mincard() << " simplices in a minimal triangulation." << std::endl;
    }
    return segm.mincard();
  }

  const size_type __no_of_all_triangs(const parameter_type      no,
				      const parameter_type      rank,
				      const PointConfiguration& points,
				      const Chirotope&          chiro,
				      const SymmetryGroup&      symmetries,
				      const bool                output_triangs,
				      const bool                only_fine_triangs
				      ) {
    SymmetricExtensionGraphMaster segm(no, rank, points, chiro, symmetries, output_triangs, only_fine_triangs, false);
    if (CommandlineOptions::verbose()) {
      std::cerr << segm.nodecount() << " partial triangulations visited in total." << std::endl;
      std::cerr << segm.deadendcount() << " branching deadends." << std::endl;
      std::cerr << segm.earlydeadendcount() << " early detected deadends." << std::endl;
      std::cerr << segm.maxiter_coversimptighten() << " max iterations in coversimp tightening." << std::endl;
      std::cerr << segm.symcount() << " symmetry classes";
      if (!CommandlineOptions::skip_orbitcount()) {
	std::cerr << " --- " << segm.totalcount() << " triangulations in total";
      }
      std::cerr << "." << std::endl;
    }
    return segm.symcount();
  }

  const size_type __no_of_triangs(const parameter_type        no,
				  const parameter_type        rank,
				  const PointConfiguration&   points,
				  const Chirotope&            chiro,
				  const SymmetryGroup&        symmetries,
				  const SimplicialComplex&    seed,
				  const symmetryptr_datapair& seed_symmetryptrs,
				  const Volumes*              voltableptr,
				  const bool                  output_triangs,
				  const bool                  fine_only) {
    SymmetricFlipGraph sfg(no, rank, points, chiro, 
			   symmetries, seed, seed_symmetryptrs,
			   voltableptr,
			   output_triangs, fine_only);
    if (CommandlineOptions::verbose()) {
      std::cerr << sfg.symcount() << " symmetry classes";
      if (!CommandlineOptions::skip_orbitcount()) {
	std::cerr << " --- " << sfg.totalcount() << " triangulations in total";
      }
      std::cerr << "." << std::endl;
    }
    return sfg.symcount();
  }

  int ComputeTriangs::run(const int flags) {
    if (CommandlineOptions::lp_solver_needed()) {
      // some solvers need initialization of global constants etc.:
      RegularityCheck::init();
    }

    {
      const bool input_chiro    = flags & INPUT_CHIRO;
      const bool compute_all    = flags & COMPUTE_ALL;
      const bool fine_only      = flags & FINE_ONLY;
      const bool output_triangs = flags & OUTPUT_TRIANGS;
      const bool preprocess     = flags & PREPROCESS;
      const bool findflips      = flags & FINDFLIPS;
      const bool unimodular     = flags & UNIMODULAR;
      const bool findmin        = flags & FINDMIN;

      if (CommandlineOptions::verbose()) {
	std::cerr << std::endl;
	std::cerr << "------------------------------------------------------------\n";
	std::cerr << "------------------ " << PACKAGE << " VERSION " << VERSION << " --------------------\n";
	std::cerr << "Triangulations of Point Configurations and Oriented Matroids\n";
	std::cerr << "--------------------- by Joerg Rambau ----------------------\n";
	std::cerr << "------------------------------------------------------------\n";
	std::cerr << std::endl;
#ifdef STL_CONTAINERS
	std::cerr << "       -- using STL containers for hash tables --" << std::endl;
#endif
#ifdef STL_SYMMETRIES
	std::cerr << "       -- using STL containers for symmetries  --" << std::endl;
#endif
#ifdef STL_FLIPS
	std::cerr << "       --    using STL containers for flips    --" << std::endl;
#endif
#ifdef STL_CHIROTOPE
	std::cerr << "       -- using STL containers for chirotopes  --" << std::endl;
#endif
	std::cerr << std::endl;
      }

      // terminate if non-consistent options have been chosen:
      if (input_chiro && CommandlineOptions::check_regular()) {
	std::cerr << "regularity check not possible if only chirotope is given; exiting." << std::endl;
	exit(1);
      }
      Chirotope chiro;
      PointConfiguration points = PointConfiguration();
      if (input_chiro) {
	if (CommandlineOptions::output_asy()) {
	  std::cerr << "graphics output cancelled because of chirotope input." << std::endl;
	}
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
	if (points.rank() < points.rowdim()) {
	  std::cerr << "point configuration has " << points.rowdim() << " rows of rank " << points.rank() << std::endl;
	  points.transform_to_full_rank();
	  std::cerr << "resulting no of rows after transformation: " << points.rank() << std::endl;
	  points.pretty_print(std::cerr);
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

	// read symmetries anyway into dummy structure to be able to read seed later:
	SymmetryGroup dummy_symmetries(no);
	dummy_symmetries.read(std::cin);
	if (CommandlineOptions::verbose()) {
	  std::cerr << "no valid symmetry generators found." << std::endl;
	}
      }
    
      if (compute_all) {
	if (findmin) {
	  const size_type K = __min_triang_card(no,
						rank,
						points,
						chiro,
						symmetries,
						output_triangs,
						fine_only);
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "... done." << std::endl;
	  }
	  if (!output_triangs) {
	    std::cout << K << std::endl;
	  }
	  goto TERM_OK;
	}
	else {
	  const size_type N = __no_of_all_triangs(no,
						  rank,
						  points,
						  chiro,
						  symmetries,
						  output_triangs,
						  fine_only);
	  if (CommandlineOptions::verbose()) {
	    std::cerr << N << " symmetry classes of triangulations in total." << std::endl;
	    std::cerr << "... done." << std::endl;
	  }
	  if (!output_triangs) {
	    std::cout << N << std::endl;
	  }
	  goto TERM_OK;
	}
      }
      else {
	Volumes* voltableptr = 0;
	if (CommandlineOptions::use_gkz()
	    && CommandlineOptions::symmetries_are_isometric()
	    && !points.empty()) {
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "computing volumes table ..." << std::endl;
	  }
	  voltableptr = new Volumes(points);
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "... done." << std::endl;
	  }
	}
	
	if (CommandlineOptions::memopt()) {
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "no preprocessing of simplex table to save memory" << std::endl;
	  }
	}
	else {
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "preprocessing simplex table for all simplices up to rank "
		      << rank + 1 << " ..." << std::endl;
	  }
	  if (voltableptr) {
	    SimplicialComplex::preprocess_index_table(no, 0, rank + 1, *voltableptr, true);
	  }
	  else {
	    SimplicialComplex::preprocess_index_table(no, 0, rank + 1, chiro, true);
	  }
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "... done: "
		      << SimplicialComplex::no_of_simplices(rank)
		      << " full-dimensional simplices in rank " << rank << std::endl;
	  }
	}
	SimplicialComplex seed;
	bool input_seed;
	if (!seed.read(std::cin)) {
	  input_seed = false;
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
	else {
	  input_seed = true;
	}
	if (CommandlineOptions::verbose()) {
	  std::cerr << "computing symmetries of seed ..." << std::endl;
	}
	const symmetryptr_datapair seed_symmetryptrs(symmetries.stabilizer_ptrs(seed));
	if (CommandlineOptions::verbose()) {
	  std::cerr << "... done." << std::endl;
	  std::cerr << seed_symmetryptrs.first.size() << " symmetries in total in seed." << std::endl;
	  if (CommandlineOptions::debug()) {
	    std::cerr << "symmetries:" << std::endl;
	    std::cerr << symmetries << std::endl;
	  }
	}
	if (CommandlineOptions::check()) {
	  if (CommandlineOptions::verbose()) {
	    std::cerr << "checking seed triangulation ..." << std::endl;
	  }
	  if (!(CheckTriang(seed, seed_symmetryptrs, chiro, symmetries, fine_only))()) {
	    if (CommandlineOptions::verbose()) {
	      std::cerr << "seed triangulation " << std::endl
			<< seed << std::endl
			<< "not valid." << std::endl;
	    }
	    goto TERM_ERROR;
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
	  const TriangFlips tf(chiro, tn, seed_symmetryptrs, fine_only);
	  if (CommandlineOptions::verbose()) {
	    std::cerr << tf.flips().size() << " flips in total." << std::endl;
	    std::cerr << "... done." << std::endl;
	  }
	  if (output_triangs) {
	    std::cout << tf << std::endl;
	  }
	  else {
	    std::cout << tf.flips().size() << std::endl;
	  }
	  goto TERM_OK;
	}
	if (CommandlineOptions::verbose()) {
	  std::cerr << "exploring all seed-connected symmetry classes of triangulations by flipping ..." << std::endl;
	}
	const size_type N = __no_of_triangs(no, rank, points, chiro, 
					    symmetries,
					    seed, seed_symmetryptrs,
					    voltableptr,
					    output_triangs, fine_only);
	if (CommandlineOptions::verbose()) {
	  std::cerr << N << " symmetry classes of triangulations in total." << std::endl;
	  std::cerr << "... done." << std::endl;
	}
	if (!output_triangs) {
	  std::cout << N << std::endl;
	}
	delete voltableptr;
	goto TERM_OK;
      }
    }

  TERM_ERROR:
    if (CommandlineOptions::lp_solver_needed()) {
      // some solvers need termination of global constants etc.:
      RegularityCheck::term();
    }
    return 1;
  
  TERM_OK:
    if (CommandlineOptions::lp_solver_needed()) {
      // some solvers need termination of global constants etc.:
      RegularityCheck::term();
    }
    return 0;
  }

}; // namespace topcom

// eof ComputeTriangs.cc
