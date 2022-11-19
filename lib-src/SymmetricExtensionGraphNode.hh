//////////////////////////////////////////////////////////////////////////
// SymmetricExtensionGraphNode.hh
// produced: 27/11/2020 jr
// last change: 27/11/2020 jr
/////////////////////////////////////////////////////////////////////////
#ifndef SYMMETRICEXTENSIONGRAPHNODE_HH
#define SYMMETRICEXTENSIONGRAPHNODE_HH

#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <unordered_map>
#include <mutex>

#include "ContainerIO.hh"
#include "HashKey.hh"
#include "LabelSet.hh"
#include "Symmetry.hh"
#include "SimplicialComplex.hh"
#include "Admissibles.hh"
#include "Incidences.hh"
#include "Volumes.hh"
#include "PartialTriang.hh"

namespace topcom {

  
  // the following type can store for each symmetry g
  // the minimal index of the symmetric difference of a partial triangulation T and its image g(T)
  // where the convention is that the critical simplex index is std::numeric_limits<size_type>::max() if S = g(S)
  // (the simplex with this index decides the lexicographic comparison);
  // in order to save memory, we store a vector of critical indices
  // where critical_element[idx] is the index of the critical simplex for symmetries[idx]:
  typedef std::vector<size_type> critical_simpidx_table_type;

  // for the case of scarce memory we offer a cache for the symmetries represented by images of
  // simplex indices; to this end, a pair of indices is defined with the necessary features
  // for an unordered map:
  class IndexPair : public std::pair<size_type, size_type> {
  public:
    inline IndexPair(const size_type simpidx, const size_type symidx) : std::pair<size_type, size_type>(simpidx, symidx) {}
    inline size_type keysize() const { return 2; }
    inline size_type key(const size_type n) const { return n == 0 ? first : second; }
    inline bool operator==(const IndexPair& ip) { return (second == ip.second) && (first == ip.first); }
  };

  // use if memory is not an issue:
  typedef std::vector<std::vector<size_type> >                            symmetry_table_type;

  // use if memory is scarce:
  typedef std::pair<IndexPair, size_type>                                 symmetry_cache_entry_type;
  typedef std::vector<symmetry_cache_entry_type>                          symmetry_cache_type;
  typedef std::queue<IndexPair>                                           symmetry_keyvec_type;
 
  class SymmetricExtensionGraphNode {
  public:
    static symmetry_table_type               _symmetry_images_by_element;
    static thread_local symmetry_cache_type  _symmetry_images_by_element_cache;
    // static thread_local symmetry_keyvec_type _symmetry_images_by_element_keyvec;
  private:
    const SymmetryGroup*        _symmetriesptr;
    PartialTriang               _partial_triang;
    critical_simpidx_table_type _critsimpidx_table;
  private:
    SymmetricExtensionGraphNode();
  public:
    // initializer for static data:
    inline static void init_simpidx_table(const SymmetryGroup*, const size_type);
    inline static void init_simpidx_cache(const SymmetryGroup*);
    
    // constructors:
    inline SymmetricExtensionGraphNode(const SymmetricExtensionGraphNode&);
    inline SymmetricExtensionGraphNode(const SymmetricExtensionGraphNode&&);
    inline SymmetricExtensionGraphNode(const SymmetricExtensionGraphNode&,
				       const PartialTriang&,
				       const critical_simpidx_table_type&);
    inline SymmetricExtensionGraphNode(const SymmetricExtensionGraphNode&,
				       PartialTriang&&,
				       critical_simpidx_table_type&&);

    // compute a node from scratch for double-check purposes
    // (the root node is computed for pt = empty partial triangulation):
    SymmetricExtensionGraphNode(const SymmetryGroup*,
				const PartialTriang&&);
    
    // destructor:
    inline ~SymmetricExtensionGraphNode();

    // assignment:
    inline SymmetricExtensionGraphNode& operator=(const SymmetricExtensionGraphNode&);
    inline SymmetricExtensionGraphNode& operator=(const SymmetricExtensionGraphNode&&);

    // functions:
 
    // the following is the core function:
    // it checks whether the child node of this node by extending partial triangulation by a new simplex
    // is lex-minimal; it is assumed that the new simplex has an index that is larger than all the existing
    // simplices in the partial triangulation; the critical simplex table is updated during the checking process
    // and returned in the second part of the returned pair:
    bool child_is_lexmin(const Simplex&, critical_simpidx_table_type*, size_type*) const;
    // the same, but based on the symmetry group representation on points:
    bool child_is_lexmin_lean(const Simplex&, critical_simpidx_table_type*, size_type*) const;

    // auxiliary function to compute critical simplex from scratch for a symmetry:
    size_type critical_simpidx(const SimplicialComplex&, const Symmetry&) const;
    // the same, but based on the symmetry group representation on points:
    size_type critical_simpidx_lean(const SimplicialComplex&, const Symmetry&, const size_type) const;

    // accessors:
    inline const SymmetryGroup*               symmetries_ptr()          const { return _symmetriesptr; }
    inline const PartialTriang&               partial_triang()          const { return _partial_triang; }
    inline const critical_simpidx_table_type& critsimpidx_table()       const { return _critsimpidx_table; }

    // stream input/output:
    inline std::istream& read(std::istream&);
    inline std::ostream& write(std::ostream&) const;
  };

  // initializer for static data:
  inline void SymmetricExtensionGraphNode::init_simpidx_table(const SymmetryGroup* sgptr, const size_type n) {
    if (CommandlineOptions::simpidx_symmetries()) {

      // here, we simply reorganize the symmetries in a more cache coherent way in a new table:
      try {
	_symmetry_images_by_element.resize(sgptr->n(), std::vector<size_type>(sgptr->size(), std::numeric_limits<size_type>::max()));
      }
      catch (...) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "SymmetricExtensionGraphNode::SymmetricExtensionGraphNode(const SymmetryGroup&, const PartialTriang&): "
		  << "allocation of " << sgptr->size() * sgptr->n() << " int elements failed - exiting"
		  << std::endl;
	exit(1);
      }
      for (size_type symidx = 0; symidx < sgptr->size(); ++symidx) {
	for (parameter_type elm = 0; elm < sgptr->n(); ++elm) {
	  _symmetry_images_by_element[elm][symidx] = (*sgptr)[symidx][elm];
	}
      }
    }
    else {
      if (!CommandlineOptions::memopt()) {

	// here, we prepare a complete table that is populated on the fly when needed:
	try {
	  _symmetry_images_by_element.resize(n, std::vector<size_type>(sgptr->size(), std::numeric_limits<size_type>::max()));
	}
	catch (...) {
	  std::lock_guard<std::mutex> lock(IO_sync::mutex);
	  std::cerr << "SymmetricExtensionGraphNode::SymmetricExtensionGraphNode(const SymmetryGroup&, const PartialTriang&): "
		    << "allocation of " << sgptr->size() * n << " int elements failed - exiting"
		    << std::endl;
	  exit(1);
	}
      }
      else if (!CommandlineOptions::parallel_enumeration()) {

	// in this case, the main thread needs to initialize the thread_local cache:
	init_simpidx_cache(sgptr);
      }
    }      
  }

  inline void SymmetricExtensionGraphNode::init_simpidx_cache(const SymmetryGroup* sgptr) {
    if (!CommandlineOptions::simpidx_symmetries() && CommandlineOptions::memopt()) {
      if (CommandlineOptions::localcache() > 0) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "init cache:" << std::endl;
	_symmetry_images_by_element_cache.resize(CommandlineOptions::localcache() / CommandlineOptions::no_of_threads() + 1,
						 std::pair<IndexPair, size_type>(IndexPair(std::numeric_limits<size_type>::max(),
											   std::numeric_limits<size_type>::max()),
										 std::numeric_limits<size_type>::max()));
      }
    }
  }
  
  // constructors:
  inline SymmetricExtensionGraphNode::SymmetricExtensionGraphNode(const SymmetricExtensionGraphNode& segn) :
    _symmetriesptr(segn._symmetriesptr),
    _partial_triang(segn._partial_triang),
    _critsimpidx_table(segn._critsimpidx_table) {
  }

  inline SymmetricExtensionGraphNode::SymmetricExtensionGraphNode(const SymmetricExtensionGraphNode&& segn) :
    _symmetriesptr(segn._symmetriesptr),
    _partial_triang(std::move(segn._partial_triang)),
    _critsimpidx_table(std::move(segn._critsimpidx_table)) {
  }

  inline SymmetricExtensionGraphNode::SymmetricExtensionGraphNode(const SymmetricExtensionGraphNode&  node,
								  const PartialTriang&                partial_triang,
								  const critical_simpidx_table_type&  critsimpidx_table) :
    _symmetriesptr(node._symmetriesptr),
    _partial_triang(partial_triang),
    _critsimpidx_table(critsimpidx_table) {
  }

  inline SymmetricExtensionGraphNode::SymmetricExtensionGraphNode(const SymmetricExtensionGraphNode&  node,
								  PartialTriang&&                     partial_triang,
								  critical_simpidx_table_type&&       critsimpidx_table) :
    _symmetriesptr(node._symmetriesptr),
    _partial_triang(std::move(partial_triang)),
    _critsimpidx_table(std::move(critsimpidx_table)) {
  }

  // destructor:
  inline SymmetricExtensionGraphNode::~SymmetricExtensionGraphNode() {
#ifdef DEBUG
    std::lock_guard<std::mutex> lock(IO_sync::mutex);
    size_type max_bucket_size = 0;
    for (size_type n = 0; n < _symmetry_images_by_element_cache.bucket_count(); ++n) {
      if (max_bucket_size < _symmetry_images_by_element_cache.bucket_size(n)) {
	max_bucket_size = _symmetry_images_by_element_cache.bucket_size(n);
      }
    }
    std::cerr << "max bucket size of cache is " << max_bucket_size << std::endl;
#endif
  }

  // assignment:
  inline SymmetricExtensionGraphNode& SymmetricExtensionGraphNode::operator=(const SymmetricExtensionGraphNode& segn) {
    if (this == &segn) {
      return *this;
    }
    _symmetriesptr           = segn._symmetriesptr;
    _partial_triang          = segn._partial_triang;
    _critsimpidx_table       = segn._critsimpidx_table;
    return *this;
  }

  inline SymmetricExtensionGraphNode& SymmetricExtensionGraphNode::operator=(const SymmetricExtensionGraphNode&& segn) {
    if (this == &segn) {
      return *this;
    }
    _symmetriesptr           = segn._symmetriesptr;
    _partial_triang          = std::move(segn._partial_triang);
    _critsimpidx_table       = std::move(segn._critsimpidx_table);
    return *this;
  }

  // stream input/output:
  inline std::istream& SymmetricExtensionGraphNode::read(std::istream& ist) {
    std::cerr << "SymmetricExtensionGraphNode::read(std::istream& ist):"
	      << " reading in a pointer-based structure not supported - exiting."
	      << std::endl;
    exit(1);
    return ist;
  }

  inline std::istream& operator>>(std::istream& ist, SymmetricExtensionGraphNode& ssgn) {
    return ssgn.read(ist);
  }

  inline std::ostream& SymmetricExtensionGraphNode::write(std::ostream& ost) const {
    const char colon = ':';
    const char comma = ',';
    const char lbracket = '[';
    const char rbracket = ']';
    ost << lbracket << _partial_triang << comma
	<< _critsimpidx_table << rbracket;
    return ost;
  }

  inline std::ostream& operator<<(std::ostream& ost, const SymmetricExtensionGraphNode& ssgn) {
    return ssgn.write(ost);
  }

}; // namespace topcom

#endif

// eof SymmetricExtensionGraphNode.hh
