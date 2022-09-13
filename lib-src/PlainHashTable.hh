////////////////////////////////////////////////////////////////////////////////
// 
// PlainHashTable.hh
//    produced: 01/09/97 jr
// last change: 22/01/2020 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef PLAINHASHTABLE_HH
#define PLAINHASHTABLE_HH


#ifndef UNLINKEDHASH
#include "PlainLinkedHashTable.hh"
#define PlainHashTable PlainLinkedHashTable
#else
#include "PlainUnlinkedHashTable.hh"
#define PlainHashTable PlainUnlinkedHashTable
#endif

namespace topcom {
};

#endif
// eof PlainHashTable.hh
