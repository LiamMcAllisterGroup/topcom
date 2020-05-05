//////////////////////////////////////////////////////////////////////////
// SymmetricBFS.hh
// produced: 29/06/98 jr
// last change: 29/06/98 jr
/////////////////////////////////////////////////////////////////////////
#ifndef SYMMETRICBFS_HH
#define SYMMETRICBFS_HH

#include <iostream>
#include <fstream>
#include <string>

#include <assert.h>
#include <set>
#include <map>
// #include <unordered_set>
// #include <unordered_map>

#include "HashSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "Flip.hh"
#include "MarkedFlips.hh"
#include "TriangNode.hh"
#include "TriangFlips.hh"
#include "Symmetry.hh"
#include "RegularityCheck.hh"

#ifndef STL_CONTAINERS
typedef HashSet<SimplicialComplex>            triang_container_type;
typedef HashSet<TriangNode>                   orbit_type;
typedef HashMap<TriangNode, TriangFlips>      tnode_container_type; // Node->Flips
#else
typedef std::set<SimplicialComplex>            triang_container_type;
typedef std::set<TriangNode>                   orbit_type;
typedef std::map<TriangNode, TriangFlips>      tnode_container_type; // Node->Flips
// typedef std::tr1::unordered_set<SimplicialComplex>            triang_container_type;
// typedef std::tr1::unordered_set<TriangNode>                   orbit_type;
// typedef std::tr1::unordered_map<TriangNode, TriangFlips>      tnode_container_type; // Node->Flips

inline std::ostream& operator<<(std::ostream& ost, const tnode_container_type& tnc) {
  for (tnode_container_type::const_iterator iter = tnc.begin();
       iter != tnc.end();
       ++iter) {
    ost << iter->first << std::endl;
  }
  return ost;
}

inline std::istream& operator>>(std::istream& ist, tnode_container_type& tnc) {
  char dash;
  char arrow;

  TriangNode tn_reader;
  TriangFlips tf_reader;
  
  char c;

  tnc.clear();
  ist >> std::ws >> c;
  if (c == '[') {
    while (ist >> std::ws >> c) {
      if (c == ']') {
	break;
      }
      if (c == ',') {
	continue;
      }
      ist.putback(c);
      if (!(ist >> std::ws >> tn_reader)) {
#ifdef READ_DEBUG
	std::cerr << "std::istream& operator>>(std::istream&, tnode_container_type&): "
		  << "key not found." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
      if (!(ist >> std::ws >> dash >> arrow)) {
#ifdef READ_DEBUG
	std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
		  << "`->' not found." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
      if (!(ist >> std::ws >> tf_reader)) {
#ifdef READ_DEBUG
	std::cerr << "std::istream& operator>>(std::istream&, chirotope_data&): "
		  << "data not found." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
      tnc[tn_reader] = tf_reader;
    }
  }
  else {
#ifdef READ_DEBUG
    std::cerr << "std::istream& operator>>(std::istream&, tnode_container_type&): "
	 << "missing `" << '[' << "'." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  ist.clear(std::ios::goodbit);
  return ist;
}

#endif

#ifdef CHECK_NEW
static tnode_container_type all_triangs;
#endif

class __sbfs_cout_triang_base {
public:
  inline virtual void operator()(const size_type no,
				 const TriangNode& tn) const {}
};

class __sbfs_cout_triang : public __sbfs_cout_triang_base {
public:
  inline virtual void operator()(const size_type no,
				 const TriangNode& tn) const {
    std::cout << "T[" << tn.ID() << "]:=";
    std::cout << tn << ";" << std::endl;
  }
};

class __sbfs_output_pred_base {
public:
  inline virtual bool operator()(const PointConfiguration& points,
				 const Chirotope&          chiro,
				 const TriangNode&         tn) const {
    return true;
  }
};

class __sbfs_output_checkno : public __sbfs_output_pred_base {
public:
  inline virtual bool operator()(const PointConfiguration& points,
				 const Chirotope&          chiro,
				 const TriangNode&         tn) const {
    return (tn.card() == CommandlineOptions::no_of_simplices());
  }
};

class __sbfs_output_checknonregularity : public __sbfs_output_pred_base {
public:
  inline virtual bool operator()(const PointConfiguration& points,
				 const Chirotope&          chiro,
				 const TriangNode&         tn) const {
    assert (points.no() == chiro.no());
    return !RegularityCheck(points, chiro, tn).is_regular();
  }
};

class __sbfs_search_pred_base {
public:
  inline virtual bool operator()(const PointConfiguration& points,
				 const Chirotope&          chiro,
				 const TriangNode&         tn) const {
    return true;
  }
};

class __sbfs_search_checkreg : public __sbfs_search_pred_base {
public:
  inline virtual bool operator()(const PointConfiguration& points,
				 const Chirotope&          chiro,
				 const TriangNode&         tn) const {
    assert (points.no() == chiro.no());
    return RegularityCheck(points, chiro, tn).is_regular();
  }
};

class __sbfs_search_seedsupportonly : public __sbfs_search_pred_base {
private:
  IntegerSet _seed_support;
public:
  inline __sbfs_search_seedsupportonly(const IntegerSet& seed_support) : 
    _seed_support(seed_support) {}
  inline virtual bool operator()(const PointConfiguration& points,
				 const Chirotope&          chiro,
				 const TriangNode&         tn) const {
    return (tn.support() == _seed_support);
  }
};

class __sbfs_search_seedsupportandregular : public __sbfs_search_pred_base {
private:
  IntegerSet _seed_support;
public:
  inline __sbfs_search_seedsupportandregular(const IntegerSet& seed_support) : 
    _seed_support(seed_support) {}
  inline virtual bool operator()(const PointConfiguration& points,
				 const Chirotope&          chiro,
				 const TriangNode&         tn) const {
    assert (points.no() == chiro.no());
    return ((tn.support() == _seed_support) && (RegularityCheck(points, chiro, tn).is_regular()));
  }
};

class __sbfs_search_sometimes : public __sbfs_search_pred_base {
public:
  inline virtual bool operator()(const PointConfiguration& points,
				 const Chirotope&          chiro,

				 const TriangNode&         tn) const {

    static long count(0);
    
    if (CommandlineOptions::debug()) {
       std::cerr << "checking regularity? ";
    }
    if (count++ == CommandlineOptions::sometimes_frequency()) {
      count = 0;
      if (CommandlineOptions::debug()) {
	std::cerr << "yes." << std::endl;
      }
      if (CommandlineOptions::verbose()) {
       	std::cerr << "checking regularity ..." << std::endl;
      }
      const bool found_regular = RegularityCheck(points, chiro, tn).is_regular();
      if (found_regular) {
	std::cerr << "regular triangulation found in component:" << std::endl;
	std::cerr << "triangulation: " << std::endl;
	std::cerr << tn << std::endl;
	exit(1);
      }
      if (CommandlineOptions::verbose()) {
	std::cerr <<  "... done." << std::endl;
      }
      return true;
    }
    else {
      if (CommandlineOptions::debug()) {
	std::cerr << " not yet." << std::endl;
      }
      return true;
    }
  }
};

class __sbfs_search_min_no_of_verts : public __sbfs_search_pred_base {
private: 
  mutable size_type _min_no_of_verts;
public:

  // constructor:
  inline __sbfs_search_min_no_of_verts(const size_type min_no_of_verts) :
    _min_no_of_verts(min_no_of_verts) {}

  // function:
  inline virtual bool operator()(const PointConfiguration& points,
				 const Chirotope&          chiro,
				 const TriangNode&         tn) const {
    const size_type tn_support_card(tn.support().card());
    if (tn_support_card < _min_no_of_verts) {
      _min_no_of_verts = tn_support_card;
      if (CommandlineOptions::verbose()) {
	std::cerr << "found triangulation with one vertex removed:" << std::endl;
	std::cerr << tn << std::endl;
	std::cerr << "support:" << std::endl;
	std::cerr << tn.support() << std::endl;
	std::cerr << "number of vertices:" << std::endl;
	std::cerr << tn_support_card << std::endl;
      }
      return true;
    }
    else {
      if (tn_support_card > _min_no_of_verts) {
	return false;
      }
      else {
	return true;
      }
    }
  }
};


class SymmetricBFS {
private:
  typedef __sbfs_cout_triang_base             cout_triang_base_type;
  typedef __sbfs_cout_triang                  cout_triang_type;
  typedef __sbfs_output_pred_base             output_pred_base_type;
  typedef __sbfs_output_checkno               output_checkno_type;
  typedef __sbfs_output_checknonregularity    output_checknonregularity_type;
  typedef __sbfs_search_pred_base             search_pred_base_type;
  typedef __sbfs_search_checkreg              search_checkreg_type;
  typedef __sbfs_search_sometimes             search_checksometimes_type;
  typedef __sbfs_search_seedsupportonly       search_seedsupportonly_type;
  typedef __sbfs_search_seedsupportandregular search_seedsupportandregular_type;
  typedef __sbfs_search_min_no_of_verts       search_min_no_of_verts_type;
private:
  const parameter_type            _no;
  const parameter_type            _rank;
  const PointConfiguration*       _pointsptr;
  const Chirotope*                _chiroptr;
  const SymmetryGroup             _symmetries;
  SymmetryGroup                   _node_symmetries;
  tnode_container_type            _previous_triangs;
  tnode_container_type            _new_triangs;
  tnode_container_type::iterator  _find_iter;
  size_type                       _totalcount;
  size_type                       _symcount;
  size_type                       _reportcount;
  size_type                       _flipcount;
private:
  triang_container_type           _all_triangs;
private:
  TriangNode                      _representative;
  Symmetry                        _transformation;
  size_type                       _orbitsize;
  orbit_type                      _orbit;
  bool                            _rep_has_search_pred;
private:
  cout_triang_base_type*          _cout_triang_ptr;
  output_pred_base_type*          _output_pred_ptr;
  search_pred_base_type*          _search_pred_ptr;
  bool                            _only_fine_triangs;
  bool                            _print_triangs;
  bool                            _only_unimodular_triangs;
private:
  size_type                       _processed_count;
  int                             _dump_no;
  std::fstream                    _dump_str;
private:
  SymmetricBFS();
  SymmetricBFS(const SymmetricBFS&);
  SymmetricBFS& operator=(const SymmetricBFS&);
public:
  // constructor:
  inline SymmetricBFS(const parameter_type, 
		      const parameter_type,
		      const PointConfiguration&,
		      const Chirotope&,
		      const SymmetryGroup&,
		      const SimplicialComplex&,
		      const SymmetryGroup&,
		      const bool = false,
		      const bool = false);
  // destructor:
  inline ~SymmetricBFS();
  // results:
  inline const size_type totalcount() const { return _totalcount; }
  inline const size_type symcount()   const { return _symcount; }
  // internal methods:
  void _mark_equivalent_flips(const TriangNode&, 
			      const tnode_container_type::iterator,
			      const FlipRep&);
  void _mark_equivalent_flips(const TriangNode&,
			      TriangFlips&, 
			      const FlipRep&);
  const int _old_symmetry_class(const TriangNode&);
  void _process_newtriang(const TriangNode&, 
			  const TriangFlips&, 
			  const TriangNode&, 
			  const FlipRep&);
  void _process_flips(const TriangNode&, const TriangFlips&);
  void _bfs_step();
  void _bfs();
  // stream output/input:
  std::istream& read(std::istream&);
  inline std::ostream& write(std::ostream&) const;
  friend inline std::istream& operator>>(std::istream& ist, SymmetricBFS& sbfs);
  friend inline std::ostream& operator<<(std::ostream& ost, const SymmetricBFS& sbfs);

};

inline SymmetricBFS::SymmetricBFS(const parameter_type      no, 
				  const parameter_type      rank,
				  const PointConfiguration& points,
				  const Chirotope&          chiro,
				  const SymmetryGroup&      symmetries,
				  const SimplicialComplex&  seed,
				  const SymmetryGroup&      seed_symmetries,
				  const bool                print_triangs,
				  const bool                only_fine_triangs) :
  _no(no), _rank(rank), _pointsptr(&points), _chiroptr(&chiro),
  _symmetries(symmetries), _node_symmetries(seed_symmetries),
  _previous_triangs(), _new_triangs(),
  _find_iter(_previous_triangs.begin()),
  _totalcount(0), _symcount(0), _flipcount(0), _reportcount(CommandlineOptions::report_frequency()),
  _representative(_symcount, no, rank, seed), _transformation(_no),
  _orbitsize(0), _orbit(),
  _rep_has_search_pred(false),
  _print_triangs(print_triangs),
  _only_fine_triangs(only_fine_triangs),
  _processed_count(0L),
  _dump_no(0) {
  
  IntegerSet seed_support(seed.support());
  if (_print_triangs) {
    _cout_triang_ptr = new cout_triang_type();
  }
  else {
    _cout_triang_ptr = new cout_triang_base_type();
  }
  if (CommandlineOptions::no_of_simplices() > 0) {
    _output_pred_ptr = new output_checkno_type();
  }
  else if (CommandlineOptions::check_nonregular()) {
    _output_pred_ptr = new output_checknonregularity_type();
  }
  else {
    _output_pred_ptr = new output_pred_base_type();
  }
  if (CommandlineOptions::check_regular() && CommandlineOptions::dont_add_points()) {
    _search_pred_ptr = new search_seedsupportandregular_type(seed_support);
  }
  else if (CommandlineOptions::check_regular()) {
    _search_pred_ptr = new search_checkreg_type();
  }
  else if (CommandlineOptions::dont_add_points()) {
    _search_pred_ptr = new search_seedsupportonly_type(seed_support);
  }
  else if (CommandlineOptions::check_sometimes()) {
    _search_pred_ptr = new search_checksometimes_type();
  }
  else if (CommandlineOptions::reduce_points()) {
    _search_pred_ptr = new search_min_no_of_verts_type(seed_support.card());
  }
  else {
    _search_pred_ptr = new search_pred_base_type();
  }
  if (CommandlineOptions::read_status()) {
    std::ifstream read_str(CommandlineOptions::read_file());
    
    read(read_str);

    std::cerr << "SymmetricBFS initialized from file " << CommandlineOptions::read_file() << std::endl;
    if (CommandlineOptions::debug()) {
      std::cerr << "data:" 
	      << std::endl
	      << *this
	      << std::endl;
    }
  }
  else {
    const TriangNode current_triang = TriangNode(_symcount, no, rank, seed);
    if (CommandlineOptions::simple()) {
      const TriangFlips current_flips;
      _old_symmetry_class(current_triang);
      if (_orbitsize > 0) {
	if ((*_output_pred_ptr)(points, chiro, current_triang)) {
	  _totalcount += _orbitsize;
	  ++_symcount;
	  --_reportcount;
	  (*_cout_triang_ptr)(_symcount, current_triang);
	}
	_all_triangs.insert(current_triang);
	_previous_triangs[current_triang] = current_flips;
      }
    }
    else {
      _old_symmetry_class(current_triang);
      if (_orbitsize > 0) {
	if ((*_output_pred_ptr)(points, chiro, current_triang)) {
	  _totalcount += _orbitsize;
	  ++_symcount;
	  --_reportcount;
	  (*_cout_triang_ptr)(_symcount, current_triang);
	}
	const TriangFlips current_flips = TriangFlips(*_chiroptr,
						      current_triang, 
						      _node_symmetries,
						      _only_fine_triangs);
	_previous_triangs[current_triang] = current_flips;
      }
    }
  }
  _bfs();
}

inline SymmetricBFS::~SymmetricBFS() {
  delete _cout_triang_ptr;
  delete _output_pred_ptr;
  delete _search_pred_ptr;
}

// stream output/input:

inline std::ostream& SymmetricBFS::write(std::ostream& ost) const {
  ost << "TOPCOM SymmetricBFS dump start:" << std::endl;
  ost << "_dump_no             " << _dump_no << std::endl;
  ost << "_no                  " << _no << std::endl;
  ost << "_rank                " << _rank << std::endl;
  if (_pointsptr) {
    ost << "_points              " << *_pointsptr << std::endl;
  }
  if (_chiroptr) {
    ost << "_chiro               " << *_chiroptr << std::endl;
  }
  ost << "_symmetries          " << _symmetries << std::endl;
  ost << "_print_triangs       " << _print_triangs << std::endl;
  ost << "_only_fine_triangs   " << _only_fine_triangs << std::endl;
  ost << "_previous_triangs    " << _previous_triangs << std::endl;
  ost << "_new_triangs         " << _new_triangs << std::endl;
  ost << "_totalcount          " << _totalcount << std::endl;
  ost << "_symcount            " << _symcount << std::endl;
  ost << "_reportcount         " << _reportcount << std::endl;
  ost << "SymmetricBFS dump end." << std::endl;
  return ost;
}

inline std::istream& operator>>(std::istream& ist, SymmetricBFS& sbfs) {
  return sbfs.read(ist);
}

inline std::ostream& operator<<(std::ostream& ost, const SymmetricBFS& sbfs) {
  return sbfs.write(ost);
}

#endif

// eof SymmetricBFS.hh
