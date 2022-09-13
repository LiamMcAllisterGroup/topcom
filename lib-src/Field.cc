////////////////////////////////////////////////////////////////////////////////
// 
// Field.cc
//
//    produced: 30/03/98 jr
// last change: 30/03/98 jr
// 
////////////////////////////////////////////////////////////////////////////////

#include "Field.hh"

namespace topcom {

  // #ifdef DOUBLE_ARITHMETIC
  // const Field Field::EPSILON  = Field(1E-99);
  // #endif

  const Field FieldConstants::ZERO     = Field(0);
  const Field FieldConstants::ONE      = Field(1);
  const Field FieldConstants::MINUSONE = Field(-1);

}; // namespace topcom

// eof Field.cc
