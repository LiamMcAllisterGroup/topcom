////////////////////////////////////////////////////////////////////////////////
// 
// IntegerSet.cc
//
//    produced: 21/08/97 jr
// last change: 21/08/97 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "IntegerSet.hh"
#include "Symmetry.hh"

namespace topcom {

  // IntegerSet:
#ifdef WATCH_MAXBLOCKNO
  size_type IntegerSet::_maxblockno = 0;
#endif

#ifdef BITCONSTANTS
  block_type IntegerSet::_bit[64] = {
    0x0000000000000001, 0x0000000000000002, 0x0000000000000004, 0x0000000000000008, 
    0x0000000000000010, 0x0000000000000020, 0x0000000000000040, 0x0000000000000080, 
    0x0000000000000100, 0x0000000000000200, 0x0000000000000400, 0x0000000000000800, 
    0x0000000000001000, 0x0000000000002000, 0x0000000000004000, 0x0000000000008000, 
    0x0000000000010000, 0x0000000000020000, 0x0000000000040000, 0x0000000000080000, 
    0x0000000000100000, 0x0000000000200000, 0x0000000000400000, 0x0000000000800000, 
    0x0000000001000000, 0x0000000002000000, 0x0000000004000000, 0x0000000008000000, 
    0x0000000010000000, 0x0000000020000000, 0x0000000040000000, 0x0000000080000000, 
    0x0000000100000000, 0x0000000200000000, 0x0000000400000000, 0x0000000800000000, 
    0x0000001000000000, 0x0000002000000000, 0x0000004000000000, 0x0000008000000000, 
    0x0000010000000000, 0x0000020000000000, 0x0000040000000000, 0x0000080000000000, 
    0x0000100000000000, 0x0000200000000000, 0x0000400000000000, 0x0000800000000000, 
    0x0001000000000000, 0x0002000000000000, 0x0004000000000000, 0x0008000000000000, 
    0x0010000000000000, 0x0020000000000000, 0x0040000000000000, 0x0080000000000000, 
    0x0100000000000000, 0x0200000000000000, 0x0400000000000000, 0x0800000000000000, 
    0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000
  };
  block_type IntegerSet::_not_bit[64] = {
    ~0x0000000000000001, ~0x0000000000000002, ~0x0000000000000004, ~0x0000000000000008, 
    ~0x0000000000000010, ~0x0000000000000020, ~0x0000000000000040, ~0x0000000000000080, 
    ~0x0000000000000100, ~0x0000000000000200, ~0x0000000000000400, ~0x0000000000000800, 
    ~0x0000000000001000, ~0x0000000000002000, ~0x0000000000004000, ~0x0000000000008000, 
    ~0x0000000000010000, ~0x0000000000020000, ~0x0000000000040000, ~0x0000000000080000, 
    ~0x0000000000100000, ~0x0000000000200000, ~0x0000000000400000, ~0x0000000000800000, 
    ~0x0000000001000000, ~0x0000000002000000, ~0x0000000004000000, ~0x0000000008000000, 
    ~0x0000000010000000, ~0x0000000020000000, ~0x0000000040000000, ~0x0000000080000000, 
    ~0x0000000100000000, ~0x0000000200000000, ~0x0000000400000000, ~0x0000000800000000, 
    ~0x0000001000000000, ~0x0000002000000000, ~0x0000004000000000, ~0x0000008000000000, 
    ~0x0000010000000000, ~0x0000020000000000, ~0x0000040000000000, ~0x0000080000000000, 
    ~0x0000100000000000, ~0x0000200000000000, ~0x0000400000000000, ~0x0000800000000000, 
    ~0x0001000000000000, ~0x0002000000000000, ~0x0004000000000000, ~0x0008000000000000, 
    ~0x0010000000000000, ~0x0020000000000000, ~0x0040000000000000, ~0x0080000000000000, 
    ~0x0100000000000000, ~0x0200000000000000, ~0x0400000000000000, ~0x0800000000000000, 
    ~0x1000000000000000, ~0x2000000000000000, ~0x4000000000000000, ~0x8000000000000000
  };
#endif
  // ------------------------------------------------------------
  // Lookup tables for find and count operations.
  // stolen from the gcc-2.95 STL

#ifndef BIT_LINEAR_SEARCH
  unsigned char IntegerSet::_S_bit_count[] = {
    0, /*   0 */ 1, /*   1 */ 1, /*   2 */ 2, /*   3 */ 1, /*   4 */
    2, /*   5 */ 2, /*   6 */ 3, /*   7 */ 1, /*   8 */ 2, /*   9 */
    2, /*  10 */ 3, /*  11 */ 2, /*  12 */ 3, /*  13 */ 3, /*  14 */
    4, /*  15 */ 1, /*  16 */ 2, /*  17 */ 2, /*  18 */ 3, /*  19 */
    2, /*  20 */ 3, /*  21 */ 3, /*  22 */ 4, /*  23 */ 2, /*  24 */
    3, /*  25 */ 3, /*  26 */ 4, /*  27 */ 3, /*  28 */ 4, /*  29 */
    4, /*  30 */ 5, /*  31 */ 1, /*  32 */ 2, /*  33 */ 2, /*  34 */
    3, /*  35 */ 2, /*  36 */ 3, /*  37 */ 3, /*  38 */ 4, /*  39 */
    2, /*  40 */ 3, /*  41 */ 3, /*  42 */ 4, /*  43 */ 3, /*  44 */
    4, /*  45 */ 4, /*  46 */ 5, /*  47 */ 2, /*  48 */ 3, /*  49 */
    3, /*  50 */ 4, /*  51 */ 3, /*  52 */ 4, /*  53 */ 4, /*  54 */
    5, /*  55 */ 3, /*  56 */ 4, /*  57 */ 4, /*  58 */ 5, /*  59 */
    4, /*  60 */ 5, /*  61 */ 5, /*  62 */ 6, /*  63 */ 1, /*  64 */
    2, /*  65 */ 2, /*  66 */ 3, /*  67 */ 2, /*  68 */ 3, /*  69 */
    3, /*  70 */ 4, /*  71 */ 2, /*  72 */ 3, /*  73 */ 3, /*  74 */
    4, /*  75 */ 3, /*  76 */ 4, /*  77 */ 4, /*  78 */ 5, /*  79 */
    2, /*  80 */ 3, /*  81 */ 3, /*  82 */ 4, /*  83 */ 3, /*  84 */
    4, /*  85 */ 4, /*  86 */ 5, /*  87 */ 3, /*  88 */ 4, /*  89 */
    4, /*  90 */ 5, /*  91 */ 4, /*  92 */ 5, /*  93 */ 5, /*  94 */
    6, /*  95 */ 2, /*  96 */ 3, /*  97 */ 3, /*  98 */ 4, /*  99 */
    3, /* 100 */ 4, /* 101 */ 4, /* 102 */ 5, /* 103 */ 3, /* 104 */
    4, /* 105 */ 4, /* 106 */ 5, /* 107 */ 4, /* 108 */ 5, /* 109 */
    5, /* 110 */ 6, /* 111 */ 3, /* 112 */ 4, /* 113 */ 4, /* 114 */
    5, /* 115 */ 4, /* 116 */ 5, /* 117 */ 5, /* 118 */ 6, /* 119 */
    4, /* 120 */ 5, /* 121 */ 5, /* 122 */ 6, /* 123 */ 5, /* 124 */
    6, /* 125 */ 6, /* 126 */ 7, /* 127 */ 1, /* 128 */ 2, /* 129 */
    2, /* 130 */ 3, /* 131 */ 2, /* 132 */ 3, /* 133 */ 3, /* 134 */
    4, /* 135 */ 2, /* 136 */ 3, /* 137 */ 3, /* 138 */ 4, /* 139 */
    3, /* 140 */ 4, /* 141 */ 4, /* 142 */ 5, /* 143 */ 2, /* 144 */
    3, /* 145 */ 3, /* 146 */ 4, /* 147 */ 3, /* 148 */ 4, /* 149 */
    4, /* 150 */ 5, /* 151 */ 3, /* 152 */ 4, /* 153 */ 4, /* 154 */
    5, /* 155 */ 4, /* 156 */ 5, /* 157 */ 5, /* 158 */ 6, /* 159 */
    2, /* 160 */ 3, /* 161 */ 3, /* 162 */ 4, /* 163 */ 3, /* 164 */
    4, /* 165 */ 4, /* 166 */ 5, /* 167 */ 3, /* 168 */ 4, /* 169 */
    4, /* 170 */ 5, /* 171 */ 4, /* 172 */ 5, /* 173 */ 5, /* 174 */
    6, /* 175 */ 3, /* 176 */ 4, /* 177 */ 4, /* 178 */ 5, /* 179 */
    4, /* 180 */ 5, /* 181 */ 5, /* 182 */ 6, /* 183 */ 4, /* 184 */
    5, /* 185 */ 5, /* 186 */ 6, /* 187 */ 5, /* 188 */ 6, /* 189 */
    6, /* 190 */ 7, /* 191 */ 2, /* 192 */ 3, /* 193 */ 3, /* 194 */
    4, /* 195 */ 3, /* 196 */ 4, /* 197 */ 4, /* 198 */ 5, /* 199 */
    3, /* 200 */ 4, /* 201 */ 4, /* 202 */ 5, /* 203 */ 4, /* 204 */
    5, /* 205 */ 5, /* 206 */ 6, /* 207 */ 3, /* 208 */ 4, /* 209 */
    4, /* 210 */ 5, /* 211 */ 4, /* 212 */ 5, /* 213 */ 5, /* 214 */
    6, /* 215 */ 4, /* 216 */ 5, /* 217 */ 5, /* 218 */ 6, /* 219 */
    5, /* 220 */ 6, /* 221 */ 6, /* 222 */ 7, /* 223 */ 3, /* 224 */
    4, /* 225 */ 4, /* 226 */ 5, /* 227 */ 4, /* 228 */ 5, /* 229 */
    5, /* 230 */ 6, /* 231 */ 4, /* 232 */ 5, /* 233 */ 5, /* 234 */
    6, /* 235 */ 5, /* 236 */ 6, /* 237 */ 6, /* 238 */ 7, /* 239 */
    4, /* 240 */ 5, /* 241 */ 5, /* 242 */ 6, /* 243 */ 5, /* 244 */
    6, /* 245 */ 6, /* 246 */ 7, /* 247 */ 5, /* 248 */ 6, /* 249 */
    6, /* 250 */ 7, /* 251 */ 6, /* 252 */ 7, /* 253 */ 7, /* 254 */
    8  /* 255 */
  }; // end _Bit_count

  unsigned char IntegerSet::_S_first_one[] = {
    0, /*   0 */ 0, /*   1 */ 1, /*   2 */ 0, /*   3 */ 2, /*   4 */
    0, /*   5 */ 1, /*   6 */ 0, /*   7 */ 3, /*   8 */ 0, /*   9 */
    1, /*  10 */ 0, /*  11 */ 2, /*  12 */ 0, /*  13 */ 1, /*  14 */
    0, /*  15 */ 4, /*  16 */ 0, /*  17 */ 1, /*  18 */ 0, /*  19 */
    2, /*  20 */ 0, /*  21 */ 1, /*  22 */ 0, /*  23 */ 3, /*  24 */
    0, /*  25 */ 1, /*  26 */ 0, /*  27 */ 2, /*  28 */ 0, /*  29 */
    1, /*  30 */ 0, /*  31 */ 5, /*  32 */ 0, /*  33 */ 1, /*  34 */
    0, /*  35 */ 2, /*  36 */ 0, /*  37 */ 1, /*  38 */ 0, /*  39 */
    3, /*  40 */ 0, /*  41 */ 1, /*  42 */ 0, /*  43 */ 2, /*  44 */
    0, /*  45 */ 1, /*  46 */ 0, /*  47 */ 4, /*  48 */ 0, /*  49 */
    1, /*  50 */ 0, /*  51 */ 2, /*  52 */ 0, /*  53 */ 1, /*  54 */
    0, /*  55 */ 3, /*  56 */ 0, /*  57 */ 1, /*  58 */ 0, /*  59 */
    2, /*  60 */ 0, /*  61 */ 1, /*  62 */ 0, /*  63 */ 6, /*  64 */
    0, /*  65 */ 1, /*  66 */ 0, /*  67 */ 2, /*  68 */ 0, /*  69 */
    1, /*  70 */ 0, /*  71 */ 3, /*  72 */ 0, /*  73 */ 1, /*  74 */
    0, /*  75 */ 2, /*  76 */ 0, /*  77 */ 1, /*  78 */ 0, /*  79 */
    4, /*  80 */ 0, /*  81 */ 1, /*  82 */ 0, /*  83 */ 2, /*  84 */
    0, /*  85 */ 1, /*  86 */ 0, /*  87 */ 3, /*  88 */ 0, /*  89 */
    1, /*  90 */ 0, /*  91 */ 2, /*  92 */ 0, /*  93 */ 1, /*  94 */
    0, /*  95 */ 5, /*  96 */ 0, /*  97 */ 1, /*  98 */ 0, /*  99 */
    2, /* 100 */ 0, /* 101 */ 1, /* 102 */ 0, /* 103 */ 3, /* 104 */
    0, /* 105 */ 1, /* 106 */ 0, /* 107 */ 2, /* 108 */ 0, /* 109 */
    1, /* 110 */ 0, /* 111 */ 4, /* 112 */ 0, /* 113 */ 1, /* 114 */
    0, /* 115 */ 2, /* 116 */ 0, /* 117 */ 1, /* 118 */ 0, /* 119 */
    3, /* 120 */ 0, /* 121 */ 1, /* 122 */ 0, /* 123 */ 2, /* 124 */
    0, /* 125 */ 1, /* 126 */ 0, /* 127 */ 7, /* 128 */ 0, /* 129 */
    1, /* 130 */ 0, /* 131 */ 2, /* 132 */ 0, /* 133 */ 1, /* 134 */
    0, /* 135 */ 3, /* 136 */ 0, /* 137 */ 1, /* 138 */ 0, /* 139 */
    2, /* 140 */ 0, /* 141 */ 1, /* 142 */ 0, /* 143 */ 4, /* 144 */
    0, /* 145 */ 1, /* 146 */ 0, /* 147 */ 2, /* 148 */ 0, /* 149 */
    1, /* 150 */ 0, /* 151 */ 3, /* 152 */ 0, /* 153 */ 1, /* 154 */
    0, /* 155 */ 2, /* 156 */ 0, /* 157 */ 1, /* 158 */ 0, /* 159 */
    5, /* 160 */ 0, /* 161 */ 1, /* 162 */ 0, /* 163 */ 2, /* 164 */
    0, /* 165 */ 1, /* 166 */ 0, /* 167 */ 3, /* 168 */ 0, /* 169 */
    1, /* 170 */ 0, /* 171 */ 2, /* 172 */ 0, /* 173 */ 1, /* 174 */
    0, /* 175 */ 4, /* 176 */ 0, /* 177 */ 1, /* 178 */ 0, /* 179 */
    2, /* 180 */ 0, /* 181 */ 1, /* 182 */ 0, /* 183 */ 3, /* 184 */
    0, /* 185 */ 1, /* 186 */ 0, /* 187 */ 2, /* 188 */ 0, /* 189 */
    1, /* 190 */ 0, /* 191 */ 6, /* 192 */ 0, /* 193 */ 1, /* 194 */
    0, /* 195 */ 2, /* 196 */ 0, /* 197 */ 1, /* 198 */ 0, /* 199 */
    3, /* 200 */ 0, /* 201 */ 1, /* 202 */ 0, /* 203 */ 2, /* 204 */
    0, /* 205 */ 1, /* 206 */ 0, /* 207 */ 4, /* 208 */ 0, /* 209 */
    1, /* 210 */ 0, /* 211 */ 2, /* 212 */ 0, /* 213 */ 1, /* 214 */
    0, /* 215 */ 3, /* 216 */ 0, /* 217 */ 1, /* 218 */ 0, /* 219 */
    2, /* 220 */ 0, /* 221 */ 1, /* 222 */ 0, /* 223 */ 5, /* 224 */
    0, /* 225 */ 1, /* 226 */ 0, /* 227 */ 2, /* 228 */ 0, /* 229 */
    1, /* 230 */ 0, /* 231 */ 3, /* 232 */ 0, /* 233 */ 1, /* 234 */
    0, /* 235 */ 2, /* 236 */ 0, /* 237 */ 1, /* 238 */ 0, /* 239 */
    4, /* 240 */ 0, /* 241 */ 1, /* 242 */ 0, /* 243 */ 2, /* 244 */
    0, /* 245 */ 1, /* 246 */ 0, /* 247 */ 3, /* 248 */ 0, /* 249 */
    1, /* 250 */ 0, /* 251 */ 2, /* 252 */ 0, /* 253 */ 1, /* 254 */
    0, /* 255 */
  }; // end _First_one
#endif

  void IntegerSet::_expand(const size_type new_no_of_blocks) {
#ifdef RESIZE_DEBUG
    std::cout << "IntegerSet::_expand()" << std::endl;
#endif
    size_type new_memsize = _memsize;
    while (new_memsize < new_no_of_blocks) {
      new_memsize *= 2;
    }
    block_type* bp(0);
    try {
      bp = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, new_memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << new_memsize << " blocks failed in IntegerSet::_contract(...) - exiting" << std::endl;
      exit(1);
    }
    if (_bitrep) {
      // std::memcpy(bp, _bitrep, _memsize * sizeof(block_type));
      // std::memset(bp + _memsize, 0UL, (new_memsize - _memsize) * sizeof(block_type));
      for (size_type i = 0; i < _memsize; ++i) {
        bp[i] = _bitrep[i];
      }
      for (size_type i = _memsize; i < new_memsize; ++i) {
        bp[i] = 0UL;
      }
      std::allocator_traits<std::allocator<block_type> >::deallocate(block_allocator, _bitrep, _memsize);
      _bitrep = 0;
      _memsize = 0;
    }
    _memsize = new_memsize;
    _bitrep = bp;
  }

  void IntegerSet::_contract(const size_type new_no_of_blocks) {
#ifdef RESIZE_DEBUG
    std::cout << "IntegerSet::_contract()" << std::endl;
#endif
#ifdef NO_CONTRACTION
    return;
#endif
    size_type new_memsize = _memsize;
    while (new_memsize > 4 * new_no_of_blocks + 1) {
      new_memsize /= 2;
    }
    block_type* bp(0);
    try {
      bp = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, new_memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << new_memsize << " blocks failed in IntegerSet::_contract(...) - exiting" << std::endl;
      exit(1);
    }
    if (_bitrep) {
      // std::memcpy(bp, _bitrep, new_memsize * sizeof(block_type));
      for (size_type i = 0; i < new_memsize; ++i) {
        bp[i] = _bitrep[i];
      }
      std::allocator_traits<std::allocator<block_type> >::deallocate(block_allocator, _bitrep, _memsize);
      _bitrep = 0;
      _memsize = 0;
    }
    _memsize = new_memsize;
    _bitrep = bp;
  }

  // copy constructor from permuted IntegerSet:
  IntegerSet::IntegerSet(const IntegerSet& is,
			 const Symmetry& s,
			 const size_type bound_on_max_elem) :
    _no_of_blocks(is._no_of_blocks), _memsize(is._memsize), _invariant(is._invariant) {
    const size_type new_no_of_blocks(calc_no_of_blocks(bound_on_max_elem - 1));
    if (new_no_of_blocks > _no_of_blocks) {
      _no_of_blocks = new_no_of_blocks;
    }
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0UL; i < is._no_of_blocks; ++i) {
      _bitrep[i] = is._bitrep[i];
    }
    for (size_type i = is._no_of_blocks; i < _no_of_blocks; ++i) {
      _bitrep[i] = 0UL;
    }
    for (IntegerSet::const_iterator iter = is.begin();
	 iter != is.end();
	 ++iter) {
      (*this) ^= *iter;
      (*this) ^= s(*iter);
    }
  }

  IntegerSet::IntegerSet(const size_type len, block_type* init) : 
    _no_of_blocks(0UL), _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const size_type, block_type*)" << std::endl;
#endif
    while (_memsize < len) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < len; ++i) {
      _bitrep[i] = init[i];
      if (_bitrep[i] != 0UL) {
	_invariant ^= _bitrep[i];
	_no_of_blocks = i;
      }
    }
    for (size_type i = len; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
#ifdef CHECK_INVARIANT
    size_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const size_type, block_type*):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

  IntegerSet::IntegerSet() : _no_of_blocks(0), _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet()" << std::endl;
#endif
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    _bitrep[0] = 0UL;
#ifdef CHECK_INVARIANT
    size_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet():";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }  

  IntegerSet::IntegerSet(const IntegerSet& s) :
    _no_of_blocks(s._no_of_blocks), _memsize(s._memsize), _invariant(s._invariant) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const IntegerSet&)" << std::endl;
#endif
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = s._bitrep[i];
    }
#ifdef CHECK_INVARIANT
    size_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const IntegerSet& s):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

  IntegerSet::IntegerSet(IntegerSet&& s) :
    _bitrep(s._bitrep), _no_of_blocks(s._no_of_blocks), _memsize(s._memsize), _invariant(s._invariant) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(IntegerSet&&)" << std::endl;
#endif
    s._no_of_blocks = 0UL;
    s._memsize = 1UL;
    try {
      s._bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, s._memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << s._memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    s._bitrep[0] = 0UL;
    s._invariant = 0UL;
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }
  
  IntegerSet::IntegerSet(const SparseIntegerSet& sis) :
    _no_of_blocks(0UL), _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const SparseIntegerSet&)" << std::endl;
#endif
    if (sis.card() == 0) {
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
	exit(1);
      }
      _bitrep[0] = 0UL;
      return;
    }
    size_type maxel(0);
    for (SparseIntegerSet::const_iterator iter = sis.begin(); iter != sis.end(); ++iter) {
      if (*iter > maxel) {
	maxel = *iter;
      }
    }
    _no_of_blocks = calc_no_of_blocks(maxel);
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
    for (SparseIntegerSet::const_iterator iter = sis.begin(); iter != sis.end(); ++iter) {
#ifdef BITCONSTANTS
      _bitrep[*iter / block_len] |= _bit[*iter % block_len];
#else
      _bitrep[*iter / block_len] |= (bit_one << (*iter % block_len));
#endif
    }
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      _invariant ^= _bitrep[i];
    }
#ifdef CHECK_INVARIANT
    size_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const SparseIntegerSet& sis):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

  IntegerSet::IntegerSet(const size_type start, const size_type stop) :
    _no_of_blocks(0UL), _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const size_type, const size_type)" << std::endl;
#endif
    if (!(start < stop)) {
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
	exit(1);
      }
      _bitrep[0] = 0UL;
      return;
    }
    _no_of_blocks = calc_no_of_blocks(stop - 1);
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const size_type, const size_type):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
    fill(start, stop);
  }

  IntegerSet::IntegerSet(const size_type elem) :
    _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const size_type)" << std::endl;
#endif
    _no_of_blocks = calc_no_of_blocks(elem);
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
#ifdef BITCONSTANTS
    _bitrep[elem / block_len] |= _bit[elem % block_len];
#else
    _bitrep[elem / block_len] |= (bit_one << (elem % block_len));
#endif
    _invariant ^= _bitrep[elem / block_len];
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const size_type):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

  IntegerSet::IntegerSet(const size_type len, const size_type* init) : 
    _no_of_blocks(0), _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const size_type, const size_type*)"
	      << std::endl;
#endif
    if (len == 0) {
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
	exit(1);
      }
      _bitrep[0] = 0UL;
      _no_of_blocks = 0; 
      return;
    }
    size_type maxel = 0;
    for (size_type i = 0; i < len; i++) {
      if (init[i] > maxel) {
	maxel = init[i];
      }
    }
    _no_of_blocks = calc_no_of_blocks(maxel);
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
    for (size_type i = 0; i < len; i++) {
#ifdef BITCONSTANTS
      _bitrep[init[i] / block_len] |= _bit[init[i] % block_len];
#else
      _bitrep[init[i] / block_len] |= (bit_one << (init[i] % block_len));
#endif
    }
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      _invariant ^= _bitrep[i];
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const size_type, const size_type*):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

#ifndef STL_CONTAINERS
  IntegerSet::IntegerSet(const Array<size_type>& init) : 
    _no_of_blocks(0UL), _memsize(1UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const Array<size_type>&)"
	      << std::endl;
#endif
    const size_type len = init.size();
    if (len == 0) {
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
	exit(1);
      }
      _bitrep[0] = 0UL;
      return;
    }
    size_type maxel = 0;
    for (size_type i = 0; i < len; i++) {
      if (init[i] > maxel) {
	maxel = init[i];
      }
    }
    _no_of_blocks = calc_no_of_blocks(maxel);
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
    for (size_type i = 0; i < len; i++) {
#ifdef BITCONSTANTS
      _bitrep[init[i] / block_len] |= _bit[init[i] % block_len];
#else
      _bitrep[init[i] / block_len] |= (bit_one << (init[i] % block_len));
#endif
    }
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      _invariant ^= _bitrep[i];
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const Array<size_type>&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

  IntegerSet::IntegerSet(const PlainArray<size_type>& init) : 
    _no_of_blocks(0UL), _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const PlainArray<size_type>&)"
	      << std::endl;
#endif
    const size_type len = init.size();
    if (len == 0) {
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
	exit(1);
      }
      _bitrep[0] = 0UL;
      return;
    }
    size_type maxel = 0;
    for (size_type i = 0; i < len; i++) {
      if (init[i] > maxel) {
	maxel = init[i];
      }
    }
    _no_of_blocks = calc_no_of_blocks(maxel);
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
    for (size_type i = 0; i < len; i++) {
#ifdef BITCONSTANTS
      _bitrep[init[i] / block_len] |= _bit[init[i] % block_len];
#else
      _bitrep[init[i] / block_len] |= (bit_one << (init[i] % block_len));
#endif
    }
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      _invariant ^= _bitrep[i];
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const PlainArray<size_type>&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

#else

  IntegerSet::IntegerSet(const std::set<size_type>& init) : 
    _no_of_blocks(0UL), _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const std::vector<size_type>&)"
	      << std::endl;
#endif
    const size_type len = init.size();
    if (len == 0) {
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
	exit(1);
      }
      _bitrep[0] = 0UL;
      return;
    }
    size_type maxel = 0;
    for (std::set<size_type>::const_iterator inititer = init.begin();
	 inititer != init.end();
	 ++inititer) {
      if (*inititer > maxel) {
	maxel = *inititer;
      }
    }
    _no_of_blocks = calc_no_of_blocks(maxel);
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
    for (std::set<size_type>::const_iterator inititer = init.begin();
	 inititer != init.end();
	 ++inititer) {
#ifdef BITCONSTANTS
      _bitrep[*inititer / block_len] |= _bit[*inititer % block_len];
#else
      _bitrep[*inititer / block_len] |= (bit_one << (*inititer % block_len));
#endif
    }
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      _invariant ^= _bitrep[i];
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const std::vector<size_type>&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

  IntegerSet::IntegerSet(const std::set<parameter_type>& init) : 
    _no_of_blocks(0UL), _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const std::vector<size_type>&)"
	      << std::endl;
#endif
    const size_type len = init.size();
    if (len == 0) {
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
	exit(1);
      }
      _bitrep[0] = 0UL;
      return;
    }
    size_type maxel = 0;
    for (std::set<parameter_type>::const_iterator inititer = init.begin();
	 inititer != init.end();
	 ++inititer) {
      if (*inititer > maxel) {
	maxel = *inititer;
      }
    }
    _no_of_blocks = calc_no_of_blocks(maxel);
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
    for (std::set<parameter_type>::const_iterator inititer = init.begin();
	 inititer != init.end();
	 ++inititer) {
#ifdef BITCONSTANTS
      _bitrep[*inititer / block_len] |= _bit[*inititer % block_len];
#else
      _bitrep[*inititer / block_len] |= (bit_one << (*inititer % block_len));
#endif
    }
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      _invariant ^= _bitrep[i];
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const std::vector<size_type>&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

  IntegerSet::IntegerSet(const std::vector<size_type>& init) : 
    _no_of_blocks(0UL), _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const std::vector<size_type>&)"
	      << std::endl;
#endif
    const size_type len = init.size();
    if (len == 0) {
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
	exit(1);
      }
      _bitrep[0] = 0UL;
      return;
    }
    size_type maxel = 0;
    for (size_type i = 0; i < len; i++) {
      if (init[i] > maxel) {
	maxel = init[i];
      }
    }
    _no_of_blocks = calc_no_of_blocks(maxel);
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
    for (size_type i = 0; i < len; i++) {
#ifdef BITCONSTANTS
      _bitrep[init[i] / block_len] |= _bit[init[i] % block_len];
#else
      _bitrep[init[i] / block_len] |= (bit_one << (init[i] % block_len));
#endif
    }
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      _invariant ^= _bitrep[i];
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const std::vector<size_type>&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

  IntegerSet::IntegerSet(const std::vector<parameter_type>& init) : 
    _no_of_blocks(0UL), _memsize(1UL), _invariant(0UL) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::IntegerSet(const std::vector<size_type>&)"
	      << std::endl;
#endif
    const size_type len = init.size();
    if (len == 0) {
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
	exit(1);
      }
      _bitrep[0] = 0UL;
      return;
    }
    size_type maxel = 0;
    for (size_type i = 0; i < len; i++) {
      if (init[i] > maxel) {
	maxel = init[i];
      }
    }
    _no_of_blocks = calc_no_of_blocks(maxel);
    while (_memsize < _no_of_blocks) {
      _memsize *= 2;
    }
    try {
      _bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    for (size_type i = 0; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
    for (size_type i = 0; i < len; i++) {
#ifdef BITCONSTANTS
      _bitrep[init[i] / block_len] |= _bit[init[i] % block_len];
#else
      _bitrep[init[i] / block_len] |= (bit_one << (init[i] % block_len));
#endif
    }
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      _invariant ^= _bitrep[i];
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::IntegerSet(const std::vector<size_type>&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
  }

#endif

  IntegerSet::~IntegerSet() {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet::~IntegerSet()" << std::endl;
#endif
    if (_bitrep && (_memsize > 0)) {
      std::allocator_traits<std::allocator<block_type> >::deallocate(block_allocator, _bitrep, _memsize);
      _bitrep = 0;
      _memsize = 0;
    }
  }

  // assignment:
  IntegerSet& IntegerSet::operator=(const IntegerSet& s) {
    if (this == &s) {
      return *this;
    }
    if (s.empty()) {
      return clear();
    }
    _invariant = s._invariant;
    if (_memsize != s._memsize) {
      if (_bitrep) {
	std::allocator_traits<std::allocator<block_type> >::deallocate(block_allocator, _bitrep, _memsize);
	_bitrep = 0;
	_memsize = 0;
      }
      _memsize = s._memsize;
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::operator=(...) - exiting" << std::endl;
	exit(1);
      }
    }
    _no_of_blocks = s._no_of_blocks;
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      _bitrep[i] = s._bitrep[i];
    }
    for (size_type i = _no_of_blocks; i < _memsize; ++i) {
      _bitrep[i] = 0UL;
    }
  
#ifdef CHECK_INVARIANT
    block_type check_invariant(0);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::operator=(const IntegerSet&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef COMPUTATIONS_DEBUG
    if (*this != s) {
      std::cerr << "IntegerSet::operator=(const IntegerSet&): "
		<< "assigment error." << std::endl;
      exit(1);
    }
    for (size_type i = _no_of_blocks; i < _memsize; ++i) {
      if (_bitrep[i] != 0) {
	std::cerr << "IntegerSet::IntegerSet::operator=(const IntegerSet&): "
		  << "computational error."
		  << std::endl;
	std::cerr << i << "-th block not zero, but >= " 
		  << "_no_of_blocks = " << _no_of_blocks
		  << std::endl;
	exit(1);
      }
    }
#endif
    return *this;
  }

  IntegerSet& IntegerSet::operator=(IntegerSet&& s) {
    if (this == &s) {
      return *this;
    }
    if (s.empty()) {
      return clear();
    }
    _invariant = s._invariant;
    _no_of_blocks = s._no_of_blocks;
    if (_bitrep && (s._bitrep) && (_bitrep != s._bitrep)) {
      std::allocator_traits<std::allocator<block_type> >::deallocate(block_allocator, _bitrep, _memsize);
      _bitrep = 0;
      _memsize = 0;
      _memsize = s._memsize;
      _bitrep = s._bitrep;
    }
    else {
      std::cerr << "IntegerSet::IntegerSet(...): *this/argument is an uninitialized IntegerSet - exiting" << std::endl;
      exit(1);
    }
    s._no_of_blocks = 0UL;
    s._memsize = 1UL;
    try {
      s._bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, s._memsize);
    }
    catch (...) {
      std::cerr << "allocation of " << s._memsize << " blocks failed in IntegerSet::IntegerSet(...) - exiting" << std::endl;
      exit(1);
    }
    s._bitrep[0] = 0UL;
    s._invariant = 0UL;
    return *this;
  }

  // modifier:
  IntegerSet& IntegerSet::clear() {
    if (_memsize > 1UL) {
      if (_bitrep) {
	std::allocator_traits<std::allocator<block_type> >::deallocate(block_allocator, _bitrep, _memsize);
	_bitrep = 0;
	_memsize = 0;
      }
      _memsize = 1UL;
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::clear() - exiting" << std::endl;
	exit(1);
      }
    }
    else if (_memsize < 1UL) {
      _memsize = 1UL;
      try {
	_bitrep = std::allocator_traits<std::allocator<block_type> >::allocate(block_allocator, _memsize);
      }
      catch (...) {
	std::cerr << "allocation of " << _memsize << " blocks failed in IntegerSet::clear() - exiting" << std::endl;
	exit(1);
      }
    }
    _bitrep[0] = 0UL;
    _invariant = 0UL;
    _no_of_blocks = 0;
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::clear():";
      std::cerr << "error in invariant - exiting" << std::endl;
      exit(1);
    }
#endif
    return *this;
  }

  IntegerSet& IntegerSet::fill(const size_type start, const size_type stop) {
    if (!(start < stop)) {
      return *this;
    }
    const size_type first_block = start / block_len;
    const size_type last_block = (stop - 1) / block_len;
    if (last_block >= _no_of_blocks) {
      _no_of_blocks = last_block + 1;
      if (_memsize < _no_of_blocks) {
	_expand(_no_of_blocks);
      }
    }
    if (first_block == last_block) {
      for (size_type j = start % block_len; j < (stop - 1) % block_len + 1; ++j) {
#ifdef BITCONSTANTS
	_bitrep[first_block] |= _bit[j];
#else
	_bitrep[first_block ] |= (bit_one << j);
#endif	
      }
    }
    else {
      for (size_type j = start % block_len; j < block_len; ++j) {
#ifdef BITCONSTANTS
	_bitrep[first_block] |= _bit[j];
#else
	_bitrep[first_block] |= (bit_one << j);
#endif
      }
      for (size_type i = first_block; i < last_block; ++i) {
	_bitrep[i] |= all_bits;
      }
      for (size_type j = 0; j < (stop - 1) % block_len + 1; ++j) {
#ifdef BITCONSTANTS
	_bitrep[last_block] |= _bit[j];
#else
	_bitrep[last_block] |= (bit_one << j);
#endif
      }
    }
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      _invariant ^= _bitrep[i];
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::fill(const size_type, const size_type):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
    return *this;
  }

  // cardinality:
  const size_type IntegerSet::card() const {
    //   size_type count = 0;
    //   for (size_type i = 0; i < _no_of_blocks; i++) {
    //     block_type b = _bitrep[i];
    //     if (b == 0UL) {
    //       continue;
    //     }
    //     for (size_type j = 0; j < block_len; j++) {
    // #ifdef BITCONSTANTS
    //       count += (b & _bit[j]);
    // #else
    //       count += ((b >> j) & bit_one);
    // #endif
    //     }
    //   }
    //   return count;

    size_type count(0);

    // algorithm taken from "http://www.caam.rice.edu/~dougm/twiddle/BitCount.html"
    for (size_type i = 0; i < _no_of_blocks; i++) {
      block_type tmpbits = _bitrep[i];
      while (tmpbits) {
	++count;
	tmpbits &= tmpbits - 1;
      }
    }
    return count;


    //   // algorithm taken from "http://www.devx.com/free/tips/tipview.asp?content_id=3839"
    //   size_type count(0);
    //   for (size_type i = 0; i < _no_of_blocks; i++) {
    //     block_type bits = _bitrep[i];
    //     bits = bits - ((bits >> 1) & 0x55555555);
    //     bits = ((bits >> 2) & 0x33333333) + (bits & 0x33333333);
    //     bits = ((bits >> 4) + bits) & 0x0F0F0F0F;
    //     count += (bits * 0x01010101) >> 24;
    //   }
    //   return count;
  }

  // int __fls(uint64_t x) {
  //   x|=(x>>1);
  //   x|=(x>>2);
  //   x|=(x>>4);
  //   x|=(x>>8);
  //   x|=(x>>16);
  //   x|=(x>>32);
  //   x++;
  //   x>>=1;
  //   return std::log2(x);
  // }

  const size_type IntegerSet::min_elem() const {
    if (_no_of_blocks == 0) {
      std::cerr << "IntegerSet::min_elem() const: "
		<< "min element of empty set is not defined - exiting"
		<< std::endl;
      exit(1);
    }
    return *begin();
  }

  const size_type IntegerSet::max_elem() const {
    if (_no_of_blocks == 0) {
      std::cerr << "IntegerSet::max_elem() const: "
		<< "max element of empty set is not defined - exiting"
		<< std::endl;
      exit(1);
    }
    for (size_type i = _no_of_blocks; i > 0; --i) {
      if (_bitrep[i - 1] != 0) {
	// if the compiler does not support a built-in
	// count-leading-zeroes-long function, use __fls from above:
	// return (i - 1) * block_len + __fls(_bitrep[i]);
	return i * block_len - __builtin_clzl(_bitrep[i - 1]) - 1;
      }
    }
    // there are no bits set in all bitrep's - set is empty:
    std::cerr << "IntegerSet::max_elem() const: "
	      << "max element of empty set is not defined - exiting"
	      << std::endl;
    exit(1);
  }

  // relations:
  const bool IntegerSet::contains(const size_type elem) const {
    size_type block_pos = elem / block_len;
    if (block_pos >= _no_of_blocks) {
      return false;
    }
#ifdef BITCONSTANTS
    if ((_bit[elem % block_len] & _bitrep[block_pos]) != 0UL) {
#else
      if (((bit_one << (elem % block_len)) & _bitrep[block_pos]) != 0UL) {
#endif
	return true;
      }
      else {
	return false;
      }
#ifdef BITCONSTANTS
    }
#else
  }
#endif
  
  const bool IntegerSet::superset(const IntegerSet& is) const {
    if (_no_of_blocks < is._no_of_blocks) {
      return false;
    }
    for (size_type i = 0; i < is._no_of_blocks; ++i) {
      if (~_bitrep[i] & is._bitrep[i]) {
	return false;
      }
    }
    return true;
  }

  const bool IntegerSet::subset(const IntegerSet& is) const {
    return is.superset(*this);
  }

  const bool IntegerSet::disjoint(const IntegerSet& is) const {
    size_type min_no_of_blocks = _no_of_blocks < is._no_of_blocks ? _no_of_blocks : is._no_of_blocks;

    for (size_type i = 0; i < min_no_of_blocks; ++i) {
      if (_bitrep[i] & is._bitrep[i]) {
	return false;
      }
    }
    return true;
  }

  const bool IntegerSet::lexsmaller(const IntegerSet& is) const {
    // we implement the true left-to-right lexicographic order on integer sets of same cardinality;
    // for integer sets with differing cardinality, on the sets with identical prefix the order is reversed:
    size_type min_no_of_blocks = _no_of_blocks < is._no_of_blocks ? _no_of_blocks : is._no_of_blocks;
    for (size_type i = 0; i < min_no_of_blocks; ++i) {
      // e.g.:  reverse ~6 = reverse ~0b0110 = 0b1001 = {0, 3} <_lex {1, 2} = 0b0110 = reverse ~0b1001 = reverse ~9
      // e.g.:  reverse ~0 = reverse ~0b1111 = 0b0000 = {0}    <_lex {1}    = 0b0001 = reverse ~0b0111 = reverse ~7
      if (_kbitreverse(~_bitrep[i]) < _kbitreverse(~is._bitrep[i])) {
	return true;
      }
      if (_kbitreverse(~_bitrep[i]) > _kbitreverse(~is._bitrep[i])) {
	return false;
      }
    }
    if (_no_of_blocks < is._no_of_blocks) {
      return true;
    }
    if (_no_of_blocks > is._no_of_blocks) {
      return false;
    }
    return false;  
  }

  const bool IntegerSet::colexgreater(const IntegerSet& s) const {

    // we implement the true right-to-left lexicographic order on the integer sets of the same cardinality:
    if (_no_of_blocks > s._no_of_blocks) {
      return true;
    }
    if (_no_of_blocks < s._no_of_blocks) {
      return false;
    }
    for (size_type j = 0; j < _no_of_blocks; ++j) {
      // go backwards through the blocks:
      size_type i(_no_of_blocks - j - 1);
      if (_bitrep[i] < s._bitrep[i]) {
	return false;
      }
      if (_bitrep[i] > s._bitrep[i]) {
	return true;
      }
    }
    return false;
  }

  // boolean operators:
  const bool IntegerSet::operator==(const IntegerSet& s) const {
    if (_invariant != s._invariant) {
      return false;
    }
    if (_no_of_blocks != s._no_of_blocks) {
      return false;
    }
    for (size_type i = 0; i < _no_of_blocks; i++) {
      if (_bitrep[i] != s._bitrep[i]) {
	return false;
      }
    }
    return true;
  }

  const bool IntegerSet::operator<(const IntegerSet& s) const {

    // we implement the lexicographic order on the integers representing the invariant and the bitstring:
    const size_type min_no_of_blocks = _no_of_blocks < s._no_of_blocks ? _no_of_blocks : s._no_of_blocks;
  
    if (_invariant < s._invariant) {
      return true;
    }
    if (_invariant > s._invariant) {
      return false;
    }
    for (size_type i = 0; i < min_no_of_blocks; ++i) {
      if (_bitrep[i] < s._bitrep[i]) {
	return true;
      }
      if (_bitrep[i] > s._bitrep[i]) {
	return false;
      }
    }
    return (_no_of_blocks < s._no_of_blocks);
  }

  const bool IntegerSet::operator>(const IntegerSet& s) const {
    return (s < (*this));
  }
  

  const size_type IntegerSet::operator[](const size_type index) const {
    size_type i, j;

#ifdef DEBUG
    if (index >= card()) {
      std::cerr << "IntegerSet::operator[]: index out of range!" << std::endl;
    }
#endif
    size_type count = 0;
    for (i = 0; i < _no_of_blocks; i++) {
      for (j = 0; j < block_len; j++) {
	if ((_bitrep[i] >> j) & bit_one) {
	  if (count++ == index) {
	    return (j + i * block_len);
	  }
	}
      }
    }
    return 0;
  }

  // adding and deleting an element:

  IntegerSet& IntegerSet::operator+=(const size_type elem) {
    size_type block_pos = elem / block_len;
    if (block_pos >= _no_of_blocks) {
      // 	memset(&(_bitrep[_no_of_blocks]), 0, (size - _no_of_blocks) * sizeof(block));
      _no_of_blocks = block_pos + 1;
      if (_memsize < _no_of_blocks) {
	_expand(_no_of_blocks);
      }
    }
    _invariant ^= _bitrep[block_pos];
#ifdef BITCONSTANTS
    _bitrep[block_pos] |= _bit[elem % block_len];
#else
    _bitrep[block_pos] |= (bit_one << (elem % block_len));
#endif
    _invariant ^= _bitrep[block_pos];
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::operator+=(const size_type):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef COMPUTATIONS_DEBUG
    if (!contains(elem)) {
      std::cerr << "IntegerSet::operator+=(const size_type): "
		<< "computational error."
		<< std::endl;
      std::cerr << *this << " does not contain inserted element " << elem
		<< std::endl;
      exit(1);
    }
    if (_no_of_blocks <= block_pos) {
      std::cerr << "IntegerSet::operator+=(const size_type): "
		<< "computational error."
		<< std::endl;
      std::cerr << "_no_of_blocks = " << _no_of_blocks << " <= " 
		<< "block_pos = " << block_pos
		<< std::endl;
      exit(1);
    }
    for (size_type i = _no_of_blocks; i < _memsize; ++i) {
      if (_bitrep[i] != 0UL) {
	std::cerr << "IntegerSet::operator+=(const size_type): "
		  << "computational error."
		  << std::endl;
	std::cerr << i << "-th block not zero, but >= " 
		  << "_no_of_blocks = " << _no_of_blocks
		  << std::endl;
	exit(1);
      }
    }
#endif
    return *this;
  }

  IntegerSet& IntegerSet::operator-=(const size_type elem) {
    size_type block_pos = elem / block_len;

    if (block_pos >= _no_of_blocks) {
      return *this;
    }
    _invariant ^= _bitrep[block_pos];
#ifdef BITCONSTANTS
    _bitrep[block_pos] &= _not_bit[elem % block_len];
#else
    _bitrep[block_pos] &= ~(bit_one << (elem % block_len));
#endif
    _invariant ^= _bitrep[block_pos];  
    if (block_pos < _no_of_blocks - 1) {
      return *this;
    }
    if (_bitrep[block_pos] != 0UL) {
      return *this;
    }
    _no_of_blocks = block_pos + 1;
    while (_no_of_blocks > 0) {
      if (_bitrep[_no_of_blocks - 1] != 0UL) {
	break;
      }
      else {
	--_no_of_blocks;
      }
    }
    if (4 * _no_of_blocks + 1 < _memsize) {
      _contract(_no_of_blocks);
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::operator-=(const size_type):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef COMPUTATIONS_DEBUG
    if (contains(elem)) {
      std::cerr << "IntegerSet::operator-=(const size_type): "
		<< "computational error."
		<< std::endl;
      std::cerr << *this << " contains erased element " << elem
		<< std::endl;
      exit(1);
    }
    for (size_type i = _no_of_blocks; i < _memsize; ++i) {
      if (_bitrep[i] != 0UL) {
	std::cerr << "IntegerSet::operator-=(const size_type): "
		  << "computational error."
		  << std::endl;
	std::cerr << i << "-th block not zero, but >= " 
		  << "_no_of_blocks = " << _no_of_blocks
		  << std::endl;
	exit(1);
      }
    }
#endif
    return *this;
  }

  IntegerSet& IntegerSet::operator^=(const size_type elem) {
    size_type block_pos = elem / block_len;
    if (block_pos >= _no_of_blocks) {
      // 	memset(&(_bitrep[_no_of_blocks]), 0, (size - _no_of_blocks) * sizeof(block));
      _no_of_blocks = block_pos + 1;
      if (_memsize < _no_of_blocks) {
	_expand(_no_of_blocks);
      }
    }
    _invariant ^= _bitrep[block_pos];
#ifdef BITCONSTANTS
    _bitrep[block_pos] ^= _bit[elem % block_len];
#else
    _bitrep[block_pos] ^= (bit_one << (elem % block_len));
#endif
    _invariant ^= _bitrep[block_pos];
  
    if (block_pos < _no_of_blocks - 1) {
      return *this;
    }
    if (_bitrep[block_pos] != 0UL) {
      return *this;
    }
    _no_of_blocks = block_pos + 1;
    while (_no_of_blocks > 0) {
      if (_bitrep[_no_of_blocks - 1] != 0UL) {
	break;
      }
      else {
	--_no_of_blocks;
      }
    }
    if (4 * _no_of_blocks + 1 < _memsize) {
      _contract(_no_of_blocks);
    }
#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::operator^=(const size_type):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef WATCH_MAXBLOCKNO
    set_maxblockno();
#endif
    return *this;
  }

  // union, difference, and intersection with sets:

  IntegerSet& IntegerSet::operator+=(const IntegerSet& s) {
    if (s.empty()) {
      return *this;
    }
    _invariant = 0UL;
    if (_no_of_blocks < s._no_of_blocks) {
      for (size_type i = 0; i < _no_of_blocks; i++) {
	_bitrep[i] |= s._bitrep[i];
	_invariant ^= _bitrep[i];
      }
      if (_memsize < s._no_of_blocks) {
	_expand(s._no_of_blocks);
      }
      for (size_type i = _no_of_blocks; i < s._no_of_blocks; ++i) {
	_bitrep[i] = s._bitrep[i];
	_invariant ^= _bitrep[i];
      }
      _no_of_blocks = s._no_of_blocks;
#ifdef WATCH_MAXBLOCKNO
      set_maxblockno();
#endif
    }
    else {
      for (size_type i = 0; i < s._no_of_blocks; i++) {
	_bitrep[i] |= s._bitrep[i];
	_invariant ^= _bitrep[i];
      }
      for (size_type i = s._no_of_blocks; i < _no_of_blocks; i++) {
	_invariant ^= _bitrep[i];
      }      
    }

#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::operator+=(const IntegerSet&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef COMPUTATIONS_DEBUG
    if (!superset(s)) {
      std::cerr << "IntegerSet::operator+=(const IntegerSet&): "
		<< "computational error."
		<< std::endl;
      std::cerr << *this << " not a superset of sum operand " << s
		<< std::endl;
      exit(1);
    }
    for (size_type i = _no_of_blocks; i < _memsize; ++i) {
      if (_bitrep[i] != 0UL) {
	std::cerr << "IntegerSet::operator+=(const IntegerSet&): "
		  << "computational error."
		  << std::endl;
	std::cerr << i << "-th block not zero, but >= " 
		  << "_no_of_blocks = " << _no_of_blocks
		  << std::endl;
	exit(1);
      }
    }
#endif
    return *this;
  }

  IntegerSet& IntegerSet::operator-=(const IntegerSet& s) {
    if (s.empty() || empty()) {
      return *this;
    }
    if (_no_of_blocks > s._no_of_blocks) {
      _invariant = 0UL;
      for (size_type i = 0; i < s._no_of_blocks; ++i) {
	_bitrep[i] &= ~(s._bitrep[i]);
	_invariant ^= _bitrep[i];
      }
      for (size_type i = s._no_of_blocks; i < _no_of_blocks; ++i) {
	_invariant ^= _bitrep[i];
      }
    }
    else {
      size_type new_no_of_blocks = 0;
      _invariant = 0UL;
      for (size_type i = 0; i < _no_of_blocks; ++i) {
	if ((_bitrep[i] &= ~(s._bitrep[i])) != 0UL) {
	  new_no_of_blocks = i + 1;
	  _invariant ^= _bitrep[i];	    
	}
      }
      _no_of_blocks = new_no_of_blocks;
      if (4 * _no_of_blocks + 1 < _memsize) {
	_contract(_no_of_blocks);
      }
    }

#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::operator-=(const IntegerSet&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef COMPUTATIONS_DEBUG
    if (!disjoint(s)) {
      std::cerr << "IntegerSet::operator-=(const IntegerSet&): "
		<< "computational error."
		<< std::endl;
      std::cerr << *this << " not disjoint to difference operand " << s
		<< std::endl;
      exit(1);
    }
    for (size_type i = _no_of_blocks; i < _memsize; ++i) {
      if (_bitrep[i] != 0UL) {
	std::cerr << "IntegerSet::operator-=(const IntegerSet&): "
		  << "computational error."
		  << std::endl;
	std::cerr << i << "-th block not zero, but >= " 
		  << "_no_of_blocks = " << _no_of_blocks
		  << std::endl;
	exit(1);
      }
    }
#endif
    return *this;
  }

  IntegerSet& IntegerSet::operator*=(const IntegerSet& s) {
    if (empty()) {
      return *this;
    }
    if (s.empty()) {
      return clear();
    }
    if (_no_of_blocks > s._no_of_blocks) {
      _invariant = 0UL;
      for (size_type i = s._no_of_blocks; i < _no_of_blocks; ++i) {
	_bitrep[i] = 0UL;
      }
      _no_of_blocks = s._no_of_blocks;
    }
    size_type new_no_of_blocks = 0;
    _invariant = 0UL;
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      if ((_bitrep[i] &= s._bitrep[i]) != 0UL) {
	new_no_of_blocks = i + 1;
	_invariant ^= _bitrep[i];	    
      }
    }
    _no_of_blocks = new_no_of_blocks;
    if (4 * _no_of_blocks + 1 < _memsize) {
      _contract(_no_of_blocks);
    }

#ifdef CHECK_INVARIANT
    block_type check_invariant(0);
    for (size_type i = 0UL; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::operator*=(const IntegerSet&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
#ifdef COMPUTATIONS_DEBUG
    if (!subset(s)) {
      std::cerr << "IntegerSet::operator*=(const IntegerSet&): "
		<< "computational error."
		<< std::endl;
      std::cerr << *this << " not a subset of intersection operand " << s
		<< std::endl;
      exit(1);
    }
    for (size_type i = _no_of_blocks; i < _memsize; ++i) {
      if (_bitrep[i] != 0UL) {
	std::cerr << "IntegerSet::operator*=(const IntegerSet&): "
		  << "computational error."
		  << std::endl;
	std::cerr << i << "-th block not zero, but >= " 
		  << "_no_of_blocks = " << _no_of_blocks
		  << std::endl;
	exit(1);
      }
    }
#endif
    return *this;
  }

  IntegerSet& IntegerSet::operator^=(const IntegerSet& s) {
    if (s.empty()) {
      return *this;
    }
    if (_no_of_blocks < s._no_of_blocks) {
      if (_memsize < s._no_of_blocks) {
	_expand(s._no_of_blocks);
      }
      _invariant = 0UL;
      for (size_type i = 0; i < _no_of_blocks; i++) {
	_bitrep[i] ^= s._bitrep[i];
	_invariant ^= _bitrep[i];
      }
      for (size_type i = _no_of_blocks; i < s._no_of_blocks; ++i) {
	_bitrep[i] = s._bitrep[i];
	_invariant ^= _bitrep[i];
      }
      _no_of_blocks = s._no_of_blocks;
#ifdef WATCH_MAXBLOCKNO
      set_maxblockno();
#endif
    }
    else {
      size_type new_no_of_blocks = 0;
      _invariant = 0UL;
      for (size_type i = 0; i < s._no_of_blocks; ++i) {
	if ((_bitrep[i] ^= s._bitrep[i]) != 0UL) {
	  new_no_of_blocks = i + 1;
	  _invariant ^= _bitrep[i];	    
	}
      }
      for (size_type i = s._no_of_blocks; i < _no_of_blocks; ++i) {
	if ((_bitrep[i]) != 0UL) {
	  new_no_of_blocks = i + 1;
	  _invariant ^= _bitrep[i];	    
	}
      }
      _no_of_blocks = new_no_of_blocks;
      if (4 * _no_of_blocks + 1 < _memsize) {
	_contract(_no_of_blocks);
      }
    }

#ifdef CHECK_INVARIANT
    block_type check_invariant(0UL);
    for (size_type i = 0; i < _no_of_blocks; ++i) {
      check_invariant ^= _bitrep[i];
    }
    if (check_invariant != _invariant) {
      std::cerr << "IntegerSet::operator^=(const IntegerSet&):";
      std::cerr << "error in invariant" << std::endl;
      exit(1);
    }
#endif
    return *this;
  }

  // other modifiers:
  IntegerSet& IntegerSet::remove_max(const size_type how_many) {
    if (empty()) {
      return *this;
    }
    for (size_type i = 0; i < how_many; ++i) {
      *this -= max_elem();
      if (empty()) {
	break;
      }
    }
    return *this;
  }
  
  IntegerSet& IntegerSet::remove_min(const size_type how_many) {
    if (empty()) {
      return *this;
    }
    for (size_type i = 0; i < how_many; ++i) {
      *this -= *begin();
      if (empty()) {
	break;
      }
    }
    return *this;
  }
  
  IntegerSet& IntegerSet::permute(const Symmetry& s) {
    IntegerSet cpy(*this);
    for (IntegerSet::const_iterator iter = cpy.begin();
	 iter != cpy.end();
	 ++iter) {
      if (*iter != s(*iter)) {
	(*this) ^= *iter;
	(*this) ^= s(*iter);
      }
    }
    return *this;
  }

  // the same but a new set is returned:

  IntegerSet IntegerSet::operator+(const size_type elem) const {
    IntegerSet s(*this);
    s += elem;
    return s;
  }

  IntegerSet IntegerSet::operator-(const size_type elem) const {
    IntegerSet s(*this);
    s -= elem;
    return s;
  }

  // IntegerSet IntegerSet::operator+(const IntegerSet& s1) const {
  //   IntegerSet s(*this);
  //   s += s1;
  //   return s;
  // }

  // IntegerSet IntegerSet::operator-(const IntegerSet& s1) const {
  //   IntegerSet s(*this);
  //   s -= s1;
  //   return s;
  // }
 
  // IntegerSet IntegerSet::operator^(const IntegerSet& s1) const {
  //   IntegerSet s(*this);
  //   s ^= s1;
  //   return s;
  // }

  // IntegerSet IntegerSet::operator*(const IntegerSet& s1) const {
  //   IntegerSet s(*this);
  //   s *= s1;
  //   return s;
  // }

  IntegerSet IntegerSet::operator+(const IntegerSet& s) const {

    // check for trivial results:
    if (empty()) {
      return s;
    }
    if (s.empty()) {
      return *this;
    }
    IntegerSet result;
  
    // first determine the size of the result:
    result._no_of_blocks = _no_of_blocks > s._no_of_blocks? _no_of_blocks : s._no_of_blocks;
  
    // perform only the minimal amount of allocation necessary:
    result._expand(result._no_of_blocks);

    // finally, assign the intersection values:
    if (_no_of_blocks == s._no_of_blocks) {
      result._invariant = 0UL;
      for (size_type i = 0; i < result._no_of_blocks; ++i) {
	result._bitrep[i] = (_bitrep[i] | s._bitrep[i]);
	result._invariant ^= result._bitrep[i];
      }
    }
    else if (_no_of_blocks < s._no_of_blocks) {
      result._invariant = 0UL;
      for (size_type i = 0; i < _no_of_blocks; ++i) {
	result._bitrep[i] = (_bitrep[i] | s._bitrep[i]);
	result._invariant ^= result._bitrep[i];
      }
      for (size_type i = _no_of_blocks; i < s._no_of_blocks; ++i) {
	result._bitrep[i] = s._bitrep[i];
	result._invariant ^= result._bitrep[i];
      }      
    }
    else {
      result._invariant = 0UL;
      for (size_type i = 0; i < s._no_of_blocks; ++i) {
	result._bitrep[i] = (_bitrep[i] | s._bitrep[i]);
	result._invariant ^= result._bitrep[i];
      }
      for (size_type i = s._no_of_blocks; i < _no_of_blocks; ++i) {
	result._bitrep[i] = _bitrep[i];
	result._invariant ^= result._bitrep[i];
      }      
    }
    return result;
  }

  IntegerSet IntegerSet::operator-(const IntegerSet& s) const {

    // check for trivial result:
    if (empty()) {
      return *this;
    }
    if (s.empty()) {
      return *this;
    }
    IntegerSet result;
    if (_no_of_blocks > s._no_of_blocks) {
      result._no_of_blocks = _no_of_blocks;
      result._expand(result._no_of_blocks);

      // finally, assign the difference values:
      result._invariant = 0UL;
      for (size_type i = 0; i < s._no_of_blocks; ++i) {
	result._bitrep[i] = (_bitrep[i] & ~(s._bitrep[i]));
	result._invariant ^= result._bitrep[i];
      }
      for (size_type i = s._no_of_blocks; i < _no_of_blocks; ++i) {
	result._bitrep[i] = _bitrep[i];
	result._invariant ^= result._bitrep[i];
      }
    }
    else {

      // first determine the real size of the result after the operation:
      result._no_of_blocks = _no_of_blocks;
      while (result._no_of_blocks > 0) {
	if ((_bitrep[result._no_of_blocks - 1] & ~(s._bitrep[result._no_of_blocks - 1])) == 0) {
	  --result._no_of_blocks;
	}
	else {
	  break;
	}
      }
      if (result._no_of_blocks == 0) {
	return result;
      }

      // perform only the minimal amount of allocation necessary:
      result._expand(result._no_of_blocks);

      // finally, assign the difference values:
      result._invariant = 0UL;
      for (size_type i = 0; i < result._no_of_blocks; ++i) {
	result._bitrep[i] = (_bitrep[i] & ~(s._bitrep[i]));
	result._invariant ^= result._bitrep[i];
      }
    }  
    return result;
  }

  IntegerSet IntegerSet::operator*(const IntegerSet& s) const {

    // check for trivial empty sets:
    if (empty()) {
      return *this;
    }
    if (s.empty()) {
      return s;
    }
    IntegerSet result;
    
    // first determine the size of the result:
    result._no_of_blocks = _no_of_blocks < s._no_of_blocks? _no_of_blocks : s._no_of_blocks;
    while (result._no_of_blocks > 0) {
      if ((_bitrep[result._no_of_blocks - 1] & s._bitrep[result._no_of_blocks - 1]) == 0) {
	--result._no_of_blocks;
      }
      else {
	break;
      }
    }
    if (result._no_of_blocks == 0) {
      return result;
    }

    // perform only the minimal amount of allocation necessary:
    result._expand(result._no_of_blocks);

    // finally, assign the intersection values:
    result._invariant = 0UL;
    for (size_type i = 0; i < result._no_of_blocks; ++i) {
      result._bitrep[i] = (_bitrep[i] & s._bitrep[i]);
      result._invariant ^= result._bitrep[i];
    }

    return result;
  }

  // IntegerSet IntegerSet::operator^(const IntegerSet& s1) const {
  //   IntegerSet s(*this);
  //   s ^= s1;
  //   return s;
  // }

  IntegerSet IntegerSet::operator^(const IntegerSet& s) const {
    // check for trivial results:
    if (empty()) {
      return s;
    }
    if (s.empty()) {
      return *this;
    }

    IntegerSet result;

    // first determine the size of the result:
    result._no_of_blocks = _no_of_blocks > s._no_of_blocks? _no_of_blocks : s._no_of_blocks;

    if (_no_of_blocks == s._no_of_blocks) {

      // in this case, the actual result maybe smaller than the maximum length of the operands:
      while (result._no_of_blocks > 0) {
	if ((_bitrep[result._no_of_blocks - 1] ^ s._bitrep[result._no_of_blocks - 1]) == 0) {
	  --result._no_of_blocks;
	}
	else {
	  break;
	}
      }
    }
  
    if (result._no_of_blocks == 0) {
      return result;
    }

    // perform only the minimal amount of allocation necessary:
    result._expand(result._no_of_blocks);

    // finally, assign the xor values:
    if (_no_of_blocks == s._no_of_blocks) {
      result._invariant = 0UL;
      for (size_type i = 0; i < result._no_of_blocks; ++i) {
	result._bitrep[i] = (_bitrep[i] ^ s._bitrep[i]);
	result._invariant ^= result._bitrep[i];
      }
    }
    else if (_no_of_blocks < s._no_of_blocks) {
      result._invariant = 0UL;
      for (size_type i = 0; i < _no_of_blocks; ++i) {
	result._bitrep[i] = (_bitrep[i] ^ s._bitrep[i]);
	result._invariant ^= result._bitrep[i];
      }
      for (size_type i = _no_of_blocks; i < s._no_of_blocks; ++i) {
	result._bitrep[i] = s._bitrep[i];
	result._invariant ^= result._bitrep[i];
      }
    }
    else {
      result._invariant = 0UL;
      for (size_type i = 0; i < s._no_of_blocks; ++i) {
	result._bitrep[i] = (_bitrep[i] ^ s._bitrep[i]);
	result._invariant ^= result._bitrep[i];
      }
      for (size_type i = s._no_of_blocks; i < _no_of_blocks; ++i) {
	result._bitrep[i] = _bitrep[i];
	result._invariant ^= result._bitrep[i];
      }
    }

    return result;
  }

  // other out-of-place functions:
  IntegerSet IntegerSet::lexmin_subset(const size_type card) const {
    IntegerSet result(*this);
    return result.remove_max(this->card() - card);
  }
  
  IntegerSet IntegerSet::lexmax_subset(const size_type card) const {
    IntegerSet result(*this);
    return result.remove_min(this->card() - card);
  }
  
  IntegerSet IntegerSet::permute(const Symmetry& s) const {
    IntegerSet result(*this);
    for (IntegerSet::const_iterator iter = begin();
	 iter != end();
	 ++iter) {
      if (*iter != s(*iter)) {
	result ^= *iter;
	result ^= s(*iter);
      }
    }
    return result;
  }

  
  // the intersection with a set of integer sets,
  // returns cardinality of intersection:
  // 0 means 0 elements, 1 means 1 element, 2 means 2 or more elements:
  const int IntegerSet::intersection_card(const IntegerSet** setarray, 
					  const size_type size,
					  size_type& common_index) const {
    if (empty()) {
      return 0;
    }
    if (size == 0) {
      IntegerSet::const_iterator iter = begin();
      common_index = *iter;
      if (++iter == end()) {
	return 1;
      }
      else {
	return 2;
      }
    }
    size_type min_no_of_blocks = _no_of_blocks;
    // get minimal _no_of_blocks:
    for (size_type j = 0; j < size; ++j) {
      if (setarray[j]->empty()) {
	return 0;
      }
      if (setarray[j]->_no_of_blocks < min_no_of_blocks) {
	min_no_of_blocks = setarray[j]->_no_of_blocks;
      }
    }
    // intersect and keep control over uniqueness:
    int intersection_card = 0;
    block_type computation_buffer = 0;
    for (size_type i = 0; i < min_no_of_blocks; ++i) {
      computation_buffer = _bitrep[i];
      for (size_type j = 0; j < size; ++j) {
	computation_buffer &= setarray[j]->_bitrep[i];
	if (computation_buffer == 0) {
	  break;
	}
      }
      if (computation_buffer == 0) {
	continue;
      }
      // analyse computation_buffer:
      for (size_type k = 0; k < block_len; ++k) {
	if ((computation_buffer & (bit_one << k)) != 0) {
	  common_index = k + i * block_len;
	  if (++intersection_card > 1) {
	    return 2;
	  }
	}
      }
    }
    if (intersection_card == 0) {
      return 0;
    }
#ifdef COMPUTATIONS_DEBUG
    for (size_type j = 0; j < size; ++j) {
      if (!setarray[j]->contains(common_index)) {
	std::cerr << "IntegerSet::intersection_card"
		  << "(const IntegerSet**, const IntegerSet&, size_type&): "
		  << "computational error."
		  << std::endl;
	std::cerr << *setarray[j] << " does not contain common_index " 
		  << common_index
		  << std::endl;
	exit(1); 
      }
    }
#endif
    return 1;
  }

  // as a special case of the previous function:
  // 0 means 0 elements, 1 means at least 1 element:
  const int IntegerSet::intersection_nonempty(const IntegerSet** setarray, 
					      const size_type size,
					      size_type& common_index) const {
    if (empty()) {
      return 0;
    }
    if (size == 0) {
      return 1;
    }
    size_type min_no_of_blocks = _no_of_blocks;
    // get minimal _no_of_blocks:
    for (size_type j = 0; j < size; ++j) {
      if (setarray[j]->empty()) {
	return 0;
      }
      if (setarray[j]->_no_of_blocks < min_no_of_blocks) {
	min_no_of_blocks = setarray[j]->_no_of_blocks;
      }
    }
    // intersect and keep control over uniqueness:
    int intersection_card = 0;
    block_type computation_buffer = 0;
    for (size_type i = 0; i < min_no_of_blocks; ++i) {
      computation_buffer = _bitrep[i];
      for (size_type j = 0; j < size; ++j) {
	computation_buffer &= setarray[j]->_bitrep[i];
	if (computation_buffer == 0) {
	  break;
	}
      }
      if (computation_buffer == 0) {
	continue;
      }
      for (size_type k = 0; k < block_len; ++k) {
	if ((computation_buffer & (bit_one << k)) != 0) {
	  common_index = k + i * block_len;
	  return 1;
	}
      }
    }
    if (intersection_card == 0) {
      return 0;
    }
#ifdef COMPUTATIONS_DEBUG
    for (size_type j = 0; j < size; ++j) {
      if (!setarray[j]->contains(common_index)) {
	std::cerr << "IntegerSet::intersection_nonempty"
		  << "(const IntegerSet**, const IntegerSet&, size_type&): "
		  << "computational error."
		  << std::endl;
	std::cerr << *setarray[j] << " does not contain common_index " 
		  << common_index
		  << std::endl;
	exit(1); 
      }
    }
#endif
    return 1;
  }

  // specialization for two IntegerSets:
  const bool IntegerSet::intersection_nonempty(const IntegerSet& is) const {
    if (empty() || is.empty()) {
      return false;
    }
    size_type min_no_of_blocks = _no_of_blocks < is._no_of_blocks ? _no_of_blocks : is._no_of_blocks;
    for (size_type i = 0; i < min_no_of_blocks; ++i) {
      if ((_bitrep[i] & is._bitrep[i]) != 0) {
	return true;
      }
    }
    return false;
  }

  // stream input:
  std::istream& IntegerSet::read(std::istream& ist) {
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
	  std::cerr << "IntegerSet::read(std::istream&): Not an integer." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "IntegerSet::read(std::istream&): Missing ``{''." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  // stream output:
  std::ostream& IntegerSet::write(std::ostream& ost) const {
    size_type count = 0;
    size_type card = this->card();

    ost << "{";
    for (IntegerSet::const_iterator iter = begin(); iter != end(); ++iter) {
      ost << *iter;
      count++;
      if (count < card) {
	ost << ",";
      }
    }
#ifdef CHECK_INVARIANT
    ost << ':' << _invariant;
#endif
#ifdef DEBUG
    ost << "[" << _no_of_blocks
	<< "," << _memsize 
	<< ";" << keysize();
    for (size_type n = 0; n < keysize(); ++n)
      ost << "(" << key(n) << ")";

    ost << "]";
#endif
    ost << "}";
    return ost;
  }


  // IntegerSet iterator:

  __is_const_iterator::__is_const_iterator(const IntegerSet& s) :
    _container(&s),
    _current_block(0),
    _current_bit(0) {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "__is_const_iterator::__is_const_iterator(const IntegerSet&):" << std::endl;
#endif
    for (size_type i = 0; i < _container->_no_of_blocks; i++) {
      if (_container->_bitrep[i] == 0UL) {
	continue;
      }    
      block_type b = _container->_bitrep[i];
#ifndef BIT_LINEAR_SEARCH

      // here we proceed by looking up the first bit in a byte table:
      for (size_type j = 0; j < bytes_per_block; ++j) {
	unsigned char current_byte(static_cast<unsigned char>(b & ~(unsigned char)0UL));
	if (current_byte) {
	  _current_block = i;
	  _current_bit   = j * byte_len + IntegerSet::_S_first_one[current_byte];
#ifdef COMPUTATIONS_DEBUG
	  std::cout << "first set bit is " << _current_bit << " in block " << _current_block << std::endl;
#endif
	  return;
	}
	b >>= byte_len;
      }
#else

      // here we proceed by linear search for the first bit:
      for (size_type j = 0; j < block_len; j++) {
#ifdef BITCONSTANTS
	if ((b & _bit[j]) != 0UL) {
#else
	  if ((b & (bit_one << j)) != 0UL) {
#endif
	    _current_block = i;
	    _current_bit = j;
	    return;
#ifdef BITCONSTANTS
	  }	
#else
	}
#endif
      }
#endif
    }
    _current_block = _container->_no_of_blocks; // set to _container->end()
    _current_bit = block_len;
  }

  __is_const_iterator& __is_const_iterator::operator++() {  
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "__is_const_iterator::operator++():" << std::endl;
#endif
    if (_current_block >= _container->_no_of_blocks) {
      _current_block = _container->_no_of_blocks;
      _current_bit = block_len;
      return *this;
    }

#ifndef BIT_LINEAR_SEARCH

    // here we proceed by looking up the first bit in a byte table:
    if (++_current_bit < block_len) {  

      // we might find another bit in the same block:
      block_type b = _container->_bitrep[_current_block];
    
      // mask away bits before _current_bit:
      b &= (all_bits << _current_bit);
    
#ifdef COMPUTATIONS_DEBUG
      std::cout << "current bit is " << _current_bit << " in block " << _current_block << std::endl;
#endif
    
      if (b != 0UL) {
	for (size_type j = 0; j < bytes_per_block; ++j) {
	  unsigned char current_byte(static_cast<unsigned char>(b & ~(unsigned char)0UL));
#ifdef COMPUTATIONS_DEBUG
	  std::cout << "checking byte " << j << "=" << (size_type)current_byte << " in block " << _current_block << std::endl;
#endif
	  if (current_byte) {
	    _current_bit   = j * byte_len + IntegerSet::_S_first_one[current_byte];
	    return *this;
	  }
	  b >>= byte_len;
	}
      }
    }
  
    // investigate subsequent blocks:
    for (size_type i = _current_block + 1; i < _container->_no_of_blocks; i++) {
      if (_container->_bitrep[i] == 0UL) {
	continue;
      }    
      block_type b = _container->_bitrep[i];
      for (size_type j = 0; j < bytes_per_block; ++j) {
	unsigned char current_byte(static_cast<unsigned char>(b & ~(unsigned char)0UL));
#ifdef COMPUTATIONS_DEBUG
	std::cout << "checking byte " << j << "=" << (size_type)current_byte << " in subsequent block " << i << std::endl;
#endif
	if (current_byte) {
	  _current_block = i;
	  _current_bit   = j * byte_len + IntegerSet::_S_first_one[current_byte];
	  return *this;
	}
	b >>= byte_len;
      }
    }
#else

    // here we proceed by linear search for the first bit:
    for (size_type j = _current_bit + 1; j < block_len; j++) {
#ifdef BITCONSTANTS
      if ((_container->_bitrep[_current_block] & _bit[j]) != 0UL) {
#else
	if ((_container->_bitrep[_current_block] & (bit_one << j)) != 0UL) {
#endif
	  _current_bit = j;
	  return *this;
#ifdef BITCONSTANTS
	}	
#else
      }
#endif
    }
    for (size_type i = _current_block + 1; i < _container->_no_of_blocks; i++) {
      if (_container->_bitrep[i] == 0UL) {
	continue;
      }
      block_type b = _container->_bitrep[i];
      for (size_type j = 0; j < block_len; j++) {
#ifdef BITCONSTANTS
	if ((_container->_bitrep[i] & _bit[j]) != 0UL) {
#else
	  if ((_container->_bitrep[i] & (bit_one << j)) != 0UL) {
#endif
	    _current_block = i;
	    _current_bit = j;
	    return *this;
#ifdef BITCONSTANTS
	  }	
#else
	}
#endif
      }
    }
#endif
    _current_block = _container->_no_of_blocks;
    _current_bit = block_len;
    return *this;
  }

}; // namespace topcom
  
// eof IntegerSet.cc
