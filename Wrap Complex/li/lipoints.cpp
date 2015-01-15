/*
 * lipoints.C -- Class methods for Points and Point Arrays
 *
 * author -- Mike Facello
 * 
 * date -- 10/9/96
 */

#include <li/lipoints.h>


/*-----------------------------------------------------------------------*/
/*------------------------- CLASS LI_Point ------------------------------*/
/*-----------------------------------------------------------------------*/

LI_Point::
LI_Point (void)
{
  // Do nothing...
}

void LI_Point::
alloc (void)
{
  if (coords_.data[0].lia_obj_ == NULL) coords_.data[0].alloc();
  if (coords_.data[1].lia_obj_ == NULL) coords_.data[1].alloc();
  if (coords_.data[2].lia_obj_ == NULL) coords_.data[2].alloc();
  if (coords_.data[3].lia_obj_ == NULL) coords_.data[3].alloc();
}

void LI_Point::
free (void)
{
  coords_.data[0].free();
  coords_.data[1].free();
  coords_.data[2].free();
  coords_.data[3].free();
}

/*-----------------------------------------------------------------------*/

void LI_Point::
load(double x[4], int w, int a, int compute)
{
  lia_ffpload (coords_.data[0].lia_obj_, w, a, x[0]);
  lia_ffpload (coords_.data[1].lia_obj_, w, a, x[1]);
  lia_ffpload (coords_.data[2].lia_obj_, w, a, x[2]);
  lia_ffpload (coords_.data[3].lia_obj_, w, a, x[3]);
  if (compute)
  {
    computeSumSquares();
  }
}

/*-----------------------------------------------------------------------*/

void LI_Point::
load(const LI_Coord& x, const LI_Coord& y, const LI_Coord& z, 
     const LI_Coord& r)
{ 
  coords_.data[0] = x; coords_.data[1] = y; coords_.data[2] = z; coords_.data[3] = r;
  computeSumSquares();
}

/*-----------------------------------------------------------*/

Bool LI_Point::
binInit (BinInput& bi)
{
  return coords_.data[0].binInit(bi) &&
         coords_.data[1].binInit(bi) &&
         coords_.data[2].binInit(bi) &&
         coords_.data[3].binInit(bi);  
}


/*-----------------------------------------------------------------------*/

void LI_Point::
computeSumSquares(void)
{
  LI tmp[2];

  //weighted


  if (coords_.data[3].sign() == -1)
    tmp[1].mult(coords_.data[3], coords_.data[3]);
  else
    {
     tmp[1].mult(coords_.data[3], coords_.data[3]);
     tmp[1].negate();
    }
/*
  coords_.data[3].negate();
  tmp[1] = coords_.data[3];
*/

  tmp[0].mult (coords_.data[0], coords_.data[0]);
  coords_.data[3].add(tmp[0],tmp[1]);

  tmp[1].mult (coords_.data[1], coords_.data[1]);
  tmp[0].add(coords_.data[3],tmp[1]);
  tmp[1].mult (coords_.data[2], coords_.data[2]);
  coords_.data[3].add(tmp[0], tmp[1]);


/*
  LI tmp3;
  tmp3.mult(coords_.data[0],coords_.data[0]);
  tmp[0].sub(tmp3,coords_.data[3]);
  tmp3.mult(coords_.data[1],coords_.data[1]);
  tmp[1].add(tmp3,tmp[0]);
  tmp3.mult(coords_.data[2],coords_.data[2]);
  coords_.data[3].add(tmp3,tmp[1]);
*/

}

/*-----------------------------------------------------------------------*/

ostream& 
operator<< (ostream& s, LI_Point &other)
{
  register int i;
  for (i=1; i<=4; i++)
    s << other[i] << "  ";
  s << endl;
  return s;
}

void LI_Point:: // size is known and dumped
binDump (BinOutput& bo)
{
  for(int i=0;i<=3;i++) coords_.data[i].binDump(bo);
}



/*-----------------------------------------------------------------------*/
/*------------------------- CLASS LI_PArray -----------------------------*/
/*-----------------------------------------------------------------------*/

#define LI_PARRAY_MAGIC 2206918

LI_PArray::
LI_PArray (int size, int length, int decimals)
  : PointArray<LI_Point> (size),
	magic_(LI_PARRAY_MAGIC),
  liConvert_ (new LI_Converter (length, decimals))
{
  // do nothing...
}

/*----------------------------------------------------------*/

extern int legacy_offset;

Bool LI_PArray::
binInit (BinInput& bi)
{
  if (!magic_.binInit(bi, LI_PARRAY_MAGIC))
    return FALSE;

  /* int inSize = */ bi.readInt();
  /* int liSize = */ bi.readInt();
  /* int liCoordSize = */ bi.readInt();
  int length = bi.readInt();
  int decimals = bi.readInt();

  // Set size of long integers
  delete liConvert_;
  liConvert_ = new LI_Converter (length, decimals);

  int stackSize = bi.readInt();
  iStack_.resize (stackSize);
  float rSize = (float) (stackSize? 1.0/stackSize : 1.0);
  LI_Point point;
  legacy_offset = 0; // offset for reading old legacy code
  for(int i=1;i<=stackSize;i++)
  {
    progressUpdate (rSize*i);
    iStack_.push(point);
    if(!iStack_.top().binInit(bi))
      return FALSE;
  }

  return TRUE;

}

/*---------------------------------------------------------*/

RealPArray * LI_PArray::
getNewRealArray (BinInput &bi)
{
  if (!magic_.binInit(bi, LI_PARRAY_MAGIC))
    return NULL;

  /* int inSize = */ bi.readInt();
  /* int liSize = */ bi.readInt();
  /* int liCoordSize = */ bi.readInt();
  int length = bi.readInt();
  int decimals = bi.readInt();

  // Set size of long integers
  delete liConvert_;
  liConvert_ = new LI_Converter (length, decimals);
  int stackSize = bi.readInt();
  RealPArray *rArray = NULL;

  if (length <= 7)
       rArray = new FloatPArray (stackSize);
  else
       rArray = new DoublePArray (stackSize);

  
  float rSize = (float) (stackSize? 1.0/stackSize : 1.0);
  LI_Point lipoint;
  double tmp[3];
  int j;
  legacy_offset = 0; // offset for reading old legacy code
  for(int i=1;i<=stackSize;i++)
  {
    progressUpdate (rSize*i);
    if(!lipoint.binInit(bi))
    {
      delete rArray;
      return NULL;
    }
  
    for (j = 1; j <= 3; j++)
	    tmp[j-1] = liConvert_->toReal(lipoint[j]);
    rArray->addPoint (tmp);
  }

  return rArray;

}

/*---------------------------------------------------------*/

LI_PArray::
LI_PArray (LI_PArray& pArray)
  :PointArray<LI_Point>(pArray),
   magic_(LI_PARRAY_MAGIC),
   liConvert_(new LI_Converter(*pArray.liConvert_))
{

}

/*---------------------------------------------------------*/

LI_PArray::
LI_PArray (RealPArray& rArray, int length, int decimals)
  : PointArray<LI_Point>(rArray.size()),
    magic_ (LI_PARRAY_MAGIC),
    liConvert_ (new LI_Converter(length, decimals))

{
 double x[4];
 tmpPoint_.alloc();
 for (int i=1; i<=rArray.size(); i++)
 {
   rArray.getPoint(i, x);
   liConvert_->toLi(x, 1, tmpPoint_);
   add(tmpPoint_);
 }
}

/*---------------------------------------------------------*/

void LI_PArray::
binDump (BinOutput& bo)
{
  magic_.binDump(bo);  

  int outSize = LI::getSize()*sizeof(Lia)/2+4;
   // inSize is size of representing string;
  bo.writeInt(outSize);
  bo.writeInt(LI::getSize());
  bo.writeInt(LI_Coord::getSize());

  bo.writeInt(liConvert_->getFixedW());
  bo.writeInt(liConvert_->getFixedA());

  int stackSize = iStack_.size();
  bo.writeInt(stackSize);

  for(int i=1;i<=stackSize; i++)
    iStack_[i].binDump(bo);

}

void LI_PArray::
binDump (RealPArray * rArray, BinOutput& bo)
{
  magic_.binDump(bo);  

  int outSize = LI::getSize()*sizeof(Lia)/2+4;
   // inSize is size of representing string;
  bo.writeInt(outSize);
  bo.writeInt(LI::getSize());
  bo.writeInt(LI_Coord::getSize());

  int w = liConvert_->getFixedW();
  int a = liConvert_->getFixedA();
  bo.writeInt(w);
  bo.writeInt(a);

  int pSize = rArray->size();
  bo.writeInt(pSize);

  double x[4];
  LI_Point lipoint;
  lipoint.alloc();
  for(int i=1;i<=pSize; i++)
  {
    rArray->getPoint(i, x);
    lipoint.load(x, w, a, FALSE);
    lipoint.binDump(bo);
  }

}

/*-----------------------------------------------------------------------*/

LI_PArray::
~LI_PArray (void)
{
  int stackSize = iStack_.size();
  for (int i=1; i<=stackSize; i++)
    iStack_[i].free();
  delete liConvert_;
}

/*-----------------------------------------------------------------------*/

void LI_PArray::
setConverter (LI_Converter *lic)
{
  delete liConvert_;
  liConvert_ = lic;
}


/*-----------------------------------------------------------------------*/

LI_Converter *LI_PArray::
getConverter (void)
{
  return liConvert_;
}


/*-----------------------------------------------------------------------*/

//VIndex LI_PArray::
int LI_PArray::
addPoint (int x, int y, int z, int r)
{
  liConvert_->toLI (x, tmpPoint_[1]);
  liConvert_->toLI (y, tmpPoint_[2]);
  liConvert_->toLI (z, tmpPoint_[3]);
  liConvert_->toLI (r, tmpPoint_[4]);
  tmpPoint_.computeSumSquares ();
  return add (tmpPoint_);
}

/*-----------------------------------------------------------------------*/
// TO DO: convert floating points using truncation
//VIndex LI_PArray::
int LI_PArray::
addPoint (float x, float y, float z, float r)
{
  liConvert_->toLI (x, tmpPoint_[1]);
  liConvert_->toLI (y, tmpPoint_[2]);
  liConvert_->toLI (z, tmpPoint_[3]);
  liConvert_->toLI (r, tmpPoint_[4]);
  tmpPoint_.computeSumSquares ();
  return add (tmpPoint_);
}

/*-----------------------------------------------------------------------*/
// TO DO: convert floating points using truncation
//VIndex LI_PArray::
int LI_PArray::
addPoint (double x, double y, double z, double r)
{
  liConvert_->toLI (x, tmpPoint_[1]);
  liConvert_->toLI (y, tmpPoint_[2]);
  liConvert_->toLI (z, tmpPoint_[3]);
  liConvert_->toLI (r, tmpPoint_[4]);
  tmpPoint_.computeSumSquares ();
  return add (tmpPoint_);
}

/*-----------------------------------------------------------------------*/

//VIndex LI_PArray::
int LI_PArray::
addPoint (char *x, char *y, char *z, char *r)
{
  liConvert_->toLI (x, tmpPoint_[1]);
  liConvert_->toLI (y, tmpPoint_[2]);
  liConvert_->toLI (z, tmpPoint_[3]);
  liConvert_->toLI (r, tmpPoint_[4]);
  tmpPoint_.computeSumSquares ();
  return add (tmpPoint_);
}

/*-----------------------------------------------------------------------*/

void LI_PArray::
//loadPoint (VIndex v, double x, double y, double z, double r)
loadPoint (int v, double x, double y, double z, double r)
{
  liConvert_->toLI (x, tmpPoint_[1]);
  liConvert_->toLI (y, tmpPoint_[2]);
  liConvert_->toLI (z, tmpPoint_[3]);
  liConvert_->toLI (r, tmpPoint_[4]);
  (*this)[v].load(tmpPoint_[1], tmpPoint_[2], tmpPoint_[3], tmpPoint_[4]);
}

/*-----------------------------------------------------------------------*/

void LI_PArray::
//getPoint (VIndex v, double& x, double& y, double& z, double& r)
getPoint (int v, double& x, double& y, double& z, double& r)
{
  x = liConvert_->toReal((*this)[v][1]);
  y = liConvert_->toReal((*this)[v][2]); 
  z = liConvert_->toReal((*this)[v][3]);
  r = liConvert_->toReal((*this)[v][4]);
}


/*-----------------------------------------------------------------------*/

RealPArray *LI_PArray::
getNewRealArray (void)
{
  int sz = size ();
  int w = liConvert_->getFixedW();
  RealPArray *rpArray;

  if (w <= 7)
       rpArray = new FloatPArray (sz);
  else
       rpArray = new DoublePArray (sz);
       
  double tmp[3];
  
  for (register int i = 1; i <= sz; i++)
  {
    for (register int j = 1; j <= 3; j++)
	  tmp[j-1] = liConvert_->toReal(iStack_[i][j]);
    rpArray->addPoint (tmp);
  }

  return (rpArray);
}

/*-----------------------------------------------------------------------*/

ostream&
operator<< (ostream& s, LI_PArray& arr)
{
  register int i;

  for (i=1; i<=arr.iStack_.size(); i++)
    s << arr.iStack_[i];
  return s;
}

