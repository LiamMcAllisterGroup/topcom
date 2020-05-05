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

bool Vector::is_zero() const {
  for (size_type index = 0; index < maxindex(); ++index) {
    if ((*this)[index] != ZERO) {
      return false;
    }
  }
  return true;
}
Vector& Vector::canonicalize() {
  for (size_type index = 0; index < maxindex(); ++index) {
    (*this)[index].canonicalize();
  }
  return *this;
}

Vector& Vector::add(const Vector& vector) {
#ifdef INDEX_CHECK
  assert(maxindex() == vector.maxindex());
#endif
  for (size_type index = 0; index < maxindex(); ++index) {
    (*this)[index] += vector[index];
  }
  return *this;
}
Vector& Vector::scale(const Field& factor) {
  for (size_type index = 0; index < maxindex(); ++index) {
    (*this)[index] *= factor;
  }
  return *this;  
}
Vector& Vector::stack(const Vector& vector) {
  const size_type current_maxindex(maxindex());
  resize(maxindex() + vector.maxindex());
  for (size_type i = 0; i < vector.maxindex(); ++i) {
    (*this)[i + current_maxindex] = vector[i];
  }
  return *this;
}
Field inner_product(const Vector& vector1, const Vector& vector2) {
#ifdef INDEX_CHECK
  assert(vector2.maxindex() == vector1.maxindex());
#endif
  Field result(ZERO);
  for (size_type i = 0; i < vector1.maxindex(); ++i) {
    result += (vector1[i] * vector2[i]);
  }
  return result;
}
bool lex_abs_compare(const Vector& vector1, 
		     const Vector& vector2, 
		     const size_type start) {
#ifdef INDEX_CHECK
  assert(vector1.maxindex() == vector2.maxindex());
#endif
  if (start == vector1.maxindex()) {
    return false;
  }
  if (abs(vector1[start]) > abs(vector2[start])) {
    return false;
  }
  else if (abs(vector1[start]) == abs(vector2[start])) {
    return lex_abs_compare(vector1, vector2, start + 1);
  }
  else {
    return true;
  }
}

// eof Vector.cc
