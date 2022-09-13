//////////////////////////////////////////////////////////////////////////
// SymmetricSubsetGraphNode.hh
// produced: 27/11/2020 jr
// last change: 27/11/2020 jr
/////////////////////////////////////////////////////////////////////////
#ifndef SYMMETRICSUBSETGRAPHNODE_HH
#define SYMMETRICSUBSETGRAPHNODE_HH

#include <iostream>
#include <iomanip>
#include <vector>

#include "ContainerIO.hh"
#include "LabelSet.hh"
#include "Symmetry.hh"
#include "StrictStairCaseMatrix.hh"
#include "StrictStairCaseMatrixTrans.hh"

namespace topcom {

  // we make the enumaration dependent on the matrix class used for elimination
  // since circuit enumeration needs a more powerful class than cocircuit enumeration:
  
  typedef enum { circuits, cocircuits, cocircuits_independent } ssg_mode_type;
  template <ssg_mode_type mode>
  class NodeMatrixClass;
  
  // for circuits we need a more capable matrix class:
  template <>
  class NodeMatrixClass<circuits> : public StrictStairCaseMatrixTrans {};
  
  // for cocircuits based on the enumeration of independent hyperplanar subsets
  // we need a simpler matrix class:
  template <>
  class NodeMatrixClass<cocircuits_independent> : public StrictStairCaseMatrix {};
  
  // for cocircuits based on the enumeration of the complete zero-sets
  // the simpler class suffices as well:
  template <>
  class NodeMatrixClass<cocircuits> : public StrictStairCaseMatrix {};
  
  // the following type can store for each symmetry g
  // the maximal value of the symmetric difference of a subset S and its image g(S)
  // where the convention is that the critical element is std::numeric_limits<parameter_type>::max() if S = g(S)
  // (this value decides the colexicographic comparison);
  // in order to save memory, we store a vector of critical elements
  // where critical_element[idx] is the critical element for symmetries[idx]:
  
  typedef std::vector<parameter_type> critical_element_table_type;

  // for each element i, store the index set of symmetries with critical element i
  // with the convention that the stabilizer of S is at index n:
  typedef std::vector<LabelSet> critical_element_classification_type;

  // to offer the choice what whether to use check by iteration or by sets
  // we need to hand over a pair of the data structures:
  typedef std::pair<critical_element_table_type, critical_element_classification_type> critical_element_data_type;
  
  template <ssg_mode_type mode>
  class SymmetricSubsetGraphNode {
  public:
    typedef NodeMatrixClass<mode> matrix_type;
  private:
    // the following leaner global data structure is used in the iterative symmetry check
    // for reasons of a better cache-coherence:
    static std::vector<std::vector<parameter_type> > _symmetry_images_by_element;

    // the following more excessive global data structures are for the set-based symmetry check:
    static std::vector<LabelSet> _increasing_symidxsets;
    static std::vector<LabelSet> _decreasing_symidxsets;
    static std::vector<LabelSet> _stabilizing_symidxsets;

    // the following specifies a matrix of LabelSets with
    // the entry at (i, j) contains the index set of all
    // permutations mapping i to [at least] j:
    static std::vector<std::vector<LabelSet> > _elm_to_img_classification;
    static std::vector<std::vector<LabelSet> > _elm_to_at_least_img_classification;
  private:
    const SymmetryGroup*                 _symmetriesptr;
    LabelSet                             _subset;
    matrix_type                          _matrix;
    critical_element_table_type          _critelem_table;
    critical_element_classification_type _critelem_classification;
    size_type                            _stabilizer_card;
  private:
    SymmetricSubsetGraphNode();
  public:
    // constructors:
    inline SymmetricSubsetGraphNode(const SymmetricSubsetGraphNode&);
    inline SymmetricSubsetGraphNode(SymmetricSubsetGraphNode&&);
    inline SymmetricSubsetGraphNode(const SymmetryGroup*,
				    const LabelSet&,
				    const matrix_type&,
				    const critical_element_data_type&,
				    const size_type);
    inline SymmetricSubsetGraphNode(const SymmetryGroup*,
				    LabelSet&&,
				    matrix_type&&,
				    critical_element_data_type&&,
				    const size_type);
    
    // construct the root node with empty subset and empty matrix
    // using the symmetry group to initialize the critical element table:
    SymmetricSubsetGraphNode(const SymmetryGroup*);
    
    // compute a node from scratch for double-check purposes:
    SymmetricSubsetGraphNode(const SymmetryGroup*,
			     const LabelSet&,
			     const matrix_type&);
    
    // destructor:
    inline ~SymmetricSubsetGraphNode();
    
    // assignment:
    inline SymmetricSubsetGraphNode& operator=(const SymmetricSubsetGraphNode&);
    inline SymmetricSubsetGraphNode& operator=(SymmetricSubsetGraphNode&&);
    
    // functions:
    
    // the following is the core function:
    // it checks whether the child node of this node by extending subset by a new element
    // is colex-maximal; it is assumed that the new element is smaller than all the existing
    // elements of subset; the critical element table is updated during the checking process
    // in the internal computation buffer _next_critelem_table:
    inline bool child_is_colexmax(const parameter_type, critical_element_data_type*, size_type*) const;
    bool child_is_colexmax_by_iter(const parameter_type, critical_element_table_type*, size_type*) const;
    bool child_is_colexmax_by_sets(const parameter_type, critical_element_classification_type*, size_type*) const;
    
    // auxiliary function to compute critical element from scratch for a symmetry:
    parameter_type critical_element(const LabelSet&, const Symmetry&) const;
    
    // accessors:
    inline const LabelSet&                    subset()                const { return _subset; }
    inline const matrix_type&                 matrix()                const { return _matrix; }
    inline const critical_element_table_type& critelem_table()        const { return _critelem_table; }
    inline const size_type                    stabilizer_card()       const { return _stabilizer_card; }
    
    // stream input/output:
    inline std::istream& read(std::istream&);
    inline std::ostream& write(std::ostream&) const;
  };

  // static members:
  template <ssg_mode_type mode>
  std::vector<std::vector<parameter_type> > SymmetricSubsetGraphNode<mode>::_symmetry_images_by_element;

  template <ssg_mode_type mode>
  std::vector<LabelSet> SymmetricSubsetGraphNode<mode>::_increasing_symidxsets;

  template <ssg_mode_type mode>
  std::vector<LabelSet> SymmetricSubsetGraphNode<mode>::_decreasing_symidxsets;

  template <ssg_mode_type mode>
  std::vector<LabelSet> SymmetricSubsetGraphNode<mode>::_stabilizing_symidxsets;

  template <ssg_mode_type mode>
  std::vector<std::vector<LabelSet> > SymmetricSubsetGraphNode<mode>::_elm_to_img_classification;

  template <ssg_mode_type mode>
  std::vector<std::vector<LabelSet> > SymmetricSubsetGraphNode<mode>::_elm_to_at_least_img_classification;

  // constructors:
  template <ssg_mode_type mode>
  inline SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetricSubsetGraphNode<mode>& ssgn) :
    _symmetriesptr(ssgn._symmetriesptr),
    _subset(ssgn._subset),
    _matrix(ssgn._matrix),
    _critelem_table(ssgn._critelem_table),
    _critelem_classification(ssgn._critelem_classification),
    _stabilizer_card(ssgn._stabilizer_card) {
  }

  template <ssg_mode_type mode>
  inline SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(SymmetricSubsetGraphNode<mode>&& ssgn) :
    _symmetriesptr(ssgn._symmetriesptr),
    _subset(std::move(ssgn._subset)),
    _matrix(std::move(ssgn._matrix)),
    _critelem_table(std::move(ssgn._critelem_table)),
    _critelem_classification(std::move(ssgn._critelem_classification)),
    _stabilizer_card(ssgn._stabilizer_card) {
  }

  template <ssg_mode_type mode>
  inline SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup*                   symmetriesptr,
								  const LabelSet&                        subset,
								  const matrix_type&                     matrix,
								  const critical_element_data_type&      critelem_data,
								  const size_type                        stabilizer_card) :
    _symmetriesptr(symmetriesptr),
    _subset(subset),
    _matrix(matrix),
    _critelem_table(critelem_data.first),
    _critelem_classification(critelem_data.second),
    _stabilizer_card(stabilizer_card) {
  }

  template <ssg_mode_type mode>
  inline SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup*                   symmetriesptr,
								  LabelSet&&                             subset,
								  matrix_type&&                          matrix,
								  critical_element_data_type&&           critelem_data,
								  const size_type                        stabilizer_card) :
    _symmetriesptr(symmetriesptr),
    _subset(std::move(subset)),
    _matrix(std::move(matrix)),
    _critelem_table(std::move(critelem_data.first)),
    _critelem_classification(std::move(critelem_data.second)),
    _stabilizer_card(stabilizer_card) {
  }

  // construct the root node with empty subset and empty matrix
  // using the symmetry group to initialize the critical element table:
  template <ssg_mode_type mode>
  SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup* sgptr) :
    _symmetriesptr(sgptr),
    _subset(),
    _matrix(),
    _critelem_table(_symmetriesptr->size(), std::numeric_limits<parameter_type>::max()),
    _critelem_classification(),
    _stabilizer_card(sgptr->size()) {
    
    if (!(CommandlineOptions::use_classified_symmetries() || CommandlineOptions::use_switch_tables() || CommandlineOptions::use_naive_symmetries())) {
      if (CommandlineOptions::memopt()) {
	try {
	  _symmetry_images_by_element.resize(_symmetriesptr->n(), std::vector<parameter_type>(_symmetriesptr->size()));
	}
	catch (...) {
	  std::cerr << "SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup*): "
		    << "allocation of " << _symmetriesptr->size() * _symmetriesptr->n() << " int elements failed - exiting"
		    << std::endl;
	  exit(1);
	}
	for (size_type symidx = 0; symidx < _symmetriesptr->size(); ++symidx) {
	  for (parameter_type elm = 0; elm < _symmetriesptr->n(); ++elm) {
	    _symmetry_images_by_element.at(elm).at(symidx) = _symmetriesptr->at(symidx).at(elm);
	  }
	}
      }
      else {

	// init the static vector of symmetry indices that strictly increase the respective elements:
	try {
	  _increasing_symidxsets.resize(_symmetriesptr->n(), LabelSet());
	}
	catch (...) {
	  std::cerr << "SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup*, const LabelSet&, const matrix_type&): "
		    << "allocation of " << _symmetriesptr->n() << " LabelSet elements failed - exiting."
		    << std::endl;
	  exit(1);
	}

	// init the static vector of symmetry indices that strictly decrease the respective elements:
	try {
	  _decreasing_symidxsets.resize(_symmetriesptr->n(), LabelSet());
	}
	catch (...) {
	  std::cerr << "SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup*, const LabelSet&, const matrix_type&): "
		    << "allocation of " << _symmetriesptr->n() << " LabelSet elements failed - exiting."
		    << std::endl;
	  exit(1);
	}

	// init the static vector of symmetry indices that strictly tabilize the respective elements:
	try {
	  _stabilizing_symidxsets.resize(_symmetriesptr->n(), LabelSet());
	}
	catch (...) {
	  std::cerr << "SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup*, const LabelSet&, const matrix_type&): "
		    << "allocation of " << _symmetriesptr->n() << " LabelSet elements failed - exiting."
		    << std::endl;
	  exit(1);
	}

	// build the increasing/decreasing/stabilizing index sets:
	for (parameter_type elem = 0; elem < _symmetriesptr->n(); ++elem) {
	  for (size_type symidx = 0; symidx < _symmetriesptr->size(); ++symidx) {
	    const Symmetry& g(_symmetriesptr->at(symidx));
	    if (g[elem] < elem) {
	      _decreasing_symidxsets.at(elem) += symidx;
	    }
	    else if (g[elem] > elem) {
	      _increasing_symidxsets.at(elem) += symidx;
	    }
	    else {
	      _stabilizing_symidxsets.at(elem) += symidx;
	    }
	  }
	}

	// build the static matrix of symmetry indices mapping i to at least j > i:
	try {
	  _elm_to_img_classification.resize(_symmetriesptr->n(), std::vector<LabelSet>(_symmetriesptr->n(), LabelSet()));
	  _elm_to_at_least_img_classification.resize(_symmetriesptr->n(), std::vector<LabelSet>(_symmetriesptr->n(), LabelSet()));
	}
	catch (...) {
	  std::cerr << "SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup*, const LabelSet&, const matrix_type&): "
		    << "allocation of " << _symmetriesptr->n() * _symmetriesptr->n() << " LabelSet elements failed - exiting."
		    << std::endl;
	  exit(1);
	}
	for (parameter_type elm = 0; elm < _symmetriesptr->n(); ++elm) {
	  for (LabelSet::const_iterator inciter = _increasing_symidxsets.at(elm).begin();
	       inciter != _increasing_symidxsets.at(elm).end();
	       ++inciter) {
	    const size_type symidx(*inciter);
	
	    // first, we collect the indices of symmetries mapping elm to img in index set at position (elm, img):
	    const Symmetry& g(_symmetriesptr->at(symidx));
	    _elm_to_img_classification.at(elm).at(g(elm)) += symidx;
	    _elm_to_at_least_img_classification.at(elm).at(g(elm)) += symidx;
	  }
	}
	for (parameter_type elm = 0; elm < _symmetriesptr->n(); ++elm) {
	  for (parameter_type img = _symmetriesptr->n() - 2; img > elm; --img) {

	    // next, we backwardly add at position (elm, img) all indices of symmetries mapping elm to img+1,
	    // so that finally at position (elm, img) we have all indices of symmetries mapping elm to at least img:
	    _elm_to_at_least_img_classification.at(elm).at(img) += _elm_to_at_least_img_classification.at(elm).at(img + 1);
	  }
	}

	// next, build the critical-elements classification vector for the empty set:
	try {
	  _critelem_classification.resize(_symmetriesptr->n() + 1, LabelSet());
	}
	catch (...) {
	  std::cerr << "SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup*, const LabelSet&, const matrix_type&): "
		    << "allocation of " << _symmetriesptr->n() + 1 << " LabelSet elements failed - exiting."
		    << std::endl;
	  exit(1);
	}
    
	// all symmetries stabilize the empty set:
	_critelem_classification.at(sgptr->n()) = LabelSet(0, sgptr->size());
      }
    }
  }

  template <ssg_mode_type mode>
  SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup* sgptr,
							   const LabelSet& subset,
							   const matrix_type& matrix) :
    _symmetriesptr(sgptr),
    _subset(subset),
    _matrix(matrix),
    _critelem_table(),
    _critelem_classification(),
    _stabilizer_card(0) {
    if (CommandlineOptions::memopt()) {
      try {
	_critelem_table.reserve(sgptr->size());
      }
      catch (...) {
	std::cerr << "SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup*, const LabelSet&, const matrix_type&): "
		  << "allocation of " << sgptr->size() << " int elements failed - exiting."
		  << std::endl;
	exit(1);
      }
      for (SymmetryGroup::const_iterator iter = sgptr->begin();
	   iter != sgptr->end();
	   ++iter) {
	const Symmetry& g(*iter);
	_critelem_table.push_back(critical_element(subset, g));
      }
    }
    else {
      try {
	_critelem_classification.resize(sgptr->n() + 1, LabelSet());
      }
      catch (...) {
	std::cerr << "SymmetricSubsetGraphNode<mode>::SymmetricSubsetGraphNode(const SymmetryGroup*, const LabelSet&, const matrix_type&): "
		  << "allocation of " << sgptr->n() + 1 << " int elements failed - exiting."
		  << std::endl;
	exit(1);
      }
      for (size_type symidx = 0; symidx < _symmetriesptr->size(); ++symidx) {
	const Symmetry& g(_symmetriesptr->at(symidx));
	parameter_type critelem = critical_element(subset, g);
	if (critelem == std::numeric_limits<parameter_type>::max()) {
	  critelem = sgptr->n();
	  ++_stabilizer_card;
	}
	_critelem_classification.at(critelem) += symidx;
      }
    }
  }

  // destructor:
  template <ssg_mode_type mode>
  inline SymmetricSubsetGraphNode<mode>::~SymmetricSubsetGraphNode() {}

  // assignment:
  template <ssg_mode_type mode>
  inline SymmetricSubsetGraphNode<mode>& SymmetricSubsetGraphNode<mode>::operator=(const SymmetricSubsetGraphNode<mode>& ssgn) {
    if (this == &ssgn) {
      return *this;
    }
    _symmetriesptr           = ssgn._symmetriesptr;
    _subset                  = ssgn._subset;
    _matrix                  = ssgn._matrix;
    _critelem_table          = ssgn._critelem_table;  
    _critelem_classification = ssgn._critelem_classification;  
    _stabilizer_card         = ssgn._stabilizer_card;  
    return *this;
  }

  template <ssg_mode_type mode>
  inline SymmetricSubsetGraphNode<mode>& SymmetricSubsetGraphNode<mode>::operator=(SymmetricSubsetGraphNode<mode>&& ssgn) {
    if (this == &ssgn) {
      return *this;
    }
    _symmetriesptr           = ssgn._symmetriesptr;
    _subset                  = std::move(ssgn._subset);
    _matrix                  = std::move(ssgn._matrix);
    _critelem_table          = std::move(ssgn._critelem_table);
    _critelem_classification = std::move(ssgn._critelem_classification);  
    _stabilizer_card         = ssgn._stabilizer_card;  
    return *this;
  }

  // functions:

  // the following is the core function:
  // it checks whether the child node of this node by extending subset by a new element
  // is colex-maximal; it is assumed that the new element is smaller than all the existing
  // elements of subset; the critical element table is updated during the checking process
  // and returned if the extended subset is colex maximal:

  template <ssg_mode_type mode>
  bool SymmetricSubsetGraphNode<mode>::child_is_colexmax(const parameter_type new_elem,
							 critical_element_data_type* new_critelem_dataptr,
							 size_type* stabilizer_cardptr) const {
    if (CommandlineOptions::memopt()) {
      return child_is_colexmax_by_iter(new_elem, &new_critelem_dataptr->first, stabilizer_cardptr);
    }
    else {
      return child_is_colexmax_by_sets(new_elem, &new_critelem_dataptr->second, stabilizer_cardptr);
    }
  }
  
  template <ssg_mode_type mode>
  bool SymmetricSubsetGraphNode<mode>::child_is_colexmax_by_iter(const parameter_type new_elem,
								 critical_element_table_type* new_critelem_tableptr,
								 size_type* stabilizer_cardptr) const {
    const bool local_debug = false;
  
    if (local_debug || CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "SymmetricSubsetGraphNode<mode>::child_is_colexmax(const parameter_type new_elem):" << std::endl;
      std::cerr << "checking extension of subset " << _subset << " by new_element " << new_elem << " ..." << std::endl;
    }
    // first, we compute the extended subset:
    LabelSet new_subset(_subset);

    // generate a table to save the updated critical elements:
    new_subset += new_elem;
    if (local_debug || CommandlineOptions::debug()) {
      if (_critelem_table.size() != _symmetriesptr->size()) {
	std::lock_guard<std::mutex> lock(IO_sync::mutex);
	std::cerr << "SymmetricSubsetGraphNode<mode>::child_is_colexmax_by_iter(const parameter_type, critical_element_table_type*) const: "
		  << "|symmetry group| != |critical elements table| - exiting." << std::endl;
	exit(1);
      }
    }
    try {
      new_critelem_tableptr->reserve(_critelem_table.size());
    }
    catch (...) {
      std::cerr << "SymmetricSubsetGraphNode<mode>::child_is_colexmax(const parameter_type, critical_element_table_type*) const: "
		<< "allocation of " << _critelem_table.size() << " int elements failed - exiting."
		<< std::endl;
      exit(1);
    }
    const std::vector<parameter_type>& img_of_elm_vec = _symmetry_images_by_element.at(new_elem);
    *stabilizer_cardptr = 0;
    for (size_type idx = 0; idx < _symmetriesptr->size(); ++idx) {
      const Symmetry& g = (*_symmetriesptr).at(idx);
      const parameter_type& image_of_new_elem = img_of_elm_vec.at(idx);

      // the critical element equals new_elem in case g(S) = S,
      // which is indicated by a critical element of std::numeric_limits<parameter_type>::max() for g w.r.t. S:
      const parameter_type& critelem = _critelem_table.at(idx);
      
      // remove debug check because this is a very tight loop:
      // if (local_debug || CommandlineOptions::debug()) {
      // 	std::lock_guard<std::mutex> lock(IO_sync::mutex);
      // 	std::cerr << "... critical element for " << g << " with respect to " << _subset
      // 		  << " is " << critelem << std::endl;
      // 	std::cerr << _subset << " -> g -> " << g.map(_subset) << std::endl;
      // }
      // if (local_debug || CommandlineOptions::debug()) {
      // 	std::lock_guard<std::mutex> lock(IO_sync::mutex);
      // 	std::cerr << "... critical element for " << g << " with respect to " << new_subset
      // 		  << " is " << critelem << std::endl;
      // 	std::cerr << new_subset << " -> g -> " << g.map(new_subset) << std::endl;
      // }

      if (critelem == std::numeric_limits<parameter_type>::max()) {

	// in this case, g(S) = S, and new_elem is automatically the critical element:
	if (image_of_new_elem == new_elem) {
	  new_critelem_tableptr->emplace_back(std::numeric_limits<parameter_type>::max());
	  ++(*stabilizer_cardptr);
	  continue;
	}
	if (image_of_new_elem < new_elem) {
	  new_critelem_tableptr->emplace_back(new_elem);
	  continue;
	}
	return false;
      }
      
      if (image_of_new_elem == critelem) {

	// this is the complicated case:
	// the image g(j) of the new element under the symmetry g
	// is equal to the critical element,
	// thus, there is a new critical element for g w.r.t. S union j,
	// and we have to bite the dust by mapping more than one value:
      
	// no shortcut left: we need to compute the new critical element from scratch:
	const parameter_type new_critelem = critical_element(new_subset, g);
	if ((new_critelem != std::numeric_limits<parameter_type>::max()) && !new_subset.contains(new_critelem)) {

	  // in this case, neither the new subset is fixed nor the new critical element is
	  // in the image of the new subset, thus the new subset is not colex maximal:
	  return false;
	}

	// in this case, the new subset is fixed or the critical element is in the new subset,
	// thus it is colex maximal:
	// remove debug check because this is a very tight loop:
	// if (local_debug || CommandlineOptions::debug()) {
	//   std::lock_guard<std::mutex> lock(IO_sync::mutex);
	//   std::cerr << "... symmetry " << g << " not colex increasing" << std::endl;
	//   std::cerr << new_subset << " -> g -> " << g.map(new_subset) << std::endl;
	// }
	new_critelem_tableptr->emplace_back(new_critelem);
	if (new_critelem == std::numeric_limits<parameter_type>::max()) {

	  // update stabilizer count:
	  ++(*stabilizer_cardptr);
	}
	continue;
      }
      if (image_of_new_elem < critelem) {

	// the image g(j) of the new element j under the symmetry g
	// is strictly smaller than the critical element,
	// thus, S union j is colex-greater than g(S union j),
	// the critical element remains unchanged,
	// and we continue with the next symmetry:
	// remove debug check because this is a very tight loop:
	// if (local_debug || CommandlineOptions::debug()) {
	//   std::lock_guard<std::mutex> lock(IO_sync::mutex);
	//   std::cerr << "... symmetry " << g << " not colex increasing" << std::endl;
	//   std::cerr << new_subset << " -> g -> " << g.map(new_subset) << std::endl;
	// }
	new_critelem_tableptr->emplace_back(critelem);
	continue;
      }
      
      // the image g(j) of the new element j under the symmetry g
      // is strictly smaller than the critical element,
      // thus, g(S union j) is colex-greater than S union j,
      // we do not need updated critical elements,
      // and we return false; the critical-element table built so far has no meaning in this case:
      // remove debug check because this is a very tight loop:
      // if (local_debug || CommandlineOptions::debug()) {
      //   std::lock_guard<std::mutex> lock(IO_sync::mutex);
      //   std::cerr << "... symmetry " << g << " is colex increasing" << std::endl;
      //   std::cerr << new_subset << " -> g -> " << g.map(new_subset) << std::endl;
      // }
      return false;
    }

    // we have not found any colex-increasing symmetry;
    // in that case, all symmetries have been scanned,
    // and therefore all critical elements have been updated:
    return true;
  }

  template <ssg_mode_type mode>
  bool SymmetricSubsetGraphNode<mode>::child_is_colexmax_by_sets(const parameter_type new_elem,
								 critical_element_classification_type* new_critelem_classificationptr,
								 size_type* stabilizer_cardptr) const {
    const bool local_debug = false;
  
    if (local_debug || CommandlineOptions::debug()) {
      std::lock_guard<std::mutex> lock(IO_sync::mutex);
      std::cerr << "SymmetricSubsetGraphNode<mode>::child_is_colexmax(const parameter_type new_elem):" << std::endl;
      std::cerr << "checking extension of subset " << _subset << " by new_element " << new_elem << " ..." << std::endl;
    }
    
    // first, we compute the extended subset:
    const LabelSet new_subset = _subset + new_elem;
    if (_critelem_classification.at(_symmetriesptr->n()).intersection_nonempty(_increasing_symidxsets.at(new_elem))) { 
      
      // if such a symmetry exists, the new subset cannot be colex max:
      return false;
    }
    for (LabelSet::const_iterator subsetiter = _subset.begin();
	 subsetiter != _subset.end();
	 ++subsetiter) {
      const parameter_type critelem = *subsetiter;
      if (critelem >= _symmetriesptr->n() - 1) {
	break;
      }
      if (_critelem_classification.at(critelem).intersection_nonempty(_elm_to_at_least_img_classification.at(new_elem).at(critelem + 1))) {

	// if such a symmetry exists, the new subset cannot be colex max:
	return false;
      }
    }

    // only now initialize the data with the values of the current node:
    try {
      new_critelem_classificationptr->insert(new_critelem_classificationptr->begin(), _critelem_classification.begin(), _critelem_classification.end());
    }
    catch (...) {
      std::cerr << "SymmetricSubsetGraphNode<mode>::child_is_colexmax_by_sets(const parameter_type, critical_element_classification_type*) const: "
		<< "copy of " << _critelem_classification.size() << " int elements failed - exiting."
		<< std::endl;
      exit(1);
    }
    *stabilizer_cardptr = _stabilizer_card;
    LabelSet next_symidx_set;

    for (LabelSet::const_iterator subsetiter = _subset.begin();
	 subsetiter != _subset.end();
	 ++subsetiter) {
      const parameter_type critelem = *subsetiter;
      if ((next_symidx_set = _critelem_classification.at(critelem) * _elm_to_img_classification.at(new_elem).at(critelem)).empty()) {
	continue;
      }
      for (LabelSet::const_iterator critsetiter = next_symidx_set.begin();
	   critsetiter != next_symidx_set.end();
	   ++critsetiter) {
	const size_type& symidx = *critsetiter;
	const Symmetry& g = (*_symmetriesptr).at(symidx);
	const parameter_type& image_of_new_elem = g.at(new_elem);
	
	// this is the complicated case:
	// the image g(j) of the new element under the symmetry g
	// is equal to the critical element,
	// thus, there is a new critical element for g w.r.t. S union j,
	// and we need to compute the new critical element from scratch:
	const parameter_type new_critelem = critical_element(new_subset, g);
	if (new_critelem == std::numeric_limits<parameter_type>::max()) {
	  
	  // in this case, the new subset is fixed, thus it is colex maximal,
	  // and the critical-element table needs an update:
	  (*new_critelem_classificationptr).at(critelem) -= symidx;
	  (*new_critelem_classificationptr).at(_symmetriesptr->n()) += symidx;
	  ++(*stabilizer_cardptr);
	  continue;
	}
	else {
	  if (new_subset.contains(new_critelem)) {
	    
	    // in this case, the critical element is in the new subset,
	    // thus it is colex maximal with new critical element,
	    // and the critical-element table needs an update:
	    (*new_critelem_classificationptr).at(critelem) -= symidx;
	    (*new_critelem_classificationptr).at(new_critelem) += symidx;
	    continue;
	  }
	  
	  // in this case, neither the new subset is fixed nor the new critical element is
	  // in the image of the new subset, thus the new subset is not colex maximal:
	  return false;
	}
      }
      
      // we have not found any colex-increasing symmetry for symmetries mapping
      // new_elem to critelem;
      // for this case, all symmetries with possibly new critical elements
      // have been scanned, and therefore all their critical elements have been updated
    }
    if ((next_symidx_set = _critelem_classification.at(_symmetriesptr->n()) * _decreasing_symidxsets.at(new_elem)).empty()) {
      return true;
    }
    for (LabelSet::const_iterator critsetiter = next_symidx_set.begin();
	 critsetiter != next_symidx_set.end();
	 ++critsetiter) {
      
      // for all these symmetries, the new subset is colex max, but the critical-elements table needs an update,
      // since the new subset is not fixed by these symmetries, so that new_elem becomes critical:
      const size_type& symidx = *critsetiter;      
      (*new_critelem_classificationptr).at(_symmetriesptr->n()) -= symidx;
      --(*stabilizer_cardptr);
      (*new_critelem_classificationptr).at(new_elem) += symidx;
    }    
    return true;
  }

  // auxiliary function to compute critical element from scratch for a symmetry:
  template <ssg_mode_type mode>
  parameter_type SymmetricSubsetGraphNode<mode>::critical_element(const LabelSet& subset,
								  const Symmetry& g) const {
    const LabelSet symdiff(subset ^ g.map(subset));
    if (symdiff.empty()) {
      return std::numeric_limits<parameter_type>::max();
    }
    else {
      return symdiff.max_elem();
    }
  }

  // stream input/output:
  template <ssg_mode_type mode>
  inline std::istream& SymmetricSubsetGraphNode<mode>::read(std::istream& ist) {
    std::cerr << "SymmetricSubsetGraphNode::read(std::istream& ist):"
	      << " reading in a pointer-based structure not supported - exiting."
	      << std::endl;
    exit(1);
    return ist;
  }

  template <ssg_mode_type mode>
  inline std::istream& operator>>(std::istream& ist, SymmetricSubsetGraphNode<mode>& ssgn) {
    return ssgn.read(ist);
  }

  template <ssg_mode_type mode>
  inline std::ostream& SymmetricSubsetGraphNode<mode>::write(std::ostream& ost) const {
    const char colon = ':';
    const char comma = ',';
    const char lbracket = '[';
    const char rbracket = ']';
    ost << lbracket << _subset << colon
	<< _matrix << comma
	<< _critelem_table << comma
	<< _critelem_classification << rbracket;
    return ost;
  }

  template <ssg_mode_type mode>
  inline std::ostream& operator<<(std::ostream& ost, const SymmetricSubsetGraphNode<mode>& ssgn) {
    return ssgn.write(ost);
  }

};

#endif

// eof SymmetricSubsetGraphNode.hh
