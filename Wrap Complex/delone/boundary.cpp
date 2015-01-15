
#include "boundary.h"

//----------------------------------------------------------------------------

DBoundary::
DBoundary (DComplex * dc)
  : TristModifier(dc->getTrist()),
	  dc_ (dc),
		obj_(NULL),
		cbf_(NULL),
    vertTag_(NULL),
    edgeTag_(NULL),
    reportSimplex_(NULL)
{
}

//----------------------------------------------------------------------------

int DBoundary::
reportSimplex3_ (OrTri ef)
{
  return (obj_->*cbf_)(dc_->createSimplex (Sym_ (ef), 3));
}

//----------------------------------------------------------------------------

int DBoundary::
reportSimplex2_ (OrTri ef)
{
  return (obj_->*cbf_)(Simplex(ef.index(), 2));
}

//----------------------------------------------------------------------------

int DBoundary::
reportSimplex1_ (OrTri ef)
{
  ef = Sym_ (ef);
	for (int i=0; i<3; i++, ef = Enext_(ef))
	{
    int ix = dc_->getIndex(ef, 1);
		if (edgeTag_->test(ix)) continue;
		if (!(obj_->*cbf_)(Simplex(ix, 1)))
		  return 0;
		edgeTag_->set(ix);
	}
	return 1;
}

//----------------------------------------------------------------------------

int DBoundary::
reportSimplex0_ (OrTri ef)
{
  ef = Sym_ (ef);
	for (int i=0; i<3; i++, ef = Enext_(ef))
	{
    VIndex v = Org_(ef);
		if (vertTag_->test(v)) continue;
		if (!(obj_->*cbf_)(Simplex(v, 0)))
		  return 0;
		vertTag_->set(v);
	}
	return 1;
}

//----------------------------------------------------------------------------

int DBoundary::
reportSimplexAll_ (OrTri ef)
{
  return (reportSimplex0_ (ef) &&
	        reportSimplex1_ (ef) &&
          reportSimplex2_ (ef));
}

//----------------------------------------------------------------------------

inline int DBoundary::
enumerateBoundary_ (void)
{
  int ix;
  BitVectorIter iter(dc_->getTrist()->getHullBV());
	for (iter.first(); !iter.isDone(); iter.next())
	{
    ix = iter.curItem();
		if (!(this->*reportSimplex_)(OrTri(ix >> 1, ix & 1)))
		  return 0;
	}
  return 1;
}

//----------------------------------------------------------------------------

int DBoundary::
bdEnum (SimplexCB cbf, RgiObject * obj)
{
  cbf_ = cbf; obj_ = obj;
  BVTag vTag(dc_->getBVPool()); vertTag_ = &vTag;
  BVTag eTag(dc_->getBVPool()); edgeTag_ = &eTag;
	reportSimplex_ = &DBoundary::reportSimplexAll_;
  return enumerateBoundary_ ();
}

//----------------------------------------------------------------------------

int DBoundary::
kBdEnum (Dim k, SimplexCB cbf, RgiObject * obj)
{
  cbf_ = cbf; obj_ = obj;
  BVTag tag(dc_->getBVPool());
	switch (k)
	{
	 case 0:
	   reportSimplex_ = &DBoundary::reportSimplex0_;
		 vertTag_ = &tag;
		 break;
	 case 1:
	   reportSimplex_ = &DBoundary::reportSimplex1_;
		 edgeTag_ = &tag;
		 break;
	 case 2:
	   reportSimplex_ = &DBoundary::reportSimplex2_;
		 break;
	 case 3:
	   reportSimplex_ = &DBoundary::reportSimplex3_;
		 break;
   default: Assert_always(0);
	}
  return enumerateBoundary_ ();
}

//----------------------------------------------------------------------------
