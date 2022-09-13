////////////////////////////////////////////////////////////////////////////////
// 
// Volumes.hh 
//
//    produced: 18/02/2020 jr
// last change: 18/02/2020 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef VOLUMES_HH
#define VOLUMES_HH

#include <assert.h>

#include "CommandlineOptions.hh"
#include "Field.hh"
#include "Simplex.hh"

#ifndef STL_CONTAINERS
#include "HashMap.hh"
namespace topcom {
  typedef HashMap<Simplex, Field> volumes_data;
}
#else
#include <unordered_map>
namespace topcom {
  typedef std::unordered_map<Simplex, Field, Hash<Simplex> > volumes_data;
};
#endif

namespace topcom {

  class PointConfiguration;

  class Volumes : public volumes_data {
  private:
    static const char    start_char;
    static const char    end_char;
    static const char    delim_char;
    static const char*   map_chars;
  public:
    // constructors:
    inline Volumes();
    inline Volumes(const Volumes&);
    Volumes(const PointConfiguration&, const bool = false);
    // destructor:
    inline ~Volumes();
    // functions:
    inline bool has_smaller_vol_than(const Simplex&, const Simplex&) const;
    inline bool has_larger_vol_than(const Simplex&, const Simplex&) const;
    // stream input/output:
    std::istream& read(std::istream&);
    inline friend std::istream& operator>>(std::istream&, Volumes&);
    std::ostream& write(std::ostream&) const;
    inline friend std::ostream& operator<<(std::ostream&, const Volumes&);
  };

  // constructors:
  inline Volumes::Volumes() : volumes_data() {}
  inline Volumes::Volumes(const Volumes& volumes) : volumes_data(volumes) {}
  // destructor:
  inline Volumes::~Volumes() {}
  // functions:
  inline bool Volumes::has_smaller_vol_than(const Simplex& s1, const Simplex& s2) const {
    const volumes_data::const_iterator iter1 = this->find(s1);
    const volumes_data::const_iterator iter2 = this->find(s2);
    if (iter2 == this->end()) {
      return false;
    }
    else if (iter1 == this->end()) {
      return true;
    }
    else {
      return iter1->second < iter2->second;
    }
  }
  inline bool Volumes::has_larger_vol_than(const Simplex& s1, const Simplex& s2) const {
    return this->has_smaller_vol_than(s2, s1);
  }
  
  inline std::istream& operator>>(std::istream& ist, Volumes& volumes) {
    return volumes.read(ist);
  }
  inline std::ostream& operator<<(std::ostream& ost, const Volumes& volumes) {
    return volumes.write(ost);
  }

  class CompareSimplexVolumes {
  private:
    const Volumes* _voltableptr;
  public:
    CompareSimplexVolumes() : _voltableptr(0) {}
    CompareSimplexVolumes(const Volumes& voltable) : _voltableptr(&voltable) {}
    inline bool operator()(const Simplex& s1, const Simplex& s2) {
      if (_voltableptr) {
	return _voltableptr->has_larger_vol_than(s1, s2);
      }
      else {
	return false;
      }
    }
  };
  
}; // namespace topcom


#endif

// eof Volumes.hh
