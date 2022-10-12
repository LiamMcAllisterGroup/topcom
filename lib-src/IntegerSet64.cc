////////////////////////////////////////////////////////////////////////////////
// 
// IntegerSet64.cc
//
//    produced: 21/08/97 jr
// last change: 21/08/97 jr
// 
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <ctype.h>
#include <string.h>

#include "IntegerSet64.hh"
#include "Symmetry.hh"

namespace topcom {

  // IntegerSet64:

#ifdef BITCONSTANTS
  block_type IntegerSet64::_bit[64] = {
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
  block_type IntegerSet64::_not_bit[64] = {
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
  unsigned char IntegerSet64::_S_bit_count[] = {
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

  unsigned char IntegerSet64::_S_first_one[] = {
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

  IntegerSet64::IntegerSet64() :
    _bitrep(no_bits) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64()" << std::endl;
#endif
  }  

  IntegerSet64::IntegerSet64(const IntegerSet64& s) :
    _bitrep(s._bitrep) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64(const IntegerSet64&)" << std::endl;
#endif
  }

  IntegerSet64::IntegerSet64(IntegerSet64&& s) :
    _bitrep(s._bitrep) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64(IntegerSet64&&)" << std::endl;
#endif
    s._bitrep = no_bits;
  }
  
  IntegerSet64::IntegerSet64(const SparseIntegerSet& sis) :
    _bitrep(no_bits) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64(const SparseIntegerSet&)" << std::endl;
#endif
    if (sis.card() == 0) {
      return;
    }
    for (SparseIntegerSet::const_iterator iter = sis.begin(); iter != sis.end(); ++iter) {
#ifdef BITCONSTANTS
      _bitrep |= _bit[*iter % block_len];
#else
      _bitrep |= (bit_one << (*iter % block_len));
#endif
    }
  }

  IntegerSet64::IntegerSet64(const size_type start, const size_type stop) :
    _bitrep(no_bits) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64(const size_type, const size_type)" << std::endl;
#endif
    if (stop == 0) {
      return;
    }
    fill(start, stop);
  }

  IntegerSet64::IntegerSet64(const size_type elem) :
    _bitrep(no_bits) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64(const size_type)" << std::endl;
#endif
#ifdef BITCONSTANTS
    _bitrep |= _bit[elem % block_len];
#else
    _bitrep |= (bit_one << (elem % block_len));
#endif
  }

  IntegerSet64::IntegerSet64(const size_type len, const size_type* init) : 
    _bitrep(no_bits) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64(const size_type, const size_type*)"
	      << std::endl;
#endif
    if (len == 0) {
      return;
    }
    for (size_type i = 0; i < len; ++i) {
#ifdef BITCONSTANTS
      _bitrep |= _bit[init[i] % block_len];
#else
      _bitrep |= (bit_one << (init[i] % block_len));
#endif
    }
  }

#ifndef STL_CONTAINERS
  IntegerSet64::IntegerSet64(const Array<size_type>& init) : 
    _bitrep(no_bits) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64(const Array<size_type>&)"
	      << std::endl;
#endif
    const size_type len = init.size();
    if (len == 0) {
      return;
    }
    for (size_type i = 0; i < len; ++i) {
#ifdef BITCONSTANTS
      _bitrep |= _bit[init[i] % block_len];
#else
      _bitrep |= (bit_one << (init[i] % block_len));
#endif
    }
  }

  IntegerSet64::IntegerSet64(const PlainArray<size_type>& init) : 
    _bitrep(no_bits) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64(const PlainArray<size_type>&)"
	      << std::endl;
#endif
    const size_type len = init.size();
    if (len == 0) {
      return;
    }
    for (size_type i = 0; i < len; ++i) {
#ifdef BITCONSTANTS
      _bitrep |= _bit[init[i] % block_len];
#else
      _bitrep |= (bit_one << (init[i] % block_len));
#endif
    }
  }

#else

  IntegerSet64::IntegerSet64(const std::set<parameter_type>& init) : 
    _bitrep(no_bits) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64(const std::set<parameter_type>&)"
	      << std::endl;
#endif
    const size_type len = init.size();
    if (len == 0) {
      return;
    }
    for (std::set<parameter_type>::const_iterator inititer = init.begin();
	 inititer != init.end();
	 ++inititer) {
#ifdef BITCONSTANTS
      _bitrep |= _bit[*inititer % block_len];
#else
      _bitrep |= (bit_one << (*inititer % block_len));
#endif
    }
  }

  IntegerSet64::IntegerSet64(const std::vector<parameter_type>& init) : 
    _bitrep(no_bits) {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::IntegerSet64(const std::vector<parameter_type>&)"
	      << std::endl;
#endif
    const size_type len = init.size();
    if (len == 0) {
      return;
    }
    for (size_type i = 0; i < len; ++i) {
#ifdef BITCONSTANTS
      _bitrep |= _bit[init[i] % block_len];
#else
      _bitrep |= (bit_one << (init[i] % block_len));
#endif
    }
  }

#endif

  IntegerSet64::~IntegerSet64() {
#ifdef CONSTRUCTOR_DEBUG
    std::cout << "IntegerSet64::~IntegerSet64()" << std::endl;
#endif
  }

  // assignment:
  IntegerSet64& IntegerSet64::operator=(const IntegerSet64& s) {
    _bitrep = s._bitrep;
    return *this;
  }

  IntegerSet64& IntegerSet64::operator=(IntegerSet64&& s) {
    _bitrep = s._bitrep;
    return *this;
  }

  // modifier:
  IntegerSet64& IntegerSet64::clear() {
    _bitrep = no_bits;
    return *this;
  }

  IntegerSet64& IntegerSet64::fill(const size_type start, const size_type stop) {
    if (start >= stop) {
      return *this;
    }
    for (size_type j = start % block_len; j < (stop - 1) % block_len + 1; ++j) {
#ifdef BITCONSTANTS
      _bitrep |= _bit[j];
#else
      _bitrep |= (bit_one << j);
#endif	
    }
    return *this;
  }

  // cardinality:
  const parameter_type IntegerSet64::card() const {
    size_type count(0);

    // algorithm taken from "http://www.caam.rice.edu/~dougm/twiddle/BitCount.html"
    size_type tmpbits = _bitrep;
    while (tmpbits) {
      ++count;
      tmpbits &= tmpbits - 1;
    }
    return count;
  }

  const size_type IntegerSet64::min_elem() const {
    if (empty()) {
      std::cerr << "IntegerSet::min_elem() const: "
		<< "min element of empty set is not defined - exiting"
		<< std::endl;
      exit(1);
    }
    return *begin();
  }

  const parameter_type IntegerSet64::max_elem() const {
    if (_bitrep != 0) {
      // if the compiler does not support a built-in
      // count-leading-zeroes-long function, use __fls from above:
      // return (i - 1) * block_len + __fls(_bitrep[i]);
      return block_len - __builtin_clzl(_bitrep) - 1;
    }
    else {
      return -1;
    }
  }

  // relations:
  const bool IntegerSet64::contains(const size_type elem) const {
#ifdef BITCONSTANTS
    if ((_bit[elem % block_len] & _bitrep) != no_bits) {
#else
      if (((bit_one << (elem % block_len)) & _bitrep) != no_bits) {
#endif
	return true;
#ifdef BITCONSTANTS
      }
#else
    }
#endif
    else {
      return false;
    }
  }
  
  const bool IntegerSet64::superset(const IntegerSet64& is) const {
    return (!(~_bitrep & is._bitrep));
  }

  const bool IntegerSet64::subset(const IntegerSet64& is) const {
    return is.superset(*this);
  }

  const bool IntegerSet64::disjoint(const IntegerSet64& is) const {
    return (!(_bitrep & is._bitrep));
  }

  const bool IntegerSet64::lexsmaller(const IntegerSet64& s) const {
    // we implement the true left-to-right lexicographic order on the integer set:
    return (_kbitreverse(~_bitrep) < _kbitreverse(~s._bitrep));
  }

  const bool IntegerSet64::colexgreater(const IntegerSet64& s) const {
    // we implement the true right-to-left lexicographic order on the integer set:
    return (_bitrep > s._bitrep);
  }

  // boolean operators:
  const bool IntegerSet64::operator==(const IntegerSet64& s) const {
    return (_bitrep == s._bitrep);
  }

  const bool IntegerSet64::operator<(const IntegerSet64& s) const {
    return (_bitrep < s._bitrep);
  }

  const bool IntegerSet64::operator>(const IntegerSet64& s) const {
    return (s < (*this));
  }
  

  const size_type IntegerSet64::operator[](const size_type index) const {
    size_type j;
    size_type count = 0;
    for (j = 0; j < block_len; j++) {
      if ((_bitrep >> j) & bit_one) {
	if (count++ == index) {
	  return j;
	}
      }
    }
    return 0;
  }

  // adding and deleting an element:

  IntegerSet64& IntegerSet64::operator+=(const size_type elem) {
#ifdef BITCONSTANTS
    _bitrep |= _bit[elem % block_len];
#else
    _bitrep |= (bit_one << (elem % block_len));
#endif
    return *this;
  }

  IntegerSet64& IntegerSet64::operator-=(const size_type elem) {
#ifdef BITCONSTANTS
    _bitrep &= _not_bit[elem % block_len];
#else
    _bitrep &= ~(bit_one << (elem % block_len));
#endif
    return *this;
  }

  IntegerSet64& IntegerSet64::operator^=(const size_type elem) {
#ifdef BITCONSTANTS
    _bitrep ^= _bit[elem % block_len];
#else
    _bitrep ^= (bit_one << (elem % block_len));
#endif
    return *this;
  }

  // union, difference, and intersection with sets:

  IntegerSet64& IntegerSet64::operator+=(const IntegerSet64& s) {
    _bitrep |= s._bitrep;
    return *this;
  }

  IntegerSet64& IntegerSet64::operator-=(const IntegerSet64& s) {
    _bitrep &= ~(s._bitrep);
    return *this;
  }

  IntegerSet64& IntegerSet64::operator*=(const IntegerSet64& s) {
    _bitrep &= s._bitrep;
    return *this;
  }

  IntegerSet64& IntegerSet64::operator^=(const IntegerSet64& s) {
    _bitrep ^= s._bitrep;
    return *this;
  }

  // other modifiers:
  IntegerSet64& IntegerSet64::remove_max(const size_type how_many) {
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
  
  IntegerSet64& IntegerSet64::remove_min(const size_type how_many) {
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
  
  IntegerSet64& IntegerSet64::permute(const Symmetry& s) {
    IntegerSet64 cpy(*this);
    for (IntegerSet64::const_iterator iter = cpy.begin();
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

  IntegerSet64 IntegerSet64::operator+(const size_type elem) const {
    IntegerSet64 s(*this);
    s += elem;
    return s;
  }

  IntegerSet64 IntegerSet64::operator-(const size_type elem) const {
    IntegerSet64 s(*this);
    s -= elem;
    return s;
  }

  IntegerSet64 IntegerSet64::operator+(const IntegerSet64& s1) const {
    IntegerSet64 s(*this);
    s += s1;
    return s;
  }

  IntegerSet64 IntegerSet64::operator-(const IntegerSet64& s1) const {
    IntegerSet64 s(*this);
    s -= s1;
    return s;
  }

  IntegerSet64 IntegerSet64::operator*(const IntegerSet64& s1) const {
    IntegerSet64 s(*this);
    s *= s1;
    return s;
  }

  IntegerSet64 IntegerSet64::operator^(const IntegerSet64& s1) const {
    IntegerSet64 s(*this);
    s ^= s1;
    return s;
  }

  // other out-of-place functions:
  IntegerSet64 IntegerSet64::lexmin_subset(const size_type card) const {
    IntegerSet64 result(*this);
    return result.remove_max(this->card() - card);
  }
  
  IntegerSet64 IntegerSet64::lexmax_subset(const size_type card) const {
    IntegerSet64 result(*this);
    return result.remove_min(this->card() - card);
  }
  
  IntegerSet64 IntegerSet64::permute(const Symmetry& s) const {
    IntegerSet64 result(*this);
    for (IntegerSet64::const_iterator iter = begin();
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
  const int IntegerSet64::intersection_card(const IntegerSet64** setarray, 
					    const size_type size,
					    size_type& common_index) const {
    if (empty()) {
      return 0;
    }
    if (size == 0) {
      IntegerSet64::const_iterator iter = begin();
      common_index = *iter;
      if (++iter == end()) {
	return 1;
      }
      else {
	return 2;
      }
    }

    // intersect and keep control over uniqueness:
    int intersection_card = 0;
    block_type computation_buffer = 0;
    computation_buffer = _bitrep;
    for (size_type j = 0; j < size; ++j) {
      computation_buffer &= setarray[j]->_bitrep;
      if (computation_buffer == 0) {
	return 0;
      }
    }

    // analyse computation_buffer:
    for (size_type k = 0; k < block_len; ++k) {
      if ((computation_buffer & (bit_one << k)) != 0) {
	common_index = k;
	if (++intersection_card > 1) {
	  return 2;
	}
      }
    }
#ifdef COMPUTATIONS_DEBUG
    for (size_type j = 0; j < size; ++j) {
      if (!setarray[j]->contains(common_index)) {
	std::cerr << "IntegerSet64::intersection_card"
		  << "(const IntegerSet64**, size_type, size_type&): "
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
  const int IntegerSet64::intersection_nonempty(const IntegerSet64** setarray, 
						const size_type size,
						size_type& common_index) const {
     if (empty()) {
      return 0;
    }
    if (size == 0) {
      return 1;
    }

    // intersect and keep control over uniqueness:
    int intersection_card = 0;
    block_type computation_buffer = 0;
    computation_buffer = _bitrep;
    for (size_type j = 0; j < size; ++j) {
      computation_buffer &= setarray[j]->_bitrep;
      if (computation_buffer == 0) {
	return 0;
      }
    }
    return 1;
  }

  // specialization for two IntegerSets:
  const bool IntegerSet64::intersection_nonempty(const IntegerSet64& is) const {
    if (empty() || is.empty()) {
      return false;
    }
    if ((_bitrep & is._bitrep) != 0) {
      return true;
    }
    return false;
  }

  // stream input:
  std::istream& IntegerSet64::read(std::istream& ist) {
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
	  std::cerr << "IntegerSet64::read(std::istream&): Not an integer." << std::endl;
#endif
	  ist.clear(std::ios::failbit);
	  return ist;
	}
      }
    }
    else {
#ifdef READ_DEBUG
      std::cerr << "IntegerSet64::read(std::istream&): Missing ``{''." << std::endl;
#endif
      ist.clear(std::ios::failbit);
      return ist;
    }
    ist.clear(std::ios::goodbit);
    return ist;
  }

  // stream output:
  std::ostream& IntegerSet64::write(std::ostream& ost) const {
    size_type count = 0;
    size_type card = this->card();

    ost << "{";
    for (IntegerSet64::const_iterator iter = begin(); iter != end(); ++iter) {
      ost << *iter;
      count++;
      if (count < card) {
	ost << ",";
      }
    }
    ost << "}";
    return ost;
  }


  // IntegerSet64 iterator:

  __is64_const_iterator::__is64_const_iterator(const IntegerSet64& s) :
    _container(&s),
    _current_bit(0) {
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "__is64_const_iterator::__is64_const_iterator(const IntegerSet64&):" << std::endl;
#endif
    block_type b = _container->_bitrep;
#ifndef BIT_LINEAR_SEARCH
    // here we proceed by looking up the first bit in a byte table:
    for (size_type j = 0; j < bytes_per_block; ++j) {
      unsigned char current_byte(static_cast<unsigned char>(b & byte_one));
      if (current_byte) {
	_current_bit   = j * byte_len + IntegerSet64::_S_first_one[current_byte];
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
      if ((b & _bit[j]) != no_bits) {
#else
	if ((b & (bit_one << j)) != no_bits) {
#endif
	  _current_bit = j;
	  return;
#ifdef BITCONSTANTS
	}
#else
      }
#endif
    }
#endif
    _current_bit = block_len;
  }

  __is64_const_iterator& __is64_const_iterator::operator++() {  
#ifdef CONSTRUCTOR_DEBUG
    std::cerr << "__is64_const_iterator::operator++():" << std::endl;
#endif
#ifndef BIT_LINEAR_SEARCH

    // here we proceed by looking up the first bit in a byte table:
    if (++_current_bit < block_len) {  

      // we might find another bit:
      block_type b = _container->_bitrep;
    
      // mask away bits before _current_bit:
      b &= (all_bits << _current_bit);
    
      if (b != no_bits) {
	for (size_type j = 0; j < bytes_per_block; ++j) {
	  unsigned char current_byte(static_cast<unsigned char>(b & byte_one));
	  if (current_byte) {
	    _current_bit   = j * byte_len + IntegerSet64::_S_first_one[current_byte];
	    return *this;
	  }
	  b >>= byte_len;
	}
      }
    }
#else

    // here we proceed by linear search for the first bit:
    for (size_type j = _current_bit + 1; j < block_len; j++) {
#ifdef BITCONSTANTS
      if ((_container->_bitrep & _bit[j]) != no_bits) {
#else
	if ((_container->_bitrep & (bit_one << j)) != no_bits) {
#endif
	  _current_bit = j;
	  return *this;
#ifdef BITCONSTANTS
	}	
#else
      }
#endif
    }
    const block_type& b = _container->_bitrep;
    if (b == no_bits) {
      _current_bit = block_len;
      return *this;
    }
    for (size_type j = 0; j < block_len; j++) {
#ifdef BITCONSTANTS
      if ((_container->_bitrep & _bit[j]) != no_bits) {
#else
	if ((_container->_bitrep & (bit_one << j)) != no_bits) {
#endif
	  _current_bit = j;
	  return *this;
#ifdef BITCONSTANTS
	}
#else
      }
#endif
    }
#endif
    _current_bit = block_len;
    return *this;
  }
      
}; // namespace topcom
 
// eof IntegerSet64.cc
