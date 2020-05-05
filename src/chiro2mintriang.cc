#include "HashMap.hh"
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

typedef HashSet<PartialTriang> triang_container_type;

PartialTriang bfs(const size_type no,
		  const size_type rank,
		  triang_container_type* all_triangs_ptr,
		  const size_type card) {
  if (CommandlineOptions::verbose()) {
    std::cerr << "current cardinality: " << card << std::endl;
  }
  triang_container_type new_all_triangs;
  SimplicialComplex forbidden;
  for (triang_container_type::const_iterator iter = all_triangs_ptr->begin(); iter != all_triangs_ptr->end(); ++iter) {
    const PartialTriang& current_partial_triang(*iter);
    for (SimplicialComplex::const_iterator sciter = current_partial_triang.admissibles().begin();
	 sciter != current_partial_triang.admissibles().end(); 
	 ++sciter) {
      const Simplex new_simp(*sciter);
      PartialTriang next_partial_triang(PartialTriang(current_partial_triang, new_simp, forbidden));
      forbidden += new_simp;
      if (next_partial_triang.freefacets().is_empty()) {
	// complete triang:
#ifdef SUPER_VERBOSE
	std::cout << "triangulation found: " << next_partial_triang << std::endl;
	std::cout << "number of simplices: " << next_partial_triang.card() << std::endl;
#endif
	return next_partial_triang;
      }
      else if (next_partial_triang.admissibles().is_empty()) {
	// dead end:
#ifdef SUPER_VERBOSE
	std::cerr << '\t' <<  "uncoverable free facets: " << next_partial_triang.freefacets() << std::endl;
	std::cerr << '\t' <<  "dead end." << std::endl;
#endif
      }
      else {
	// partial triang:
	new_all_triangs.insert(next_partial_triang);
	if (CommandlineOptions::verbose()) {
	  if (new_all_triangs.load() % 10000 == 0) {
	    std::cerr << new_all_triangs.load() << " partial triangulations of card " << card << " so far." << std::endl;
	  }
	}
      }
    }
    forbidden.clear();
  }
  all_triangs_ptr->clear();
  return bfs(no, rank, &new_all_triangs, card + 1);
}

PartialTriang find_minimum_triang(const size_type no,
				  const size_type rank, 
				  const Admissibles& admtable, 
				  const InteriorFacets& intfacets) {
  PartialTriang root(no, rank, admtable, intfacets);
  triang_container_type all_triangs;
  SimplicialComplex forbidden;
  if (CommandlineOptions::verbose()) {
    std::cerr << "current cardinality: " << 1 << std::endl;
  }
  for (PartialTriang::const_iterator sciter = root.admissibles().begin();
       sciter != root.admissibles().end();
       ++sciter) {
    const Simplex start_simp(*sciter);
    forbidden += start_simp;
    PartialTriang next_partial_triang(root, start_simp, forbidden);
    if (next_partial_triang.freefacets().is_empty()) {
      // complete triang:
#ifdef SUPER_VERBOSE
      std::cout << "triangulation found: " << next_partial_triang << std::endl;
      std::cout << "number of simplices: " << next_partial_triang.card() << std::endl;
#endif
      return next_partial_triang;
    }
    all_triangs.insert(next_partial_triang);
  }
  return bfs(no, rank, &all_triangs, 2);
}

int main(const int argc, const char** argv) {
  CommandlineOptions::init(argc, argv);
  if (CommandlineOptions::verbose()) {
    std::cerr << std::endl;
    std::cerr << "-------------------------------------------------------\n";
    std::cerr << "counting all triangulations of a point configuration\n";
    std::cerr << "-------------------------------------------------------\n";
    std::cerr << std::endl;
  }
  Chirotope chiro;
  if (chiro.read_string(std::cin)) {
    if (CommandlineOptions::verbose()) {
      std::cerr << "computing dual chirotope ..." << std::endl;
    }
    Chirotope* dualptr = new Chirotope(chiro.dual());
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
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
    Admissibles admissibles(*circuitsptr, chiro);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
    delete circuitsptr;
    if (CommandlineOptions::verbose()) {
      std::cerr << "computing cocircuits ..." << std::endl;
    }
    Cocircuits* cocircuitsptr = new Cocircuits(*dualptr);
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
    delete dualptr;
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
      std::cerr << "searching for minimum triangulation ..." << std::endl;
    }
    std::cout << find_minimum_triang(no, rank, admissibles, interiorfacets) << std::endl;
    if (CommandlineOptions::verbose()) {
      std::cerr << "... done." << std::endl;
    }
    return 0;
  }
  else {
    std::cerr << argv[0] << ": error while reading chirotope." << std::endl;
    return 1;
  }

  return 0;
}
