/* clib/lidet.C  ---  long integer determinants. */

/* All procedures detI (for I=2,3,4) calculate small determinants of Lia
   arguments employing expansion by minors (here: by the last column).  The
   expansion is implemented in a bottom-up (dynamic programming) fashion.
   According to the tables Harald Rosenberger's PhD thesis, this IS the best
   method to compute exact d-by-d determinants for d <= 5.  It clearly defeats
   Gaussian elimination for d <= 10. */

/*--------------------------------------------------------------------------*/

#include "lidet.h"

/*--------------------------------------------------------------------------*/

/* Macro for 2-by-2 determinant.
   RESULT = X11 * X22 - X12 * X21. */

#define DET2(X11,X12,X21,X22,RESULT) \
   { \
     wspace_[0].mult (X11, X22); \
     wspace_[1].mult (X12, X21); \
     RESULT.sub (wspace_[0], wspace_[1]); \
   }

/*--------------------------------------------------------------------------*/

/* Macro for expansion of 3-by-3 determinant by last row.
   RESULT = X13 * SUB23 - X23 * SUB13 + X33 * SUB12. */

#define EX3(X13,SUB23,X23,SUB13,X33,SUB12,RESULT) \
   { \
     wspace_[0].mult (X33, SUB12); \
     wspace_[1].mult (X23, SUB13); \
     wspace_[3].sub  (wspace_[0], wspace_[1]); \
     wspace_[0].mult (X13, SUB23); \
     RESULT.add (wspace_[3], wspace_[0]); \
   }

/*--------------------------------------------------------------------------*/

void LI_DetEvaluator::
det2 (const LI & x11, const LI & x12, 
      const LI & x21, const LI & x22,
      LI & result)
{
  num_calls_[2] ++;
  DET2 (x11, x12,  x21, x22,  result);
}

/*--------------------------------------------------------------------------*/

void LI_DetEvaluator::
det3 (const LI & x11, const LI & x12, const LI & x13, 
      const LI & x21, const LI & x22, const LI & x23, 
      const LI & x31, const LI & x32, const LI & x33,
      LI & result)
{
  num_calls_[3] ++;
  DET2 (x11, x12,  x21, x22,  wspace_[D12]);
  DET2 (x11, x12,  x31, x32,  wspace_[D13]);
  DET2 (x21, x22,  x31, x32,  wspace_[D23]);
  ;
  EX3 (x13, wspace_[D23],  x23, wspace_[D13],  x33, wspace_[D12],  result);
}

/*--------------------------------------------------------------------------*/

void LI_DetEvaluator::
det4 (const LI & x11, const LI & x12, const LI & x13, const LI & x14,
      const LI & x21, const LI & x22, const LI & x23, const LI & x24,
      const LI & x31, const LI & x32, const LI & x33, const LI & x34,
      const LI & x41, const LI & x42, const LI & x43, const LI & x44,
      LI & result)
{
  num_calls_[4] ++;
  DET2 (x11, x12,  x21, x22,  wspace_[D12]);
  DET2 (x11, x12,  x31, x32,  wspace_[D13]);
  DET2 (x11, x12,  x41, x42,  wspace_[D14]);
  DET2 (x21, x22,  x31, x32,  wspace_[D23]);
  DET2 (x21, x22,  x41, x42,  wspace_[D24]);
  DET2 (x31, x32,  x41, x42,  wspace_[D34]);
  ;
  EX3 (x13, wspace_[D23],  x23, wspace_[D13],  x33, 
       wspace_[D12],  wspace_[D123]);
  EX3 (x13, wspace_[D24],  x23, wspace_[D14],  x43, 
       wspace_[D12],  wspace_[D124]);
  EX3 (x13, wspace_[D34],  x33, wspace_[D14],  x43, 
       wspace_[D13],  wspace_[D134]);
  EX3 (x23, wspace_[D34],  x33, wspace_[D24],  x43, 
       wspace_[D23],  wspace_[D234]);
  ;
  wspace_[0].mult (x44, wspace_[D123]);
  wspace_[1].mult (x34, wspace_[D124]);
  wspace_[3].sub  (wspace_[0], wspace_[1]);
  wspace_[0].mult (x24, wspace_[D134]);
  wspace_[1].mult (x14, wspace_[D234]);
  wspace_[4].sub  (wspace_[0], wspace_[1]);
  result.add (wspace_[3], wspace_[4]);
}

/*--------------------------------------------------------------------------*/

ostream& 
operator<< (ostream& s, LI_DetEvaluator& evaluator)
{
  s << "LI Determinant evaluations\n\n";
  s.width(12);
  s << evaluator.num_calls_[2] << " . 2 x 2 determinants\n";
  s.width(12);
  s << evaluator.num_calls_[3] << " . 3 x 3 determinants\n";
  s.width(12);
  s << evaluator.num_calls_[4] << " . 4 x 4 determinants\n\n";

  return s;
}

/*--------------------------------------------------------------------------*/
