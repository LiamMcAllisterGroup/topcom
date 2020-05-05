////////////////////////////////////////////////////////////////////////////////
// 
// CommandlineOptions.cc
//
//    produced: 04 Oct 1999 jr
// last change: 04 Oct 1999 jr
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "CommandlineOptions.hh"

bool      CommandlineOptions::_verbose                 = false;
bool      CommandlineOptions::_debug                   = false;
bool      CommandlineOptions::_memopt                  = false;
bool      CommandlineOptions::_check                   = false;
bool      CommandlineOptions::_simple                  = false;
bool      CommandlineOptions::_neighborcount           = false;
bool      CommandlineOptions::_input_chiro             = false;
bool      CommandlineOptions::_fine_only               = false;
bool      CommandlineOptions::_reduce_points           = false;
bool      CommandlineOptions::_dont_add_points         = false;
bool      CommandlineOptions::_dont_change_card        = false;
bool      CommandlineOptions::_output_triangs          = false;
bool      CommandlineOptions::_output_flips            = false;
bool      CommandlineOptions::_compute_all             = false;
bool      CommandlineOptions::_preprocess              = false;
bool      CommandlineOptions::_check_regular           = false;
bool      CommandlineOptions::_check_nonregular        = false;
bool      CommandlineOptions::_check_sometimes         = false;
bool      CommandlineOptions::_ignore_symmetries       = false;
bool      CommandlineOptions::_symmetries_are_affine   = false;
bool      CommandlineOptions::_output_heights          = false;
bool      CommandlineOptions::_use_soplex              = false;
bool      CommandlineOptions::_dump_status             = false;
bool      CommandlineOptions::_read_status             = false;
size_type CommandlineOptions::_report_frequency        = 1000UL;
size_type CommandlineOptions::_sometimes_frequency     = 10000UL;
size_type CommandlineOptions::_chirocache              = 1000000UL;
size_type CommandlineOptions::_localcache              = 1000000UL;
size_type CommandlineOptions::_no_of_simplices         = 0L;
size_type CommandlineOptions::_dump_frequency          = 1L;
int       CommandlineOptions::_dump_rotations          = 2;
const char* CommandlineOptions::_dump_file             = "TOPCOM.dump";
const char* CommandlineOptions::_read_file             = "TOPCOM.dump";

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

      std::cout << "options concerning shell output:" << std::endl;
      std::cout << "-h or --help         : print this help message" << std::endl;
      std::cout << "-v                   : write verbose information to stderr." << std::endl;
      std::cout << "--reportfrequency [n]: report every [n]th new triangulation." << std::endl;
      std::cout << "-d                   : write debug information to stderr." << std::endl;
      std::cout << "--heights            : write height vectors for triangulations to stdout (implies --regular)." 
		<< std::endl;
      std::cout << "--flips              : write flips as pairs of triangulation IDs to stdout." 
		<< std::endl;
      std::cout << std::endl;
     
      std::cout << "options for checking input:" << std::endl;
      std::cout << "--checktriang        : check any given triangulations for correctness." << std::endl;
      std::cout << std::endl;

      std::cout << "options for reporting properties of discovered triangulations:" << std::endl;
      std::cout << "--flipdeficiency     : report flip deficiency in triangulations to stderr." << std::endl;
      std::cout << "--checkfrequency [n] : check for regularity every [n]th triangulation and exit if regular." 
		<< std::endl;
      std::cout << "--frequency [n]      : (deprecated) check for regularity every [n]th triangulation and exit if regular." 
		<< std::endl;
      std::cout << std::endl;

      std::cout << "options concerning which triangulations are output (no influence on exploration):" << std::endl;
      std::cout << "--cardinality [n]    : count/output only triangulations of cardinality [n]." << std::endl;
      std::cout << "--nonregular         : count/output only non-regular triangulations." << std::endl;
      std::cout << std::endl;

      std::cout << "options concerning which triangulations are explored:" << std::endl;
      std::cout << "--regular            : consider only regular triangulations" << std::endl;
      std::cout << "--noinsertion        : never use a point that is unused in the seed triangulation." << std::endl;
      std::cout << "--reducepoints       : try to remove the number of use points, reported to stderr." << std::endl;
      std::cout << "--keepcard           : never change the cardinality of triangulations by flipping." << std::endl;
      std::cout << std::endl;

      std::cout << "options concerning symmetries:" << std::endl;
      std::cout << "--affine             : assume that given symmetries are affine." << std::endl;
      std::cout << "--nosymmetries       : ignore symmetries." << std::endl;
      std::cout << std::endl;

      std::cout << "options for dumping and reading the status of an interrupted computation:" << std::endl;
      std::cout << "--dump               : dump status of previous computation (last completed BFS round) into file" << std::endl;
      std::cout << "--dumpfile [filename]: set dump file name to [filename] (default: TOPCOM.dump)" << std::endl;
      std::cout << "--dumpfrequency [n]  : dump each [n]th completed BFS round (default: 1)" << std::endl;
      std::cout << "--dumprotations [n]  : dump into [n] rotating files" << std::endl;
      std::cout << "--read               : read status of previous computation (last completed BFS round) from file" << std::endl;
      std::cout << "--readfile [filename]: set dump file name to read to [filename] (default: TOPCOM.dump)" << std::endl;
      std::cout << std::endl;
      

      std::cout << "options concerning internal behaviour:" << std::endl;
      std::cout << "--memopt             : try to reduce memory consumption." << std::endl;
      std::cout << "--chirocache [n]     : set the chirotope cache to [n] elements." << std::endl;
      std::cout << "--localcache [n]     : set the cache for local data to [n] elements ." << std::endl;
      std::cout << "--soplex             : use soplex for regularity checks." << std::endl;

      exit(0);
    }

    // output options:
    if (strcmp(argv[i], "-v") == 0) {
      _verbose = true;

      std::cerr << " -v             : verbose output activated" << std::endl;
    }
    if (strcmp(argv[i], "--reportfrequency") == 0) {
      if (argc > i + 1) {
	_report_frequency = (size_type)atol(argv[i+1]);
	_verbose = true;

	std::cerr << "--reportrequency : report every " << _dump_frequency << "th new triangulation" << std::endl;
      }
    }
    if (strcmp(argv[i], "-d") == 0) {
      std::cerr << " -d             : debug output activated" << std::endl;

      _debug = true;
    }
    if (strcmp(argv[i], "--heights") == 0) {
      std::cerr << "--heights       : output of defining heights activated" << std::endl;

      _output_heights = true;
      _check_sometimes = false;
      _check_regular = true;
    }

    if (strcmp(argv[i], "--flips") == 0) {
      std::cerr << "--flips        : output of flips activated" << std::endl;

      _output_flips = true;
    }

    // options for checking input:
    if (strcmp(argv[i], "--checktriang") == 0) {
      std::cerr << "--checktriangs  : check seed triangulation activated" << std::endl;

      _check = true;
    }

    // options for reporting properties of discovered triangulations:
    if (strcmp(argv[i], "--flipdeficiency") == 0) {
      std::cerr << "--flipdeficiency: \t search for flip deficiency activated" << std::endl;

      _neighborcount = true;
    }
    if (strcmp(argv[i], "--frequency") == 0) {
      std::cerr << "WARNING:" << std::endl;
      std::cerr << "--frequency deprecated, please use --checkfrequency instead." << std::endl;
      if (_check_regular) {
	std::cerr << "--frequency cannot be used with --regular, ignoring --frequency." << std::endl;
      }
      else {
	_check_regular = false;
	_check_sometimes = true;
	if (argc > i + 1) {
	  _sometimes_frequency = (size_type)atol(argv[i+1]);
	}

        std::cerr << "--frequency     : check regularity every " << _sometimes_frequency << "th triangulation acticated" << std::endl;
      }
    }
    if (strcmp(argv[i], "--checkfrequency") == 0) {
      if (_check_regular) {
	std::cerr << "--checkfrequency cannot be used with --regular, ignoring --checkfrequency." << std::endl;
      }
      else {
	_check_regular = false;
	_check_sometimes = true;
	if (argc > i + 1) {
	  _sometimes_frequency = (size_type)atol(argv[i+1]);
	}

        std::cerr << "--frequency     : check regularity every " << _sometimes_frequency << "th triangulation acticated" << std::endl;
      }
    }

    // options concerning which triangulations are output (no influence on explorartion):
    if (strcmp(argv[i], "--cardinality") == 0) {
      if (argc > i + 1) {
	_no_of_simplices = (size_type)atol(argv[i+1]);
      }

      std::cerr << "--cardinality   : restrict to triangulations with " << _no_of_simplices << " simplices" << std::endl;
    }

    if (strcmp(argv[i], "--nonregular") == 0) {
      std::cerr << "--nonregular    : search for non-regular triangulations activated" << std::endl;

      _check_nonregular = true;
    }

    // options concerning which triangulations are explored:
    if (strcmp(argv[i], "--regular") == 0) {
      if (_check_sometimes) {
	std::cerr << "--regular cannot be used with --frequency, ignoring --regular." << std::endl;
      }
      else {
	_check_sometimes = false;
	_check_regular = true;

        std::cerr << "--regular       : check for regular triangulations activated" << std::endl;
      }
    }
    if (strcmp(argv[i], "--reducepoints") == 0) {
      _reduce_points = true;

      std::cerr << "--reducepoints  : reduce points heuristics activated" << std::endl;
    }
    if (strcmp(argv[i], "--noinsertion") == 0) {
      _dont_add_points = true;

      std::cerr << "--noinsertion   : never add points activated" << std::endl;
    }
    if (strcmp(argv[i], "--keepcard") == 0) {
      _dont_change_card = true;

      std::cerr << "--keepcard   : never change cardinality of triangulations by flipping activated" << std::endl;
    }

    // options concerning symmetries:
    if (strcmp(argv[i], "--affine") == 0) {
      _symmetries_are_affine = true;

      std::cerr << "--affine        : assumption that symmetries are affine activated" << std::endl;
    }
    if (strcmp(argv[i], "--nosymmetries") == 0) {
      _ignore_symmetries = true;

      std::cerr << "--nosymmetries  : ignoring symmetries activated" << std::endl;
    }

    // options concerning internals:
    if (strcmp(argv[i], "--memopt") == 0) {
      _memopt = true;

      std::cerr << "--memopt        : higher memory efficiency activated" << std::endl;
    }
    if (strcmp(argv[i], "--chirocache") == 0) {
      if (argc > i + 1) {
	_chirocache = (size_type)atol(argv[i+1]);

	std::cerr << "--chirocache    : cache for lazy chirotope set to " << _chirocache << " elements" << std::endl;
      }
    }
    if (strcmp(argv[i], "--localcache") == 0) {
      if (argc > i + 1) {
	_localcache = (size_type)atol(argv[i+1]);

	std::cerr << "--localcache    : cache for local data set to " << _localcache << " elements" << std::endl;
      }
    }
    if (strcmp(argv[i], "--dumpfrequency") == 0) {
      if (argc > i + 1) {
	_dump_frequency = (size_type)atol(argv[i+1]);
	_dump_status = true;

	std::cerr << "--dumpfrequency : dump every " << _dump_frequency << "th processed triangulation" << std::endl;
      }
    }
    if (strcmp(argv[i], "--dumprotations") == 0) {
      if (argc > i + 1) {
	_dump_rotations = (size_type)atol(argv[i+1]);
	_dump_status    = true;

	std::cerr << "--dumprotations : rotate dump into " << _dump_rotations << " many dump files" << std::endl;
      }
    }
    if (strcmp(argv[i], "--dumpfile") == 0) {
      if (argc > i + 1) {
	_dump_file = argv[i+1];
	_dump_status = true;

	std::cerr << "--dumpfile      : dump to file " << _dump_file << std::endl;
      }
    }
    if (strcmp(argv[i], "--dump") == 0) {
      _dump_status = true;

      std::cerr << "--dump          : dump status of computation into file activated" << std::endl;
    }
    if (strcmp(argv[i], "--readfile") == 0) {
      if (argc > i + 1) {
	_read_file = argv[i+1];
	_read_status = true;

	std::cerr << "--readfile      : read from file " << _dump_file << std::endl;
      }
    }
    if (strcmp(argv[i], "--read") == 0) {
      _read_status = true;

      std::cerr << "--read            : read status from file" << std::endl;
    }
    if (strcmp(argv[i], "--soplex") == 0) {
#ifdef HAVE_LIBSOPLEX      
      _use_soplex = true;

      std::cerr << "--soplex        : soplex activated" << std::endl;
#else
      std::cerr << "This binary was compiled without soplex support, ignoring --soplex." << std::endl;
#endif
    }

    // some options that are not documented:
    if (strcmp(argv[i], "-s") == 0) {
      _simple = true;
    }
    if (strcmp(argv[i], "-A") == 0) {
      _compute_all = true;
    }
    if (strcmp(argv[i], "-C") == 0) {
      _input_chiro = true;
    }
    if (strcmp(argv[i], "-F") == 0) {
      _fine_only = true;
    }
    if (strcmp(argv[i], "-O") == 0) {
      _output_triangs = true;
    }
    if (strcmp(argv[i], "-E") == 0) {
      _output_flips = true;
    }
    if (strcmp(argv[i], "-P") == 0) {
      _preprocess = true;
    }
  }
  std::cerr << "... done." << std::endl;
}

// eof CommandlineOptions.cc
