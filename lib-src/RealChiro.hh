////////////////////////////////////////////////////////////////////////////////
// 
// RealChiro.hh 
//
//    produced: 13/03/98 jr
// last change: 13/03/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef REALCHIRO_HH
#define REALCHIRO_HH

#include <assert.h>
#include <iostream>

#include "Global.hh"

#ifdef USE_SPARSEINTSET
#include "SparseIntegerSet.hh"
#else
#include "LabelSet.hh"
#endif

#include "CommandlineOptions.hh"

#include "StairCaseMatrix.hh"
#include "PointConfiguration.hh"
#include "Permutation.hh"

namespace topcom {

#ifdef USE_SPARSEINTSET
  typedef SparseIntegerSet                basis_type;
#else
  typedef LabelSet                        basis_type;
#endif

#ifdef STL_CHIROTOPE
#include <unordered_map>
  typedef std::unordered_map<basis_type, std::pair<short int, Field>, Hash<basis_type> > chirotope_data;
#else
#include "HashMap.hh"
  typedef HashMap<basis_type, Pair<short int, Field> >                                   chirotope_data;
#endif

  inline const char int2sign(const short int i) {
    switch(i) {
    case  1:	return '+';
    case -1:	return '-';
    default:	return '0';
    }
  }

  inline const short int sign2int(const char c) {
    switch(c) {
    case '+':	return 1;
    case '-':	return -1;
    default:	return 0;
    }
  }

  class RealChiro : public chirotope_data {
  private:
    static const char    start_char;
    static const char    end_char;
    static const char    delim_char;
    static const char*   map_chars;
  private:
    parameter_type  _no;
    parameter_type  _rank;
  protected:
    bool            _has_dets;
  public:
    // constructors:
    inline RealChiro();
    inline RealChiro(const RealChiro&);
    inline RealChiro(const parameter_type&, const parameter_type&, const bool = false);
    RealChiro(const PointConfiguration&, bool preprocess=true);
    // destructor:
    inline ~RealChiro();
    //assignment:
    inline RealChiro& operator=(const RealChiro& chiro);
    // accessors:
    inline parameter_type no() const;
    inline parameter_type rank() const;
    inline bool           has_dets() const;
    inline size_type      size() const;
    inline Field det(const basis_type&) const;
    inline Field det(const Permutation&) const;
    // functions:
    void erase_random();
    const basis_type find_non_deg_basis() const;
    RealChiro dual() const;
    // operators:
    inline const int operator()(const basis_type&) const;
    const int operator()(const basis_type&  prebasis,
			 const Permutation& lex_extension_perm) const;
    inline const bool operator==(const RealChiro&) const;
    inline const bool operator!=(const RealChiro&) const;
    // stream output/input:
    std::istream& read(std::istream&);
    std::istream& read_string(std::istream&);
    std::ostream& write(std::ostream&) const;
    std::ostream& print_string(std::ostream&) const;
    std::ostream& print_dualstring(std::ostream&) const;
    inline friend std::ostream& operator<<(std::ostream&, const RealChiro& chiro);
    inline friend std::istream& operator>>(std::istream&, RealChiro& chiro);
  private:
    // internal algorithms:
    void _recursive_chiro(const StairCaseMatrix&    current,
			  const PointConfiguration& points,
			  const basis_type&         basis,
			  const parameter_type      start,
			  const parameter_type      step,
			  const bool                already_dependent);
    void _recursive_find_nd_basis(const StairCaseMatrix&    current,
				  const PointConfiguration& points,
				  const basis_type&         basis,
				  const parameter_type      start,
				  const parameter_type      step,
				  const bool                already_dependent);
  };

  inline RealChiro::RealChiro() : chirotope_data(), _no(0), _rank(0), _has_dets(false) {
  }
  inline RealChiro::RealChiro(const RealChiro& chiro) : 
    chirotope_data(chiro), _no(chiro._no), _rank(chiro._rank), _has_dets(false) {
  }
  inline RealChiro::RealChiro(const parameter_type& no,
			      const parameter_type& rank,
			      const bool            has_dets) :
    chirotope_data(), _no(no), _rank(rank), _has_dets(has_dets) {}

  // destructor:
  inline RealChiro::~RealChiro() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "RealChiro::~RealChiro(): destructor called" << std::endl;
#endif
  }

  //assignment:
  inline RealChiro& RealChiro::operator=(const RealChiro& chiro) {
    if (this == &chiro) {
      return *this;
    }
    chirotope_data::operator=(chiro);
    _no = chiro._no;
    _rank = chiro._rank;
    _has_dets = chiro._has_dets;
    return *this;
  }

  // accessors:
  inline parameter_type RealChiro::no()       const { return _no; }
  inline parameter_type RealChiro::rank()     const { return _rank; }
  inline bool           RealChiro::has_dets() const { return _has_dets; }
  inline size_type      RealChiro::size()     const { 
#ifdef STL_CHIROTOPE
    return static_cast<size_type>(chirotope_data::size());
#else
    return static_cast<size_type>(chirotope_data::size());
#endif
  }
  inline Field RealChiro::det(const basis_type& basis) const {
    if (!_has_dets) {
      std::cerr << "Field RealChiro::det(const basis_type&) const: "
		<< "det requested, but chirotope has no dets - exiting" << std::endl;
      exit(1);
    }
    return find(basis)->second.second;
  }
  
  inline Field RealChiro::det(const Permutation& basisperm) const {
    int permsign = basisperm.sign();
    if (permsign > 0) {
      return det(basis_type(basisperm));
    }
    else {
      return -det(basis_type(basisperm));
    }
  }

  // operators:
  inline const int RealChiro::operator()(const basis_type& basis) const {
#ifdef INDEX_CHECK
    std::cerr << "basis: " << basis << std::endl;
    if (!(member(basis))) {
      std::cerr << *this << " does not contain " << basis << std::endl;
      exit(1);
    }
#endif
#ifndef STL_CHIROTOPE
    return member(basis)->first;
#else
    return find(basis)->second.first;
#endif
  }

  inline const bool RealChiro::operator==(const RealChiro& chiro) const {
#ifdef STL_CHIROTOPE
    return ((*this) == chiro);
#else
    return chirotope_data::operator==(chiro);
#endif
  }

  inline const bool RealChiro::operator!=(const RealChiro& chiro) const {
    return !operator==(chiro);
  }

  // stream output/input:
  inline std::istream& operator>>(std::istream& ist, RealChiro& chiro) {
    return chiro.read(ist);
  }

  inline std::ostream& operator<<(std::ostream& ost, const RealChiro& chiro) {
    return chiro.write(ost);
  }

}; // namespace topcom

#endif

// eof RealChiro.hh
