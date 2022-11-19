////////////////////////////////////////////////////////////////////////////////
// 
// Statistics.hh 
//
//    produced: 17 Sep 2021 jr
// last change: 17 Sep 2021 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef STATISTICS_HH
#define STATISTICS_HH

#include <stdlib.h>
#include <cstring>
#include <fstream>

#include "Global.hh"
#include "CommandlineOptions.hh"

namespace topcom {

  // statistical data is collected in a purely static class (i.e., a global variable):
  class Statistics {
  private:
    static std::fstream _stats_stream;
    static size_type    _no_of_map_calls;
  public:
    Statistics() {}
    Statistics(const Statistics&) {}
  public:
    // initialization:
    static void init();
    
    // termination:
    static void term();

    // accessors:
    inline static size_type no_of_map_calls()           { return _no_of_map_calls; }
    inline static std::fstream& stats_stream()          { return _stats_stream; }

    // modifiers:
    inline static void new_map_call()                   { ++_no_of_map_calls; }
  };

}; // namespace topcom

#endif

// eof Statistics.hh
