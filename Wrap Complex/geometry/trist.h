/*
 * trist.h -- C++ Header file for the Trist class
 *
 * author -- Mike Facello
 *
 * date -- 10/17/96
 */

#ifndef __RGI_TRIST_H__
#define __RGI_TRIST_H__

#define NUM_BITS  32
#include <iostream>
#include <basic/basic.h>
#include <basic/ortri.h>
#include <basic/rarray.h>
#include <basic/bitvector.h>
#include <basic/callbackobject.h>
#include <basic/dumpable.h>
#include <basic/index.h>

class Trist;
class TristIter;
class StackBV;
class VertArray;

/************************************************************************/

/************************************************************************/

/*
 * Class TristInfo
 *
 * This class stores information about a Trist structure (defined below).
 * It includes counters for the basic Trist operations, and also some
 * memory allocation information
 *
 */

class TristInfo
{
public:
  TristInfo (void);
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);

  Basic_counter orgs_, syms_, enexts_, fnexts_, fsplices_;
  int minEfFnexts_;

  // These are not initialized by the constructor ...

  int bpt_;
  unsigned long bytes, maxbytes_;
};


/************************************************************************/

/*
 * Class TristNum
 *
 * This class stores information about the number of faces stored in the
 * trist, including tetrahedra, triangles, edges and vertices.
 *
 *------------------------------------------------------------------------
 *------------------------------------------------------------------------
 * NOTE:  This is probably better associated with the Delaunay Complex,
 * and will probably be moved there later.
 *------------------------------------------------------------------------
 *------------------------------------------------------------------------
 *
 */

class TristNum
{
public:
  int t_, f_, e_, v_;
  int fh_, eh_, vh_;
  int tProper_;
  int tFlat_;

  Bool operator== (TristNum &tn);   // These two split trist_num_eq apart
  Bool hullEqual (TristNum &tn);
  void check (void);
  
  friend class Trist;
};

typedef int (RgiObject::*OrTriCB) (OrTri);

/************************************************************************/

/*
 * Class Trist
 *
 * A Trist object is a triangle-based geometry representation.  It uses
 * oriented triangles as the basis for connectivity information. 
 * Operations are provided for creating and deleting triangles, linking 
 * them into the current structure, navigating the structure, and accessing
 * information about the structure.
 *
 */

class Trist : public ProgressCallbackObject
{
  friend class TristModifierAdmin;

public:
  Trist(int size = 10);
  Trist(Trist& tr);
  ~Trist(void);

  // Debugging tests

  void debugOn (void);
  void debugOff (void);

  // Creating and Deleting triangles

  // return specified version, zero by default
  OrTri makeTri  (VIndex i, VIndex j, VIndex k, int version = 0);
            // returns a new OrTri from corresponding Trist contents (binInit).
  OrTri makeTri  (VIndex* ind, OrTri * ortri);
  void  delTri   (FIndex ix);
  void  delTri   (OrTri ef);
  void  delTri   (IterStack<FIndex>& triStack);
  void  delTri   (IterStack<OrTri>& triStack);
  void  delTri   (BitVector& bv);
  void  delTriReset (BitVector& bv);
  
  // Accessing information about triangles

  int maxTri      (void);
  int numTri      (void);
  int    numHullTri   (void) { return onHull_->getNumSet (); }

  void triangle  (OrTri ef, VIndex &i, VIndex &j, VIndex &k);
  void tetra     (OrTri ef, VIndex &i, VIndex &j,
		          VIndex &k, VIndex &o);
  void getVertices (OrTri ef, Dim d, VertArray & v);
  void getSortedVertices (OrTri ef, Dim d, VertArray & v);
  void printTriangle (ostream& s, FIndex ix);
  void printTriangle (ostream& s, OrTri ef);
  Bool deleted    (FIndex ix);
  BitVector& getTriangleBV (void)
    { return triArr_.getBitVector (); }
	BitVector& getHullBV (void) { return *onHull_; }

  void   hullFacetSet (OrTri ef, Bool flag);
  Bool   isOnHull     (FIndex tr);     // replaces
                                       //   trist_hull_triangle
  Bool   isOnHull     (OrTri  ef);     // replaces trist_hull_facet

  OrTri  tetraMinEf   (OrTri ef);             // These should go away later!!
  OrTri  edgeMinEf    (OrTri ef);
  void   updateStarCenter (OrTri ef, VIndex a);
  void   updateStarCenter (OrTri ef, VIndex a, BVTag& tag);

  // in case of integer representation of OrTri
  void   updateStarCenterOrTri (StackBV& efStar, VIndex a);
  void   updateStarCenterFIndex (StackBV& efStar, VIndex b, VIndex a);
  void   getCompleteStar (OrTri ef, StackBV& tag, Bool useIntRep = FALSE);
  void   getSimpleStar (OrTri ef, StackBV& tag, Bool useIntRep = FALSE);

  // for this function, use only indices (for now)
	void   getCompleteStar(OrTri ef, StackBV& tag, 
	                       BitVector& bv, Bool rev = FALSE);
  

  // Basic navigation operations
 
  VIndex org    (OrTri ef);
  VIndex dest   (OrTri ef);
  OrTri  sym    (OrTri ef);
  OrTri  enext  (OrTri ef);
  OrTri  enext2 (OrTri ef);
  OrTri  fnext  (OrTri ef);
  OrTri  orient (OrTri ef, VIndex a);
  OrTri  orient (FIndex f, VIndex a);
  OrTri  turn   (OrTri ef);
  OrTri  fan    (OrTri ef); // assuming manifold, rotate around org(ef) for the direction of 
                            // dest(enext(ef)) to dest(ef)
  OrTri  fan    (OrTri ef,int i); // fan i times
  OrTri  afan   (OrTri ef); // the other way
  OrTri  afan    (OrTri ef,int i); // afan i times
  OrTri  lmSearch (OrTri ef,VIndex a); // local manifold search for tri having org = a starting with ef

  int degree(OrTri); // degree of org, assuming manifold
	OrTri  fnextBV (OrTri ef, BitVector& bv, 
	                          Bool rev = FALSE, Bool useIntRep = FALSE);

	void fnext(OrTri &ot, const OrTri &ef);

  // indices operation

  Index ix(OrTri,Dim);

  // Linking and unlinking triangles

  void   fsplice (OrTri a, OrTri b);
  void   fmerge  (OrTri d, OrTri e);
	void   flip (OrTri ef);
	void flip (FIndex f);

  OrTri flipEdge(OrTri abc,OrTri& adc,OrTri& bcd,BitVector* triBV=NULL); // assumming manifold, returning the flipped edge
  void insertVertex(OrTri,VIndex,OrTri&,OrTri&,OrTri&,BitVector* triBV=NULL);
  void splitEdge(OrTri abc,VIndex vi,OrTri& cvb,OrTri& cav,OrTri& adv,OrTri& dbv);

  // Testing routines

  Bool  testTriangle (OrTri ef, VIndex x, VIndex y, VIndex z);
  Bool  testOpenTetra (OrTri a, VIndex x, VIndex y, VIndex z, VIndex o);


  // Getting information about the structure
  //
  // These two methods should probably be moved to Delaunay Complex...

  int    numVertices   (void);
  TristNum  tristNum  (void);
  // const Trist_info& trist_info (void);          Is this really needed?

  // Iteration
  //
  // These functions iterate through all (non-deleted) triangles in the
  // triangle array, returning a FIndex for each.  Two iteration methods
  // are provided:  a callback iteration and an external iteration.

  void iter(void (* func) (FIndex ix));

  friend class TristIter;
  TristIter *newIter (void);    // Return a new Trist iterator

  // Packing
  //
  // NOTE:  this will use the pack procedure in the FreeArray, but after
  // this is finished, the fnext_ pointers need to be adjusted.  Trist is
  // responsible for doing this.

  IMap * pack(Bool returnFlag = FALSE);    
  void reindexTriangles (IMap& map);
  void reindexVertices (IMap& newVert);
  // Output operators

  void printInfo (ostream &s);
  friend ostream& operator<< (ostream& s, Trist& trist);
  // assumed to be packed, save hull vector
  Bool binInitOld (BinInput& bi);
  void binDumpOld (BinOutput& bo);
  // don't assume it to be packed, don't save hull and info
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
  int enumVertices (BitVector * triangles = NULL, 
	               OrTriCB cbFunc = NULL, 
				   RgiObject * obj = NULL);
  int enumEdges (BitVector * triangles = NULL, 
	               OrTriCB cbFunc = NULL, 
				   RgiObject * obj = NULL);
  int enumTriangles (BitVector * triangles = NULL, 
	               OrTriCB cbFunc = NULL, 
				   RgiObject * obj = NULL);
  int enumTetra (OrTriCB cbFunc = NULL, 
				   RgiObject * obj = NULL);

  void setPackedIndex(FIndex,EIndex,EIndex,EIndex,TIndex,TIndex);

  class TrImp
  {
  public:
    
    VIndex origin_[3];
    OrTri  fnext_ [6];
	EIndex ei_[3];
	TIndex ti_[3];
    void binDump (BinOutput& bo) { bo.writeInts( (int *)((void *) this), 9); }
    Bool binInit (BinInput& bi) { return bi.readInts((int *)((void *) this), 9); }
  };

protected:
  Magic magic_;
  TristInfo ti_;                // Info about the trist structure
  BitVector *onHull_;            // Triangles on the hull
  FreeArrayDumpable<TrImp> triArr_;     // The triangle array
  Bool debugFlag_;              // Perform testing?
  
  static int vo[6];             // origins wrt triangle version
  static int ve[6];             // enexts  ...

  // Move all variables concerning number of elements to FreeArray:
  //
  //    max_org (?? -- what is this??)
  //    max_triangle, last_triangle, used_triangles, next_reusable_triangle
  //
  // FreeArray should know about packing, and could return a map of the
  //    movement.
  //

  // Other functions that should go elsewhere:
  //
  //   trist_pack -- distributed between FreeArray and this class
  //         - FreeArray method should return a "map" object for use in fixing
  //           specific data issues
  //
  //   trist_peel -- where??  Delaunay Builder?  Is this a general enough
  //                 operation to be in Trist itself?  Maybe...
  //
  //   trist_color_* -- Delaunay Builder
  //
  //   trist_nvx_* -- a new object, if needed...
  //
  //   hull_compare ??  -- This would require trist knows more about
  //                  packing than FreeArray
  //
  //
};



/****************************************************************************/

/*
 * Class TristIter
 *
 * This class is an external iterator for a Trist.  The method
 * first() is used to initialize the iterations, with next() continuing
 * to the next element.  If no next element exists, isDone() is true.
 * curItem() and curIndex() are used to access information about the
 * current item.
 *
 */

 class TristIter : public BitVectorIter
{
public:
  TristIter (Trist& tr) : BitVectorIter(tr.getTriangleBV()) { }
  ~TristIter (void) { }
};


/****************************************************************************/

/*
 * Class TristHullIter
 *
 * External iterator for Convex Hull (Oriented) Triangle of Trist.
 * Very similar to TristIter.
 *
 */

 class TristHullIter : public BitVectorIter
{
public:
  TristHullIter (Trist & tr) : BitVectorIter (tr.getHullBV()) { }
  ~TristHullIter (void) { }
  OrTri curOrTri() { int a = curItem(); return OrTri (a >> 1, a & 1); }
};

#include <geometry/trist.hpp>

#endif //  __GCTRIST_H__
