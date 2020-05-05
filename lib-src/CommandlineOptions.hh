////////////////////////////////////////////////////////////////////////////////
// 
// CommandlineOptions.hh 
//
//    produced: 04 Oct 1999 jr
// last change: 04 Oct 1999 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef COMMANDLINEOPTIONS_HH
#define COMMANDLINEOPTIONS_HH

#include <stdlib.h>
#include <cstring>

#include "Global.hh"
//#include "Poly.h"

class CommandlineOptions {
private:
  static bool      _verbose;
  static bool      _debug;
  static bool      _memopt;
  static bool      _check;
  static bool      _simple;
  static bool      _neighborcount;
  static bool      _input_chiro;
  static bool      _fine_only;
  static bool      _reduce_points;
  static bool      _dont_add_points;
  static bool      _dont_change_card;
  static bool      _output_triangs;
  static bool      _output_flips;
  static bool      _compute_all;
  static bool      _preprocess;
  static bool      _check_regular;
  static bool      _check_nonregular;
  static bool      _check_sometimes;
  static bool      _ignore_symmetries;
  static bool      _symmetries_are_affine;
  static bool      _output_heights;
  static bool      _use_soplex;
  static bool      _dump_status;
  static bool      _read_status;
  static size_type _report_frequency;
  static size_type _sometimes_frequency;
  static size_type _chirocache;
  static size_type _localcache;
  static size_type _no_of_simplices;
  static size_type _dump_frequency;
  static int       _dump_rotations;
  static const char* _dump_file;
  static const char* _read_file;
  // Polymake object:
  // static polymake::Poly _polymakeobj;
public:
  CommandlineOptions() {}
  CommandlineOptions(const CommandlineOptions&) {}
public:
  // initialization:
  static void init(const int argc, const char** argv);
  // accessors:
  inline static bool      verbose()                 { return _verbose; }
  inline static bool      debug()                   { return _debug; }
  inline static bool      memopt()                  { return _memopt; }
  inline static bool      check()                   { return _check; }
  inline static bool      simple()                  { return _simple; }
  inline static bool      neighborcount()           { return _neighborcount; }
  inline static bool      input_chiro()             { return _input_chiro; }
  inline static bool      fine_only()               { return _fine_only; }
  inline static bool      reduce_points()           { return _reduce_points; }
  inline static bool      dont_add_points()         { return _dont_add_points; }
  inline static bool      dont_change_card()        { return _dont_change_card; }
  inline static bool      compute_all()             { return _compute_all; }
  inline static bool      preprocess()              { return _preprocess; }
  inline static bool      output_triangs()          { return _output_triangs; }
  inline static bool      output_flips()            { return _output_flips; }
  inline static bool      check_regular()           { return _check_regular; }
  inline static bool      check_nonregular()        { return _check_nonregular; }
  inline static bool      check_sometimes()         { return _check_sometimes; }
  inline static bool      ignore_symmetries()       { return _ignore_symmetries; }
  inline static bool      symmetries_are_affine()   { return _symmetries_are_affine; }
  inline static bool      output_heights()          { return _output_heights; }
  inline static bool      use_soplex()              { return _use_soplex; }
  inline static bool      dump_status()             { return _dump_status; }
  inline static bool      read_status()             { return _read_status; }
  inline static size_type report_frequency()        { return _report_frequency; }
  inline static size_type sometimes_frequency()     { return _sometimes_frequency; }
  inline static size_type chirocache()              { return _chirocache; }
  inline static size_type localcache()              { return _localcache; }
  inline static size_type no_of_simplices()         { return _no_of_simplices; }
  inline static size_type dump_frequency()          { return _dump_frequency; }
  inline static int       dump_rotations()          { return _dump_rotations; }

  inline static const char* dump_file()               { return _dump_file; }
  inline static const char* read_file()               { return _read_file; }

  // Polymake object:
  // inline static polymake::Poly& polymakeobj { return _polymakeobj; }
};

#endif

// eof CommandlineOptions.hh
