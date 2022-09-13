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
#include <fstream>

#include "Global.hh"

namespace topcom {

  class CommandlineOptions {
  private:
    static bool      _verbose;
    static bool      _debug;
    static bool      _parallel_enumeration;
    static bool      _workbuffercontrol;
    static bool      _parallel_symmetries;
    static bool      _use_random_order;
    static bool      _use_volume_order;
    static bool      _use_volumes;
    static bool      _use_gkz;
    static bool      _use_switch_tables;
    static bool      _use_classified_symmetries;
    static bool      _use_naive_symmetries;
    static bool      _preprocess_points;
    static bool      _simpidx_symmetries;
    static bool      _full_extension_check;
    static bool      _no_extension_check;
    static bool      _extension_check_first;
    static bool      _memopt;
    static bool      _check;
    static bool      _neighborcount;
    static bool      _input_chiro;
    static bool      _fine_only;
    static bool      _reduce_points;
    static bool      _dont_add_points;
    static bool      _dont_change_card;
    static bool      _output_triangs;
    static bool      _output_flips;
    static bool      _output_asy;
    static bool      _output_stats;
    static bool      _compute_all;
    static bool      _preprocess_chiro;
    static bool      _check_regular;
    static bool      _check_unimodular;
    static bool      _check_nonregular;
    static bool      _check_sometimes;
    static bool      _ignore_symmetries;
    static bool      _skip_orbitcount;
    static bool      _symmetries_are_affine;
    static bool      _symmetries_are_isometric;
    static bool      _output_heights;
    static bool      _use_qsopt_ex;
    static bool      _use_soplex;
    static bool      _dump_status;
    static bool      _read_status;
    static size_type _report_frequency;
    static size_type _sometimes_frequency;
    static size_type _chirocache;
    static size_type _localcache;
    static size_type _no_of_simplices;
    static size_type _max_no_of_simplices;
    static size_type _dump_frequency;
    static size_type _min_nodebudget;
    static size_type _max_nodebudget;
    static int       _scale_nodebudget;
    static int       _no_of_threads;
    static int       _min_workbuffersize;
    static int       _max_workbuffersize;
    static int       _dump_rotations;
    static int       _no_of_symtables;
    static const char*  _input_file;
    static const char*  _dump_file;
    static const char*  _read_file;
    static const char*  _asy_file;
    static const char*  _stats_file;
    // auxiliary variables:
    static bool      _lp_solver_needed;
    static int       _user_no_of_threads;
  public:
    CommandlineOptions() {}
    CommandlineOptions(const CommandlineOptions&) {}
  public:
    // initialization:
    static void init(const int argc, const char** argv);
    // accessors:
    inline static bool      verbose()                   { return _verbose; }
    inline static bool      debug()                     { return _debug; }
    inline static bool      parallel_enumeration()      { return _parallel_enumeration; }
    inline static bool      workbuffercontrol()         { return _workbuffercontrol; }
    inline static bool      parallel_symmetries()       { return _parallel_symmetries; }
    inline static bool      use_random_order()          { return _use_random_order; }
    inline static bool      use_volume_order()          { return _use_volume_order; }
    inline static bool      use_volumes()               { return _use_volumes; }
    inline static bool      use_gkz()                   { return _use_gkz; }
    inline static bool      use_switch_tables()         { return _use_switch_tables; }
    inline static bool      use_classified_symmetries() { return _use_classified_symmetries; }
    inline static bool      use_naive_symmetries()      { return _use_naive_symmetries; }
    inline static bool      preprocess_points()         { return _preprocess_points; }
    inline static bool      simpidx_symmetries()        { return _simpidx_symmetries; }
    inline static bool      full_extension_check()      { return _full_extension_check; }
    inline static bool      no_extension_check()        { return _no_extension_check; }
    inline static bool      extension_check_first()     { return _extension_check_first; }
    inline static bool      memopt()                    { return _memopt; }
    inline static bool      check()                     { return _check; }
    inline static bool      neighborcount()             { return _neighborcount; }
    inline static bool      input_chiro()               { return _input_chiro; }
    inline static bool      fine_only()                 { return _fine_only; }
    inline static bool      reduce_points()             { return _reduce_points; }
    inline static bool      dont_add_points()           { return _dont_add_points; }
    inline static bool      dont_change_card()          { return _dont_change_card; }
    inline static bool      compute_all()               { return _compute_all; }
    inline static bool      preprocess_chiro()          { return _preprocess_chiro; }
    inline static bool      output_triangs()            { return _output_triangs; }
    inline static bool      output_flips()              { return _output_flips; }
    inline static bool      output_asy()                { return _output_asy; }
    inline static bool      output_stats()              { return _output_stats; }
    inline static bool      check_regular()             { return _check_regular; }
    inline static bool      check_unimodular()          { return _check_unimodular; }
    inline static bool      check_nonregular()          { return _check_nonregular; }
    inline static bool      check_sometimes()           { return _check_sometimes; }
    inline static bool      skip_orbitcount()           { return _skip_orbitcount; }
    inline static bool      ignore_symmetries()         { return _ignore_symmetries; }
    inline static bool      symmetries_are_affine()     { return _symmetries_are_affine; }
    inline static bool      symmetries_are_isometric()  { return _symmetries_are_isometric; }
    inline static bool      output_heights()            { return _output_heights; }
    inline static bool      use_qsopt_ex()              { return _use_qsopt_ex; }
    inline static bool      use_soplex()                { return _use_soplex; }
    inline static bool      dump_status()               { return _dump_status; }
    inline static bool      read_status()               { return _read_status; }
    inline static size_type report_frequency()          { return _report_frequency; }
    inline static size_type sometimes_frequency()       { return _sometimes_frequency; }
    inline static size_type chirocache()                { return _chirocache; }
    inline static size_type localcache()                { return _localcache; }
    inline static size_type no_of_simplices()           { return _no_of_simplices; }
    inline static size_type max_no_of_simplices()       { return _max_no_of_simplices; }
    inline static size_type dump_frequency()            { return _dump_frequency; }
    inline static size_type min_nodebudget()            { return _min_nodebudget; }
    inline static size_type max_nodebudget()            { return _max_nodebudget; }
    inline static int       scale_nodebudget()          { return _scale_nodebudget; }
    inline static int       no_of_threads()             { return _no_of_threads; }
    inline static int       min_workbuffersize()        { return _min_workbuffersize; }
    inline static int       max_workbuffersize()        { return _max_workbuffersize; }
    inline static int       dump_rotations()            { return _dump_rotations; }
    inline static int       no_of_symtables()           { return _no_of_symtables; }

    inline static const char*   input_file()            { return _input_file; }
    inline static const char*   dump_file()             { return _dump_file; }
    inline static const char*   read_file()             { return _read_file; }
    inline static const char*   asy_file()              { return _asy_file; }
    inline static const char*   stats_file()            { return _stats_file; }

    // auxiliary queries:
    inline static int       lp_solver_needed()          { return _lp_solver_needed; }
    inline static int       user_no_of_threads()        { return _user_no_of_threads; }

  };

}; // namespace topcom

#endif

// eof CommandlineOptions.hh
