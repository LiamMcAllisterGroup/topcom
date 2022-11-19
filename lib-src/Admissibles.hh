////////////////////////////////////////////////////////////////////////////////
// 
// Admissibles.hh 
//
//    produced: 19/04/98 jr
// last change: 19/04/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef ADMISSIBLES_HH
#define ADMISSIBLES_HH

#include <assert.h>

#include "SimplicialComplex.hh"
#include "VertexFacetTable.hh"

#include "CommandlineOptions.hh"

#include "Chirotope.hh"
#include "Circuits.hh"

namespace topcom {
#ifndef STL_CONTAINERS
#include "HashMap.hh"
  typedef HashMap<Simplex, SimplicialComplex> admissibles_data;
#else
#include <unordered_map>
  typedef std::unordered_map<Simplex, SimplicialComplex, Hash<Simplex> > admissibles_data;
#endif

  class Admissibles : public admissibles_data {
  private:
    class Worker {
    private:
      int                     _ID;
      SimplicialComplex       _input;
      admissibles_data        _output;
    private:
      Worker() = delete;
    public:
      inline Worker(const Worker& init_worker) : _ID(init_worker._ID), _input(init_worker._input), _output(init_worker._output) {}
      inline Worker(const Worker&& init_worker) : _ID(init_worker._ID), _input(std::move(init_worker._input)), _output(std::move(init_worker._output)) {}
      inline Worker(const int init_ID, const SimplicialComplex& init_input) : _ID(init_ID), _input(init_input), _output() {}
      inline Worker& operator=(const Worker& worker) {
	if (this == &worker) {
	  return *this;
	}
	_ID = worker._ID;
	_input = worker._input;
	_output = worker._output;
	return *this;
      }
      inline Worker& operator=(const Worker&& worker) {
	if (this == &worker) {
	  return *this;
	}
	_ID = worker._ID;
	_input = std::move(worker._input);
	_output = std::move(worker._output);
	return *this;
      }
      void operator()(const SimplicialComplex&, const VertexFacetTable&, const Circuits&);
    public:
      inline       int                ID    () const { return _ID; }
      inline const admissibles_data&  output() const { return _output; }
    };
  private:
    static const char    start_char;
    static const char    end_char;
    static const char    delim_char;
    static const char*   map_chars;
  public:
    // constructors:
    inline Admissibles();
    inline Admissibles(const Admissibles&);
    Admissibles(const Circuits&,
		const PointConfiguration&,
		const Chirotope&,
		const bool = false); // only fine
    // destructor:
    inline ~Admissibles();
    // assignment:
    inline Admissibles& operator=(const Admissibles&);
    // accessors:
    inline const SimplicialComplex simplices() const;
    // stream input/output:
    std::istream& read(std::istream&);
    inline friend std::istream& operator>>(std::istream&, Admissibles&);
    std::ostream& write(std::ostream&) const;
    inline friend std::ostream& operator<<(std::ostream&, const Admissibles&);
  };

  // constructors:
  inline Admissibles::Admissibles() : admissibles_data() {}
  inline Admissibles::Admissibles(const Admissibles& admissibles) : admissibles_data(admissibles) {}
  // destructor:
  inline Admissibles::~Admissibles() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "Admissibles::~Admissibles(): destructor called." << std::endl;
#endif
  }
  // assignment:
  inline Admissibles& Admissibles::operator=(const Admissibles& admissibles) {
    if (this == &admissibles) {
      return *this;
    }
    admissibles_data::operator=(admissibles);
    return *this;
  }

  // accessors:
  inline const SimplicialComplex Admissibles::simplices() const {
    SimplicialComplex result;
    for (admissibles_data::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      result += iter->first;
    }
    return result;
  }

  inline std::istream& operator>>(std::istream& ist, Admissibles& admissibles) {
    return admissibles.read(ist);
  }
  inline std::ostream& operator<<(std::ostream& ost, const Admissibles& admissibles) {
    return admissibles.write(ost);
  }
  
}; // namespace topcom

#endif

// eof Admissibles.hh
