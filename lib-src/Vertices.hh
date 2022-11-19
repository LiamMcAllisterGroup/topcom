////////////////////////////////////////////////////////////////////////////////
// 
// Vertices.hh
//
//    produced: 14 Jul 2021 jr
// last change: 14 Jul 2021 jr
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef VERTICES_HH
#define VERTICES_HH

#include <assert.h>

#include "LabelSet.hh"
#include "SimplicialComplex.hh"

#include "CommandlineOptions.hh"

#include "PointConfiguration.hh"
#include "Chirotope.hh"
#include "Circuits.hh"

namespace topcom {

  typedef parameter_type          vertex_type;
  typedef LabelSet                vertices_data;

  class Vertices : public vertices_data {
  private:
    parameter_type _no;
    parameter_type _rank;
  public:
    // constructors:
  private:
    inline Vertices();
  public:
    inline Vertices(const parameter_type& no, const parameter_type& rank);
    inline Vertices(const Vertices&);
    Vertices(const Circuits&);
    // destructor:
    inline ~Vertices();
    // accessors:
    inline const parameter_type no() const;
    inline const parameter_type rank() const;
    // stream output/input:
    std::ostream& write(std::ostream&) const;
    std::istream& read(std::istream&);

    friend inline std::ostream& operator<<(std::ostream& ost, const Vertices& v) {
      return v.write(ost);
    }
    friend inline std::istream& operator>>(std::istream& ist, Vertices& v) {
      return v.read(ist);
    }
  };

  // constructors:
  inline Vertices::Vertices(const parameter_type& no, const parameter_type& rank) : vertices_data(), _no(no), _rank(rank) {}
  inline Vertices::Vertices(const Vertices& vertices) : 
    vertices_data(vertices), _no(vertices._no), _rank(vertices._rank) {}
  // destructor:
  inline Vertices::~Vertices() {}
  // accessors:
  inline const parameter_type Vertices::no()   const { return _no; }
  inline const parameter_type Vertices::rank() const { return _rank; }

}; // namespace topcom

#endif

// eof Vertices.hh
