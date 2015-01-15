#include <basic/kdtree.h>
#include <basic/miscmath.h>

KdTree::
KdTree (DoublePArray& rArray, double epsilon, int nodeSize, int leafSize)
: epsilon_(epsilon),
  node_(nodeSize),
  rArray_(rArray)
{
  node_.push(Node());
}

KdTree::
~KdTree (void)
{

}

void KdTree::
reset (void)
{
  node_.reset();
  node_.push(Node());
  divDimBV_.reset();
}

void KdTree::
setEpsilon (double epsilon)
{
  epsilon_ = epsilon; 
}

double KdTree::
getEpsilon (void)
{
  return epsilon_; 
}

//-------------------------------------------------------------------------

int KdTree::
nOfPoints (void)
{
  return rArray_.size();
}

inline int KdTree::
sameValue_(double value1,double value2,double epsilon)
{
	if(epsilon==0.0)
		return value1==value2;
	else
		return fabs(value1 - value2) <= epsilon;
}

inline int KdTree::
samePoint_(double x1[3], double x2[3],double epsilon)
{
	if(epsilon==0.0)
		return (x1[0]==x2[0])&&(x1[1]==x2[1])&&(x1[2]==x2[2]);
	else
  {
    double tmp[3];
    return (vlength2(vsub(x1, x2, tmp)) <= epsilon*epsilon);
  }
}

inline int KdTree::
getDivideDim_ (int i)
{
  return divDimBV_.test(i<<=1)? 2 : divDimBV_.test(++i);
}

inline void KdTree::
setDivideDim_ (int i, int dim)
{
   switch (dim)
   {
   case 0:
     divDimBV_.clear(i<<=1), divDimBV_.clear(++i); break;
   case 1:
     divDimBV_.clear(i<<=1), divDimBV_.set(++i); break;
   case 2:
     divDimBV_.set(i<<=1), divDimBV_.clear(++i); break;
   default:
     Assert_always(0);
   }
}



//-------------------------------------------------------------------------

int KdTree::
addPoint (double x[3])
{
  return addPoint_ (x, 1, TRUE);
}

int KdTree::
addPoint_ (double x[3], int nodeIx, Bool create)
{
  while (nodeIx)
  {
    Node& node = node_[nodeIx];
    Bool greater = x[getDivideDim_(nodeIx)] > node.dividePos;
    int& nextNodeIx = greater? node.greaterChild : node.smallerChild;
    if(nextNodeIx == 0)
    {
      if(!create)
        return 0;
      
      nextNodeIx = rArray_.size() + 1;
      return rArray_.addPoint(x);
    }
    else if(nextNodeIx > 0) // leaf
    {
      double *coord = rArray_[nextNodeIx].getData();
      if (samePoint_(x, coord, epsilon_))
        return nextNodeIx;
      if(!create)
        return 0;
      
      // now create a new node, which will point to a leaf
      int newNodeIx = node_.size() + 1;
      int oldLeafIx = nextNodeIx;
      nextNodeIx = -newNodeIx;
      Node newNode;
      int i=0, divideDim = 0;
      // find dividePos and divideDim
      double diff = fabs(x[i] - coord[i]);
      double maxDiff = diff;
      for(i=1; i<3; i++)
      {
        diff = fabs(x[i] - coord[i]);
        if(diff > maxDiff)
          diff = maxDiff, divideDim = i;
      }
      newNode.dividePos = 0.5*(x[divideDim] + coord[divideDim]);
      // create a new leaf
      int newLeafIx = rArray_.size() + 1;
      if (x[divideDim] > newNode.dividePos)
        newNode.greaterChild = newLeafIx, newNode.smallerChild = oldLeafIx;
      else
        newNode.smallerChild = newLeafIx, newNode.greaterChild = oldLeafIx;
      
      rArray_.addPoint(x);
      node_.push(newNode);
      setDivideDim_(node_.size(), divideDim);
      return newLeafIx;
    }
    else // negative - next node
    {
      // try to find alternative
      if (epsilon_ && sameValue_(x[getDivideDim_(nodeIx)], node.dividePos, epsilon_))
      {
        int altNodeIx = greater? node.smallerChild : node.greaterChild;
        int leafIx = 0;
        if(altNodeIx < 0) 
          leafIx = addPoint_(x, -altNodeIx, FALSE); // loop through nodes
        else if(altNodeIx > 0)
        {
          if(samePoint_(x, rArray_[altNodeIx].getData(), epsilon_))
            leafIx = altNodeIx;
        }
        
        if(leafIx)
          return leafIx;
      }
      nodeIx = -nextNodeIx;
    }
  }
  Assert(0);
  return 0;
}


