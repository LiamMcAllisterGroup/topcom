////////////////////////////////////////////////////////////////////////////////
// 
// Global.hh
//
//    produced: 19/03/98 jr
// last change: 20/03/98 jr
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef GLOBAL_HH
#define GLOBAL_HH

#include <mutex>

namespace topcom {

#ifndef VERBOSE_LEVEL
#define VERBOSE_LEVEL 0
#endif
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL   0
#endif

#include <cstdint>

  typedef enum { lexmin_mode, colexmax_mode } comparison_mode_type;
  
  typedef std::uint64_t      block_type;       // type for blocks of a bitstring
  typedef std::size_t        size_type;        // type for sizes of containers like memory size and numbers of objects
  typedef int                parameter_type;   // type for element labels like no or rank, *** must be signed! ***

  // generic computations:
  inline size_type _factorial(const parameter_type n) {
    size_type result(1);
    for (parameter_type i = 2; i < n; ++i) {
      result *= i;
    }
    return result;
  }

  
  // the binomial is usually used to compute a size for a cache:
  inline size_type _binomial(const parameter_type n, const parameter_type k) {
    size_type kk(k);
    if (n - k < k) {
      kk = n - k;
    }
    if (k == 0) {
      return 1;
    }
    if (k == 1) {
      return n;
    }
    if (kk < 1) {
      return 1;
    }
    size_type result(1);
    for (parameter_type i = 0; i < kk; ++i) {
      result *= n - i;
      result /= i + 1;
    }
    return result;
  }

  // a bit-reverse implementation by Knuth from
  // https://matthewarcus.wordpress.com/2012/11/18/reversing-a-64-bit-word/
  // for 64bit bitstrings read on 2020/02/02:
  template <typename T, T m, int k>
  static inline T _swapbits(T p) {
    T q = ((p>>k)^p)&m;
    return p^q^(q<<k);
  }

  inline block_type _kbitreverse (block_type n)
  {
    static const block_type m0 = 0x5555555555555555LLU;
    static const block_type m1 = 0x0300c0303030c303LLU;
    static const block_type m2 = 0x00c0300c03f0003fLLU;
    static const block_type m3 = 0x00000ffc00003fffLLU;
    n = ((n>>1)&m0) | (n&m0)<<1;
    n = _swapbits<block_type, m1, 4>(n);
    n = _swapbits<block_type, m2, 8>(n);
    n = _swapbits<block_type, m3, 20>(n);
    n = (n >> 34) | (n << 30);
    return n;
  }

}; // namespace topcom

#endif

// eof Global.hh
