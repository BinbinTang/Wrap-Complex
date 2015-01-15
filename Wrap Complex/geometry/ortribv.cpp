//
// ortribv.cpp -- A bitvector-type structure for oriented triangles


#include "ortribv.h"

OrTriBV::
OrTriBV (int numTriangles)	
{
	int i;
	for (i = 0; i <= MaxOrientation_; i++)
		bv_[i] = new BitVector (numTriangles);
	for (i = 0; i <= MaxTemp_; i++)
		tempBV_[i] = NULL;
}


/*--------------------------------------------------------------------------*/

OrTriBV::
~OrTriBV (void)	
{
	int i;
	for (i = 0; i <= MaxOrientation_; i++)
		delete bv_[i];
	for (i = 0; i <= MaxTemp_; i++)
		delete tempBV_[i];
}

/*--------------------------------------------------------------------------*/

OrTriBV::
OrTriBV (const OrTriBV & other)
{
	int i;
	for (i = 0; i <= MaxOrientation_; i++)
		bv_[i] = new BitVector (*other.bv_[i]);
	for (i = 0; i <= MaxTemp_; i++)
		tempBV_[i] = NULL;
}

/*--------------------------------------------------------------------------*/

const OrTriBV & OrTriBV::
operator= (const OrTriBV & other)
{
	if (&other == this)
		return *this;

	for (int i = 0; i <= MaxOrientation_; i++)
	{
		delete bv_[i];
		bv_[i] = new BitVector (*other.bv_[i]);
	}

    invalidateBVs ();

	return *this;
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
set (OrTri ef)
{
  bv_[ef.version() % 2]->set (ef.index ());
}

/*--------------------------------------------------------------------------*/

int OrTriBV::
setTest (OrTri ef)
{
  FIndex ix = ef.index ();
  int orientation = ef.version () % 2;

  int alreadySet = bv_[orientation]->test (ix);
  bv_[orientation]->set (ix);
  if (! alreadySet)
	  invalidateBVs ();
  return (! alreadySet);
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
setAll (BitVector & other)
{
	delete bv_[0];
	bv_[0] = new BitVector (other);
	delete bv_[1];
	bv_[1] = new BitVector (other);
	invalidateBVs ();
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
set (Index ix)
{
  bv_[0]->set (ix);
  bv_[1]->set (ix);
}

/*--------------------------------------------------------------------------*/

int OrTriBV::
setTest (Index ix)
{
  int alreadySet = (bv_[0]->test (ix) && bv_[1]->test (ix));
  bv_[0]->set (ix);
  bv_[1]->set (ix);
  
  if (! alreadySet)
	invalidateBVs ();
  return (! alreadySet);
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
clear (OrTri ef)
{
  bv_[ef.version() % 2]->clear (ef.index ());
}

/*--------------------------------------------------------------------------*/

int OrTriBV::
clearTest (OrTri ef)
{
  FIndex ix = ef.index ();
  int orientation = ef.version () % 2;

  int alreadyClear = !bv_[orientation]->test (ix);
  bv_[orientation]->clear (ix);
  if (! alreadyClear)
	invalidateBVs ();
  return (! alreadyClear);
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
clear (Index ix)
{
  bv_[0]->clear (ix);
  bv_[1]->clear (ix);
}

/*--------------------------------------------------------------------------*/

int OrTriBV::
clearTest (Index ix)
{
  int alreadyClear = (! (bv_[0]->test (ix) || bv_[1]->test (ix)));
  bv_[0]->clear (ix);
  bv_[1]->clear (ix);
  if (! alreadyClear)
	invalidateBVs ();
  return (! alreadyClear);
}

/*--------------------------------------------------------------------------*/

int OrTriBV::
test (OrTri ef)
{
  return bv_[ef.version() % 2]->test (ef.index ());
}

/*--------------------------------------------------------------------------*/

int OrTriBV::
test (Index ix)
{
  return (bv_[0]->test (ix) || bv_[1]->test (ix));
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
reset (void)
{
	for (int i = 0; i <= MaxOrientation_; i++)
		bv_[i]->reset ();
    invalidateBVs ();
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
and_op (BitVector& other)
{
	for (int i = 0; i <= MaxOrientation_; i++)
		bv_[i]->and_op (other);	
    invalidateBVs ();
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
or_op (BitVector& other)
{
	for (int i = 0; i <= MaxOrientation_; i++)
		bv_[i]->or_op (other);	
    invalidateBVs ();
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
xor_op (BitVector& other)
{
	for (int i = 0; i <= MaxOrientation_; i++)
		bv_[i]->xor_op (other);	
    invalidateBVs ();
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
sub_op (BitVector& other)
{
	for (int i = 0; i <= MaxOrientation_; i++)
		bv_[i]->sub_op (other);	
    invalidateBVs ();
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
neg_op (void)
{
	for (int i = 0; i <= MaxOrientation_; i++)
		bv_[i]->neg_op ();	
    invalidateBVs ();
}

/*--------------------------------------------------------------------------*/

int OrTriBV::
getNumSet (void)
{
	return getIndexBV ()->computeNumSet ();
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
remap (IMap& map)
{
	for (int i = 0; i <= MaxOrientation_; i++)
		bv_[i]->remap (map);	
    invalidateBVs ();
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
getBVs (BitVector* &bv0, BitVector* &bv1)
{
	bv0 = bv_[0];
	bv1 = bv_[1];
}

/*--------------------------------------------------------------------------*/

BitVector * OrTriBV::
getOrientedBV (int orientation)
{
	Assert (orientation <= MaxOrientation_);
	return bv_[orientation];
}

/*--------------------------------------------------------------------------*/

BitVector * OrTriBV::
getIndexBV (void)
{
	if (tempBV_[0] == NULL)
	{
		tempBV_[0] = new BitVector (*bv_[0]);
		tempBV_[0]->or_op (*bv_[1]);
	}

	return tempBV_[0];
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
invalidateBVs (void)
{
	delete tempBV_[0];
	tempBV_[0] = NULL;
}

/*--------------------------------------------------------------------------*/

void OrTriBV::
binDump (BinOutput& bo)
{
  for(int i=0; i<=MaxOrientation_; i++)
  {
    bv_[i]->binDump(bo);
  }
}

Bool OrTriBV::
binInit (BinInput& bi)
{
  invalidateBVs();
  for(int i=0; i<=MaxOrientation_; i++)
  {
    if(!bv_[i]->binInit(bi))
      return FALSE;
  }
  return TRUE;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

OrEdgeBV::
OrEdgeBV (int numTriangles)	
	: bv_ (numTriangles)
{
}

/*--------------------------------------------------------------------------*/

void OrEdgeBV::
set (OrTri ef)
{
  bv_.set (ef.getIntRep () >> 1);
}

/*--------------------------------------------------------------------------*/

void OrEdgeBV::
clear (OrTri ef)
{
  bv_.clear (ef.getIntRep () >> 1);
}

/*--------------------------------------------------------------------------*/

int OrEdgeBV::
test (OrTri ef)
{
  return bv_.test (ef.getIntRep () >> 1);
}

/*--------------------------------------------------------------------------*/

void OrEdgeBV::
reset (void)
{
	bv_.reset ();
}

/*--------------------------------------------------------------------------*/

void OrEdgeBV::
and_op (OrEdgeBV& other)
{
	bv_.and_op (other.bv_);
}

/*--------------------------------------------------------------------------*/

void OrEdgeBV::
or_op (OrEdgeBV& other)
{
	bv_.or_op (other.bv_);
}

/*--------------------------------------------------------------------------*/

void OrEdgeBV::
xor_op (OrEdgeBV& other)
{
	bv_.xor_op (other.bv_);
}

/*--------------------------------------------------------------------------*/

void OrEdgeBV::
sub_op (OrEdgeBV& other)
{
	bv_.sub_op (other.bv_);
}

/*--------------------------------------------------------------------------*/

void OrEdgeBV::
neg_op (void)
{
	bv_.neg_op ();
}

/*--------------------------------------------------------------------------*/

int OrEdgeBV::
getNumSet (void)
{
	return bv_.getNumSet ();
}

void OrEdgeBV::
binDump (BinOutput& bo)
{
  bv_.binDump(bo);
}

Bool OrEdgeBV::
binInit (BinInput& bi)
{
  return bv_.binInit(bi);
}




