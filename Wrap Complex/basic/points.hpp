/*
 * points.C -- Class methods for Points and Point Arrays
 *
 * author -- Mike Facello
 * 
 * date -- 10/9/96
 */

#include <basic/points.h>

/*-----------------------------------------------------------------------*/
/*--------------------------- CLASS Point -------------------------------*/
/*-----------------------------------------------------------------------*/

// Constructors, Destructors, and Assignment

template <class T, class sz>
Point<T, sz>::
Point (void)
{
  // Do nothing...
}

/*-----------------------------------------------------------------------*/

template <class T, class sz>
Point<T, sz>::
Point (Point<T,sz> const &other)
{ 
  coords_ = other.coords_;
}

/*-----------------------------------------------------------------------*/

template <class T, class sz>
Point<T,sz>::
~Point (void)
{
  // Do nothing...
}

/*-----------------------------------------------------------------------*/

template <class T, class sz>
Point<T, sz> const & Point<T, sz>::
operator= (Point<T, sz> const &other)
{ 
  if (this != &other)
  {
    coords_ = other.coords_;
  }
  return (*this);
}

template <class T, class sz>
T * Point<T, sz>::
getData (void)
{
  return coords_.data;
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Indexing operators

template <class T, class sz>
T& Point<T, sz>::operator[] (int index)
{
	Assert (index >= 1 && index <= sz::NUM_COORDS);
  return coords_.data[index-1];
}

/*-----------------------------------------------------------------------*/

template <class T, class sz>
const T& Point<T, sz>::operator[] (int index) const
{
	Assert (index >= 1 && index <= sz::NUM_COORDS);
  return coords_.data[index-1];
}

/*-----------------------------------------------------------------------*/

#ifdef __GNUC__

template <class T, class sz>
int RealPoint<T, sz>::
operator== (const RealPoint<T, sz> & other) const
{
  register int i = 0;
  do {
    if (coords_.data[i] == other.coords_.data[i])
        return 0;
  } while (++i < sz::NUM_COORDS);

  return 1;
}


template <class T, class sz>
int RealPoint<T, sz>::
operator!= (const RealPoint<T, sz> & other) const
{
  return ! (*this == other);
}

#else

template <class T, class sz>
int operator== (const RealPoint<T, sz> & p1, const RealPoint<T, sz> & p2)
{
  register int i = 0;
  do {
    if (p1.coords_.data[i] == p2.coords_.data[i])
        return 0;
  } while (++i < sz::NUM_COORDS);

  return 1;
}


template <class T, class sz>
int operator!= (const RealPoint<T, sz> & p1, const RealPoint<T, sz> & p2)
{
  return ! (p1 == p2);
}

#endif


/*---------------------------------------------------------------------*/

/*

template <class T, WeightedCoordNumber>
int
operator== (Point<T, WeightedCoordNumber> & p1, Point<T, WeightedCoordNumber> & p2)
{
  register int i = 0;
  do {
    if (p1.coords_.data[i] == p2.coords_.data[i])
        return 0;
  } while (++i < WeightedCoordNumber::NUM_COORDS);

  return 1;
}


template <class T, WeightedCoordNumber>
int
operator!= (Point<T, WeightedCoordNumber> & p1, Point<T, WeightedCoordNumber> & p2)
{
  return ! (p1 != p2);
}

*/

//----------------------------------------------------

template <class T, class sz>
int Point<T, sz>::
getNumCoords (void)
{
	return sz::NUM_COORDS;
}


/*-----------------------------------------------------------------------*/

// Output operators

template <class T, class sz>
ostream& 
operator<< (ostream& s, const Point<T, sz> &other)
{
  register int i;

  for (i=0; i<sz::NUM_COORDS; i++)
    s << other[i] << "  ";
  s << endl;
  return s;
}


/*-----------------------------------------------------------------------*/
/*-------------------------CLASS RealPoint ------------------------------*/
/*-----------------------------------------------------------------------*/

template <class T, class sz>
RealPoint<T, sz>::
RealPoint (void)
{
  // Do nothing...

}

/*-----------------------------------------------------------------------*/

template <class T, class sz>
void RealPoint<T, sz>::
load(int x, int y, int z, int r)
{
  load ((double) x, (double) y, (double) z, (double) r);
}

/*-----------------------------------------------------------------------*/

template <class T, class sz>
void RealPoint<T, sz>::
load(double x, double y, double z, double r)
{
  double * p[4];
  p[0] = &x; p[1] = &y; p[2] = &z; p[3] = &r;
  
  for (register int i=0; i<sz::NUM_COORDS; i++)
  {
    coords_.data[i] = (T) *p[i];
  }
}

/*-----------------------------------------------------------------------*/

template <class T, class sz>
void RealPoint<T, sz>::
load (double c[])
{
	for (register int i=0; i<sz::NUM_COORDS; i++)
  {
    coords_.data[i] = (T) c[i];
  }
}

template <class T, class sz>
void RealPoint<T, sz>::
load (float c[])
{
	for (register int i=0; i<sz::NUM_COORDS; i++)
  {
    coords_.data[i] = (T) c[i];
  }
}




/*-----------------------------------------------------------------------*/

template <class T, class sz>
void RealPoint<T, sz>::
get (double& x, double& y, double& z, double& r) const
{
  register int i;
  double * p[4];
  p[0] = &x; p[1] = &y; p[2] = &z; p[3] = &r;

  for (i=0; i<sz::NUM_COORDS; i++)
  {
    *p[i] = coords_.data[i];
  }
  for (; i < 4; i++)
  {
    *p[i] = 0;
  }
}

/*-----------------------------------------------------------------------*/

template <class T, class sz>
void RealPoint<T, sz>::
get (double& x, double& y, double& z) const
{
  double * p[3];
  p[0] = &x; p[1] = &y; p[2] = &z; 

  for (register int i=0; i<3; i++)
  {
    *p[i] = coords_.data[i];
  }
}


/*-----------------------------------------------------------------------*/

template <class T, class sz>
void RealPoint<T, sz>::
get (double c[]) const
{
	for (int i=0; i<sz::NUM_COORDS; i++)
  {
    c[i] = coords_.data[i];
  }
}

template <class T, class sz>
void RealPoint<T, sz>::
get (float c[]) const
{
	for (int i=0; i<sz::NUM_COORDS; i++)
  {
    c[i] = (float) coords_.data[i];
  }
}



/*-----------------------------------------------------------------------*/

template <class T, class sz>
void RealPoint<T, sz>::
mul (double coef)
{
  for (int i=0; i<sz::NUM_COORDS; i++)
  {
    coords_.data[i] *= (T) coef;
  }
}

/*-----------------------------------------------------------------------*/

template <class T, class sz>
RealPoint<T, sz>::
RealPoint (BinInput& bi)
{
	binInit(bi);
}

template <class T, class sz>
void RealPoint<T, sz>::
binDump( BinOutput& bo, double scale)
{
	double buf[sz::NUM_COORDS];
	for(int i=0;i<=sz::NUM_COORDS-1;i++) buf[i] = coords_.data[i]*scale;
	bo.writeDoubles(buf, sz::NUM_COORDS);
}

template <class T, class sz>
Bool RealPoint<T, sz>::
binInit (BinInput& bi)
{
	double buf[sz::NUM_COORDS];
  bi.readDoubles(buf, sz::NUM_COORDS);
  for(int i=0;i<=sz::NUM_COORDS-1;i++) coords_.data[i] = (T) buf[i];
  return TRUE;
}







/*-----------------------------------------------------------------------*/
/*------------------------ CLASS PointArray -----------------------------*/
/*-----------------------------------------------------------------------*/

template <class T>
PointArray<T>::
PointArray (int size) : iStack_(size)
{
  
}

template <class T>
PointArray<T>::
PointArray (PointArray<T> &p) : iStack_(p.iStack_)
{
  
}

template <class T>
const PointArray<T> &PointArray<T>::
operator= (const PointArray<T> &other)
{
  iStack_ = other.iStack_;
  return *this;
}


/*-----------------------------------------------------------------------*/

template <class T>
void PointArray<T>::
sort (int (*compFun) (const void *, const void *))
{
  iStack_.sort(compFun);
}

/*-----------------------------------------------------------------------*/

template <class T>
void PointArray<T>::
removeDuplicates (void)
{
  iStack_.removeDuplicates ();
}

template <class T>
void PointArray<T>::
resize (int newSize)
{
  iStack_.resize (newSize);
}


/*-----------------------------------------------------------------------*/

template <class T>
int PointArray<T>::
add (T &newVert)  
{ 
  iStack_.push (newVert);
  return iStack_.size(); 
}

/*-----------------------------------------------------------------------*/

template <class T>
int PointArray<T>::
removeLast (void)  
{ 
  int res = iStack_.size(); 
  if (res) 
  {
    iStack_.pop ();
    res--;
  }
  return res;
}

template <class T>
int PointArray<T>::
removeLasts (int n)
{
  return iStack_.reduceSizeBy(n);
}

/*-----------------------------------------------------------------------*/

template <class T>
int PointArray<T>::
size (void) const
{ 
  return iStack_.size(); 
}

/*-----------------------------------------------------------------------*/

template <class T>
void PointArray<T>::
reset (void)
{ 
  iStack_.reset(); 
}

/*-----------------------------------------------------------------------*/

template <class T>
T &PointArray<T>::
operator[] (int index) 
{ 
  return iStack_[index]; 
}

/*-----------------------------------------------------------------------*/

template <class T>
const T &PointArray<T>::
operator[] (int index) const
{ 
  //  PointArray<T> *tmp = (PointArray<T> *) this;
  
  //  return tmp->iStack_[index]; 
  return iStack_[index]; 
}

/*-----------------------------------------------------------------------*/

template <class T>
void PointArray<T>::
iter (void (* func) (T &elt)) 
{ 
  iStack_.iter(func); 
}

/*-----------------------------------------------------------------------*/



template <class T>
ostream&
operator<< (ostream& s, const PointArray<T>& arr)
{
  register int i;

  for (i=1; i<=arr.iStack_.size; i++)
    s << arr.iStack_[i];
  return s;
}

/*-----------------------------------------------------------------------*/
/*-------------------------CLASS RealPointArray--------------------------*/
/*-----------------------------------------------------------------------*/

#define REAL_PARRAY_MAGIC 8904444

template <class T>
RealPointArray<T>::
RealPointArray (int size) : PointArray<T> (size) ,
                        magic_(REAL_PARRAY_MAGIC),
												scale_ (1.0)
{
  // Do nothing...
}




template <class T>
RealPointArray<T>::
RealPointArray (RealPointArray<T>& p) : PointArray<T> (p) ,
                             magic_(p.magic_),
							 scale_ (p.getScale())
{

}

template <class T>
RealPointArray<T>::
RealPointArray (RealPArray& p) : PointArray<T> (p.size()) ,
                             magic_(REAL_PARRAY_MAGIC),
							 scale_ (p.getScale())
{
 double x[4];
 for(int i=1; i<=p.size(); i++)
 {
   p.getPoint(i, x);
   addPoint(x);
 }
}

template <class T>
RealPointArray<T>::
~RealPointArray (void)
{

}


/*-----------------------------------------------------------------------*/

template <class T>
void RealPointArray<T>::
binDump (BinOutput& bo, int lastElem)
{
 magic_.binDump(bo);
 int stackSize = (lastElem > 0)? lastElem : iStack_.size();
 bo.writeInt(stackSize);
 float r = stackSize? 1.0f/stackSize : 1.0f;
 float fraction  = 0.0f;

 for(int i=1;i<=stackSize; i++)
 {
    iStack_[i].binDump(bo, scale_);
    fraction += r;
    if (!(i % 10))
      progressUpdate(fraction);
 }
}

template <class T>
void RealPointArray<T>::
binDump (BinOutput& bo,BitVector *bv1,BitVector *bv2)
{
  magic_.binDump(bo);
  int stackSize = iStack_.size();
  int i,count = 0;;
 
  for(i=1;i<=stackSize; i++) 
    if((!bv1->test(i))&&(!bv2->test(i)))
      count++;

  bo.writeInt(count);
  float r = stackSize? 1.0f/stackSize : 1.0f;
  float fraction  = 0.0f;

  for(i=1;i<=stackSize; i++)
  {
    if((!bv1->test(i))&&(!bv2->test(i)))
      iStack_[i].binDump(bo, scale_);

    fraction += r;
    if (!(i % 10))
      progressUpdate(fraction);
  }
}

/*-----------------------------------------------------------------------*/
template <class T>
int RealPointArray<T>::
size (void) const { return PointArray<T>::size(); }

template <class T>
RealPArray * RealPointArray<T>::
getNewRealPArray (void)
{ return new RealPointArray<T> (*this); }

template <class T>
RealPArray * RealPointArray<T>::
getNewEmptyRealPArray (int size) 
{ return new RealPointArray<T> (size); }

template <class T>
void RealPointArray<T>::
sort(int (*compFun) (const void *, const void *))
{ PointArray<T>::sort(compFun); }

template <class T>
void RealPointArray<T>::
removeDuplicates (void)
{ PointArray<T>::removeDuplicates (); }


template <class T>
void RealPointArray<T>::
resize (int newSize)
{ PointArray<T>::resize (newSize); }


template <class T>
void RealPointArray<T>::
reset (void)
{ PointArray<T>::reset (); }

template <class T>
double RealPointArray<T>::
getScale (void)
{ return scale_; }



/*-----------------------------------------------------------------------*/

template <class T>
ostream&
operator<< (ostream& s, const RealPointArray<T>& arr)
{
  register int i;

  for (i=1; i<=arr.iStack_.size(); i++)
    s << arr.iStack_[i];
  return s;
}

/*-----------------------------------------------------------------------*/

template <class T>
int RealPointArray<T>::
addPoint(double x, double y, double z, double r)
{
 T rp;
 rp.load(x, y, z, r);
 return add(rp);
}

template <class T>
int RealPointArray<T>::
removeLastPoint(void)
{
  return PointArray<T>::removeLast();
}

template <class T>
int RealPointArray<T>::
removeLastPoints (int n)
{
  return PointArray<T>::removeLasts(n);
}


/*-----------------------------------------------------------------------*/

template <class T>
void RealPointArray<T>::
setScale (double scale)
{
  double coef = scale_/scale;
	scale_ = scale;
	int size = iStack_.size();
	for (int i=1; i<=size; i++)
    iStack_[i].mul(coef);
}

template <class T>
void RealPointArray<T>::
truncate (int p)
{
  double coef = exp10(p);
	double rcoef = 1.0/coef;
	int size = iStack_.size();
	double x[3];
	for (int i=1; i<=size; i++)
	{
    iStack_[i].get(x);
		x[0] = floor(x[0]*coef + 0.5)*rcoef;
    x[1] = floor(x[1]*coef + 0.5)*rcoef;
		x[2] = floor(x[2]*coef + 0.5)*rcoef;
		iStack_[i].load(x);
	}
}


/*-----------------------------------------------------------------------*/

template <class T>
int RealPointArray<T>::
addPoint(double c[])
{
 T rp;
 rp.load(c);
 return add(rp);
}

template <class T>
int RealPointArray<T>::
addPoint(float c[])
{
 T rp;
 rp.load(c);
 return add(rp);
}

template <class T>
int RealPointArray<T>::
addPoint (RgiVector& bv)
{
  T rp;
  rp.load(bv.rep());
  return add(rp);
}

template <class T>
int RealPointArray<T>::
addPoint (T& rp)
{
  return add(rp);
}

/*-----------------------------------------------------------------------*/

template <class T>
void RealPointArray<T>::
loadPoint(int v,
          double x, double y, double z, double r)
{
  (*this)[v].load(x, y, z, r);
}

template <class T>
void RealPointArray<T>::
loadPoint(int v, double c[])
{
  (*this)[v].load(c);
}

template <class T>
void RealPointArray<T>::
loadPoint(int v, float c[])
{
  (*this)[v].load(c);
}

template <class T>
void RealPointArray<T>::
loadPoint(int v, RgiVector& bv)
{
  (*this)[v].load(bv.rep());
}


/*-----------------------------------------------------------------------*/

template <class T>
void RealPointArray<T>::
getPoint(int v,
         double& x, double& y, double& z, double& w) const
{
   (*this)[v].get(x, y, z, w);
}

template <class T>
void RealPointArray<T>::
getPoint(int v,
         double& x, double& y, double& z) const
{
   (*this)[v].get(x, y, z);
}


template <class T>
void RealPointArray<T>::
getPoint(int v, double c[]) const
{
   (*this)[v].get(c);
}

template <class T>
void RealPointArray<T>::
getPoint(int v, float c[]) const
{
   (*this)[v].get(c);
}

template <class T>
void RealPointArray<T>::
getPoint (int v, RgiVector& vec) const
{
  (*this)[v].get(vec[0], vec[1], vec[2]);
}



/*-----------------------------------------------------------------------*/

template <class T>
Bool RealPointArray<T>::
binInit (BinInput& bi) 
{
  if (!magic_.binInit(bi))
    return FALSE;

  int stackSize = bi.readInt();
  iStack_.resize(stackSize);
  float r = stackSize? (float) 1.0/stackSize : 1;
  float fraction = (float) 0;

  for(int i=1;i<=stackSize;i++)
  {
    T rp (bi);  
    iStack_.push (rp);
    fraction += r;
    if (!(i % 10))
    {
       progressUpdate (fraction);
    }
  }
  return TRUE;
}

template <class T>
int RealPointArray<T>::
getClassId (void)
{
  return magic_.getId();
}

template <class T>
Bool RealPointArray<T>::
binInit (BinInput& bi, int (*test) (double *, void *), void * obj) 
{
  if (!magic_.binInit(bi))
    return FALSE;

  int stackSize = bi.readInt();
  float r = stackSize? (float) 1.0/stackSize : 1;
  float fraction = (float) 0;

  for(int i=1;i<=stackSize;i++)
  {
    T rp (bi);
    double x[4];
    rp.get(x);
    if (test(x, obj))
      iStack_.push (rp);
    fraction += r;
    if (!(i % 10))
    {
       progressUpdate (fraction);
    }
  }
  iStack_.resize(iStack_.size());
  return TRUE;
}
