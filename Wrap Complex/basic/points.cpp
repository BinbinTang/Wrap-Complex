/*
 * points.C -- Class methods for Points and Point Arrays
 *
 * author -- Mike Facello
 * 
 * date -- 10/9/96
 */

#include <basic/points.h>
#include <basic/vertexarray.h>

/*-----------------------------------------------------------------------*/
/*------------------------- CLASS IntPoint ------------------------------*/
/*-----------------------------------------------------------------------*/


IntPoint::
IntPoint (void)
{
  // Do nothing...

}

/*-----------------------------------------------------------------------*/

void IntPoint::
load(int x, int y, int z, int r)
{
  coords_.data[0] = x; coords_.data[1] = y; coords_.data[2] = z; 
  coords_.data[3] = r;
}

/*-----------------------------------------------------------------------*/

ostream& 
operator<< (ostream& s, const IntPoint &other)
{
  register int i;
  for (i=1; i<=WeightedCoordNumber::NUM_COORDS; i++)
    s << other[i] << "  ";
  s << endl;
  return s;
}

IntPoint::
IntPoint (BinInput& bi)
{
  int buf[4];
  bi.readInts(buf, 4);
  for(int i=0;i<=3;i++) coords_.data[i] = buf[i];
}

void IntPoint::
binDump(BinOutput& bo)
{
  int buf[4];
  for(int i=0;i<=3;i++) buf[i] = coords_.data[i];
  bo.writeInts(buf, 4);
}

//---------------------------------------------------------------------------

static int equalFloats(const float * f1, const float * f2, int n)
{
  for(int i=0; i<n; i++)
	  if (*f1++  != *f2++) return 0;
  return 1;
}

static int equalDoubles(const double * d1, const double * d2, int n)
{
  for(int i=0; i<n; i++)
	  if (*d1++  != *d2++) return 0;
  return 1;
}



int FloatPoint::
operator!= (const FloatPoint& p) const
{
  return !equalFloats(coords_.data, p.coords_.data, 3);
}

int FloatPointW::
operator!= (const FloatPointW& p) const
{
  return !equalFloats(coords_.data, p.coords_.data, 4);
}

int DoublePoint::
operator!= (const DoublePoint& p) const
{
  return !equalDoubles(coords_.data, p.coords_.data, 3);
}

int DoublePointW::
operator!= (const DoublePointW& p) const
{
  return !equalDoubles(coords_.data, p.coords_.data, 4);
}



/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*------------------------- CLASS IntPArray -----------------------------*/
/*-----------------------------------------------------------------------*/

IntPArray::
IntPArray (int size) : PointArray<IntPoint> (size) 
{
  // Do nothing...
}


/*-----------------------------------------------------------------------*/

ostream&
operator<< (ostream& s, const IntPArray& arr)
{
  register int i;

  for (i=1; i<=arr.iStack_.size(); i++)
    s << arr.iStack_[i];
  return s;
}


/*----------------------------------------------------------------------*/

ostream&
operator<< (ostream& s, const RealPArray& arr)
{
  int numElts = arr.size();
  register int i;
  double x, y, z, w;
  for (i=1; i<=numElts; i++)
  {
    arr.getPoint(i, x, y, z, w);
    s << x << " " << y << " " << z << " " << w << endl;
  } 

  return s;
}

/*-----------------------------------------------------------------------*/

RealPArray::
~RealPArray (void)
{

}

double RealPArray::
getAbsMax (void)
{
  int i, j;
  double tmp;
  int n = size();
  double x[4];
  double absMax = 1.0e-16;

  for (i=1; i<=n; i++)
  {
    getPoint(i, x);
    for(j=0; j<3; j++)
    {
      tmp = fabs(x[j]);
      if (tmp > absMax)
          absMax = tmp;
    }
  }

  return absMax;
}

//add by xinwei, 2005-05-04
double RealPArray::
getMaxRadii (void)
{
  int i, j;
  double tmp;
  int n = size();
  double x[4];
  double absMax = 1.0e-16;

  for (i=1; i<=n; i++)
  {
    getPoint(i, x);
    for(j=3; j<4; j++)
    {
      tmp = fabs(x[j]);
      if (tmp > absMax)
          absMax = tmp;
    }
  }

  return absMax;
}


/*-----------------------------------------------------------------------*/
//---------------------------------------------------------------------------

const int REAL_PARRAY_DOUBLE_MAGIC  = 8904445;
const int REAL_PARRAY_DOUBLEW_MAGIC = 8904446;
const int REAL_PARRAY_FLOAT_MAGIC   = 8904447;
const int REAL_PARRAY_FLOATW_MAGIC  = 8904448;


RealPArray* RealPArray::
getNewEmptyRealPArray(BinInput& bi)
{

  switch(bi.queryInt())
  {
    case REAL_PARRAY_FLOAT_MAGIC:
      return new FloatPArray;
    case REAL_PARRAY_FLOATW_MAGIC:
      return new FloatPArrayW;
    case REAL_PARRAY_DOUBLE_MAGIC:
      return new DoublePArray;
    case REAL_PARRAY_DOUBLEW_MAGIC:
      return new DoublePArrayW;
    default:
      return NULL;
  }

}

/*-----------------------------------------------------------------------*/

void RealPArray::
getBBox (double *min, double *max) const
{
	int N = size();
	if (N == 0) {
		min[0] = min[1] = min[2] = 0.0;
		max[0] = max[1] = max[2] = 0.0;
	} else {
		min[0] = min[1] = min[2] =  1e42;
		max[0] = max[1] = max[2] = -1e42;
	}

	double current[3];
	for (int i=1; i<=N; i++) {
		getPoint (i, current[0], current[1], current[2]);
		if (current[0] < min[0]) min[0] = current[0];
		if (current[1] < min[1]) min[1] = current[1];
		if (current[2] < min[2]) min[2] = current[2];
		if (current[0] > max[0]) max[0] = current[0];
		if (current[1] > max[1]) max[1] = current[1];
		if (current[2] > max[2]) max[2] = current[2];
	}
}

/*-----------------------------------------------------------------------*/

void RealPArray::
getInfo (VertexArray &array)
{
	int id = getClassId();

	if (FloatPArray::isValid(id)) {
		array = VertexArray (((FloatPArray *)this)->getData(), Float,
			sizeof(float)*3, size()+1);
	} else if(DoublePArray::isValid(id)) {
		array = VertexArray (((DoublePArray *)this)->getData(), Double,
			sizeof(double)*3, size()+1);
	} else if(FloatPArrayW::isValid(id)) {
		array = VertexArray (((FloatPArrayW *)this)->getData(), Float,
			sizeof(float)*4, size()+1);
	} else if(DoublePArrayW::isValid(id)) {
		array = VertexArray (((DoublePArrayW *)this)->getData(), Double,
			sizeof(double)*4, size()+1);
	} else {
		Assert(0);
	}
}



//---------------------------------------------------------------------------

FloatPArray::FloatPArray (int size) 
: RealPointArray<FloatPoint> (size) 
{ magic_ = Magic(REAL_PARRAY_FLOAT_MAGIC); }

FloatPArray::FloatPArray (RealPArray& p) 
: RealPointArray<FloatPoint> (p) 
{ magic_ = Magic(REAL_PARRAY_FLOAT_MAGIC); }

FloatPArray::
~FloatPArray (void)
{

}

RealPArray * FloatPArray::
getNewRealPArray (void) { return new FloatPArray (*this); }

RealPArray * FloatPArray::
getNewEmptyRealPArray (int size) { return new FloatPArray (size); }

Bool FloatPArray::
isValid (int id)
{
  return (id == REAL_PARRAY_FLOAT_MAGIC);
}

//---------------------------------------------------------------------------

FloatPArrayW::FloatPArrayW (int size) 
: RealPointArray<FloatPointW> (size) 
{ magic_ = Magic(REAL_PARRAY_FLOATW_MAGIC); }

FloatPArrayW::FloatPArrayW (RealPArray& p) 
: RealPointArray<FloatPointW> (p)
{ magic_ = Magic(REAL_PARRAY_FLOATW_MAGIC); }

RealPArray * FloatPArrayW::
getNewRealPArray (void) { return new FloatPArrayW (*this); }

RealPArray * FloatPArrayW::
getNewEmptyRealPArray (int size) { return new FloatPArrayW (size); }

FloatPArrayW::
~FloatPArrayW (void)
{

}

Bool FloatPArrayW::
isValid (int id)
{
  return (id == REAL_PARRAY_FLOATW_MAGIC);
}

//---------------------------------------------------------------------------


DoublePArray::DoublePArray (int size) 
: RealPointArray<DoublePoint> (size)
{ magic_ = Magic(REAL_PARRAY_DOUBLE_MAGIC); }

DoublePArray::DoublePArray (RealPArray& p) 
: RealPointArray<DoublePoint> (p)
{ magic_ = Magic(REAL_PARRAY_DOUBLE_MAGIC); }

RealPArray * DoublePArray::
getNewRealPArray (void) { return new DoublePArray (*this); }

RealPArray * DoublePArray::
getNewEmptyRealPArray (int size) { return new DoublePArray (size); }

DoublePArray::
~DoublePArray (void)
{

}

Bool DoublePArray::
isValid (int id)
{
  return (id == REAL_PARRAY_DOUBLE_MAGIC);
}

//---------------------------------------------------------------------------

DoublePArrayW::DoublePArrayW (int size) 
: RealPointArray<DoublePointW> (size) 
{ magic_ = Magic(REAL_PARRAY_DOUBLEW_MAGIC); }

DoublePArrayW::DoublePArrayW (RealPArray& p) 
: RealPointArray<DoublePointW> (p) 
{ magic_ = Magic(REAL_PARRAY_DOUBLEW_MAGIC); }

RealPArray * DoublePArrayW::
getNewRealPArray (void) { return new DoublePArrayW (*this); }

RealPArray * DoublePArrayW::
getNewEmptyRealPArray (int size) { return new DoublePArrayW (size); }

DoublePArrayW::
~DoublePArrayW (void)
{

}

Bool DoublePArrayW::
isValid (int id)
{
  return (id == REAL_PARRAY_DOUBLEW_MAGIC);
}

//---------------------------------------------------------------------------


