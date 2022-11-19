////////////////////////////////////////////////////////////////////////////////
// 
// Vector.cc
//
//    produced: 13/03/98 jr
// last change: 13/03/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "Vector.hh"

namespace topcom {

  const char Vector::start_char = '[';
  const char Vector::end_char   = ']';
  const char Vector::delim_char = ',';

  const parameter_type Vector::capacity = 0;

  bool Vector::is_zero() const {
    for (parameter_type index = 0; index < size(); ++index) {
      if ((*this)(index) != FieldConstants::ZERO) {
	return false;
      }
    }
    return true;
  }
  Vector& Vector::canonicalize() {
    for (parameter_type index = 0; index < size(); ++index) {
      (*this)(index).canonicalize();
    }
    return *this;
  }

  Vector& Vector::add(const Vector& vector) {
#ifdef INDEX_CHECK
    assert(size() == vector.size());
#endif
    for (parameter_type index = 0; index < size(); ++index) {
      (*this)(index) += vector(index);
    }
    return *this;
  }
  Vector& Vector::scale(const Field& factor) {
    for (parameter_type index = 0; index < size(); ++index) {
      (*this)(index) *= factor;
    }
    return *this;  
  }
  Vector& Vector::stack(const Vector& vector) {
    const parameter_type current_maxindex(size());
    resize(size() + vector.size());
    for (parameter_type i = 0; i < vector.size(); ++i) {
      (*this)(i + current_maxindex) = vector(i);
    }
    return *this;
  }

  const Vector unit_vector(const parameter_type dim, const parameter_type pos) {
    Vector result(dim, FieldConstants::ZERO);
    result(pos) = FieldConstants::ONE;
    return result;
  }

  Field inner_product(const Vector& vector1, const Vector& vector2) {
#ifdef INDEX_CHECK
    assert(vector2.size() == vector1.size());
#endif
    Field result(FieldConstants::ZERO);
    for (parameter_type i = 0; i < vector1.size(); ++i) {
      result += (vector1(i) * vector2(i));
    }
    return result;
  }
  bool lex_abs_compare(const Vector& vector1, 
		       const Vector& vector2, 
		       const parameter_type start) {
#ifdef INDEX_CHECK
    assert(vector1.size() == vector2.size());
#endif
    if (start == vector1.size()) {
      return false;
    }
    if (abs(vector1(start)) > abs(vector2(start))) {
      return false;
    }
    else if (abs(vector1(start)) == abs(vector2(start))) {
      return lex_abs_compare(vector1, vector2, start + 1);
    }
    else {
      return true;
    }
  }

  // stream input/output:
  std::istream& Vector::read(std::istream& ist) {
    char c;
    Field elem;

    vector_data::clear();
    ist >> std::ws >> c;
    if (c == start_char) {
      while (ist >> std::ws >> c) {
	if (c == end_char) {
	  break;
	}
	if (c == delim_char) {
	  continue;
	}
	ist.putback(c);
	if (ist >> elem) {
	  vector_data::push_back(elem);
	}
	else {
#ifdef READ_DEBUG
	  std::cerr << "Vector::read(std::istream& ist):"
		    << c << " not of appropriate type." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "Vector::read(std::istream& ist):"
		<< "missing `" << start_char << "'." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    this->canonicalize();
    return ist;
  }

  std::ostream& Vector::write(std::ostream& ost) const {
    ost << start_char;
    if (dim() > 0) {
      for (parameter_type i = 0; i < dim() - 1; ++i) {
	ost << (*this)[i] << delim_char;
      }
      if (dim() > 0) {
	ost << (*this)[dim() - 1];
      }
    }
    ost << end_char;
    return ost;
  }

}; // namespace topcom

// eof Vector.cc
