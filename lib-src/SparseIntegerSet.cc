////////////////////////////////////////////////////////////////////////////////
// 
// SparseIntegerSet.cc
//
//    produced: 16/03/98 jr
// last change: 16/03/98 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "SparseIntegerSet.hh"

// SparseIntegerSet:

SparseIntegerSet::SparseIntegerSet(const IntegerSet& is) : set_data() {
  for (IntegerSet::const_iterator iter = is.begin(); iter != is.end(); ++iter) {
    insert(*iter);
  }
}

SparseIntegerSet::SparseIntegerSet(const Array<integer_key>& init) : set_data() {
#ifdef CONSTRUCTOR_DEBUG
  std::cout << "SparseIntegerSet::SparseIntegerSet(const Array<size_type>&)"
       << std::endl;
#endif
  for (size_type i = 0; i < init.maxindex(); ++i) {
    insert(init[i]);
  }
}

// modifier:
SparseIntegerSet& SparseIntegerSet::fill(const integer_key start, const integer_key stop) {
  if (start >= stop) {
    return *this;
  }
  for (integer_key i = start; i < stop; ++i) {
    insert(i);
  }
  return *this;
}

// relations:
const bool SparseIntegerSet::superset(const SparseIntegerSet& sis) const {
  for (const_iterator iter = sis.begin(); iter != sis.end(); ++iter) {
    if (!contains(*iter)) {
      return false;
    }
  }
  return true;
}

const bool SparseIntegerSet::subset(const SparseIntegerSet& sis) const {
  return sis.superset(*this);
}

const bool SparseIntegerSet::disjoint(const SparseIntegerSet& sis) const {
  for (const_iterator iter = begin(); iter != end(); ++iter) {
    if (sis.contains(*iter)) {
      return false;
    }
  }
  return true;
}

// boolean operators:
const bool SparseIntegerSet::operator==(const SparseIntegerSet& s) const {
  return ((card() == s.card()) && (superset(s)));
}

const bool SparseIntegerSet::operator!=(const SparseIntegerSet& s) const {
  return (!((*this) == s));
}

const bool SparseIntegerSet::operator<(const SparseIntegerSet& s) const {
  return s.superset(*this);
}

const bool SparseIntegerSet::operator>(const SparseIntegerSet& s) const {
  return (s < (*this));
}

// adding and deleting an element:

SparseIntegerSet& SparseIntegerSet::operator+=(const size_type elem) {
  insert(elem);
  return *this;
}

SparseIntegerSet& SparseIntegerSet::operator-=(const size_type elem) {
  erase(elem);
  return *this;
}

SparseIntegerSet& SparseIntegerSet::operator^=(const size_type elem) {
  if (member(elem)) {
    erase(elem);
  }
  else {
    insert(elem);
  }
  return *this;
}

// union, difference, and intersection with sets:

SparseIntegerSet& SparseIntegerSet::operator+=(const SparseIntegerSet& s) {
  if (s.is_empty())
  return *this;

  for (const_iterator iter = s.begin(); iter != s.end(); ++iter) {
    insert(*iter);
  }
  return *this;
}

SparseIntegerSet& SparseIntegerSet::operator-=(const SparseIntegerSet& s) {
  if (s.is_empty() || is_empty())
  return *this;

  for (const_iterator iter = s.begin(); iter != s.end(); ++iter) {
    erase(*iter);
  }  
  return *this;
}

SparseIntegerSet& SparseIntegerSet::operator*=(const SparseIntegerSet& s) {
  if (is_empty())
  return *this;

  if (s.is_empty()) {
    clear();
    return *this;
  }

  const SparseIntegerSet copy_of_this(*this);
  for (const_iterator iter = copy_of_this.begin(); iter != copy_of_this.end(); ++iter) {
    if (!s.contains(*iter)) {
      erase(*iter);
    }
  }    
  return *this;
}

SparseIntegerSet& SparseIntegerSet::operator^=(const SparseIntegerSet& s) {
  if (s.is_empty())
  return *this;

  if (is_empty()) {
    *this = s;
    return *this;
  }

  for (const_iterator iter = s.begin(); iter != s.end(); ++iter) {
    *this ^= *iter;
  }
  return *this;
}

// the same but a new set is returned:

SparseIntegerSet SparseIntegerSet::operator+(const size_type elem) const {
  SparseIntegerSet s(*this);
  s += elem;
  return s;
}

SparseIntegerSet SparseIntegerSet::operator-(const size_type elem) const {
  SparseIntegerSet s(*this);
  s -= elem;
  return s;
}

SparseIntegerSet SparseIntegerSet::operator+(const SparseIntegerSet& s1) const {
  SparseIntegerSet s(*this);
  s += s1;
  return s;
}

SparseIntegerSet SparseIntegerSet::operator-(const SparseIntegerSet& s1) const {
  SparseIntegerSet s(*this);
  s -= s1;
  return s;
}

SparseIntegerSet SparseIntegerSet::operator*(const SparseIntegerSet& s1) const {
  SparseIntegerSet s(*this);
  s *= s1;
  return s;
}

SparseIntegerSet SparseIntegerSet::operator^(const SparseIntegerSet& s1) const {
  SparseIntegerSet s(*this);
  s ^= s1;
  return s;
}

// returns the cardinalities
// 0, 1, or 2 (2 or more) of
// the intersection of several SparseIntegerSet's
// (included only for compatibility with IntegerSet but not
// particularly efficient):
const size_type SparseIntegerSet::intersection_card(const SparseIntegerSet** setarray, 
						    const size_type size, 
						    size_type& common_index) const {
  if (size == 0) {
    return 0;
  }
  SparseIntegerSet intersection(*setarray[0]);
  for (size_type i = 1; i < size; ++i) {
    if ((intersection *= *setarray[i]).is_empty()) {
      return 0;
    }
  }
  common_index = *intersection.begin();
  if (intersection.card() == 1) {
    return 1;
  }
  else {
    return 2;
  }
}

const size_type SparseIntegerSet::intersection_nonempty(const SparseIntegerSet** setarray,
							const size_type size,
							size_type& common_index) const {
  if (size == 0) {
    return 0;
  }
  SparseIntegerSet intersection(*setarray[0]);
  for (size_type i = 1; i < size; ++i) {
    if ((intersection *= *setarray[i]).is_empty()) {
      return 0;
    }
  }
  common_index = *intersection.begin();
  return 1;
}

// stream input:
std::istream& SparseIntegerSet::read(std::istream& ist) {
  char c;
  size_type elem;

  clear();
  ist >> std::ws >> c;
  if (c == '{') {
    while (ist >> std::ws >> c) {
      if (isspace(c)) {
	continue;
      }
      if (c == '}') {
	break;
      }
      if (c == ',') {
	continue;
      }
      if (isdigit(c)) {
	ist.putback(c);
	ist >> elem;
	*this += elem;
      }
      else {
#ifdef READ_DEBUG
	std::cerr << "SparseIntegerSet::read(std::istream&): Not an integer." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
    }
  }
  else {
#ifdef READ_DEBUG
    std::cerr << "SparseIntegerSet::read(std::istream&): Missing ``{''." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  ist.clear(std::ios::goodbit);
  return ist;
}
  
// stream output:
std::ostream& SparseIntegerSet::write(std::ostream& ost) const {
  size_type count = 0;
  size_type card = this->card();

  ost << "{";
  for (SparseIntegerSet::const_iterator iter = begin(); iter != end(); ++iter) {
    ost << *iter;
    count++;
    if (count < card) {
      ost << ",";
    }
  }
  ost << "}";
  return ost;
}


// eof SparseIntegerSet.cc
