////////////////////////////////////////////////////////////////////////////////
// 
// TriangFlips.hh 
//
//    produced: 27/05/98 jr
// last change: 18/06/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef TRIANGFLIPS_HH
#define TRIANGFLIPS_HH

#include "Pair.hh"
#include "HashSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "Circuits.hh"
#include "Flip.hh"
#include "MarkedFlips.hh"
#include "TriangNode.hh"
#include "Symmetry.hh"

namespace topcom {

  class TriangFlips {
  private:
    parameter_type   _no;
    parameter_type   _rank;
    MarkedFlips      _flips;
  public:
    // constructors:
    inline TriangFlips();
    inline TriangFlips(const TriangFlips&);
    inline TriangFlips(TriangFlips&&);
    inline TriangFlips(const Chirotope&,
		       const TriangNode&,
		       const symmetryptr_datapair&,
		       const bool = false);
    inline TriangFlips(const Chirotope&,
		       const TriangNode&, 
		       const TriangFlips&, 
		       const TriangNode&, 
		       const Flip&, 
		       const symmetryptr_datapair&,
		       const bool = false);
    // destructor:
    inline ~TriangFlips();
    // assignment:
    inline TriangFlips& operator=(const TriangFlips& tf);
    inline TriangFlips& operator=(TriangFlips&& tf);
    // accessors:
    inline const parameter_type no()        const { return _no; }
    inline const parameter_type rank()      const { return _rank; }
    inline const MarkedFlips&   flips()     const { return _flips; }
    // modifiers:
    inline void mark_flip       (const FlipRep&);
    inline void mark_all_flips  ();
    // internal algorithms:
    void _remove_destroyed_flips(const TriangNode& tn, 
				 const Flip&);
    void _add_new_flips         (const Chirotope& chiro,
				 const TriangNode& tn, 
				 const SimplicialComplex&,
				 const symmetryptr_datapair&,
				 const bool = false,
				 const bool = false);
    // stream input/output:
    inline std::ostream& write  (std::ostream&) const;
    inline std::istream& read   (std::istream&);
    inline friend std::ostream& operator<<(std::ostream&, const TriangFlips&);
    inline friend std::istream& operator<<(std::istream&, TriangFlips&);
  };

  // constructors:
  inline TriangFlips::TriangFlips() : _no(0), _rank(0), _flips() {}
  inline TriangFlips::TriangFlips(const TriangFlips& tf) :
    _no(tf._no), _rank(tf._rank), _flips(tf._flips) {}
  inline TriangFlips::TriangFlips(TriangFlips&& tf) :
    _no(tf._no), _rank(tf._rank), _flips(std::move(tf._flips)) {}
  inline TriangFlips::TriangFlips(const Chirotope&            chiro,
				  const TriangNode&           tn, 
				  const symmetryptr_datapair& node_symmetryptrs,
				  const bool                  forbid_vertex_removal) :
    _no(tn.no()), _rank(tn.rank()), _flips() {
    _add_new_flips(chiro, tn, tn, node_symmetryptrs, forbid_vertex_removal);
  }
  inline TriangFlips::TriangFlips(const Chirotope&            chiro,
				  const TriangNode&           tn_before_flip,
				  const TriangFlips&          tf_before_flip,
				  const TriangNode&           tn_after_flip,
				  const Flip&                 flip,
				  const symmetryptr_datapair& tn_after_symmetryptrs,
				  const bool                  forbid_vertex_removal) :
    _no(tf_before_flip._no), _rank(tf_before_flip._rank),
    _flips(tf_before_flip._flips) {
    _remove_destroyed_flips(tn_before_flip, flip);
    _flips.unmark_all();
    _add_new_flips(chiro, 
		   tn_after_flip, 
		   flip.second, 
		   tn_after_symmetryptrs, 
		   forbid_vertex_removal);
  }

  // destructor:
  inline TriangFlips::~TriangFlips() {}
  // assignment:
  inline TriangFlips& TriangFlips::operator=(const TriangFlips& tf) {
    if (this == &tf) {
      return *this;
    }
    _no        = tf._no;
    _rank      = tf._rank;
    _flips     = tf._flips;
    return *this;
  }
  inline TriangFlips& TriangFlips::operator=(TriangFlips&& tf) {
    if (this == &tf) {
      return *this;
    }
    _no        = tf._no;
    _rank      = tf._rank;
    _flips     = std::move(tf._flips);
    return *this;
  }

  // modifiers:
  inline void TriangFlips::mark_flip(const FlipRep& fliprep) {
    _flips.mark(fliprep);
  }
  inline void TriangFlips::mark_all_flips() {
    _flips.mark_all();
  }

  // stream input/output:
  inline std::ostream& TriangFlips::write(std::ostream& ost) const {
#ifndef STL_FLIPS
    ost << '[' << _no << ',' << _rank << ":" << _flips << ']';
#else
    ost << '[' << _no << ',' << _rank << ":" << _flips << ']';
#endif
    return ost;
  }

  inline std::istream& TriangFlips::read(std::istream& ist) {
#ifndef STL_FLIPS
    char c;

    ist >> std::ws >> c 
	>> std::ws >> _no 
	>> std::ws >> c 
	>> std::ws >> _rank
	>> std::ws >> c 
	>> std::ws >> _flips
	>> std::ws >> c;
#else
    char c;

    ist >> std::ws >> c 
	>> std::ws >> _no 
	>> std::ws >> c 
	>> std::ws >> _rank
	>> std::ws >> c 
	>> std::ws >> _flips
	>> std::ws >> c;
#endif
    return ist;
  }

  inline std::ostream& operator<<(std::ostream& ost, const TriangFlips& tf) {
    return tf.write(ost);
  }

  inline std::istream& operator>>(std::istream& ist, TriangFlips& tf) {
    return tf.read(ist);
  }

}; // namespace topcom

#endif

// eof TriangFlips.hh
