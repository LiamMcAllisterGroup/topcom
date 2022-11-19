////////////////////////////////////////////////////////////////////////////////
// 
// Pair.hh 
//
//    produced: 12/03/98 jr
// last change: 12/03/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PAIR_HH
#define PAIR_HH

#include <iostream>

namespace topcom {

  template <class FIRST, class SECOND>
  class Pair;

  template <class FIRST, class SECOND>
  inline std::istream& operator>>(std::istream& ist, Pair<FIRST,SECOND>& p);

  template <class FIRST, class SECOND>
  inline std::ostream& operator<<(std::ostream& ost, const Pair<FIRST,SECOND>& p);

  template <class FIRST, class SECOND>
  class Pair {
  public:
    FIRST  first;
    SECOND second;
  public:
    // constructors:
    inline Pair();
    inline Pair(const Pair&);
    inline Pair(FIRST, SECOND);
    // destructor:
    inline ~Pair();
    // assignment:
    inline Pair& operator=(const Pair& p);
    // comparison:
    inline const bool operator== (const Pair& p) const;
    inline const bool operator!= (const Pair& p) const;
    inline const bool operator<(const Pair& p) const;
    inline const bool operator>(const Pair& p) const;
    // keys:
    inline const unsigned long keysize() const;
    inline const unsigned long key(unsigned long n) const;
    // functions:
    inline Pair& swap();
    // stream output/input:
    inline std::istream& read(std::istream&);
    inline std::ostream& write(std::ostream&) const;
    // friends (have to be inlined right here):
    friend std::istream& operator>><>(std::istream& ist, Pair<FIRST,SECOND>& p);
    friend std::ostream& operator<<<>(std::ostream& ost, const Pair<FIRST,SECOND>& p);
  };

  template <class FIRST, class SECOND>
  inline Pair<FIRST,SECOND>::Pair() {}
  template <class FIRST, class SECOND>
  inline Pair<FIRST,SECOND>::Pair(const Pair& p) : first(p.first), second(p.second) {}
  template <class FIRST, class SECOND>
  inline Pair<FIRST,SECOND>::Pair(FIRST f, SECOND s) : first(f), second(s) {}
  template <class FIRST, class SECOND>
  inline Pair<FIRST,SECOND>::~Pair() {}
  template <class FIRST, class SECOND>
  inline Pair<FIRST,SECOND>& Pair<FIRST,SECOND>::operator=(const Pair& p) { 
    first = p.first; 
    second = p.second; 
    return *this; 
  }
  template <class FIRST, class SECOND>
  inline const bool Pair<FIRST,SECOND>::operator==(const Pair<FIRST,SECOND>& p) const {
#ifdef CHECK_EQUALITY
    std::cerr << "checking equality of " << *this << " and " << p << std::endl;
    std::cerr << "result: " << ((first == p.first) && (second == p.second)) << std::endl;
#endif
    return ((first == p.first) && (second == p.second));
  }
  template <class FIRST, class SECOND>
  inline const bool Pair<FIRST,SECOND>::operator!=(const Pair<FIRST,SECOND>& p) const {
    return !(*this == p);
  }
  template <class FIRST, class SECOND>
  inline const bool Pair<FIRST,SECOND>::operator<(const Pair<FIRST,SECOND>& p) const {
    if (first < p.first) {
      return true;
    }
    if (first > p.first) {
      return false;
    }
    return (second < p.second);
  }
  template <class FIRST, class SECOND>
  inline const bool Pair<FIRST,SECOND>::operator>(const Pair<FIRST,SECOND>& p) const {
    return (p < *this);
  }
  template <class FIRST, class SECOND>
  inline const unsigned long Pair<FIRST,SECOND>::keysize() const { return first.keysize() + second.keysize(); }
  template <class FIRST, class SECOND>
  inline const unsigned long Pair<FIRST,SECOND>::key(unsigned long n) const {
    if (n < first.keysize()) {
      return first.key(n);
    }
    else {
      return second.key(n - first.keysize());
    }
  }

  template <class FIRST, class SECOND>
  Pair<FIRST,SECOND>& Pair<FIRST,SECOND>::swap() {
    FIRST tmp(first);
    first = second;
    second = tmp;
    return *this;
  }

  template <class FIRST, class SECOND>
  inline std::istream& Pair<FIRST,SECOND>::read(std::istream& ist) {
    char c;
  
    ist >> std::ws >> c >> std::ws >> first >> std::ws >> c >> std::ws >> second >> std::ws >> c;
    return ist;
  }
  template <class FIRST, class SECOND>
  inline std::ostream& Pair<FIRST,SECOND>::write (std::ostream& ost) const { 
    ost << '[' << first << ',' << second << ']';
    return ost;
  }

  // friends:
  template <class FIRST, class SECOND>
  inline std::istream& operator>>(std::istream& ist, Pair<FIRST,SECOND>& p) {
    return p.read(ist);
  }

  template <class FIRST, class SECOND>
  inline std::ostream& operator<<(std::ostream& ost, const Pair<FIRST,SECOND>& p) {
    return p.write(ost);
  }

}; // namespace topcom

#endif

// eof Pair.hh
