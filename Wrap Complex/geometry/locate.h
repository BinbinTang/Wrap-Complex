#ifndef __RGI_LOCATE_H__
#define __RGI_LOCATE_H__

/*
 * locate.h -- C++ Header file for Point Location in a Delaunay Complex
 *
 * author -- Mike Facello
 *
 * date -- 10/24/96
 */

#include <basic/basic.h>
#include <basic/miscmath.h>
#include <li/lipoints.h>
#include <geometry/trist.h>
#include <geometry/tristmodifier.h>
#include <geometry/fliphandler.h>


/************************************************************************/

/*
 * Class PLTechnique
 *
 * 
 *
 */

class PLTechnique : public TristModifier
{
public:
  PLTechnique (Trist  * tr, RealPArray * rArray, LI_Converter& converter);

  virtual OrTri locate (VIndex p) = 0;
  //  virtual void update (FlipEvent &fe) = 0;  Not implemented now...

protected:
  RealPArray * rArray_;
	LI_Converter& converter_;
};


/************************************************************************/

/*
 * Class JWPLocate
 *
 * This is the Jump and Walk Point Location technique.
 *
 * BIG NOTE:  This should eventually change to taking an LI_Point as
 * a parameter rather than a VIndex.  The main problem right now is the
 * specification of the SOS primitives in sosprim.h.
 */

class JWPLocate : public PLTechnique
{
public:
  JWPLocate (Trist  * tr, RealPArray * rArray, LI_Converter& converter);
 ~JWPLocate (void) {}
  OrTri locate (VIndex p);
  //  void update (FlipEvent &fe);   Not implemented now...
  int jwSearch_GetTests (void);
  int jwSearch_GetTets (void);

  class JWSearchData
  {
   public:
    VIndex p, i, j, k;
    OrTri ef;
    JWSearchData (void) { }
    JWSearchData (VIndex pp, OrTri pef, VIndex pi, VIndex pj, VIndex pk)
	  : p(pp), i(pi), j(pj), k(pk), ef(pef) { }
	void set (VIndex pp, OrTri pef, VIndex pi, VIndex pj, VIndex pk)
	{ p = pp; ef = pef; i = pi; j = pj; k = pk; }
  };

private:
  SOS_FloatEvaluator sEval_;
  IterStack<JWSearchData> jwStack_;

  OrTri jwSearch (VIndex p, int k);
  OrTri jwSearchWalk (JWSearchData& data);
  double distance (OrTri ef, VIndex p);
  
  int positive3Tests_, tetsVisited_;
};



/************************************************************************/

/*
 * Class HDagLocate
 *
 * To be implemented later if needed...
 *
 */



/************************************************************************/

/*
 * Class CascadeLocate
 *
 * To be implemented later if needed...
 *
 */


#endif // __LOCATE_H__
