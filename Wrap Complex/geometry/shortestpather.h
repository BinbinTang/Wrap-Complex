/* -------------------------------------

   shortest path finder:

	just find the shortest path on the 
	complex surface

	and it will return the sequence of 
	edges

  author:

	Ho-lun Cheng (hcheng@uiuc.edu)

----------------------------------------*/

#ifndef __RGI_SHORTESTPATHER_H__
#define __RGI_SHORTESTPATHER_H__

#include <basic/bitvector.h>
#include <basic/pqueue.h>
#include <basic/rectsel.h>
#include <geometry/comp.h>
#include <geometry/edgecycleset.h>

class ShortestPather : public TristModifier
{
public:
    ShortestPather (Complex* comp, StackBVTag& usedVx, Bool closed = FALSE, BitVector * mask = NULL);
    ~ShortestPather (void);
    void setMask (Bool closed, BitVector * mask = NULL);
    Bool findShortestPath (OrTri src,OrTri dest, OrEdgeInfoSeq& result, Bool boundary = FALSE);
    Bool findShortestPathToBoundary (OrTri srctri, OrTri& bdtri, double maxDist);
    Bool findShortestPathToSets (OrTri srctri, OrTri& bdtri, 
      ComplexEdgeSet * edges, BitVector * verts, double maxDist); // always use boundary
    void resetInternal (void); // doesn't reset used vertices
    void reset (void);
protected:
    int compareCB_ (VIndex& v1,VIndex& v2);
    virtual double length_ (double x1[3], double x2[3]);
    Complex* comp_;
    StackBVTag *  usedVx_;
    BitVector * mask_;

    StackBVTag traversed_;
    BinaryHeap<VIndex> pq_ ;
    RArray<OrEdgeInfo>   elist_;
    RArray<double> distance_;
    RArray<int>    keyLoc_;  
    IterStack<OrEdgeInfo> temp_, star_;
};


class Shortest2DPather : public ShortestPather
{
public:
  Shortest2DPather(Complex *comp, StackBVTag& usedVx, Bool closed = FALSE, BitVector * mask = NULL);
  void setSelection (PointSelection * rectsel);
protected:
  virtual double length_ (double x1[3], double x2[3]);
  PointSelection trans_;
  Bool useSelection_;
};

#endif // __SHORTEST_PATHER_H__









