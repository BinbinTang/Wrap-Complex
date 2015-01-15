/*
 * comp.h -- C++ Header file for the Complex class
 *
 * author -- Mike Facello
 *
 * date -- 10/24/96
 */

#ifndef __RGI_COMP_H__
#define __RGI_COMP_H__

#include <basic/dumpable.h>
#include <basic/tritype.h>
#include <basic/callbackobject.h>
#include <li/lipoints.h>
#include <geometry/vertarray.h>
#include <geometry/simplex.h>
#include <geometry/ksimplex.h>
#include <geometry/geomutil.h>
#include <geometry/bvtag.h>
#include <geometry/tristmodifier.h>


class ModifiedTriInfo;
class CompIter;
class Cofaces;
class OrTriBV;
class ComplexEdgeSet;

typedef int (RgiObject::*SimplexCB) (Simplex);
typedef int (RgiObject::*KsimplexCB) (Ksimplex);

/************************************************************************/

/*
 * Class Complex 
 *
 * This is an abstract base class for Complex classes.
 *
 */


class Complex : public TristModifier
{
public:
  
  Complex (void);
  Complex (Trist * tr);
  Complex (Trist * tr, LI_Converter& converter);
  virtual ~Complex (void);
  BitVectorPool& getBVPool (void);
  virtual Trist * cloneTrist (void) = 0;
  virtual RealPArray * getPArray (void) = 0;
  virtual RealPArray * getNewPArray (void) = 0; // User always gets real coords
                                                // User is responsible for
                                                // deallocation
	LI_Converter& getConverter (void) { return converter_; }
	void setConverter (LI_Converter& other) { converter_ = other; }
	void setPrecision (int lMax, int pMax);
	void getPrecision (int& lMax, int& pMax);

  virtual void getCoords (VIndex ix, double *coords) = 0;
       // coords should be an array of at least length 3.  The coords
       //      will be loaded into this array.

  virtual Bool cofaces (Ksimplex sigma, Dim k, 
               KsimplexCB, RgiObject * obj) 
							 {Assert_always (0); return TRUE;}
  virtual Bool cofaces (Simplex sigma, Dim k, 
               SimplexCB, RgiObject * obj)
               { Assert_always (0); return TRUE;}
	virtual Bool enumSimplicies (Dim k, 
               SimplexCB, RgiObject * obj) 
							 { Assert_always(0); return TRUE;}
  virtual Bool enumKsimplicies (Dim k, 
               KsimplexCB, RgiObject * obj)
               { Assert_always (0); return TRUE;}

  void getNeighborhood (OrTri ef, int depth, OrTriBV & neighbors);

  virtual int numTriangles (void);
  virtual int numClosedTri (void) = 0;
  virtual int numCompleteTri (void) = 0;
  virtual int numVertices (void);
  virtual int numVertices (BitVector * triangles);
  virtual int numEdges (void);
  virtual int numEdges (BitVector * triangles);
  virtual int numTetra (void);

  virtual Bool inComplex (FIndex f) = 0;
  virtual int inBoundary  (Simplex sigma) { Assert_always (0); return FALSE; }
  virtual int inComplex   (Simplex sigma) { Assert_always (0); return FALSE; }
  virtual int isPrincipal (Simplex sigma) { Assert_always (0); return FALSE; }
  virtual int facesOutward  (Ksimplex sigma) { Assert_always (0); return FALSE; }
  virtual OrTri FnextComplete (OrTri ef);
  virtual OrTri FnextClosed (OrTri ef);
  virtual Bool getPartialDisk (OrTri ef, StackBV& tag);
  virtual Bool getPartialDisk (OrTri ef, OrTriStack& s);
  virtual Bool getCompleteStar (OrTri ef, StackBV& tag);
  virtual Bool getVertBoundary (OrTri ef, OrTri &efBd1, OrTri &efBd2, ComplexEdgeSet * edges = NULL);

  virtual BitVector & getOrientBV (void);
	virtual OrTri getOrientedOrTri (FIndex f) { return OrTri(f, 0); }
  virtual Bool isOutside (OrTri ef) { return !(ef.version() & 1); }

  virtual void getOrientedVertices (Simplex sigma,
				    int & v0, int & v1, int & v2) 
                     { Assert_always (0); }
  virtual void getOrientedVertices (Ksimplex sigma,
				    int & v0, int & v1, int & v2)
                     { Assert_always (0); }
  virtual void getVertices (Simplex sigma,  int & v0, int & v1, int & v2);
  virtual void getVertices (Ksimplex sigma, int & v0, int & v1, int & v2);
  virtual void getVertices (Ksimplex sigma, VertArray & v);

  virtual Cofaces * getCofacesComplete (void)
     { Assert_always (0);  return ((Cofaces *) NULL); }
  virtual Cofaces * getCofacesClosed (void)
     { Assert_always (0);  return ((Cofaces *) NULL); }
  virtual BitVector& getFaceBV (TriangleType type)
    { BitVector * bv = new BitVector; Assert_always (0); return *bv; }
  ModifiedTriInfo * getModifiedTriInfo (void) { return modtri_; }
  void maintainModifiedTriInfo (Bool doMaintain = TRUE);
  Bool getBBox(double min[3], double max[3]);
  double getMaxBBoxLength (void);
  double getMinBBoxLength (void);
  void invalidateBBox (void);
protected:
  BitVector * orientBV_;
  ModifiedTriInfo * modtri_;	
private:
  void computeBBox_ (void);
  BitVectorPool bvpool_;
  
	LI_Converter converter_;
  double min_[3], max_[3];
};

/************************************************************************/

/*
 * Class CompIter
 *
 * This class is an abstract base class for external iterators for a 
 * Complex.  The method first() is used to initialize the iterations, 
 * with next() continuing to the next element.  If no next element exists, 
 * isDone() is true.  curItem() is used to access information about the
 * current item.
 *
 */

class CompIter
{
public:
  CompIter (void) {}
  
  // Iteration operations -- these just call the FArrayIter methods

  virtual void first(void) = 0;
  virtual void next(void) = 0;
  virtual int isDone(void) = 0;     // This is true when the current element 
                                //     is not valid
  virtual Simplex curItem(void) = 0;

private:
  //  Complex& comp_;
};


/************************************************************************/

/*
 * Class CompBuilder
 *
 *
 *
 */

class CompBuilder
{
public:
  CompBuilder (void) {}            // Build a new complex
  //  CompBuilder (Complex& comp) {}   // Modify an existing complex
                                   //   (Is this common to all complexes???)

  // Get the complex

  virtual Complex *getComplex (void) = 0;    // Get the final complex when it's
                                             //   finished.
};

#endif // __COMP_H__
