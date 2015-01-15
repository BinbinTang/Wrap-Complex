/*
 * dcomp.C -- Methods for the Delaunay Complex class
 *
 * author -- Mike Facello
 *
 * date -- 12/9/96
 */

#include <delone/dcomp.h>
#include <delone/dcompiter.h>
#include <delone/boundary.h>

#define DCOMPLEX_MAGIC 4564560

//-------------------------------------------------------------------------
/*---------------------------- CLASS DCOMPLEX ---------------------------*/
//-------------------------------------------------------------------------

DComplex::
DComplex (void)
  : magic_(DCOMPLEX_MAGIC),
		rArray_(NULL),
    ix_ (NULL),
    classMethod_(CLASSIFY_ENUM),

    // private handlers
//    centered_(this),
//    ceflow_(this),
//    confident_(this),
//    equivocal_(this),

    // function pointers
// Alan Comment
//    equivocalP_ (NULL),
//    confidentP_ (NULL),
//    centeredP_ (NULL),
    uPredP_ (NULL),
    ceFlowP_ (NULL),
    ldSuccP_ (NULL),
    ecFlowP_ (NULL)
{
  wse_ = new WSizeEvaluator;
//  ca_ = new ClassArray (this);
}

//-------------------------------------------------------------------------

#ifndef WIN32
/*
 * Due to compilation restrictions on unix. See header file. --swp
 */
Ksimplex DComplex::createKsimplex (Simplex sigma)
{ return Ksimplex (getFace (sigma), sigma.getDim ()); }
#endif

//-------------------------------------------------------------------------

DComplex::
DComplex (Trist  * tr, RealPArray * rArray, LI_Converter& converter)
  : Complex (tr, converter), magic_(DCOMPLEX_MAGIC),
		rArray_(rArray),
    classMethod_(CLASSIFY_ARRAY)

    // private handlers
//    centered_(this),
//    ceflow_(this),
//    confident_(this),
//    equivocal_(this),

    // function pointers
//    equivocalP_ (&DComplex::arrEquivocal),
//    confidentP_ (&DComplex::arrConfident),
//    centeredP_ (&DComplex::arrCentered),
//    uPredP_ (&DComplex::arrGetUpred),
//    ceFlowP_ (&DComplex::arrCeFlow),
//    ldSuccP_ (&DComplex::arrLdSucc),
//    ecFlowP_ (&DComplex::arrEcFlow)
{
	ix_ = NULL;
  wse_ = new WSizeEvaluator;
//  ca_ = new ClassArray (this);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

// The Big Three -- Destructor, Copy constructor, and Assignment

DComplex::
~DComplex (void)
{
  delete tr_;
  delete rArray_;
  delete ix_;
//  delete ca_;
  delete wse_;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

// Input/Output

Bool DComplex::
binInit (BinInput& bi)           // Binary Input
{
  // progress indicators
  int beginAll, beginArray, beginTrist, endTrist, endAll;
  getProgressBounds(beginAll, endAll);
  beginArray = beginAll+1; 
  endTrist = endAll - 4; 
  beginTrist = (beginArray + endTrist)/2;
  delete rArray_;
	rArray_ = NULL;
	delete tr_; tr_ = NULL;
  delete wse_; wse_ = NULL;

  if (!magic_.binInit(bi))
    return FALSE;
  {
    LI_PArray pArray(1);
    setProgressCBto (&pArray);
    pArray.setProgressBounds (beginArray, beginTrist);
    if (!(rArray_ = pArray.getNewRealArray(bi)))
      return FALSE;
    
    setConverter (*pArray.getConverter());
  }

	tr_ = new Trist;
  wse_ = new WSizeEvaluator;
  
  setProgressCBto (tr_);
  tr_->setProgressBounds (beginTrist, endTrist);
  if (!tr_->binInit(bi))
    return FALSE;
  classMethod_ = (ClassificationMethod) bi.readInt();
  int useWeights = bi.readInt();
  useWeightedGeometry(useWeights);
  delete ix_;
  ix_  = new IndexHandler (this); // initialize indices
  if (!ix_->binInit(bi))
  {
    delete ix_;
    ix_ = NULL;
    return FALSE;
  }
  ix_->init();
  return progressUpdate(1.0f);
}

//-------------------------------------------------------------------------

void DComplex::
binDump (BinOutput& bo)       // Binary Output
{
 int beginAll, beginArray, beginTrist, endTrist, endAll;
 getProgressBounds(beginAll, endAll);
 beginArray = beginAll+1; 
 endTrist = endAll - 1; 
 beginTrist = (beginArray + endTrist)/2;

 magic_.binDump(bo);

 {
   int lMax, pMax;
   getPrecision(lMax, pMax);
   LI_PArray pArray(1, lMax + pMax, pMax); // dummy pArray - only for saving
   setProgressCBto (&pArray);
   pArray.setProgressBounds (beginArray, beginTrist);
   pArray.binDump(rArray_, bo);
 }

 setProgressCBto (tr_);
  tr_->setProgressBounds (beginTrist, endTrist);
 tr_->binDump(bo);
 bo.writeInt((int) classMethod_);
 bo.writeInt(DT);
 ix_->binDump(bo);
 progressUpdate(1.0f);
}

//-------------------------------------------------------------------------

Bool DComplex::
isValid (int magic)
{
  return (magic==DCOMPLEX_MAGIC);
}


//-------------------------------------------------------------------------

ostream&
operator<< (ostream& s, DComplex& comp)
{
  // This will become more sophisticated later...
  
  TristIter tIter (*(comp.tr_));
  VIndex a, b, c;
  OrTri curEF;

  s << "/--------------------------------------------------------------/\n";
  s << "Number of vertices: " << comp.rArray_->size ();
  s << "Number of triangles: " << comp.tr_->numTri ();
  s << "The Triangulation\n\n";
    
  tIter.first ();
  while (! tIter.isDone ())
    {
      curEF = OrTri (tIter.curItem (), 0);
      comp.tr_->triangle (curEF, a, b, c);
      basic_isort3 ((int *)(&a), (int *)(&b), (int *)(&c));
      s << (int) a << " " << (int) b << " " << (int) c << endl;
      tIter.next ();
    }

  s << "/--------------------------------------------------------------/\n";

  return s;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

void DComplex::
printSimplex (ostream& s, Ksimplex sigma)
{
  s << sigma;
  printSimplex (s, sigma.getFace (), sigma.getDim ());
}

//-------------------------------------------------------------------------

void DComplex::
printSimplex (ostream& s, OrTri ef, Dim dSigma)
{
  VertArray v;

  getSortedVertices (ef, dSigma, v);

  s << "[" << (int) ef.getIntRep () << dSigma << " (" << (int) v.getVertex (1);
  if (dSigma > 0) s << ", " << (int) v.getVertex (2);
  if (dSigma > 1) s << ", " << (int) v.getVertex (3);
  if (dSigma > 2) s << ", " << (int) v.getVertex (4);
  s << ")]";
}

//-------------------------------------------------------------------------

void DComplex::
printlnSimplex (ostream& s, Ksimplex sigma)
{
  printSimplex (s, sigma);
  s << endl;
}

//-------------------------------------------------------------------------

void DComplex::
printlnSimplex (ostream& s, OrTri ef, Dim dSigma)
{
  printSimplex (s, ef, dSigma);
  s << endl;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

// Point accessors

RealPArray *DComplex::
getNewPArray (void)
{
  return (rArray_->getNewRealPArray ());
}

//-------------------------------------------------------------------------

void DComplex::
getCoords (VIndex ix, /* output */ double *coords)
{
	rArray_->getPoint(ix, coords);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

// Statistics

int DComplex::
numTriangles (void)
{ 
  return tr_->numTri (); 
}

//-------------------------------------------------------------------------

int DComplex::
numClosedTri (void)
{ 
  return tr_->numHullTri (); 
}

//-------------------------------------------------------------------------

int DComplex::
numCompleteTri (void)
{
  return tr_->numHullTri ();
}

//-------------------------------------------------------------------------

int DComplex::
numEdges (void)
{
  int V = rArray_->size();
	int F = tr_->numTri();
	int T = numTetra();
	return (V - 1 + F - T);
}

//-------------------------------------------------------------------------

int DComplex::
numVertices (void)
{
  return rArray_->size();
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

Bool DComplex::
inComplex (Simplex sigma)
{
  Touch_args (sigma.getIntRep ());
  return TRUE;
}

//-------------------------------------------------------------------------

Bool DComplex::
isBoundarySimplex (Simplex sigma)
{
  Dim dSigma = sigma.getDim ();

  if (dSigma == 3) {
    isBoundary_ = FALSE;
    cofaces (sigma, 2, (SimplexCB) &DComplex::testBoundaryB_, this);
    return isBoundary_;
  }

  if (dSigma == 2) return tr_->isOnHull (getFace (sigma).index ());

  isBoundary_ = FALSE;
  cofaces (sigma, 2, (SimplexCB) &DComplex::testBoundary_, this);
  return isBoundary_;
}

//-------------------------------------------------------------------------

OrTri DComplex::
boundaryFacet (void)
{
	int ix = tr_->getHullBV().getNextSet(0);
	return OrTri (ix >> 1, ix & 1);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

int DComplex::
testBoundary_ (Simplex sigma)
{
  if (tr_->isOnHull (getFace (sigma).index ()))
  {
    isBoundary_ = TRUE;
    return 0;
  }
	return 1;
}

//-------------------------------------------------------------------------

int DComplex::
testBoundaryB_ (Simplex sigma)
{
  if (tr_->isOnHull (getFace (sigma).index ()))
    isBoundary_ = TRUE;
	return 1;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

// External Iterators

CompIter *DComplex::
newIter (Simplex sigma) 
{ 
  return new DCompAllFacesIter (*this, sigma); 
}

//-------------------------------------------------------------------------

// Iterate over all d-dimensional faces (or cofaces) of sigma
CompIter *DComplex::
newIter (Simplex sigma, Dim d)
{
  if (d <= sigma.getDim())
    return (new DCompFacesIter (*this, sigma, d));
  else
    return (new DCompCoFacesIter (*this, sigma, d));
}

//-------------------------------------------------------------------------

// Iterate over all simplices of the convex as indicated by the
//     initialization variables
CompIter *DComplex::
newIter (Bool v, Bool e, Bool f, Bool t,
	 Bool bd, Bool interior)
{
  return (new DCompUnivIter (*this, v, e, f, t, bd, interior));
}


//-------------------------------------------------------------------------

void
DComplex::
useWeightedGeometry(int type)
{
  // do nothing for now
}

//-------------------------------------------------------------------------
