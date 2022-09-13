//////////////////////////////////////////////////////////////////////////
// PermlibPermutationGroup.hh
// produced: 05/06/2022 jr
// last change: 05/06/2022 jr
// (based on "Permlib" by Thomas Rehn [Diploma Thesis 2010])
/////////////////////////////////////////////////////////////////////////
#ifndef PERMLIBPERMUTATIONGROUP_HH
#define PERMLIBPERMUTATIONGROUP_HH

#ifdef HAVE_LIBPERMLIB

#include "Global.hh"
#include "ContainerIO.hh"
#include "LabelSet.hh"
#include "Symmetry.hh"

#include <permlib/permlib_api.h>

namespace topcom {

  template <comparison_mode_type mode>
  class PermlibPermutationGroup {
  public:
    typedef comparison_mode_type                 mode_type;
  private:
    boost::shared_ptr<permlib::PermutationGroup> _groupptr;
  private:
    PermlibPermutationGroup() = delete;
  public:
    
    // constructors:
    inline PermlibPermutationGroup(const PermlibPermutationGroup&); // copied
    inline PermlibPermutationGroup(const boost::shared_ptr<permlib::PermutationGroup>&); // from data
    PermlibPermutationGroup(const parameter_type); // contains only the identity
    PermlibPermutationGroup(const parameter_type,
			    const symmetry_collectordata&); // built from generators
    PermlibPermutationGroup(const SymmetryGroup&); // built from symmetry group
  
    // destructor:
    inline ~PermlibPermutationGroup();
  
    // copy assignment:
    inline PermlibPermutationGroup& operator=(const PermlibPermutationGroup&);

    // accessors:
    inline parameter_type               n              () const;
    inline size_type                    order          () const;

    // standard functions:
    bool                                empty          () const;
    bool                                is_in_group    (const Symmetry&) const;
    PermlibPermutationGroup             stabilizer     (const LabelSet&) const;
    size_type                           stabilizer_card(const LabelSet&) const;
    size_type                           orbit_size     (const LabelSet&) const;

    bool                                lex_decreases  (const LabelSet&) const;
    bool                                colex_increases(const LabelSet&) const;
   
    // stream input/output:
    inline std::istream& read(std::istream& ist);
    template <comparison_mode_type streammode>
    friend inline std::istream& operator>>(std::istream& ist, PermlibPermutationGroup<streammode>& st);
    inline std::ostream& write(std::ostream& ost) const;
    template <comparison_mode_type streammode>
    friend inline std::ostream& operator<<(std::ostream& ost, const PermlibPermutationGroup<streammode>& st);
  private:
    
    // internal methods:
    inline parameter_type _elem(const parameter_type) const;
    inline bool _compare_subsets(const LabelSet&, const LabelSet&) const;
  };
  
  // forward template specializations:
  template <>
  inline parameter_type PermlibPermutationGroup<lexmin_mode>::_elem(const parameter_type) const;
  
  template <>
  inline parameter_type PermlibPermutationGroup<colexmax_mode>::_elem(const parameter_type) const;

  template <>
  inline bool PermlibPermutationGroup<lexmin_mode>::_compare_subsets(const LabelSet&, const LabelSet&) const;
  
  template <>
  inline bool PermlibPermutationGroup<colexmax_mode>::_compare_subsets(const LabelSet&, const LabelSet&) const;

  // constructors:
  template <comparison_mode_type mode>
  inline PermlibPermutationGroup<mode>::PermlibPermutationGroup(const PermlibPermutationGroup& ppg) :
    _groupptr(ppg._groupptr) {
  }

  template <comparison_mode_type mode>
  inline PermlibPermutationGroup<mode>::PermlibPermutationGroup(const boost::shared_ptr<permlib::PermutationGroup>& groupptr) :
    _groupptr(groupptr) {
  }

  template <comparison_mode_type mode>
  PermlibPermutationGroup<mode>::PermlibPermutationGroup(const parameter_type n) :
    _groupptr(new permlib::PermutationGroup(n)) {
  }
     
  template <comparison_mode_type mode>
  PermlibPermutationGroup<mode>::PermlibPermutationGroup(const parameter_type n,
							 const symmetry_collectordata& generators) {
    std::vector<permlib::Permutation::ptr> genptr_collector;
    for (symmetry_collectordata::const_iterator geniter = generators.begin();
	 geniter != generators.end();
	 ++geniter) {
      genptr_collector.push_back(permlib::Permutation::ptr(new permlib::Permutation(Permutation(*geniter))));
    }
    _groupptr = permlib::construct(n, genptr_collector.begin(), genptr_collector.end());
  }
   
  template <comparison_mode_type mode>
  PermlibPermutationGroup<mode>::PermlibPermutationGroup(const SymmetryGroup& sg) {
    std::vector<permlib::Permutation::ptr> symptr_collector;
    for (SymmetryGroup::const_iterator symiter = sg.begin();
	 symiter != sg.end();
	 ++symiter) {
      symptr_collector.push_back(permlib::Permutation::ptr(new permlib::Permutation(Permutation(*symiter))));
    }
    _groupptr = permlib::construct(sg.n(), symptr_collector.begin(), symptr_collector.end());    
  }

  // destructor:
  template <comparison_mode_type mode>
  inline PermlibPermutationGroup<mode>::~PermlibPermutationGroup() {
  }

  // assignment:
  template <comparison_mode_type mode>
  inline PermlibPermutationGroup<mode>& PermlibPermutationGroup<mode>::operator=(const PermlibPermutationGroup& st) {
    if (&st == this) {
      return *this;
    }
    _groupptr.reset();
    _groupptr = new permlib::Permutation::ptr(*st._groupptr); // deep copy
    return *this;
  }

  // accessors:
  template <comparison_mode_type mode>
  inline parameter_type PermlibPermutationGroup<mode>::n() const {
    return _groupptr->n;
  }

  template <comparison_mode_type mode>
  inline size_type PermlibPermutationGroup<mode>::order() const {
    return _groupptr->order();
  }

  // standard functions:
  template <comparison_mode_type mode>
  bool PermlibPermutationGroup<mode>::empty() const {
    return (order() == 1);
  }


  template <comparison_mode_type mode>
  bool PermlibPermutationGroup<mode>::is_in_group(const Symmetry& s) const {
    return _groupptr->sifts(permlib::Permutation(Permutation(s)));
  }

  template <comparison_mode_type mode>
  PermlibPermutationGroup<mode> PermlibPermutationGroup<mode>::stabilizer(const LabelSet& is) const {
    std::vector<parameter_type> is_vec(is.convert_to_vector_of_parameter_types());
    return PermlibPermutationGroup<mode>(permlib::setStabilizer(*_groupptr, is_vec.begin(), is_vec.end()));
  }
  
  template <comparison_mode_type mode>
  size_type PermlibPermutationGroup<mode>::stabilizer_card(const LabelSet& is) const {
    return permlib::setStabilizer(*_groupptr, is.begin(), is.end()).order();
  }
  
  template <comparison_mode_type mode>
  size_type PermlibPermutationGroup<mode>::orbit_size(const LabelSet& is) const {
    return order() / stabilizer_card(is);
  }

  template <comparison_mode_type mode>
  bool PermlibPermutationGroup<mode>::lex_decreases(const LabelSet& ls) const {
    if (mode != lexmin_mode) {
      std::cerr << "PermlibPermutationGroup::lex_decreases(...): wrong mode - exiting." << std::endl;
      exit(1);
    }
    permlib::dset bitset_rep(_groupptr->n);
    for (LabelSet::const_iterator lsiter = ls.begin();
	 lsiter != ls.end();
	 ++lsiter) {
      bitset_rep.set(*lsiter);
    }
    permlib::dset lexminset = permlib::smallestSetImage(*_groupptr, bitset_rep);
    return (lexminset != bitset_rep);
  }

  template <comparison_mode_type mode>
  bool PermlibPermutationGroup<mode>::colex_increases(const LabelSet& ls) const {
    if (mode != colexmax_mode) {
      std::cerr << "PermlibPermutationGroup::colex_increases(...): wrong mode - exiting." << std::endl;
      exit(1);
    }
    permlib::dset bitset_rep(_groupptr->n);
    for (LabelSet::const_iterator lsiter = ls.begin();
    	 lsiter != ls.end();
    	 ++lsiter) {
      bitset_rep.set(_elem(*lsiter));
    }
    permlib::dset colexmaxset = permlib::smallestSetImage(*_groupptr, bitset_rep);
    return (colexmaxset != bitset_rep);
  }
  
  // stream input/output:
  template <comparison_mode_type mode>
  inline std::istream& PermlibPermutationGroup<mode>::read(std::istream& ist) {
    std::cerr << "PermlibPermutationGroup::read(std::istream&): reading into PermlibPermutationGroup not supported - exiting" << std::endl;
    exit(1);
  }
  
  template <comparison_mode_type streammode>
  inline std::istream& operator>>(std::istream& ist, PermlibPermutationGroup<streammode>& st) {
    return st.read(ist);
  }
  
  template <comparison_mode_type mode>
  inline std::ostream& PermlibPermutationGroup<mode>::write(std::ostream& ost) const {
    ost << *_groupptr;
    return ost;
  }
  
  template <comparison_mode_type streammode>
  inline std::ostream& operator<<(std::ostream& ost, const PermlibPermutationGroup<streammode>& st) {
    return st.write(ost);
  }
  
  // internal methods:
  template <>
  inline parameter_type PermlibPermutationGroup<lexmin_mode>::_elem(const parameter_type i) const {
    return i;
  }

  template <>
  inline parameter_type PermlibPermutationGroup<colexmax_mode>::_elem(const parameter_type i) const {
    return _groupptr->n - i - 1;
  }

  template <>
  inline bool PermlibPermutationGroup<lexmin_mode>::_compare_subsets(const LabelSet& ls1,
								     const LabelSet& ls2) const {
    return ls1.lexsmaller(ls2);
  }

  template <>
  inline bool PermlibPermutationGroup<colexmax_mode>::_compare_subsets(const LabelSet& ls1,
								       const LabelSet& ls2) const {
    return ls1.colexgreater(ls2);
  }
  
}; // namespace topcom

#endif

#endif

// eof PermlibPermutationGroup.hh
