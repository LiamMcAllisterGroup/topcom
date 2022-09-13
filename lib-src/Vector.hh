////////////////////////////////////////////////////////////////////////////////
// 
// Vector.hh 
//
//    produced: 21/08/97 jr
// last change: 30/10/97 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef VECTOR_HH
#define VECTOR_HH

#include <assert.h>
#include <cmath>
#include <iostream>

#include "Global.hh"

#include "Field.hh"

#include "CommandlineOptions.hh"

#ifndef STL_CONTAINERS
#include "Array.hh"
namespace topcom {
  typedef Array<Field> vector_data;
};
#else
#include <vector>
namespace topcom {
  typedef std::vector<Field> vector_data;
};
#endif

namespace topcom {


  class Vector : public vector_data {
  private:
    static const char start_char;
    static const char end_char;
    static const char delim_char;
  public:
    static const parameter_type capacity;
  public:

    // constructors:
    inline Vector();
    inline Vector(const Vector&);
    inline Vector(Vector&&);
    inline Vector(const Vector&, parameter_type min_index, parameter_type max_index);
    inline Vector(const Field&);
    inline Vector(const parameter_type, const Field& = FieldConstants::ZERO);
    inline Vector(const std::vector<parameter_type>&);

    // destructor:
    inline ~Vector();

    // assignment:
    inline Vector& operator=(const Vector&);
    inline Vector& operator=(Vector&&);

    // indexing:
    inline const Field& operator()(const parameter_type i) const;
    inline Field& operator()(const parameter_type i);

    // accessors:
    inline parameter_type dim() const;
    bool is_zero() const;

    // operations in place:
    Vector& canonicalize();
    Vector& add(const Vector&);
    Vector& scale(const Field&);
    Vector& stack(const Vector&);

    // operations out of place:
    friend const Vector unit_vector(const parameter_type, const parameter_type);
    friend Field inner_product(const Vector&, const Vector&);

    // keys for containers:
    inline const size_type keysize() const;
    inline const size_type key(const size_type n) const;

    // boolean expressions:
    // istream with canonicalize:
    std::istream& read(std::istream&);
    inline friend std::istream& operator>>(std::istream&, Vector&);
    std::ostream& write(std::ostream&) const;
    inline friend std::ostream& operator<<(std::ostream&, const Vector&);
  };

  // helpers:
  bool lex_abs_compare(const Vector&, const Vector&, const parameter_type = 0);

  // constructors:
  inline Vector::Vector() : 
    vector_data() {
    reserve(capacity);
  }

  inline Vector::Vector(const Vector& vector) : vector_data(vector) {
  }

  inline Vector::Vector(Vector&& vector) : vector_data(std::move(vector)) {
  }

  inline Vector::Vector(const Vector& vector, parameter_type min_index, parameter_type max_index) :
#ifndef STL_CONTAINERS
    vector_data(vector, min_index, max_index)
#else
    vector_data(vector.begin() + std::min<parameter_type>(min_index, vector.dim()),
		vector.begin() + std::min<parameter_type>(max_index, vector.dim()))
#endif
  {}

  inline Vector::Vector(const Field& init_entry) : 
    vector_data(1, init_entry) {
  }

  inline Vector::Vector(const parameter_type init_size, const Field& init_entry) :
    vector_data(init_size, init_entry) {
  }

  inline Vector::Vector(const std::vector<parameter_type>& vec) :
    vector_data() {
    assert(vec.size() <= std::numeric_limits<parameter_type>::max());
    for (int i = 0; i < vec.size(); ++i) {
      this->push_back(vec[i]);
    }
  }

  // destructor:
  inline Vector::~Vector() {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "destroying Vector " << *this << std::endl;
#endif
  }

  // assignment:
  inline Vector& Vector::operator=(const Vector& vector) {
    if (this == &vector) {
      return *this;
    }
    vector_data::operator=(vector);
    return *this;
  }

  inline Vector& Vector::operator=(Vector&& vector) {
    if (this == &vector) {
      return *this;
    }
    vector_data::operator=(std::move(vector));
    return *this;
  }

  // indexing:
  inline const Field& Vector::operator()(const parameter_type i) const {
    return this->at(i);
  }

  inline Field& Vector::operator()(const parameter_type i) {
    return this->at(i);
  }
  
  // accessors:
  inline parameter_type Vector::dim() const {
    assert(vector_data::size() <= std::numeric_limits<parameter_type>::max());
    return vector_data::size();
  }

  // keys for containers:
  inline const size_type Vector::keysize() const {
    return (size_type)dim();
  }

  inline const size_type Vector::key(const size_type n) const {
    return hash<Field>()((*this)[n]);
  }

  // stream input/output:
  inline std::istream& operator>>(std::istream& ist, Vector& v) {
    return v.read(ist);
  }

  inline std::ostream& operator<<(std::ostream& ost, const Vector& v) {
    v.write(ost);
    return ost;
  }

}; // namespace topcom

#endif

// eof Vector.hh
