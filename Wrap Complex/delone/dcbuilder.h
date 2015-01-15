#ifndef __RGI_DCBUILDER_H__
#define __RGI_DCBUILDER_H__

/*
 * dcbuilder.h -- C++ Header file for the Delaunay Complex Builder
 *
 * author -- Mike Facello
 *        -- Damrong, editi for dynamic triangulation.
 *                  observe #ifdef _DYNAMIC_.    Feb 16, 1998
 *
 * date -- 10/24/96
 */ 

#include "basic/miscmath.h"
#include <li/liminor.h>
#include <geometry/fliphandler.h>
#include <geometry/locate.h>
#include <delone/dcomp.h>

#ifdef _DYNAMIC_
#include <geometry/simplexset.h>
#endif

/************************************************************************/

/*
 * Class DCompBuilder
 *
 * 
 *
 */

class DCompBuilder : public TristModifier
{
public:
  // Build a new complex -- maxBefore is the maximum number of digits before
  // the decimal point in any floating point input, and maxAfter is the
  // desired maximum number of digits after the decimal point (all other
  // digits will be truncated.
  DCompBuilder (int maxBefore = 16, int maxAfter = 0, int arraySize = 10);

  DCompBuilder (DComplex *comp);     // Modify an existing complex
                                     //   (Is this common to all complexes???)
  ~DCompBuilder (void);

  // Get the complex.  The first two operations should be performed before you
  // call getComplex().

  void cleanTriangulation (void);		// peel and pack
  void reindexTriangles (IMap& newLoc);        // move the triangles to new locations
  int initIndices (void);          // initialize the indices for the triangulation
  DComplex *getComplex (void);      // Get the final complex when it's
                                    //   finished.
  FIndex theRedTri_;
  BitVector vlink;
  int theV;
  double BBoxMin_[3],BBoxMax_[3];


#ifdef _DYNAMIC_
  void maintainIndex (void);        // start maintaining index tables
#endif

  
  // Add a point to the point array only
	int addPoints (RealPArray * rArr, BitVector& skip);
  int addPoint (double x[4]);
  RealPArray * getPArray (void) { return rArray_; }
  BitVector * getRedundentBV() {return redundent_;};

  int compute  (void);             // Insert all un-inserted points

	int getProblemVertex (void) { return problemVertexIX_; }
		//- Returns the index of the problem vertex involved in a bad delone computation.

  void debugOn (void);
  void debugOff (void);

  enum Color { White, Green, Red };
  void testDC (void);

  	int getSize ();
	double* getPoint (int v);
	bool isOnHull(OrTri ef);
	int getTheForthPoint(OrTri ef);


private:

  LI_Converter converter_;               // The Points
  RealPArray * rArray_;
  BitVector *redundent_;
  DComplex *dcomp_;                 // The Complex

  FlipHandler * fh_;                // The Flip Handler
  JWPLocate * locator_;             // The Point Location Technique
  BitVector triStatus_;             // Triangle status 
                                    //     (delaunay, non-delaunay, etc.)

  IndexStack uninserted_;    // All vertices not inserted in
                                    //   the complex.
  OrTriStack flipList_, dfsStack_;
  SOS_FloatEvaluator sEval_;
  LI_VertMinorEvaluator mEval_;
  
  LI wSpace_;                       // Storage for long integer results

  Bool debugFlag_;
  Bool firstTime_;
 
#ifdef _DYNAMIC_
  Bool maintainIndex_;
#endif
 
  // "Global" variable for sorting procedure (Be very quiet about this!!)

  int lexiComparisons_;
  
  // Variables shared during Mounting procedures

  VIndex v_, i_, j_, k_;
  OrTri ijk_, rx_;


  // Statistics

  int mount1s_, mount2s_, mount3as_, mount3bs_, mount4s_;
  int skips_, eflips_, fflips_;
  int searchOutsiders_;
  int maxFlips_, maxFlipsOverflowFlag_;
  int reds_, greens_;
  
  VIndex new_v;

#ifdef _DYNAMIC_
  SimplexSet  simplexSet_;          // for communication to hash
  SimplexSet  simplexSet2_;         // tables used in mount/peel
#endif
  // Internal methods for construction
  void initial (VIndex a, VIndex b, VIndex c, VIndex o, OrTri& h);
  void removeDuplicates (IndexStack& verts);
  friend Bool lexiCompare (const int *i, const int *j);
	Bool compare (int i, int j);
  void randomize (void);

  void addND (OrTri ef);            // Adds the given oriented triangle
                                    //   to the list of non-delaunay triangles
  int  processFlips (OrTriStack& ndList, VIndex v);
                                    // Process all triangles on the stack of
                                    //   non-Delaunay triangles

  void addToFlipList (OrTri ef);
  Bool nextNDFacet   (OrTri& ef);

  Bool flipVertice  (OrTri xyz);
  Bool flipEdge     (OrTri pox,int&,int&,int&);
  Bool flipTriangle (OrTri xyz,int,int,int);

  // Routines for handlng points external to the convex hull

  void hullInsert (VIndex v, OrTriStack& the_list, OrTri& h);
  void restoreHullBits (VIndex v, OrTri hf);
  Bool isHullFacet (OrTri h);
  void collectVisibles (VIndex v, OrTri ef, OrTriStack& iStack);
  void collectVisiblesDFS (VIndex v, OrTriStack& v_list, OrTri ef);
  OrTri getHull (void);
  
  void mountT (char code, VIndex v, OrTri ijk);
  void mount1 (void);
  void mount2 (OrTri jki, OrTri kij, OrTri ivj, OrTri vji);
  void mount3a (OrTri ijv, OrTri vkj, OrTri vji);
  void mount3b (OrTri ijv, OrTri ivj, OrTri vki);
  void mount4 (OrTri vkj, OrTri vki, OrTri ivj, OrTri vji);

  Bool delaunayTest (OrTri ef);
  
  // Triangle status

  DCompBuilder::Color getColor (FIndex t);
  void  setColor (FIndex t, Color color);
  void  markTriangle (FIndex t, Color color);

  // Peeling Flat Tetrahedra

  void peelFlatTetras (void);
  void pushIfFlat (OrTriStack& q, OrTri ef, LI_Point liPoints[4]);
#ifdef _DYNAMIC_
	void prePeelDelTri_ (OrTri ef);
	void postPeelAddVertex_ (void);
#endif

  // Testing the triangulation

  void testHull (OrTri h);
  void testHullRecursive (BitVector& hb, OrTri h);
  void testComplex (void);
  void testTriangulation (void);
  void testTetraGlobal (VIndex a, VIndex b, VIndex c, VIndex d,
			int m, Bool delaunay_flag);
	


  int dcompExported_;
	int problemVertexIX_;
};

#endif // __DCBUILDER_H__
