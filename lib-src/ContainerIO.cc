////////////////////////////////////////////////////////////////////////////////
// 
// ContainerIO.cc
//
//    produced: 13/02/2020 jr
// last change: 20/02/2020 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include "ContainerIO.hh"

namespace topcom {

  std::mutex IO_sync::mutex;

  const char  ContainerChars::list_start_char = '[';
  const char  ContainerChars::list_end_char   = ']';
  const char  ContainerChars::set_start_char  = '{';
  const char  ContainerChars::set_end_char    = '}';
  const char  ContainerChars::delim_char      = ',';
  const char* ContainerChars::map_chars       = "->";

}; // namespace topcom

// eof ContainerIO.cc
