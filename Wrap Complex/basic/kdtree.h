#ifndef __RGI_KDTREE_H__
#define __RGI_KDTREE_H__

#include <basic/basic.h>
#include <basic/rarray.h>
#include <basic/points.h>
#include <basic/miscmath.h>

//----------------------------------------------------------------------

class KdTree
{
public:
  KdTree (DoublePArray& rArray, double epsilon = 0, int nodeSize = 100, int leafSize = 100);
  ~KdTree (void);
  void reset (void);
  void setEpsilon (double epsilon = 0);
  double getEpsilon (void);
  int nOfPoints (void);
  int addPoint(double x[3]);

public:
  class Node
  {
  public:
    Node (void);
    double dividePos;
    int greaterChild, smallerChild;
  };

protected:
  int sameValue_(double value1, double value2, double epsilon);
  int samePoint_(double x1[3], double x2[3], double epsilon);
  int addPoint_ (double x[3], int nodeIx, Bool create);
  int getDivideDim_ (int i);
  void setDivideDim_ (int i, int dim);
  double epsilon_;
  IterStack<Node> node_;
  BitVector divDimBV_;
  DoublePArray& rArray_;
};

//-------------------------------------------------------------------

inline KdTree::Node::
Node (void) : dividePos(0), greaterChild(0), smallerChild(0) { }


#endif

