////////////////////////////////////////////////////////////////////////////////
// 
// IndexTableArray.hh 
//
//    produced: 29/05/98 jr
// last change: 29/05/98 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef INDEXTABLEARRAY_HH
#define INDEXTABLEARRAY_HH

#include <stdlib.h>
#include <iostream>
#include <ctype.h>

#include "IntegerSet.hh"
#include "PlainArray.hh"
#include "Array.hh"
#include "PlainHashIndexTable.hh"

typedef IntegerSet Simplex;

typedef PlainHashIndexTable<Simplex> IndexTable;
typedef PlainArray<IndexTable> _IndexTableArray;

class IndexTableArray : public _IndexTableArray {
protected:
  static IndexTableArray _index_table;
public:
  inline const size_type resize(const size_type new_size) {
    if (new_size > maxindex()) {
      _IndexTableArray::resize(new_size);
    }
    return maxindex();
  }
};

#endif

// eof IndexTableArray.hh
