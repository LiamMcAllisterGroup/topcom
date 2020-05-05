////////////////////////////////////////////////////////////////////////////////
// 
// Permutation.cc
//
//    produced: 13/03/98 jr
// last change: 13/03/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "Permutation.hh"

// functions:
Permutation& Permutation::append(const parameter_type i) {
  permutation_data::append(i);
  if (i > _n) {
    _n = i;
  }
  ++_k;
  return *this;
}

Permutation& Permutation::append(const Permutation& p) {
  permutation_data::append(p);
  if (p._n > _n) {
    _n = p._n;
  }
  _k += p._k;
  return *this;
}

const int Permutation::sign() const {
  int result = 1;
  for (parameter_type i = 0; i < _k; ++i) {
    for (parameter_type j = 0; j < i; ++j) {
      if ((*this)[i] < (*this)[j]) {
	result *= -1;
      }
    }
  }
  return result;
}

const int Permutation::sign(const parameter_type split) const {
  int result = 1;
  for (parameter_type i = split; i < _k; ++i) {
    for (parameter_type j = 0; j < split; ++j) {
      if ((*this)[i] < (*this)[j]) {
	result *= -1;
      }
    }
  }
  return result;
}

const int Permutation::sort() {
  int result = 1;
  for (parameter_type i = 0; i < _k; ++i) {
    for (parameter_type j = 0; j < i; ++j) {
      if ((*this)[i] < (*this)[j]) {
	parameter_type tmp = (*this)[i];
	(*this)[i] = (*this)[j];
	(*this)[j] = tmp;
	result *= -1;
      }
    }
  }
  return result;
}

Permutation Permutation::complement() const {
  Permutation result(_n, _n - _k);
  size_type count(0);
  for (parameter_type j = 0; j < (*this)[0]; ++j) {
    result[count++] = j;
  }
  for (parameter_type i = 0; i < _k - 1; ++i) {
    for (parameter_type j = (*this)[i] + 1; j < (*this)[i + 1]; ++j) {
      result[count++] = j;
    }
  }
  for (parameter_type j = (*this)[_k - 1] + 1; j < _n; ++j) {
    result[count++] = j;
  }
  return result;
}

Permutation Permutation::deletion(const parameter_type m) const {
  Permutation result(*this);
  for (parameter_type i = m; i < _k - 1; ++i) {
    result[i] = result[i + 1];
  }
  result.resize(--result._k);
  return result;
}

Permutation Permutation::reverse() const {
  Permutation result(_n,_k);
  for (parameter_type i = 0; i < _k; ++i) {
    result[i] = (*this)[_k-i-1];
  }
  return result;
}

bool Permutation::lexnext() {
  for (parameter_type i = 0; i < _k; ++i) {
    if ((*this)[_k - i - 1] == _n - i - 1) {
      continue;
    }
    else {
      ++(*this)[_k - i - 1];
      for (parameter_type j = 0; j < i; ++j) {
	(*this)[_k - j - 1] = (*this)[_k - i - 1] + (i - j);
      }
      return true;
    }
  }
  return false;
}

std::ostream& operator<<(std::ostream& ost, const Permutation& p) {
  return (ost << (const permutation_data&)p);
}

std::istream& operator>>(std::istream& ist, Permutation& p) {
  ist >> (permutation_data&)p;
  p._k = p.maxindex();
  p._n = p._k;
  for (parameter_type i = 0; i < p.maxindex(); ++i) {
    if (p[i] + 1 > p._n) {
      p._n = p[i] + 1;
    }
  }
  return ist;
}


// eof Permutation.cc
