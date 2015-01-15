/*
 * fliphandler.h -- header file for fliphandler class
 *
 * Author -- Mike Facello
 *
 *           Damrong, edit for dynamic triangulation
 *                    note the use of #ifdef _DYNAMIC_
 *
 * Date -- 9/28/96
 */

#ifndef __RGI_FLIPHANDLER_H__
#define __RGI_FLIPHANDLER_H__

#include <iostream>
#include <iomanip>
#include <basic/basic.h>
#include <li/lia.h>
#include <li/lipoints.h>
#include <sos/sosfloat.h>
#include <geometry/tristmodifier.h>

#ifdef _DYNAMIC_
#include "geometry/simplexset.h"
#endif


/*--------------------------------------------------------------------------*/

//
// CLASS:  FlipEvent -- This type encapsulates a flip event by indicating
//                      the type of flip, and the triangles involved.
//

enum { FLIP_INVALID = 0, FLIP_INSERT, FLIP_TRIANGLE, FLIP_EDGE, FLIP_VERTEX };

class FlipEvent 
{
public:
  FlipEvent(int type = FLIP_INVALID);
  ~FlipEvent(void);
  int getType (void) const;
  void setType (int type);
  OrTri getTri(int i) const;

  // BIG NOTE:  This should go away eventually!!!  Needed for search.c now.

  OrTri *getAllTri(void);

  // IO

  friend ostream&   operator<< (ostream& s, FlipEvent& fe);

private:
  int flipType_;
  OrTri flippedTri_[10];
};

inline FlipEvent::FlipEvent(int type) : flipType_(type) { }
inline FlipEvent::~FlipEvent (void) { }
inline int FlipEvent::getType (void) const { return flipType_; }
inline void FlipEvent::setType (int type) { flipType_ = type; }
inline OrTri FlipEvent::getTri (int i) const { return flippedTri_[i]; }
inline OrTri *FlipEvent::getAllTri (void) { return flippedTri_; }


/*--------------------------------------------------------------------------*/

//
// CLASS:  FlipHandler -- This class processes flips on a trist.
//

class FlipHandler : public TristModifier
{
public:
  FlipHandler (Trist  * tr, RealPArray * rArray, LI_Converter& converter);

  // Perform a flip

  Bool insert (OrTri ijk, VIndex v, FlipEvent &flipEvent);
  Bool flipTriangle  (OrTri xyz, FlipEvent& flipEvent);
  Bool flipEdge      (OrTri pox, FlipEvent& flipEvent);
  Bool flipVertex    (OrTri ivj, FlipEvent& flipEvent);

  // IO

  friend ostream&   operator<< (ostream& s, FlipHandler& fh);

private:

  RealPArray * rArray_;
  SOS_FloatEvaluator sEval_;

  // Hidden methods

  void deleteTriangle (OrTri ef);
  void load  (OrTri xyz, VIndex x, VIndex y, VIndex z, VIndex o, VIndex p, 
	      OrTri tri[/*10*/]);
  void load2 (OrTri pox, VIndex x, VIndex y, VIndex z, VIndex o, VIndex p, 
	      OrTri tri[/*10*/]);
  void load3 (OrTri e, OrTri tri[/*10*/]);

  void diagonalize (OrTri xyz, OrTri tri[/*10*/], 
		    VIndex x, VIndex y, VIndex z, VIndex o, VIndex p);
  Bool diagonalizeTest (OrTri xpo,
			VIndex x, VIndex y, VIndex z, VIndex p, VIndex o);
  void cut (OrTri tri[/*10*/],
	    VIndex x, VIndex y, VIndex z, VIndex o, VIndex p);

#ifdef _DYNAMIC_
  SimplexSet   simplexSet_;	// report flip event through this object
#endif
};

#endif // __FLIPHANDLER_H__
