#include <basic/rectsel.h>

GeneralPointSelection::
~GeneralPointSelection (void)
{

}

//--------------------------------------------------------------------------------------

GeneralPointSelection * GeneralPointSelection::
createGeneralPointSelection (const GeneralPointSelection& other)
{
  const void * ptr = &other;
  int id = other.getId ();
  if (RectSelection::isValid(id))
    return new RectSelection (*((RectSelection *) ptr));

  if (PolySelection::isValid(id))
    return new PolySelection (*((PolySelection *) ptr));

  if (MultiplePointSelection::isValid(id))
    return new MultiplePointSelection (*((MultiplePointSelection *) ptr));

  if (MaskedSelection::isValid(id))
    return new MaskedSelection (*((MaskedSelection *) ptr));

  return (PointSelection *) NULL;
}

//----------------------------------------------------------------------------

GeneralPointSelection * GeneralPointSelection::
createEmptyGeneralPointSelection (BinInput& bi)
{
  int id = bi.queryInt();
  if (RectSelection::isValid(id))
    return new RectSelection;

  if (PolySelection::isValid(id))
    return new PolySelection;

  if (MultiplePointSelection::isValid(id))
    return new MultiplePointSelection;

  if (MaskedSelection::isValid(id))
    return new MaskedSelection;

  return (PointSelection *) NULL;
}


/******************************************************************
 *                                                                *
 *          class PointSelection                                   *
 *                                                                *
 ******************************************************************/

PointSelection::
PointSelection (void)
               : magic_(SELECTION_MAGIC)
{
  clipPlanes_ = 0;
  selectMode_ = SELECT_NONE;
  BZERO (m_, double, 16);
  m_[0] = m_[5] = m_[10] = m_[15] = 1.0;
}

//--------------------------------------------------------------------------

PointSelection::
PointSelection (double *m)
                : magic_(SELECTION_MAGIC)
{
  clipPlanes_ = 0;
  selectMode_ = SELECT_NONE;
  setupTransformMatrix_ (m);
}

//--------------------------------------------------------------------------

PointSelection::
PointSelection (PointSelection& other)
               : magic_(SELECTION_MAGIC),
			   clipPlanes_ (other.clipPlanes_)
{
  selectMode_ = other.selectMode_;
  BCOPY (other.m_, m_, sizeof (double)*16);
  BCOPY (other.clipPlane_, clipPlane_, sizeof (double)*4);
}

PointSelection::
~PointSelection (void)
{

}

//-------------------------------------------------------------------------

void PointSelection::
setupTransformMatrix_ (double *m)
{
  BCOPY (m, m_, sizeof(double) * 16);
}

//--------------------------------------------------------------------------------------

const double *PointSelection::
getClipPlane (int idx) const
{
	Assert (idx >= 0 && idx < clipPlanes_);
	return clipPlane_ + 4*idx;
}

void PointSelection::
addClipPlane (const double *p)
{
	Assert (clipPlanes_ < 6);
	BCOPY (p, (clipPlane_ + 4*clipPlanes_), (sizeof(double)*4));
	clipPlanes_++;
}

//--------------------------------------------------------------------------------------

Bool PointSelection::
transform (double x, double y, double z, double p[3]) const
{
  double r3 = 1.0/(m_[12]*x+m_[13]*y+m_[14]*z+m_[15]);
  if (r3 == 0)
    return FALSE;

  p[0] = (m_[0]*x + m_[1]*y + m_[2]*z + m_[3]) * r3;
  p[1] = (m_[4]*x + m_[5]*y + m_[6]*z + m_[7]) * r3;
  p[2] = (m_[8]*x + m_[9]*y + m_[10]*z + m_[11]) * r3;
  return TRUE;
}

//--------------------------------------------------------------------------------------

Bool PointSelection::
insideXY (double x, double y) const 
{ return FALSE; }

Bool PointSelection::
inside (double x, double y, double z) const
{
  if (selectMode_ == SELECT_NONE)
    return FALSE;
  if (selectMode_ == SELECT_ALL)
    return TRUE;

  double p[3];
  transform(x, y, z, p);

  if (p[2] < -1)
    return FALSE;
  if (p[2] > 1)
    return FALSE;
  if (selectMode_ == SELECT_ALL)
    return TRUE;

  for (int i=0; i<clipPlanes_; i++)
  {
	double dx = p[0] * clipPlane_[4*i+0];
	double dy = p[1] * clipPlane_[4*i+1];
	double dz = p[2] * clipPlane_[4*i+2];
	double dw =  1.0 * clipPlane_[4*i+3];
	double dot = dx + dy + dz + dw;
	if (dot < 0)
      return FALSE;
  }
  return insideXY (p[0], p[1]);
}

//--------------------------------------------------------------------------------------

Bool PointSelection::
binInit (BinInput& bi)
{
  if (!magic_.binInit(bi))
    return FALSE;
  selectMode_ = (SelectionMode) bi.readInt ();
  bi.readDoubles (m_, 16);
  clipPlanes_ = bi.readInt ();
  bi.readDoubles (clipPlane_, 4*clipPlanes_);
  return TRUE;
}

//--------------------------------------------------------------------------------------

void PointSelection::
binDump (BinOutput& bo)
{
  magic_.binDump(bo);
  bo.writeInt (selectMode_);
  bo.writeDoubles (m_, 16);
  bo.writeInt (clipPlanes_);
  bo.writeDoubles (clipPlane_, 4*clipPlanes_);
}

//--------------------------------------------------------------------------------------

PointSelection * PointSelection::
createEmptyPointSelection (BinInput& bi)
{
 int magic = bi.queryInt();

 if (RectSelection::isValid(magic))
   return new RectSelection;

 if (PolySelection::isValid(magic))
   return new PolySelection;

 if (MaskedSelection::isValid(magic))
   return new MaskedSelection;

 return (PointSelection *) NULL;
}

//--------------------------------------------------------------------------------------

PointSelection * PointSelection::
createPointSelection (PointSelection& other)
{
  void * ptr = &other;
  int id = other.getId ();
  if (RectSelection::isValid(id))
    return new RectSelection (*((RectSelection *) ptr));

  if (PolySelection::isValid(id))
    return new PolySelection (*((PolySelection *) ptr));

  if (MaskedSelection::isValid(id))
    return new MaskedSelection (*((MaskedSelection *) ptr));

  return (PointSelection *) NULL;
}

//--------------------------------------------------------------------------------------


/******************************************************************
 *                                                                *
 *          class RectSelection                                   *
 *                                                                *
 ******************************************************************/

RectSelection::
RectSelection (void)
              : PointSelection (),
                x1_ (0), y1_ (0),
                x2_ (0), y2_ (0)
{
  magic_ = Magic(RECTSELECTION_MAGIC);
}

//--------------------------------------------------------------------------

RectSelection::
RectSelection (int x1, int y1, int x2, int y2, double *m, int order)
               : PointSelection (m),
                 x1_(x1), y1_(y1), 
                 x2_(x2), y2_(y2)
{
  magic_ = Magic(RECTSELECTION_MAGIC);
  selectMode_ = SELECT_REGION;
  if(order) orderCoords_ ();
}

//--------------------------------------------------------------------------

RectSelection::
RectSelection (RectSelection& other)
              : PointSelection (other),
                x1_(other.x1_), y1_(other.y1_), 
                x2_(other.x2_), y2_(other.y2_)
{
  magic_ = Magic(RECTSELECTION_MAGIC);
}

RectSelection::
~RectSelection (void)
{

}

//--------------------------------------------------------------------------

Bool RectSelection::
binInit (BinInput& bi)
{
  if(!PointSelection::binInit(bi))
    return FALSE;
  x1_ = bi.readInt ();
  y1_ = bi.readInt ();
  x2_ = bi.readInt ();
  y2_ = bi.readInt ();
  return TRUE;
}

//--------------------------------------------------------------------------

void RectSelection::
binDump (BinOutput& bo)
{
   PointSelection::binDump(bo);
   bo.writeInt (x1_);
   bo.writeInt (y1_);
   bo.writeInt (x2_);
   bo.writeInt (y2_);
}

//--------------------------------------------------------------------------

void RectSelection::
orderCoords_ (void)
{
  int tmp;
  if (x1_ > x2_)
  {
    tmp = x1_; x1_ = x2_; x2_ = tmp;
  }

  if (y1_ > y2_)
  {
    tmp = y1_; y1_ = y2_; y2_ = tmp;
  }
}

//--------------------------------------------------------------------------

Bool RectSelection::
isValid (int magic)
{
  return (magic == RECTSELECTION_MAGIC);
}

//--------------------------------------------------------------------------

Bool RectSelection::
insideXY (double x, double y) const
{
  if (x <= x1_)
    return FALSE;
  if (x >= x2_)
    return FALSE;
  if (y <= y1_)
    return FALSE;
  if (y >= y2_)
    return FALSE;
  return TRUE;
}

//--------------------------------------------------------------------------

/******************************************************************
 *                                                                *
 *          class PolySelection                                   *
 *                                                                *
 ******************************************************************/

PolySelection::
PolySelection (void)
              : PointSelection (),
                s_ (3)
{
  magic_ = Magic(POLYSELECTION_MAGIC);
}

//---------------------------------------------------------------------------

PolySelection::
PolySelection (double *m)
               : PointSelection (m),
                 s_(3)
{
  magic_ = Magic(POLYSELECTION_MAGIC);
}
               
//-------------------------------------------------------------------------

PolySelection::
PolySelection (PolySelection& other)
              : PointSelection (other),
                s_ (other.s_)
{
  magic_ = Magic(POLYSELECTION_MAGIC);
}

PolySelection::
~PolySelection (void)
{

}

//-------------------------------------------------------------------------

Bool PolySelection::
binInit (BinInput& bi)
{
  if(!PointSelection::binInit (bi))
    return FALSE;
  int size = bi.readInt ();
  s_.resize(size);
  for (int i=1; i <=size; i++)
  {
    MousePoint pnt (bi.readInt (), bi.readInt ());
    s_.push(pnt);
  }
  return TRUE;
}

//-------------------------------------------------------------------------

void PolySelection::
binDump (BinOutput& bo)
{
  PointSelection::binDump (bo);
  bo.writeInt (s_.size());
  IStackIter <MousePoint> iter(s_);
  for (iter.first(); !iter.isDone(); iter.next())
  {
    bo.writeInt(iter.curItem().x);
    bo.writeInt(iter.curItem().y);
  }
}

//-------------------------------------------------------------------------

Bool PolySelection::
isValid (int magic)
{
  return (magic == POLYSELECTION_MAGIC);
}

//-------------------------------------------------------------------------

int PolySelection::
addVertex (int x, int y)
{
  MousePoint pnt(x, y);
  s_.push (pnt);
  return 1;
}

//-------------------------------------------------------------------------

int PolySelection::
addVertices (IterStack<MousePoint>& other)
{
  IStackIter<MousePoint> iter(other);
  for(iter.first(); !iter.isDone(); iter.next())
  {
    MousePoint p = iter.curItem();
    s_.push(p);
  }

  return 1;
}
               

//-------------------------------------------------------------------------

Bool PolySelection::
insideXY (double x, double y) const
{
   // s_ is internal stack of mouse points
   // draw ray up and count number of intersections
   // Assume stack is non-empty, which should be always the case,
   // otherwise selection mode is SELECT_NONE.

   int res = 0;
   double dxi, dyi, dxj, dyj;
   int n = s_.size();
   int i, j = 1;

   dxj = s_[j].x - x;
   dyj = s_[j].y - y;
   for (i = 1; i <= n; i++)
   {
     j = i % n + 1;
	 dxi = dxj;
	 dyi = dyj;
	 dxj = s_[j].x - x;
	 dyj = s_[j].y - y;

	 if (dxi == dxj)
		 continue;

	 if (dxi >= 0)
	 {
	   if (dxj >= 0)
		 continue; 
	 }
	 else // if (dxi < 0)
	 {
	   if (dxj < 0)
	     continue;
	 }
	 // now xi and xj have different signs
	 if ((dyj <= 0) && (dyi <= 0))
		 continue;

	 if ((dyj >= 0) && (dyi >= 0))
	 {
        res++;
		continue;
	 }

	 double d = dxi*dyj - dxj*dyi;

	 if ( dxi > dxj )
	 {
	   if (d > 0)
	   {
		 res++;
		 continue;
	   }
	 }
	 else // if (dxi < dxj)
	 {
	   if (d < 0)
	   {
         res++;
		 continue;
	   }
	 }

   }
   
   return (res % 2);
}

//------------------------------------------------------------------------

/*************************************************************************
 *                                                                       *
 *                      MaskedSelection                                  *
 *                                                                       *
 *************************************************************************/
#define MASKEDSELECTION_MAGIC 1671
MaskedSelection::
MaskedSelection ()
{
	x0_ = y0_ = x1_ = y1_ = 0;
	mask_ = (unsigned char *)0;
	magic_ = Magic(MASKEDSELECTION_MAGIC);
}

MaskedSelection::
MaskedSelection (int x0, int y0, int x1, int y1, unsigned char *mask,
	double *transform) :
	PointSelection (transform)
{
	Assert (x0 <= x1);
	Assert (y0 <= y1);
	x0_ = x0;
	x1_ = x1;
	y0_ = y0;
	y1_ = y1;
	mask_ = new unsigned char[size()];
	memcpy (mask_, mask, size());
}

MaskedSelection::
MaskedSelection (const MaskedSelection &other)
{
	x0_ = other.x0_;
	x1_ = other.x1_;
	y0_ = other.y0_;
	y1_ = other.y1_;
	mask_ = new unsigned char[size()];
	memcpy (mask_, other.mask_, size());
}

MaskedSelection::
~MaskedSelection ()
{
	delete[] mask_;
}

Bool MaskedSelection::
insideXY (double x, double y) const
{
	int ix = (int)floor(x);
	int iy = (int)floor(y);
	if ((ix < x0_) || (ix >= x1_) || (iy < y0_) || (iy >= y1_)) {
		return FALSE;
	}
	return mask_[(iy-y0_)*(x1_-x0_) + (ix-x0_)] != 0;
}

Bool MaskedSelection::
binInit (BinInput& bi)
{
	if (!PointSelection::binInit(bi)) {
		 return FALSE;
	}
	x0_ = bi.readInt ();
	y0_ = bi.readInt ();
	x1_ = bi.readInt ();
	y1_ = bi.readInt ();
	Assert (x0_ <= x1_);
	Assert (y0_ <= y1_);

	delete[] mask_;
	mask_ = new unsigned char[size()];
	return bi.readRawData ((char *)mask_, size());
}

void MaskedSelection::
binDump (BinOutput& bo)
{
	bo.writeInt (x0_);
	bo.writeInt (y0_);
	bo.writeInt (x1_);
	bo.writeInt (y1_);
	bo.writeRawData ((char *)mask_, size());
}

Bool MaskedSelection::
isValid (int magic)
{
	return (magic == MASKEDSELECTION_MAGIC);
}

//------------------------------------------------------------------------

/*************************************************************************
 *                                                                       *
 *                      MultiplePointSelection                           *
 *                                                                       *
 *************************************************************************/
#define MULTISELECTION_MAGIC 7129
MultiplePointSelection::
MultiplePointSelection (void)
: magic_(MULTISELECTION_MAGIC),
  selStack_(1)
{
  selectMode_ = SELECT_REGION;
}

//------------------------------------------------------------------------

MultiplePointSelection::
MultiplePointSelection (MultiplePointSelection& other)
: magic_(MULTISELECTION_MAGIC),
  selStack_(other.numSelections())
{
  selectMode_ = SELECT_REGION;
  IStackIter<GeneralPointSelection *> iter(other.selStack_);
  for (iter.first(); !iter.isDone(); iter.next())
  {
    GeneralPointSelection * srcSel = iter.curItem();
    GeneralPointSelection * newSel = 
       GeneralPointSelection::createGeneralPointSelection (*srcSel);
    selStack_.push(newSel);
  }
}

//-----------------------------------------------------------------------

MultiplePointSelection::
~MultiplePointSelection (void)
{
  reset ();
}

void MultiplePointSelection::
reset (void)
{
  while (!selStack_.empty())
  {
    delete selStack_.pop();
  }
}

//------------------------------------------------------------------------

Bool MultiplePointSelection::
inside (double x, double y, double z) const
{
  Bool isInside = FALSE;
  GeneralPointSelection * item;
  IStackIter<GeneralPointSelection *>
      iter((IterStack<GeneralPointSelection *>&)selStack_);
  for (iter.first(); !iter.isDone(); iter.next())
  {
    item = iter.curItem();
    switch (item->getSelectMode())
    {
    case SELECT_NONE:
      isInside = FALSE;
      break;
    case SELECT_ALL:
      isInside = TRUE;
      break;
    case SELECT_REGION:
      if (!isInside)
        isInside = item->inside(x, y, z);
      break;
    case UNSELECT_REGION:
      if (isInside)
        isInside = !item->inside(x, y, z);
      break;
    default:
      Assert (0);
      break;
    }
  }
  return isInside;
}

//------------------------------------------------------------------------

void MultiplePointSelection::
addSelection (const GeneralPointSelection& sel)
{
  GeneralPointSelection * newSel = 
      GeneralPointSelection::createGeneralPointSelection (sel);
  selStack_.push(newSel);
}

//------------------------------------------------------------------------

Bool MultiplePointSelection::
binInit (BinInput& bi)
{
  if (!magic_.binInit(bi))
    return FALSE;
  int numSel = bi.readInt();
  selStack_.resize(numSel);
  for(int i=1; i<=numSel; i++)
  {
    GeneralPointSelection * pSel = 
      GeneralPointSelection::createEmptyGeneralPointSelection(bi);
    if(!pSel)
      return FALSE;
    if(!pSel->binInit(bi))
      return FALSE;
    selStack_.push(pSel);
  }

  return TRUE;
}

//------------------------------------------------------------------------

void MultiplePointSelection::
binDump (BinOutput& bo)
{
  magic_.binDump(bo);
  bo.writeInt(selStack_.size());
  IStackIter <GeneralPointSelection *> iter(selStack_);
  for (iter.first(); !iter.isDone(); iter.next())
  {
    iter.curItem()->binDump(bo);
  }
}

//------------------------------------------------------------------------

void MultiplePointSelection::
selectAll (void)
{
  reset ();
  RectSelection pSel;
  pSel.selectAll();
  addSelection(pSel);
}

//------------------------------------------------------------------------

void MultiplePointSelection::
deselectAll (void)
{
  reset ();
}

//------------------------------------------------------------------------

Bool MultiplePointSelection::
isValid (int magic)
{
  return (magic == MULTISELECTION_MAGIC);
}
