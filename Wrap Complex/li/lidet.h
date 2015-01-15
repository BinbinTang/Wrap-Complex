/*
 * lia.h -- Definitions for long integer evaluator objects
 *
 * author -- Mike Facello
 *
 * date -- 10/23/96
 */

#ifndef __RGI_LIDET_H__
#define __RGI_LIDET_H__

#include "li/li.h"

/*
 * Class LI_DetEvaluator
 *
 * This class is a low level determinant evaluator for long integer
 * numbers.  All results are pointers, and it is the user's responsibility
 * to deallocate them.  ((One possibility... make LI's smart, and have them
 * delete themselves when they are not used anymore...))
 *
 * This evaluator provides 2x2, 3x3 and 4x4 determinants.  Statistics are
 * gathered on the usage of these determinants, and can be printed out
 * with the << operator.
 */

class LI_DetEvaluator
{
public:
  LI_DetEvaluator (void) 
  {
    for (register int i = 0; i < 5; i++)
      num_calls_[i] = 0;
  }

  void det2 (const LI & x11, const LI & x12, 
	     const LI & x21, const LI & x22,
	     LI & result);

  void det3 (const LI & x11, const LI & x12, const LI & x13, 
	     const LI & x21, const LI & x22, const LI & x23, 
	     const LI & x31, const LI & x32, const LI & x33,
	     LI & result);

  void det4 (const LI & x11, const LI & x12, const LI & x13, const LI & x14,
	     const LI & x21, const LI & x22, const LI & x23, const LI & x24,
	     const LI & x31, const LI & x32, const LI & x33, const LI & x34,
	     const LI & x41, const LI & x42, const LI & x43, const LI & x44,
	     LI & result);

  // Print statistics of this evaluator
  friend ostream& operator<< (ostream& s, LI_DetEvaluator& evaluator);

private:

  enum { wSize = 15 };      // The size of the workspace
  // Indices for subdeterminant results
  enum { D12 = 5, D13, D14, D23, D24, D34, D123, D124, D134, D234 };

  LI wspace_[wSize];
  int num_calls_[5];      // store number of calls for dxd in num_calls_[d]
};

#endif // LIDET_H
