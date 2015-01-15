/*
 * liminor.h -- Definitions for long integer evaluator objects
 *
 * author -- Mike Facello
 *
 * date -- 10/23/96
 */

#ifndef __RGI_LIMINOR_H__
#define __RGI_LIMINOR_H__


#include "li/li.h"


class LI_Point;
class LI_DetEvaluator;

/*
 * Class LI_VertMinorEvaluator
 *
 * This class is designed to handle minor evaluations given the points and
 * the columns involved in the minor.  All minor evaluations return pointers
 * to long integers, and the caller is responsible for deallocating these
 * pointers.  ((Option:  somehow, LI's know how to deallocate themselves...))
 *
 * Each method of this object takes some number of points and the same
 * number of columns to be used in the minor evaluation.  If the last
 * column is a 0, then it will be replaced by a column of all 1's.
 *
 * Question:  Should this be a singleton class?
 */

class LI_VertMinorEvaluator
{
public:
  LI_VertMinorEvaluator (void);

  ~LI_VertMinorEvaluator (void);

  void minor1 (const LI_Point & p1,
	       int a,
	       LI & result);
  void minor2 (const LI_Point & p1, const LI_Point & p2,
	       int a, int b,
	       LI & result);
  void minor3 (const LI_Point & p1, const LI_Point & p2, const LI_Point & p3,
	       int a, int b, int c,
	       LI & result);
  void minor4 (const LI_Point & p1, const LI_Point & p2, const LI_Point & p3,
	       const LI_Point & p4,
	       int a, int b, int c, int d,
	       LI & result);
  void minor5 (const LI_Point & p1, const LI_Point & p2, const LI_Point & p3,
	       const LI_Point & p4, const LI_Point & p5,
	       int a, int b, int c, int d, int e,
	       LI & result);

  // Output statistics about this evaluator
  friend ostream& operator<< (ostream& s, LI_VertMinorEvaluator& evaluator);
  
private:
  LI_DetEvaluator *dEval_;    // Used for evaluating determinants
  
  enum { wSize = 17 };        // Size of the workspace
  LI wspace_[wSize];          // The workspace
  int num_calls_[6];          // Number of calls to the above minors
                              //     calls to minor"d" are stored in 
                              //     num_calls_[d]
};

#endif  // __LIMINOR_H__
