/*
 * li.cpp -- definitions for the LI class.
 *
 * author -- Mike Facello
 *
 * date -- 10/8/96
 */

#include "basic/basic.h"
#include "basic/binio.h"
#include "basic/miscmath.h"
#include "li.h"
#include "lipoints.h"

#define LP_SUM  16

ostream&   operator<< (ostream& s, const LIBase& num)
{
  if (num.lia_obj_ == NULL)
    s << "0";
  else
    s << lia_deci (num.lia_obj_);

  return s;
}

/*-----------------------------------------------------------------------*/
/*---------------------------- CLASS LI_COORD ---------------------------*/
/*-----------------------------------------------------------------------*/

LI const &LI::
operator= (const LI &lnum)
{
  LIGen<LILong>::operator=( (const LIBase& ) lnum);
  return (*this);
}

LI const &LI::
operator= (const LI_Coord &lnum)
{
  LIGen<LILong>::operator=( (const LIBase& ) lnum);
  return (*this);
}


/*-----------------------------------------------------------------------*/

void LI::
validatePrecision (int& lMax, int& pMax)
{
  if ((lMax + pMax) > LP_SUM) pMax = LP_SUM - lMax;
  if (pMax < 0) pMax = 0;
}

/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// (Somewhat) expensive arithmetic operations

// operator+ is currently defined as inlined in li.h

LI 
operator- (const LI &other1, const LI &other2)
{
  LI tmp;

  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  tmp.alloc();
  lia_sub (tmp.lia_obj_, other1.lia_obj_, other2.lia_obj_);

  return tmp;
}

/*-----------------------------------------------------------------------*/

LI 
operator* (const LI &other1, const LI &other2)
{
  LI tmp;
  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  tmp.alloc();
  lia_mul (tmp.lia_obj_, other1.lia_obj_, other2.lia_obj_);

  return tmp;
}

/*-----------------------------------------------------------------------*/

LI 
operator- (const LI &other)
{
  LI tmp;

  Assert (other.lia_obj_ != NULL);
  tmp.alloc();
  tmp = other;
  lia_chs (tmp.lia_obj_);

  return tmp;
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Output operations

ostream&
operator<< (ostream& s, const LI& li)
{
  lia_clear ();
  if (li.lia_obj_ == NULL)
    s << '0';
  else
    s << lia_deci (li.lia_obj_) << " " << last_of (li.lia_obj_);
  return s;
}



/*-----------------------------------------------------------------------*/
/*---------------------------- CLASS LI_COORD ---------------------------*/
/*-----------------------------------------------------------------------*/

int legacy_offset = 0;

Bool LI_Coord::
binInit (BinInput& bi)
{
  unsigned char c = bi.readUnsignedChar();
  while(c != 250)
  {
    c = bi.readUnsignedChar(); // try to read old legacy code
    legacy_offset++;
  }
  unsigned char in[128];
  if (lia_obj_ == NULL)
    alloc();
  int liaSize = getSize();
  int inSize = getSize()+1;// length in Lia plus header
  int strSize = 4+liaSize*sizeof(Lia); // length of string
  bi.readUnsignedChars(in, strSize+legacy_offset);
  lia_loadBinString (in, inSize, lia_obj_); 
  return TRUE;
}

/*-----------------------------------------------------------------------*/

void LI_Coord::
binDump(BinOutput& bo)
{
  unsigned char c = 250;
  bo.writeUnsignedChars(&c,1);

  unsigned char out[128];
  int liaSize = getSize();
  int outSize = 4+liaSize*sizeof(Lia);
  lia_exportBinString(lia_obj_, out, outSize);
  bo.writeUnsignedChars(out, outSize);
}


/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/

LI_Coord const &LI_Coord::
operator= (const LI_Coord &lnum)
{
  LIGen<LIShort>::operator=( (const LIBase& ) lnum);
  return (*this);
}

LI_Coord const &LI_Coord::
operator= (const LI &lnum)
{
  LIGen<LIShort>::operator=( (const LIBase& ) lnum);
  return (*this);
}

/*-----------------------------------------------------------------------*/
/*-------------------------- CLASS LI_CONVERTER -------------------------*/
/*-----------------------------------------------------------------------*/

// Constructor

LI_Converter::
LI_Converter (int w, int a)
{
  fixed_w_ = w;
  fixed_a_ = a;
  init_ ();
}

/*-----------------------------------------------------------------------*/

LI_Converter::
LI_Converter (LI_Converter& other)
{
  fixed_w_ = other.fixed_w_;
  fixed_a_ = other.fixed_a_;
  init_ ();
}

LI_Converter& LI_Converter::
operator= (const LI_Converter& other)
{
  fixed_w_ = other.fixed_w_;
  fixed_a_ = other.fixed_a_;
  init_();
	return *this;
}

void LI_Converter::
setPrecision (int lMax, int pMax)
{
	fixed_w_ = lMax + pMax;
  fixed_a_ = pMax;
	init_ ();
}

void LI_Converter::
getPrecision (int& lMax, int& pMax)
{
  lMax = fixed_w_ - fixed_a_;
	pMax = fixed_a_;
}

void LI_Converter::
init_ (void)
{
  LI::setSize(Lia_DIGITS (18 * fixed_w_ + 12));
  LI_Coord::setSize(Lia_DIGITS (2 * fixed_w_ + 1));

  scale_ = exp10 ((double) fixed_a_);
	if (fixed_w_ <= 9)
    truncate_ = &LI_Converter::truncateI9_2_;
	else
		truncate_ = &LI_Converter::truncateF_;
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Conversion operations

void LI_Converter::
toLI (int inum, LIBase &li)
{
  Assert (li.lia_obj_);
  lia_load (li.lia_obj_, inum);
}

/*-----------------------------------------------------------------------*/

void LI_Converter::
toLI (float fnum, LIBase &li)
{
  Assert (li.lia_obj_);
  lia_ffpload (li.lia_obj_, fixed_w_, fixed_a_, (double) fnum);
}

/*-----------------------------------------------------------------------*/

void LI_Converter::
toLI (double dnum, LIBase &li)
{
  Assert (li.lia_obj_);
  lia_ffpload (li.lia_obj_, fixed_w_, fixed_a_, dnum);
}

/*-----------------------------------------------------------------------*/

void LI_Converter::
toLI (char *cnum, LIBase &li)
{
  Assert (li.lia_obj_);
  if (fixed_a_ == 0)
    lia_strload (li.lia_obj_, cnum, "%d");
  else
    lia_fixload (li.lia_obj_, fixed_w_, fixed_a_, cnum);
}

/*-----------------------------------------------------------------------*/

double LI_Converter::
toReal (LIBase &num)
{
  if (num.lia_obj_ == NULL)
    return 0.0;
  else
    return (lia_real (num.lia_obj_) / scale_);
}

/*-----------------------------------------------------------------------*/

char *LI_Converter::
toString (LIBase &num)
{
  char *result, *tmp;

  if (num.lia_obj_ == NULL)
    return "0.0";
  else if (fixed_a_ == 0)
    return STRDUP (lia_deci (num.lia_obj_));
  else
    {
      char *exponent = new char[80];

      tmp = lia_deci (num.lia_obj_);
      //sprintf (exponent, "e-%d", fixed_a_);
	  sprintf_s (exponent, 80, "e-%d", fixed_a_);
      result = new char [strlen(tmp) + strlen(exponent)];
      //sprintf (result, "%s%s", tmp, exponent);
	  sprintf_s (result, strlen(tmp) + strlen(exponent), "%s%s", tmp, exponent);

      return result;
    }
}

/*-----------------------------------------------------------------------*/
// Truncation functions

void LI_Converter::
truncateD_ (double x[4])
{
   // assumed to be truncated already
}

// assume all numbers are scaled to integers
void LI_Converter::
truncateF_ (double x[4])
{
  x[0] = floor(x[0] += 0.5);
  x[1] = floor(x[1] += 0.5);
  x[2] = floor(x[2] += 0.5);
  x[3] = floor(x[3] += 0.5);
}
	
void LI_Converter::
truncateI9_1_ (double x[4])
{
#define ADD 1000000000.5
#define SUB 1000000000
	x[0] += ADD; x[1] += ADD; x[2] += ADD; x[3]+= ADD;
	x[0] = int(x[0]) - SUB;
	x[1] = int(x[1]) - SUB;
	x[2] = int(x[2]) - SUB;
	x[3] = int(x[3]) - SUB;
#undef ADD
#undef SUB
}

void LI_Converter::
truncateI9_2_ (double x[4])
{
  x[0] = int((x[0] > 0)? (x[0] + 0.5) : x[0] - 0.5);
  x[1] = int((x[1] > 0)? (x[1] + 0.5) : x[1] - 0.5);
  x[2] = int((x[2] > 0)? (x[2] + 0.5) : x[2] - 0.5);
  x[3] = int((x[3] > 0)? (x[3] + 0.5) : x[3] - 0.5);
}

void LI_Converter::
truncMulToLi (double x[4], int computeSumSq, LI_Point &pt)
{
  pt.load(x, fixed_w_, 0, computeSumSq);
}

void LI_Converter::
toLi (double x[4], int computeSumSq, LI_Point & pt)
{
  double xx[4];
  vcopyw(x, xx);
  truncMul(xx);
  pt.load(xx, fixed_w_, 0, computeSumSq);
}

void LI_Converter::
truncPArray (RealPArray& rArray)
{
  double x[4];
  double rscale = 1.0/scale_;
  int n = rArray.size();
  for (int v=1; v<=n; v++)
  {
    rArray.getPoint(v, x);
    vmulw(x, scale_);
    (this->*truncate_)(x);
    vmulw(x, rscale);
    rArray.loadPoint(v, x);
  }
}


/*-----------------------------------------------------------------------*/

// Include inline functions here if inlining is not performed

#ifndef DO_INLINE
#include "li.hpp"
#endif
