/*
 * sosprim.h -- Class definitions for SOS Primitives
 *
 * author -- Mike Facello
 *
 * date -- 10/23/96
 */

#ifndef __RGI_SOSPRIM_H__
#define __RGI_SOSPRIM_H__

#include "sos/soseval.h"

class LI_PArray;

/************************************************************************/

/*
 * Class SOS_PrimEvaluator
 *
 * This class will handle the evaluation of SOS primitives.  These
 * primitives include:
 *
 *     positive3
 *     above3
 *     above4
 *     in_sphere
 *
 */

class SOS_PrimEvaluator
{
public:
  SOS_PrimEvaluator (void);

  Bool positive3 (const LI_PArray &arr, VIndex i, VIndex j, VIndex k, VIndex l);
  Bool above3 (const LI_PArray &arr, VIndex i, VIndex j, VIndex k, VIndex l);
  Bool above4 (const LI_PArray &arr, VIndex i, VIndex j, VIndex k, VIndex l, 
	       VIndex m);
  Bool inSphere (const LI_PArray &arr, VIndex o, VIndex i, VIndex j, VIndex k, 
		 VIndex p);
  Bool inSphereP (const LI_PArray &arr, VIndex o, VIndex i, VIndex j,
		  VIndex k, VIndex p);

  // NOTE:  it is also possible to have primitives that take a different
  // LI_PArray for each vertex, assuming that the point arrays know how
  // to order themselves (possibly by unique index).

  void debugOn (void);
  void debugOff (void);

  friend ostream& operator<< (ostream& s, SOS_PrimEvaluator& evaluator);

protected:

  Lambda3_Eval lambda3_;
  Lambda4_Eval lambda4_;
  Lambda5_Eval lambda5_;
  int numPos3_, numAbove3_, numAbove4_, numInSphere_, numInSphereP_;

  SOS_Result result_;

  Bool debugFlag_;
};

#endif // __SOSPRIM_H__
