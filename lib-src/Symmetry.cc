////////////////////////////////////////////////////////////////////////////////
// 
// Symmetry.cc
//
//    produced: 07/07/98 jr
// last change: 07/07/98 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include "Symmetry.hh"
#include "CommandlineOptions.hh"

// Symmetry:

Symmetry::Symmetry(const Permutation& p, const bool cycle) : Permutation(p.n(),p.n()) {
  const parameter_type np = p.n();
  const parameter_type kp = p.k();
  if (cycle) {
    if (CommandlineOptions::debug()) {
      std::cerr << "cycle to convert: " << p << std::endl;
    }
    if (kp > 1) {
      parameter_type i = 0;
      parameter_type first = p[0];
      parameter_type preimage = first;
      while (++i < kp) {
	assert(preimage < np);
	parameter_type image = p[i];
	assert(image < np);
	(*this)[preimage] = image;
	preimage = image;
      }
      (*this)[preimage] = first;
    }
  }
  else {
    if (CommandlineOptions::debug()) {
      std::cerr << "permutation to use: " << p << std::endl;
    }
    if (kp < np) {
      std::cerr << "error while initializing symmetry from partial permutation" << std::endl;
      exit(1);
    }
    else {
      for (parameter_type i = 0; i < np; ++i) {
	(*this)[i] = p[i];
      }
    }
  }
}

const SimplicialComplex Symmetry::operator()(const SimplicialComplex& sc) const {
  SimplicialComplex result;
  for (SimplicialComplex::const_iterator iter = sc.begin();
       iter != sc.end();
       ++iter) {
    result += (*this)(*iter);
  }
  return result;
}

const TriangNode Symmetry::operator()(const TriangNode& tn) const {
  SimplicialComplex result_complex((*this)((SimplicialComplex)tn));
  return TriangNode(tn.ID(), tn.no(), tn.rank(), result_complex);
}
 
const FlipRep Symmetry::operator()(const FlipRep& fr) const {
  FlipRep result;
  result.first = operator()(fr.first);
  result.second = operator()(fr.second);
  return result;
}
 
const Permutation Symmetry::operator()(const Permutation& p) const {
  assert(n() == p.n());
  Permutation result(p);
  for (parameter_type i = 0; i < result.k(); ++i) {
    result[i] = (*this)[p[i]];
  }
  return result;  
}

// tests:
const bool Symmetry::maps(const SimplicialComplex& sc1, const SimplicialComplex& sc2) const {
  for (SimplicialComplex::const_iterator iter = sc1.begin();
       iter != sc1.end();
       ++iter) {
    if (!sc2.contains((*this)(*iter))) {
      return false;
    }
  }
  return true;
}

const bool Symmetry::fixes(const SimplicialComplex& sc) const {
  return maps(sc, sc);
}

const bool Symmetry::maps(const TriangNode& tn1, const TriangNode& tn2) const {
  for (SimplicialComplex::const_iterator iter = tn1.begin();
       iter != tn1.end();
       ++iter) {
    if (!tn2.contains((*this)(*iter))) {
      return false;
    }
  }
  return true;
}

const bool Symmetry::fixes(const TriangNode& tn) const {
  return maps(tn, tn);
}

Symmetry Symmetry::operator*(const Symmetry& s) const {
  Symmetry result(*this);
  for (parameter_type i = 0; i < result.k(); ++i) {
    result[i] = (*this)[s[i]];
  }
  return result;
}

Matrix Symmetry::PermutationMatrix() const {
  const parameter_type n = this->n();
  Matrix result(n,n,ZERO);
  for (parameter_type i = 0; i < n; ++i) {
    result((*this)[i], i) = ONE;
  }
  return result;
}

Matrix Symmetry::ReducedPermutationMatrix() const {
  const parameter_type n = this->n();
  Matrix result(n-1, n-1, ZERO);
  for (parameter_type i = 0; i < n-1; ++i) {
    const parameter_type j((*this)[i]);
    if (j < n-1) {
      result(j, i) = ONE;
    }
  }
  return result;
}

std::istream& operator>>(std::istream& ist, Symmetry& s) {
#ifdef SUPER_VERBOSE
  std::cerr << "reading symmetry ...";
#endif
  ist >> (Permutation&)s;
  if (s.n() != s.k()) {
    ist.clear(std::ios::failbit);
    return ist;
  }
#ifdef SUPER_VERBOSE
  std::cerr << "n = " << s.n() << ", k = " << s.k() << "... done." << std::endl;
#endif
  return ist;
}

// SymmetryGroup:

const SimplicialComplex SymmetryGroup::operator()(const SimplicialComplex& fixed, 
						  const Simplex& simp) const {
  SimplicialComplex result;
  result += simp;
  for (const_iterator iter = begin(); iter != end(); ++iter) {
#ifndef STL_SYMMETRIES
    const Symmetry& sym(iter->key());
#else
    const Symmetry& sym(*iter);
#endif
    if (sym(fixed) != fixed) {
      continue;
    }
    result += sym(simp);
  }
  return result;
}

std::istream& SymmetryGroup::read_generators(std::istream& ist) {
  Symmetry s(_n);
  char c;

  ist >> std::ws >> c;
  if (c == '[') {
    while (ist >> std::ws >> c) {
      if (c == ']') {
	break;
      }
      if (c == ',') {
	continue;
      }
      ist.putback(c);
      if (ist >> s) {
	if (s.n() == _n) {
	  symmetry_data::insert(s);
	}
	else {
#ifdef READ_DEBUG
	  std::cerr << "SymmetryGroup::read(std::istream&):"
	       << "Symmetry " << s << " not valid." << std::endl;
#endif	  
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
      else {
#ifdef READ_DEBUG
	std::cerr << "SymmetryGroup::read(std::istream&):"
	     << "cannot find Symmetry." << std::endl;
#endif
	ist.clear(std::ios::failbit);
	return ist;
      }
    }
  }
  else {
#ifdef READ_DEBUG
    std::cerr << "PlainHashTable<HashData>::read(std::istream&):"
	 << "missing `" << '[' << "'." << std::endl;
#endif
    ist.clear(std::ios::failbit);
    return ist;
  }
  ist.clear(std::ios::goodbit);
  return ist;
}

void SymmetryGroup::_closure(const symmetry_data& sd) {

  // build containers for the symmetries multiplied and found in one phase
  // in the first phase these are the symmetries currently in the data structure
  // and the empty container, resp.:
  symmetry_data old_symmetries(*this);
  symmetry_data new_symmetries;

  if (CommandlineOptions::verbose()) {
#ifndef STL_SYMMETRIES
    std::cerr << "computing closure of " << old_symmetries.load() + 1 << " symmetries with " 
	      << sd.load() <<" symmetry generators ..." << std::endl;
#else
    std::cerr << "computing closure of " << old_symmetries.size() + 1 << " symmetries with " 
	      << sd.size() <<" symmetry generators ..." << std::endl;
#endif
  }

  // indicate whether in a phase new symmetries have been found:
  bool new_elem(true);

  while (new_elem) {

    // no new symmetry so far in this phase:
    new_elem = false;

    // multiply every element
    for (symmetry_data::const_iterator iter1 = old_symmetries.begin();
	 iter1 != old_symmetries.end();
	 ++iter1) {
#ifndef STL_SYMMETRIES
      const Symmetry& g(iter1->key());
#else
      const Symmetry& g(*iter1);
#endif
#ifdef CHECK_SYMMETRIES
      std::cout << "found symmetry " << g << std::endl;
#endif
      for (symmetry_data::const_iterator iter2 = sd.begin();
	   iter2 != sd.end();
	   ++iter2) {
#ifndef STL_SYMMETRIES
	const Symmetry& h(iter2->key());
#else
	const Symmetry& h(*iter2);
#endif
	const Symmetry gh(g * h);
#ifndef STL_SYMMETRIES
	if (!member(gh)) {
#else
	if (find(gh) == end()) {
#endif
	  new_elem = true;
#ifdef SUPER_VERBOSE
	  std::cerr << "new symmetry: " << gh << std::endl;
#endif
	  symmetry_data::insert(gh);
	  new_symmetries.insert(gh);
	}
	const Symmetry hg(h * g);
#ifndef STL_SYMMETRIES
	if (!member(hg)) {
#else
	if (find(hg) == end()) {
#endif
	  new_elem = true;
#ifdef SUPER_VERBOSE
	  std::cerr << "new symmetry: " << hg << std::endl;
#endif
	  symmetry_data::insert(hg);
	  new_symmetries.insert(hg);
	}
      }
    }
	
    if (CommandlineOptions::verbose()) {
      if (new_elem) {
#ifndef STL_SYMMETRIES
        std::cerr << load() - 1 << " symmetries so far." << std::endl;
#else
	std::cerr << size() - 1 << " symmetries so far." << std::endl;
#endif
      }
    }

    // swap old and new symmetries:
    old_symmetries = new_symmetries;
    new_symmetries.clear();
  }
  erase(Symmetry(_n));
  if (CommandlineOptions::verbose()) {
    std::cerr << "... done. " << std::endl;
  }
}

// eof Symmetry.cc
