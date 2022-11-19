////////////////////////////////////////////////////////////////////////////////
// 
// Graphics.hh 
//
//    produced: 14 Jul 2021 jr
// last change: 14 Jul 2021 jr
//
////////////////////////////////////////////////////////////////////////////////
#ifndef GRAPHICS_HH
#define GRAPHICS_HH

#include <stdlib.h>
#include <cstring>
#include <fstream>

#include "CommandlineOptions.hh"

#include "Vertices.hh"
#include "PointConfiguration.hh"
#include "SimplicialComplex.hh"
#include "LabelSet.hh"
#include "StairCaseMatrix.hh"

#include "ContainerIO.hh"

namespace topcom {

  class Graphics {
  private:
    static std::fstream _asy_stream;
    static parameter_type _no;
    static parameter_type _rank;
  public:
    Graphics() = delete;
    Graphics(const Graphics&) = delete;
  public:
    static void init_asy(const char* asy_file);
    static void header_to_asy(const parameter_type, const parameter_type);
    static void nodesize_for_pointsgraphics_to_asy();
    static void vertices_to_asy(const Vertices&);
    static void points_to_asy(const PointConfiguration&);
    static void typedef_for_labelset_to_asy();
    static void typedef_for_partialtriang_to_asy();
    static void typedef_for_matrix_to_asy();
    static void worker_to_asy();
    static void run_to_asy(const int);
    static void partialtriang_to_asy(const int, const size_type, const IntegerSet&, const SimplicialComplex&);
    static void labelset_to_asy(const int, const size_type, const LabelSet&);
    static void matrix_to_asy(const int, const size_type, const LabelSet&, const StairCaseMatrix&);
    static void arc_to_asy(const int, const size_type, const size_type, const size_type);
    static void deadend_to_asy(const int, const size_type, const size_type);
    static void earlydeadend_to_asy(const int, const size_type, const size_type);
    static void veryearlydeadend_to_asy(const int, const size_type, const size_type);
    static void missingvolume_to_asy(const int, const size_type, const size_type);
    static void notnew_to_asy(const int, const size_type, const size_type);
    static void solution_to_asy(const int, const size_type, const size_type);
    static void nodepos_to_asy();
    static void define_draw_node_for_partialtriang_to_asy();
    static void define_draw_node_for_labelset_to_asy();
    static void define_draw_node_for_matrix_to_asy();
    static void drawings_to_asy();
    static void term_asy();

    // functions:
    inline static std::fstream& asy_stream() { return _asy_stream; }
  };

}; // namespace topcom

#endif

// eof Graphics.hh
