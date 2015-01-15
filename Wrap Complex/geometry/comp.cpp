#include <geometry/comp.h>
#include <geometry/modtrinfo.h>
#include <geometry/ortribv.h>
#include <geometry/edgeset.h>

Complex::
Complex (void) 
: orientBV_ (NULL),
  modtri_(NULL),
  converter_ (0, 0)
{ 
  invalidateBBox();
}

Complex::
Complex (Trist * tr) 
: TristModifier(tr),
  orientBV_ (NULL),
  modtri_(NULL),
  converter_ (0, 0)
{ 
  invalidateBBox();
}

Complex::
Complex (Trist * tr, LI_Converter& converter)
: TristModifier(tr),
  orientBV_ (NULL),
  modtri_(NULL),
  converter_(converter)
{
  invalidateBBox();
}

Complex::
~Complex (void) 
{ 
  delete modtri_;
	delete orientBV_;
}

BitVectorPool& Complex::
getBVPool (void)
{
  return bvpool_;
}

void Complex::
setPrecision (int lMax, int pMax)
{
  converter_.setPrecision(lMax, pMax);
}

void Complex::
getPrecision (int& lMax, int& pMax)
{
  converter_.getPrecision(lMax, pMax);
}

//----------------------------------------------------------------------

void Complex::
getNeighborhood (OrTri ef, int depth, OrTriBV & neighbors)
{
  OrTriStack stack;
  OrTri curEF, tmpEF;
  
  int numCurLevel;
  int curLevel;

  Simplex sigma;
  int i;
  
  sigma = Simplex (ef.index (), 2);
  Assert (inBoundary (sigma));
  
  curLevel = 0;
  numCurLevel = 1;
  stack.push (ef);
  neighbors.set (ef);

  while (! stack.empty ())
  {
    if (numCurLevel == 0)
    {
      curLevel++;
      numCurLevel = stack.size ();
    }
    
    // Find neighbors of top triangle and put them on the stack
    curEF = stack.pop ();
    numCurLevel --;

    for (i = 0; i < 3; i++)
    {
      OrTri curTri = Fnext_ (curEF);
      while (curTri != curEF)
      {
        tmpEF = Sym_ (curTri);
        sigma = Simplex (tmpEF.index (), 2);
        if (inBoundary (sigma))
        {
          if (! neighbors.test (tmpEF))
          {
            neighbors.set (tmpEF);
            if (curLevel < depth)
              stack.push (tmpEF);
          }
          break;   // Found the adjacent neighbor -- get out
        }
        curTri = Fnext_ (curTri);
      }

      curEF = Enext_ (curEF);
    }
  }
}

void Complex::
invalidateBBox (void)
{
  // max less than min - invalid state is encoded
  min_[0] = min_[1] = min_[2] = MAXFLOAT;
  max_[0] = max_[1] = max_[2] = -MAXFLOAT;
}

void Complex::
computeBBox_ (void)
{
  BVTag usedVert(getBVPool());
  double vx[3];
  RealPArray * rArray = getPArray();
  BitVectorIter iter(getFaceBV(COMPLETE_TRI));
  for(iter.first(); !iter.isDone(); iter.next())
  {
    OrTri ef(iter.curItem(), 0);
    for(int i=0; i<3; i++, ef = ef.enext())
    {
      VIndex v = Org_(ef);
      if(usedVert.test(v))
        continue;
      usedVert.set(v);
      rArray->getPoint(v, vx);
      for(int j=0; j<3; j++)
      {
        if(vx[j] < min_[j]) min_[j] = vx[j];
        if(vx[j] > max_[j]) max_[j] = vx[j];
      }
    }
  }
}

Bool Complex::
getBBox (double min[3], double max[3])
{
  if(max_[0] < min_[0])
  {
    computeBBox_();
  }
  vcopy(min_, min), vcopy(max_, max);
  return TRUE;
}

double Complex::
getMaxBBoxLength (void)
{
  if(max_[0] < min_[0])
  {
    computeBBox_();
  }
  double bbox[3];
  vsub(max_, min_, bbox);
  return MAX(bbox[0], bbox[1], bbox[2]);
}

double Complex::
getMinBBoxLength (void)
{
  if(max_[0] < min_[0])
  {
    computeBBox_();
  }
  double bbox[3];
  vsub(max_, min_, bbox);
  // handle degenerate cases first
  if(bbox[0]==0)
    return MIN(bbox[1], bbox[2]);
  if(bbox[1]==0)
    return MIN(bbox[0], bbox[2]);
  if(bbox[2]==0)
    return MIN(bbox[0], bbox[1]);
  // general case
  return MIN(bbox[0], bbox[1], bbox[2]);
}


//----------------------------------------------------------------------

int Complex::
numTriangles (void)
{
  return getTrist()->numTri();
}



//----------------------------------------------------------------------

int Complex::
numVertices (void)
{
  return enumVertices();
}

//----------------------------------------------------------------------

int Complex::
numEdges (void)
{
  return enumEdges();
}

//----------------------------------------------------------------------

int Complex::
numEdges (BitVector * triangles)
{
  return triangles? enumEdges(triangles) : numEdges();
}

//----------------------------------------------------------------------

int Complex::
numTetra (void)
{
  return (2*tr_->numTri() - tr_->numHullTri())/4;
}

//----------------------------------------------------------------------

int Complex::
numVertices (BitVector * triangles)
{
  return triangles? enumVertices(triangles): numVertices();
}

//----------------------------------------------------------------------

void Complex::
getVertices (Simplex sigma, int & v0, int & v1, int & v2)
{
  Trist *tr = getTrist ();

  tr->triangle (OrTri (sigma.getIndex (), 0), v0, v1, v2);
}

//----------------------------------------------------------------------

void Complex::
getVertices (Ksimplex sigma, int & v0, int & v1, int & v2)
{
  Trist *tr = getTrist ();

  tr->triangle (sigma.getFace (), v0, v1, v2);
}

//----------------------------------------------------------------------

void Complex::
getVertices (Ksimplex sigma, VertArray & v)
{ 
  Trist *tr = getTrist ();

  tr->getVertices (sigma.getFace (), sigma.getDim (), v); 
}

//----------------------------------------------------------------------

OrTri Complex::
FnextComplete ( OrTri ef)
{
   Trist * tr = getTrist();
   OrTri ef1 = ef;
   do {
     ef1 = tr->fnext(ef1);
     Simplex sigma(ef1.index(), 2);
     if (inBoundary(sigma))
       break;
   } while (ef1 != ef);
   return ef1;
}

//----------------------------------------------------------------------

OrTri Complex::
FnextClosed ( OrTri ef)
{
   Trist * tr = getTrist();
   OrTri ef1 = ef;
   do {
     ef1 = tr->fnext(ef1);
     Simplex sigma(ef1.index(), 2);
     if (inBoundary(sigma) && !isPrincipal(sigma))
       break;
   } while (ef1 != ef);
   return ef1;
}

//----------------------------------------------------------------------

Bool Complex::
getCompleteStar (OrTri ef, StackBV& tag)
{
  if(!inBoundary(Simplex(ef.index(), 2)))
    return FALSE;
  tr_->getCompleteStar(ef, tag);
  StackBVIter iter(tag);
  for(iter.first(); !iter.isDone(); iter.next())
  {
    FIndex f = iter.curItem();
    if(!inBoundary(Simplex(f, 2)))
      tag.clear(f);
  }
  return TRUE;
}

Bool Complex::
getPartialDisk (OrTri ef, StackBV& tag)
{
  if(!inBoundary(Simplex(ef.index(), 2)))
    return FALSE;

  Bool isBd = FALSE;
  OrTri ef1 = ef;
  do {
    tag.set(ef1.index());
    OrTri ef2 = FnextComplete(ef1.sym().enext());
    isBd = (ef2.index()==ef1.index() || FnextComplete(ef2).index()!=ef1.index());
    ef1 = ef2;
  } while(ef1 != ef && !isBd);

  if(isBd)
  {
    ef1 = FnextComplete(ef);
    isBd = ef1.index()==ef.index() || FnextComplete(ef1).index()!=ef.index();
    while(!isBd)
    {
      tag.set(ef1.index());
      OrTri ef2 = FnextComplete(ef1.sym().enext());
      isBd = ef2.index()==ef1.index() || FnextComplete(ef2).index()!=ef1.index();
      ef1 = ef2;
    }
  }
  return isBd;
}

//----------------------------------------------------------------------

Bool Complex::
getPartialDisk (OrTri ef, OrTriStack& s)
{
  Bool isBd = FALSE;
  OrTri ef1 = ef;
  do {
    s.push(ef1);
    OrTri ef2 = FnextComplete(ef1.sym().enext());
    isBd = (ef2.index()==ef1.index() || FnextComplete(ef2).index()!=ef1.index());
    ef1 = ef2;
  } while(ef1 != ef && !isBd);

  if(isBd)
  {
    ef1 = FnextComplete(ef);
    isBd = ef1.index()==ef.index() || FnextComplete(ef1).index()!=ef.index();
    while(!isBd)
    {
      s.push(ef1);
      OrTri ef2 = FnextComplete(ef1.sym().enext());
      isBd = ef2.index()==ef1.index() || FnextComplete(ef2).index()!=ef1.index();
      ef1 = ef2;
    }
  }
  return isBd;
}

Bool Complex::
getVertBoundary (OrTri ef, OrTri &efBd1, OrTri &efBd2, ComplexEdgeSet * edges)
{
  Bool isBd = FALSE;
  OrTri ef1 = ef;
  do {
    OrTri ef2 = FnextComplete(ef1.sym().enext());
    isBd = (ef2.index()==ef1.index() || FnextComplete(ef2).index()!=ef1.index()) ||
           (edges && edges->test(ef2));
    ef1 = ef2;
  } while(ef1 != ef && !isBd);

  if(isBd)
  {
    efBd1 = ef1;
    ef1 = FnextComplete(ef);
    isBd = ef1.index()==ef.index() || FnextComplete(ef1).index()!=ef.index();
    while(!isBd)
    {
      OrTri ef2 = FnextComplete(ef1.sym().enext());
      isBd = ef2.index()==ef1.index() || FnextComplete(ef2).index()!=ef1.index() ||
             (edges && edges->test(ef2));
      ef1 = ef2;
    }

    efBd2 = ef1;
  }
  return isBd;
}

//----------------------------------------------------------------------

BitVector& Complex::
getOrientBV (void)
{
	if (orientBV_ == NULL)
		orientBV_ = new BitVector;
	return *orientBV_;
}

//-------------------------------------------------------------------

void Complex::
maintainModifiedTriInfo (Bool doMaintain)
{
  if (!doMaintain)
  {
	delete modtri_;
	modtri_ = NULL;
	return;
  }

  if (!modtri_)
  {
    Trist * tr = getTrist();
    int size = tr ? tr->maxTri() : 20;
    modtri_ = new ModifiedTriInfo (size);
  }
  else
    modtri_->reset();
}

//-------------------------------------------------------------------

