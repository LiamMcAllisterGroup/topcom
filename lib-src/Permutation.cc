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

#include <assert.h>

#include "Permutation.hh"

namespace topcom {

  const char Permutation::start_char = '[';
  const char Permutation::end_char   = ']';
  const char Permutation::delim_char = ',';

  // functions:
  Permutation& Permutation::push_back(const parameter_type i) {
    permutation_data::push_back(i);
    if (i >= _n) {
      _n = i + 1;
    }
    ++_k;
    return *this;
  }

  Permutation& Permutation::push_back(const Permutation& p) {
    for (parameter_type i = 0; i < p.size(); ++i) {
      push_back(p(i));
    }
    return *this;
  }

  const int Permutation::sign() const {
    int result = 1;
    for (parameter_type i = 0; i < _k; ++i) {
      for (parameter_type j = 0; j < i; ++j) {
	if ((*this)(i) < (*this)(j)) {
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
	if ((*this)(i) < (*this)(j)) {
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
	if ((*this)(i) < (*this)(i)) {
	  parameter_type tmp = (*this)(i);
	  (*this)(i) = (*this)(i);
	  (*this)(i) = tmp;
	  result *= -1;
	}
      }
    }
    return result;
  }

  Permutation Permutation::complement() const {
    Permutation result(_n, _n - _k);
    size_type count(0);
    for (parameter_type j = 0; j < (*this)(0); ++j) {
      result(count++) = j;
    }
    for (parameter_type i = 0; i < _k - 1; ++i) {
      for (parameter_type j = (*this)(i) + 1; j < (*this)(i + 1); ++j) {
	result(count++) = j;
      }
    }
    for (parameter_type j = (*this)(_k - 1) + 1; j < _n; ++j) {
      result(count++) = j;
    }
    return result;
  }

  Permutation Permutation::deletion(const parameter_type m) const {
    Permutation result(*this);
    for (parameter_type i = m; i < _k - 1; ++i) {
      result(i) = result(i + 1);
    }
    result.resize(--result._k);
    return result;
  }

  Permutation Permutation::reverse() const {
    Permutation result(_n,_k);
    for (parameter_type i = 0; i < _k; ++i) {
      result(i) = (*this)(_k-i-1);
    }
    return result;
  }

  bool Permutation::lexnext() {
    for (parameter_type i = 0; i < _k; ++i) {
      if ((*this)(_k - i - 1) == _n - i - 1) {
	continue;
      }
      else {
	++(*this)(_k - i - 1);
	for (parameter_type j = 0; j < i; ++j) {
	  (*this)(_k - j - 1) = (*this)(_k - i - 1) + (i - j);
	}
	return true;
      }
    }
    return false;
  }

  std::istream& Permutation::read(std::istream& ist) {
    char c;
    parameter_type elem;

    permutation_data::resize(0);
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
	  permutation_data::push_back(elem);
	}
	else {
#ifdef READ_DEBUG
	  std::cerr << "Permutation::read(std::istream& ist):"
		    << c << " not of appropriate type." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "Permutation::read(std::istream& ist):"
		<< "missing `" << start_char << "'." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  std::ostream& Permutation::write(std::ostream& ost) const {
    ost << Permutation::start_char;
    if (!empty()) {
      parameter_type i = 0;
      ost << (*this)(i);
      while(++i < size()) {
	ost << Permutation::delim_char << (*this)(i);
      }
    }
    ost << Permutation::end_char;
    return ost;
  }

  std::istream& operator>>(std::istream& ist, Permutation& p) {
    return p.read(ist);
  }

  std::ostream& operator<<(std::ostream& ost, const Permutation& p) {
    return p.write(ost);
  }

}; // namespace topcom

// eof Permutation.cc
