////////////////////////////////////////////////////////////////////////////////
// 
// CommandlineOptions.cc
//
//    produced: 04 Oct 1999 jr
// last change: 04 Oct 1999 jr
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <thread>
#include <cstdlib>

#include "CommandlineOptions.hh"

namespace topcom {

  bool      CommandlineOptions::_verbose                   = false;
  bool      CommandlineOptions::_debug                     = false;
  bool      CommandlineOptions::_preprocess_points         = false;
  bool      CommandlineOptions::_simpidx_symmetries        = false;
  bool      CommandlineOptions::_full_extension_check      = false;
  bool      CommandlineOptions::_no_extension_check        = false;
  bool      CommandlineOptions::_extension_check_first     = false;
  bool      CommandlineOptions::_memopt                    = false;
  bool      CommandlineOptions::_parallel_enumeration      = false;
  bool      CommandlineOptions::_workbuffercontrol         = false;
  bool      CommandlineOptions::_parallel_symmetries       = false;
  bool      CommandlineOptions::_use_random_order          = false;
  bool      CommandlineOptions::_use_volume_order          = false;
  bool      CommandlineOptions::_use_volumes               = false;
  bool      CommandlineOptions::_use_gkz                   = false;
  bool      CommandlineOptions::_use_switch_tables         = false;
  bool      CommandlineOptions::_use_classified_symmetries = false;
  bool      CommandlineOptions::_use_naive_symmetries      = false;
  bool      CommandlineOptions::_check                     = false;
  bool      CommandlineOptions::_neighborcount             = false;
  bool      CommandlineOptions::_input_chiro               = false;
  bool      CommandlineOptions::_fine_only                 = false;
  bool      CommandlineOptions::_reduce_points             = false;
  bool      CommandlineOptions::_dont_add_points           = false;
  bool      CommandlineOptions::_dont_change_card          = false;
  bool      CommandlineOptions::_output_triangs            = false;
  bool      CommandlineOptions::_output_flips              = false;
  bool      CommandlineOptions::_output_asy                = false;
  bool      CommandlineOptions::_output_stats              = false;
  bool      CommandlineOptions::_compute_all               = false;
  bool      CommandlineOptions::_preprocess_chiro          = false;
  bool      CommandlineOptions::_check_regular             = false;
  bool      CommandlineOptions::_check_unimodular          = false;
  bool      CommandlineOptions::_check_nonregular          = false;
  bool      CommandlineOptions::_check_sometimes           = false;
  bool      CommandlineOptions::_skip_orbitcount           = false;
  bool      CommandlineOptions::_ignore_symmetries         = false;
  bool      CommandlineOptions::_symmetries_are_affine     = false;
  bool      CommandlineOptions::_symmetries_are_isometric  = false;
  bool      CommandlineOptions::_output_heights            = false;
  bool      CommandlineOptions::_use_soplex                = false;
  bool      CommandlineOptions::_use_qsopt_ex              = false;
  bool      CommandlineOptions::_dump_status               = false;
  bool      CommandlineOptions::_read_status               = false;
  size_type CommandlineOptions::_report_frequency          = 10000UL;
  size_type CommandlineOptions::_sometimes_frequency       = 1000000UL;
  size_type CommandlineOptions::_chirocache                = 12582917UL;
  size_type CommandlineOptions::_localcache                = 12582917UL;
  size_type CommandlineOptions::_no_of_simplices           = 0UL;
  size_type CommandlineOptions::_max_no_of_simplices       = 0UL;
  size_type CommandlineOptions::_dump_frequency            = 1UL;
  size_type CommandlineOptions::_min_nodebudget            = 1UL;
  size_type CommandlineOptions::_max_nodebudget            = std::numeric_limits<size_type>::max();
  int       CommandlineOptions::_scale_nodebudget          = 100; // in percent
  int       CommandlineOptions::_no_of_threads             = 1;
  int       CommandlineOptions::_min_workbuffersize        = std::max<int>(std::thread::hardware_concurrency() - 1, 1);
  int       CommandlineOptions::_max_workbuffersize        = 2 * _min_workbuffersize;
  int       CommandlineOptions::_dump_rotations            = 2;
  int       CommandlineOptions::_no_of_symtables           = 1;
  const char*  CommandlineOptions::_input_file             = 0;
  const char*  CommandlineOptions::_dump_file              = "TOPCOM.dump";
  const char*  CommandlineOptions::_read_file              = "TOPCOM.dump.0";
  const char*  CommandlineOptions::_asy_file               = "TOPCOM_graphics.asy";
  const char*  CommandlineOptions::_stats_file             = "TOPCOM_stats.txt";

  // auxiliary information:
  bool      CommandlineOptions::_lp_solver_needed          = false;
  int       CommandlineOptions::_user_no_of_threads        = -1;

  //polymake:Poly CommandlineOptions::_polymakeobj;

  void CommandlineOptions::init(const int argc, const char** argv) {
    std::cerr << "Evaluating Commandline Options ..." << std::endl;

    for (int i = 1; i < argc; ++i) {

      // print usage message:
      if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
	std::cout << "--help or -h: \t help requested:" << std::endl;

	std::cout << "usage:" << std::endl;
	std::cout << argv[0] << "[options]" << std::endl;
	std::cout << "where the input is read from stdin" << std::endl;
	std::cout << "and the output is written to stdout." << std::endl;
	std::cout << std::endl;

	std::cout << "options concerning input/output from files:" << std::endl;
	std::cout << "-i [filename]        : read input from [filename] instead of stdin." << std::endl;
	std::cout << std::endl;

	std::cout << "options concerning output of information:" << std::endl;
	std::cout << "-h or --help         : print this help message" << std::endl;
	std::cout << "-v                   : write verbose information to stderr." << std::endl;
	std::cout << "--reportfrequency [n]: report every [n]th generated object." << std::endl;
	std::cout << "-d                   : write debug information to stderr." << std::endl;
	std::cout << "--heights            : write height vectors for triangulations to stdout (implies --regular)." 
		  << std::endl;
	std::cout << "--flips              : write flips as pairs of triangulation IDs to stdout." 
		  << std::endl;
	std::cout << "--asy                : write asymptote graphics commands into file (in rank-3 triangulations, points are drawn as well)."
		  << std::endl;
	std::cout << "--stats              : write some statistics into file."
		  << std::endl;
	std::cout << std::endl;
     
	std::cout << "options for checking input:" << std::endl;
	std::cout << "--checktriang        : check any given triangulations for correctness." << std::endl;
	std::cout << std::endl;

	std::cout << "options for reporting properties of discovered triangulations:" << std::endl;
	std::cout << "--flipdeficiency     : report flip deficiency in triangulations to stderr." << std::endl;
	std::cout << "--findregular [n]    : check for regularity every [n]th triangulation and exit if regular." << std::endl;
	std::cout << std::endl;

	std::cout << "options concerning which triangulations are output (no influence on exploration):" << std::endl;
	std::cout << "--noorbitcount       : skip the enumaration of orbits (total number of objects is unavailable)" << std::endl;
	std::cout << "--cardinality [n]    : count/output only triangulations of cardinality [n]." << std::endl;
	std::cout << "--maxcardinality [n] : count/output only triangulations of cardinality at most [n]." << std::endl;
	std::cout << "--nonregular         : count/output only non-regular triangulations." << std::endl;
	std::cout << "--unimodular         : count/output only unimodular triangulations (assumes isometric symmetries; correct only if there is a unimodular simplex)." << std::endl;
	std::cout << std::endl;

	std::cout << "options concerning which triangulations are explored:" << std::endl;
	std::cout << "--regular            : consider only regular triangulations" << std::endl;
	std::cout << "--noinsertion        : never use a point that is unused in the seed triangulation." << std::endl;
	std::cout << "--reducepoints       : try to remove the number of use points, reported to stderr." << std::endl;
	std::cout << "--keepcard           : never change the cardinality of triangulations by flipping." << std::endl;
	std::cout << std::endl;

	std::cout << "options concerning symmetries:" << std::endl;
	std::cout << "--affinesymmetries   : assume that given symmetries are affine." << std::endl;
	std::cout << "--isometricsymmetries: assume that given symmetries are isometric." << std::endl;
	std::cout << "--nosymmetries       : ignore symmetries." << std::endl;
	std::cout << std::endl;

	std::cout << "options for dumping and reading the status of an interrupted computation (only for flip graph exploration):" << std::endl;
	std::cout << "--dump               : dump status of previous computation (last completed BFS round) into file" << std::endl;
	std::cout << "--dumpfile [filename]: set dump file name to [filename] (default: TOPCOM.dump)" << std::endl;
	std::cout << "--dumpfrequency [n]  : dump each [n]th completed BFS round (default: 1)" << std::endl;
	std::cout << "--dumprotations [n]  : dump into [n] rotating files" << std::endl;
	std::cout << "--read               : read status of previous computation (last completed BFS round) from file" << std::endl;
	std::cout << "--readfile [filename]: set dump file name to read to [filename] (default: TOPCOM.dump)" << std::endl;
	std::cout << std::endl;
      

	std::cout << "options concerning internal behaviour:" << std::endl;
	std::cout << "--memopt             : try to reduce memory consumption." << std::endl;
	std::cout << "--usegkz             : use GKZ vectors as a finger print in symmetry handling (only for points with isometric symmetries)." << std::endl;
	std::cout << "--usenaivesymmetries : use naive full scan of all symmetries for symmetry handling." << std::endl;
	std::cout << "--useswitchtables    : use Jordan-Joswig-Kastner switch tables for symmetry handling." << std::endl;
	std::cout << "--usesymmetrytables  : use tables of classified symmetries for symmetry handling." << std::endl;
	std::cout << "--symtables [n]      : use [n] symtables for preprocessing symmetries." << std::endl;
	std::cout << "--preprocesschiro    : preprocess the chirotope (default for points2[n]alltriangs)." << std::endl;
	std::cout << "--preprocesspoints   : heuristically transform points (only relevant for (co)circuit enumeration)." << std::endl;
	std::cout << "--simpidxsymmetries  : preprocess a representation of the symmetry group on simplex indices (only relevant for triangulation enumeration)." << std::endl;
	std::cout << "--userandomorder     : sort simplices in preprocessed index table randomly (only for points with isometric symmetries)." << std::endl;
	std::cout << "--usevolumeorder     : sort simplices in preprocessed index table by volume (only for points with isometric symmetries)." << std::endl;
	std::cout << "--usevolumes         : use volumes to check extendability of partial triangulations (only for points with isometric symmetries)." << std::endl;
	std::cout << "--fullextensioncheck : put more effort in the check of extendability of a partial triangulation." << std::endl;
	std::cout << "--noextensioncheck   : skip the check of extendability of a partial triangulation." << std::endl;
	std::cout << "--extensioncheckfirst: check extendability prior to symmetry." << std::endl;
	std::cout << "--chirocache [n]     : set the chirotope cache to [n] elements." << std::endl;
	std::cout << "--localcache [n]     : set the cache for local data to [n] elements ." << std::endl;
	std::cout << "--qsopt_ex           : use QSopt_ex for regularity checks." << std::endl;
	std::cout << "--soplex             : use soplex for regularity checks." << std::endl;
	std::cout << std::endl;

	std::cout << "options concerning multi-threading:" << std::endl;
	std::cout << "--parallelenumeration: use multiple threads for enumeration." << std::endl;
	std::cout << "--workbuffercontrol  : control interrupt of workers by size of the current workbuffer." << std::endl;
	std::cout << "--parallelsymmetries : use multiple threads for symmetry checks." << std::endl;
	std::cout << "--threads [n]        : use [n] threads (if possible)." << std::endl;      
	std::cout << "--minnodebudget [n]  : let each thread process at least [n] nodes (to avoid multi-threading overhead)." << std::endl;      
	std::cout << "--maxnodebudget [n]  : let each thread process at most [n] nodes (to avoid thread starving)." << std::endl;      
	std::cout << "--scalenodebudget [n]: scale the default node budget by [n] percent (n integer)" << std::endl;      
	std::cout << "--minworkbuffer [n]  : (unused) try to keep the work buffer above [n] work packages (to balance overhead and thread starving)." << std::endl;      
	std::cout << "--maxworkbuffer [n]  : (unused) try to keep the work buffer below [n] work packages (to balance overhead and thread starving)." << std::endl;      
	std::cout << std::endl;
	exit(0);
      }

      // output options:
      else if (strcmp(argv[i], "-i") == 0) {
	++i;
	if (argc >i) {
	  _input_file = argv[i];
	}
	FILE* f = freopen(_input_file, "r", stdin);

	std::cerr << " -i                  : input read from " << _input_file << std::endl;
      }
      else if (strcmp(argv[i], "-v") == 0) {
	_verbose = true;

	std::cerr << " -v                  : verbose output activated" << std::endl;
      }
      else if (strcmp(argv[i], "--reportfrequency") == 0) {
	++i;
	if (argc > i) {
	  _report_frequency = (size_type)atol(argv[i]);
	  _verbose = true;

	  std::cerr << "--reportfrequency    : report every " << _report_frequency << "th new found object (implies '-v' = verbose)" << std::endl;
	}
      }
      else if (strcmp(argv[i], "-d") == 0) {
	std::cerr << " -d                  : debug output activated" << std::endl;

	_debug = true;
      }
      else if (strcmp(argv[i], "--heights") == 0) {
	std::cerr << "--heights            : output of defining heights activated" << std::endl;

	_output_heights = true;
	_check_sometimes = false;
	_check_regular = true;
	_lp_solver_needed = true;
      }

      else if (strcmp(argv[i], "--flips") == 0) {
	std::cerr << "--flips              : output of flips activated" << std::endl;

	_output_flips = true;
      }
      else if (strcmp(argv[i], "--asy") == 0) {
	_output_asy = true;

	std::cerr << "--asy                : write graphics output (in rank-3 triangulations, points are drawn as well)" << std::endl;
      }
      else if (strcmp(argv[i], "--asyfile") == 0) {
	++i;
	if (argc > i) {
	  _asy_file = argv[i];
	  _output_asy = true;

	  std::cerr << "--asyfile            : graphics output to file " << _asy_file << std::endl;
	}
      }
      else if (strcmp(argv[i], "--stats") == 0) {
	_output_stats = true;

	std::cerr << "--stats              : write statistics output" << std::endl;
      }
      else if (strcmp(argv[i], "--statsfile") == 0) {
	++i;
	if (argc > i) {
	  _stats_file = argv[i];
	  _output_stats = true;

	  std::cerr << "--statsfile          : statistics output to file " << _stats_file << std::endl;
	}
      }

      // options for checking input:
      else if (strcmp(argv[i], "--checktriang") == 0) {
	std::cerr << "--checktriangs       : check seed triangulation activated" << std::endl;

	_check = true;
      }

      // options for reporting properties of discovered triangulations:
      else if (strcmp(argv[i], "--flipdeficiency") == 0) {
	std::cerr << "--flipdeficiency     : search for flip deficiency activated" << std::endl;

	_neighborcount = true;
      }
      else if (strcmp(argv[i], "--findregular") == 0) {
	if (_check_regular) {
	  std::cerr << "--findregular cannot be used with --regular, ignoring --findregular." << std::endl;
	}
	else {
	  _check_regular = false;
	  _check_sometimes = true;
	  _lp_solver_needed = true;
	  ++i;
	  if (argc > i) {
	    _sometimes_frequency = (size_type)atol(argv[i]);
	  }

	  std::cerr << "--findregular      : check regularity every " << _sometimes_frequency << "th triangulation and exit if regular activated" << std::endl;
	}
      }

      // options concerning which triangulations are output (no influence on exploration):
      else if (strcmp(argv[i], "--cardinality") == 0) {
	++i;
	if (argc > i) {
	  _no_of_simplices = (size_type)atol(argv[i]);
	}

	std::cerr << "--cardinality        : restrict to triangulations with " << _no_of_simplices << " simplices" << std::endl;
      }

      else if (strcmp(argv[i], "--maxcardinality") == 0) {
	++i;
	if (argc > i) {
	  _max_no_of_simplices = (size_type)atol(argv[i]);
	}

	std::cerr << "--maxcardinality     : restrict to triangulations with at most " << _max_no_of_simplices << " simplices" << std::endl;
      }

      else if (strcmp(argv[i], "--nonregular") == 0) {
	std::cerr << "--nonregular         : search for non-regular triangulations activated" << std::endl;

	_check_nonregular = true;
	_lp_solver_needed = true;
      }

      else if (strcmp(argv[i], "--unimodular") == 0) {
	std::cerr << "--unimodular         : restrict to unimodular triangulations (assumes isometric symmetries; correct only if there is a unimodular simplex)" << std::endl;

	_check_unimodular = true;
	_symmetries_are_isometric = true;
      }

      // options concerning which triangulations are explored:
      else if (strcmp(argv[i], "--regular") == 0) {
	if (_check_sometimes) {
	  std::cerr << "--regular cannot be used with --frequency, ignoring --regular." << std::endl;
	}
	else {
	  _check_sometimes = false;
	  _check_regular = true;
	  _lp_solver_needed = true;

	  std::cerr << "--regular            : check for regular triangulations activated" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--reducepoints") == 0) {
	_reduce_points = true;

	std::cerr << "--reducepoints       : reduce points heuristics activated" << std::endl;
      }
      else if (strcmp(argv[i], "--noinsertion") == 0) {
	_dont_add_points = true;

	std::cerr << "--noinsertion        : never add points activated" << std::endl;
      }
      else if (strcmp(argv[i], "--keepcard") == 0) {
	_dont_change_card = true;

	std::cerr << "--keepcard           : never change cardinality of triangulations by flipping activated" << std::endl;
      }

      // options concerning symmetries:
      else if (strcmp(argv[i], "--affinesymmetries") == 0) {
	_symmetries_are_affine = true;

	std::cerr << "--affinesymmetries   : assumption that symmetries are affine activated" << std::endl;
      }
      else if (strcmp(argv[i], "--isometricsymmetries") == 0) {
	_symmetries_are_affine = true;
	_symmetries_are_isometric = true;

	std::cerr << "--isometricsymmetries: assumption that symmetries are isometric activated" << std::endl;
      }
      else if (strcmp(argv[i], "--noorbitcount") == 0) {
	_skip_orbitcount = true;

	std::cerr << "--noorbitcount       : skipping traversal of orbits activated" << std::endl;
      }
      else if (strcmp(argv[i], "--nosymmetries") == 0) {
	_ignore_symmetries = true;

	std::cerr << "--nosymmetries       : ignoring symmetries activated" << std::endl;
      }

      // options concerning internals:
      else if (strcmp(argv[i], "--preprocesspoints") == 0) {
	_preprocess_points = true;

	std::cerr << "--preprocesspoints   : heuristics preprocessing of point matrix activated" << std::endl;
      }
      else if (strcmp(argv[i], "--preprocesschiro") == 0) {
	_preprocess_chiro = true;

	std::cerr << "--preprocesschiro    : preprocessing of chirotope activated" << std::endl;
      }
      else if (strcmp(argv[i], "--simpidxsymmetries") == 0) {
	_simpidx_symmetries = true;

	std::cerr << "--simpidxsymmetries  : preprocessing of a representation of the symmetry group on simplex indices activated" << std::endl;
      }
      else if (strcmp(argv[i], "--userandomorder") == 0) {
	_use_random_order = true;

	std::cout << "--userandomorder     : sort simplices in preprocessed index table randomly" << std::endl;
      }
      else if (strcmp(argv[i], "--usevolumeorder") == 0) {
	_use_volume_order = true;

	std::cout << "--usevolumeorder     : sort simplices in preprocessed index table by volume" << std::endl;
      }
      else if (strcmp(argv[i], "--usevolumes") == 0) {
	_use_volumes = true;

	std::cout << "--usevolumes         : use volumes to check extendability of partial triangulations" << std::endl;
      }
      else if (strcmp(argv[i], "--fullextensioncheck") == 0) {
	_full_extension_check = true;

	std::cerr << "--fullextensioncheck : full extension check for partial triangulations activated" << std::endl;
      }
      else if (strcmp(argv[i], "--noextensioncheck") == 0) {
	_no_extension_check = true;

	std::cerr << "--noextensioncheck   : skip extension check for partial triangulations activated" << std::endl;
      }
      else if (strcmp(argv[i], "--extensioncheckfirst") == 0) {
	_extension_check_first = true;

	std::cerr << "--extensioncheckfirst: extension check prior to symmetry check activated" << std::endl;
      }

      else if (strcmp(argv[i], "--memopt") == 0) {
	_memopt = true;

	std::cerr << "--memopt             : higher memory efficiency activated" << std::endl;
      }
      else if (strcmp(argv[i], "--parallelenumeration") == 0) {
	_parallel_enumeration = true;
	_no_of_threads = std::max<int>(std::thread::hardware_concurrency() - 1, 1);

	std::cerr << "--parallelenumeration: multi-threading for enumeration activated" << std::endl;
      }
      else if (strcmp(argv[i], "--workbuffercontrol") == 0) {
	_workbuffercontrol = true;

	std::cerr << "--workbuffercontrol  : control of worker interrupt by work buffer activated" << std::endl;
      }
      else if (strcmp(argv[i], "--parallelsymmetries") == 0) {
	if (!_ignore_symmetries) {
	  _parallel_symmetries = true;
	  _no_of_threads = std::max<int>(std::thread::hardware_concurrency() - 1, 1);

	  std::cerr << "--parallelsymmetries : multi-threading for symmetry checks activated" << std::endl;
	}
	else {
	  std::cerr << "--parallelsymmetries : multi-threading for symmetry checks ingnored because of --nosymmetries" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--threads") == 0) {
	++i;
	if (argc > i) {
	  _user_no_of_threads = std::max<int>(1, (size_type)atol(argv[i]));

	  std::cerr << "--threads            : no of threads set to " << _user_no_of_threads << " threads" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--minnodebudget") == 0) {
	++i;
	if (argc > i) {
	  _min_nodebudget = std::max<int>(1, (size_type)atol(argv[i]));

	  std::cerr << "--minnodebudget      : min node budget set to " << _min_nodebudget << " nodes" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--maxnodebudget") == 0) {
	++i;
	if (argc > i) {
	  _max_nodebudget = std::max<int>(1, (size_type)atol(argv[i]));

	  std::cerr << "--maxnodebudget      : max node budget set to " << _max_nodebudget << " nodes" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--scalenodebudget") == 0) {
	++i;
	if (argc > i) {
	  _scale_nodebudget = std::max<int>(1, (size_type)atol(argv[i]));

	  std::cerr << "--scalenodebudget    : scaling factor of node budget set to " << _scale_nodebudget << " percent" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--minworkbuffer") == 0) {
	++i;
	if (argc > i) {
	  _min_workbuffersize = std::max<int>(1, (size_type)atol(argv[i]));

	  std::cerr << "--minworkbuffer      : min work buffer size set to " << _min_workbuffersize << " work packages" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--maxworkbuffer") == 0) {
	++i;
	if (argc > i) {
	  _max_workbuffersize = std::max<int>(1, (size_type)atol(argv[i]));

	  std::cerr << "--maxworkbuffer      : max work buffer size set to " << _max_workbuffersize << " work packages" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--usegkz") == 0) {
	_use_gkz = true;

	std::cerr << "--usegkz             : use GKZ vectors as a finger print in symmetry handling (only for points with isometric symmetries)." << std::endl;
      }
      else if (strcmp(argv[i], "--usenaivesymmetries") == 0) {
	_use_naive_symmetries = true;

	std::cerr << "--usenaivesymmeries  : use of naive full scan of all symmetries for symmetry handling activated" << std::endl;
      }
      else if (strcmp(argv[i], "--useswitchtables") == 0) {
	_use_switch_tables = true;

	std::cerr << "--useswitchtables    : use of Jordan-Joswig-Kastner switch tables activated" << std::endl;
      }
      else if (strcmp(argv[i], "--usesymmetrytables") == 0) {
	_use_switch_tables = false;
	_use_classified_symmetries = true;

	std::cerr << "--usesymmetrytables  : use of tables of classified symmetries for symmetry handling activated" << std::endl;
      }
      else if (strcmp(argv[i], "--symtables") == 0) {
	++i;
	if (argc > i) {
	  _no_of_symtables = std::max<int>(1, (size_type)atol(argv[i]));

	  std::cerr << "--symtables          : no of symmetry tables set to " << _no_of_symtables << " entries" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--chirocache") == 0) {
	++i;
	if (argc > i) {
	  _chirocache = (size_type)atol(argv[i]);

	  std::cerr << "--chirocache         : cache for lazy chirotope set to " << _chirocache << " elements" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--localcache") == 0) {
	++i;
	if (argc > i) {
	  _localcache = (size_type)atol(argv[i]);

	  std::cerr << "--localcache         : cache for local data set to " << _localcache << " elements" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--dumpfrequency") == 0) {
	++i;
	if (argc > i) {
	  _dump_frequency = (size_type)atol(argv[i]);
	  _dump_status = true;

	  std::cerr << "--dumpfrequency      : dump every " << _dump_frequency << "th processed triangulation" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--dumprotations") == 0) {
	++i;
	if (argc > i) {
	  _dump_rotations = (size_type)atol(argv[i]);
	  _dump_status    = true;

	  std::cerr << "--dumprotations      : rotate dump into " << _dump_rotations << " many dump files" << std::endl;
	}
      }
      else if (strcmp(argv[i], "--dumpfile") == 0) {
	++i;
	if (argc > i) {
	  _dump_file = argv[i];
	  _dump_status = true;

	  std::cerr << "--dumpfile           : dump to file " << _dump_file << std::endl;
	}
      }
      else if (strcmp(argv[i], "--dump") == 0) {
	_dump_status = true;

	std::cerr << "--dump               : dump status of computation into file activated" << std::endl;
      }
      else if (strcmp(argv[i], "--readfile") == 0) {
	++i;
	if (argc > i) {
	  _read_file = argv[i];
	  _read_status = true;

	  std::cerr << "--readfile           : read from file " << _dump_file << std::endl;
	}
      }
      else if (strcmp(argv[i], "--read") == 0) {
	_read_status = true;

	std::cerr << "--read               : read status from file" << std::endl;
      }
      else if (strcmp(argv[i], "--soplex") == 0) {
#ifdef HAVE_LIBSOPLEX      
	_use_soplex = true;

	std::cerr << "--soplex             : soplex activated" << std::endl;
#else
	std::cerr << "--soplex             : ignored (soplex support was not configured for this binary)" << std::endl;
#endif
      }
      else if (strcmp(argv[i], "--qsoptex") == 0) {
	_use_qsopt_ex = true;

	std::cerr << "--qsoptex            : QSopt_ex activated" << std::endl;
      }

      // some options that are not documented:
      else if (strcmp(argv[i], "-A") == 0) {
	_compute_all = true;
      }
      else if (strcmp(argv[i], "-C") == 0) {
	_input_chiro = true;
      }
      else if (strcmp(argv[i], "-F") == 0) {
	_fine_only = true;
      }
      else if (strcmp(argv[i], "-O") == 0) {
	_output_triangs = true;
      }
      else if (strcmp(argv[i], "-E") == 0) {
	_output_flips = true;
      }
      else {
	std::cout << "unknown option " << argv[i] << " - exiting" << std::endl;
	exit(1);
      }
    }

    // collect information that should not depend on order of options:
    if (_user_no_of_threads > 0) {
      _no_of_threads = _user_no_of_threads;
    }
    _min_workbuffersize = _no_of_threads;

    if ((_no_of_simplices > 0) && (_max_no_of_simplices > 0)) {
      _max_no_of_simplices = 0;
      std::cerr << "--maxcardinality [n] : ignored because of --cardinality [n]" << std::endl;
    }
    
    if (_parallel_symmetries && _use_switch_tables) {

      // switch tables do not support parallel symmetry processing:
      _parallel_symmetries = false;
      std::cerr << "--parallelsymmetries : ignored because of --useswitchtables" << std::endl;
    }

    if (_parallel_symmetries && _ignore_symmetries) {

      // if symmetries are ignored, there is no point in parallel processing symmetries:
      _parallel_symmetries = false;
      std::cerr << "--parallelsymmetries : ignored because of --nosymmetries" << std::endl;
    }

    if (_workbuffercontrol && !_parallel_enumeration) {
      
      // there is no point in workbuffercontrol if we do not use parallel enumeration:
      std::cerr << "--workbuffercontrol  : ignored because of no --parallelenumeration" << std::endl;
      _workbuffercontrol = false;
    }

    if (_use_gkz && !_symmetries_are_isometric) {
      
      // gkz is not invariant w.r.t. non-isometric symmetries:
      std::cerr << "--usegkz             : ignored because of no --isometricsymmetries" << std::endl;
      _use_gkz = false;
    }
    
    if (_use_volumes && !_symmetries_are_isometric) {
      // volume is not invariant w.r.t. non-isometric symmetries:
      std::cerr << "--usevolumes         : ignored because of no --isometricsymmetries" << std::endl;
      _use_volumes = false;
    }

    if (_use_volume_order && !_symmetries_are_isometric) {
      // volume is not invariant w.r.t. non-isometric symmetries:
      std::cerr << "--usevolumeorder     : ignored because of no --isometricsymmetries" << std::endl;
      _use_volume_order = false;
    }

    if (_simpidx_symmetries && _memopt) {
      std::cerr << "--simpidxsymmetries  : ignored because of --memopt" << std::endl;
      _simpidx_symmetries = false;
    }
    if (_use_switch_tables) {
      if ((strstr(argv[0], "check") == 0)
	  &&
	  (strstr(argv[0], "points2nalltriangs") == 0)
	  &&
	  (strstr(argv[0], "points2alltriangs") == 0)
	  &&
	  (strstr(argv[0], "points2mintriang") == 0)
	  &&
	  (strstr(argv[0], "points2ncircuits") == 0)
	  &&
	  (strstr(argv[0], "points2ncocircuits") == 0)
	  &&
	  (strstr(argv[0], "points2circuits") == 0)
	  &&
	  (strstr(argv[0], "points2cocircuits") == 0)) {
	std::cerr << "--useswitchtables    : ignored because unsupported by " << argv[0] << std::endl;
	_use_switch_tables = false;
      }
      if (!_simpidx_symmetries) {
	if ((strstr(argv[0], "check") == 0)
	    &&
	    (strstr(argv[0], "points2ncircuits") == 0)
	    &&
	    (strstr(argv[0], "points2ncocircuits") == 0)
	    &&
	    (strstr(argv[0], "points2circuits") == 0)
	    &&
	    (strstr(argv[0], "points2cocircuits") == 0)) {
	  std::cerr << "--useswitchtables    : ignored because missing --simpidxsymmetries for " << argv[0] << std::endl;
	  _use_switch_tables = false;
	}
      }
    }
    
    if (_output_asy) {
      if ((strstr(argv[0], "points2nalltriangs") == 0)
	  &&
	  (strstr(argv[0], "points2alltriangs") == 0)
	  &&
	  (strstr(argv[0], "points2mintriang") == 0)
	  &&
	  (strstr(argv[0], "points2ncircuits") == 0)
	  &&
	  (strstr(argv[0], "points2ncocircuits") == 0)
	  &&
	  (strstr(argv[0], "points2circuits") == 0)
	  &&
	  (strstr(argv[0], "points2cocircuits") == 0)) {
	std::cerr << "--asy                : ignored because unsupported by " << argv[0] << std::endl;
	_output_asy = false;
      }
    }

    if (_use_qsopt_ex && _parallel_enumeration) {
      std::cerr << "--parallelenumeration: ignored because qsopt_ex is not thread-safe" << std::endl;
      _parallel_enumeration = false;
    }
    
    std::cerr << "... done." << std::endl;
  }

}; // namespace topcom

// eof CommandlineOptions.cc
