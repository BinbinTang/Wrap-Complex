
#include <basic/points.h>
#include <basic/miscmath.h>
#include <li/lipoints.h>
#include <sos/sosfloat.h>

static unsigned long totPositive3 = 0;
static unsigned long li_Positive3 = 0;
static unsigned long totinSphereP = 0;
static unsigned long li_inSphereP = 0;

extern ofstream *os;


extern	void exactinit(void);
extern  double orient3d(double *pa, double *pb, double *pc);
extern  double orient3d(double *pa, double *pb, double *pc, double *pd);
extern  double insphere(double *pa, double *pb, double  *pc, double  *pd);


// Define SOS_TEST to compare the result with the SOS result (only in Debug mode)
// #define SOS_TEST 1

SOS_FloatEvaluator:: 
SOS_FloatEvaluator (LI_Converter& converter)
: converter_(converter)
{
  // os = new ofstream("loglog.txt");
  totPositive3 = 0;
  li_Positive3 = 0;
  totinSphereP = 0;
  li_inSphereP = 0;
	exactinit ();
  li_.alloc();
  lj_.alloc();
  lk_.alloc();
  ll_.alloc();
  lo_.alloc();
}


Bool SOS_FloatEvaluator::
positive3 (RealPArray& rarr, VIndex i, VIndex j, VIndex k, VIndex l)
{
  int d = 0;
  int s = basic_isort4 ((int *)(&i), (int *)(&j), (int *)(&k), (int *)(&l));

  double xi[4], xj[4], xk[4], xl[4], xli[4], xlj[4], xlk[4];
  rarr.getPoint(l, xl); converter_.truncMul(xl);// base
  rarr.getPoint(i, xi); converter_.truncMul(xi); vsub(xi, xl, xli); converter_.trunc(xli);
	rarr.getPoint(j, xj); converter_.truncMul(xj); vsub(xj, xl, xlj); converter_.trunc(xlj);
	rarr.getPoint(k, xk); converter_.truncMul(xk); vsub(xk, xl, xlk); converter_.trunc(xlk);

	double res = orient3d(xli, xlj, xlk);
	if (res)
  {
    d = (res > 0.0)? 1 : -1;
#ifdef SOS_TEST
   converter_.truncMulToLi(xi, 0, li_);
	 converter_.truncMulToLi(xj, 0, lj_);
	 converter_.truncMulToLi(xk, 0, lk_);
	 converter_.truncMulToLi(xl, 0, ll_);

    lambda4_.eval(li_, lj_, lk_, ll_, 
		              i, j, k, l, result_);
    int d1 = result_.sign ();
    Assert(d==d1);
#endif
	}
  else
	{
	 converter_.truncMulToLi(xi, 0, li_);
	 converter_.truncMulToLi(xj, 0, lj_);
	 converter_.truncMulToLi(xk, 0, lk_);
	 converter_.truncMulToLi(xl, 0, ll_);

    lambda4_.eval(li_, lj_, lk_, ll_, 
		              i, j, k, l, result_);
    d = result_.sign ();
    li_Positive3++;
	}
  int answer = If (Odd (s), (d == -1), (d == 1));

  return answer;
}

//--------------------------------------------------------------------------------

Bool SOS_FloatEvaluator::
above4 (RealPArray& rarr, VIndex i, VIndex j, VIndex k, VIndex l, VIndex m)
{
	return !inSphere (rarr, i, j, k, l, m);
}

//--------------------------------------------------------------------------------

Bool SOS_FloatEvaluator::
inSphere (RealPArray& rarr, VIndex o, VIndex i, VIndex j, VIndex k, VIndex p)
{
  Bool pos3 = positive3(rarr, o, i, j, k);
	Bool sphP = inSphereP(rarr, o, i, j, k, p);
	return pos3? sphP : !sphP;
}

//--------------------------------------------------------------------------------

Bool SOS_FloatEvaluator::
inSphereP (RealPArray& rarr, VIndex o, VIndex i, VIndex j, VIndex k, VIndex l)
{
    int s = basic_isort4 ((int *)(&o), (int *)(&i), (int *)(&j), (int *)(&k));
    s += basic_isort5p ((int *)(&o), (int *)(&i), (int *)(&j), (int *)(&k), (int *)(&l));

    int answer = FALSE;

    double xo[4], xi[4], xj[4], xk[4], xl[4], xlo[4], xli[4], xlj[4], xlk[4];
  
    rarr.getPoint(l, xl); converter_.truncMul(xl);
    xl[3] = Sign(xl[3])*xl[3]*xl[3];

    rarr.getPoint(o, xo); converter_.truncMul(xo);
    xo[3] = Sign(xo[3])*xo[3]*xo[3]; vsubw(xo, xl, xlo); converter_.trunc (xlo);

    rarr.getPoint(i, xi); converter_.truncMul(xi); 
    xi[3] = Sign(xi[3])*xi[3]*xi[3]; vsubw(xi, xl, xli); converter_.trunc (xli);

    rarr.getPoint(j, xj); converter_.truncMul(xj);
    xj[3] = Sign(xj[3])*xj[3]*xj[3]; vsubw(xj, xl, xlj); converter_.trunc (xlj);

    rarr.getPoint(k, xk); converter_.truncMul(xk);
    xk[3] = Sign(xk[3])*xk[3]*xk[3]; vsubw(xk, xl, xlk); converter_.trunc (xlk);

    double res = insphere(xlo, xli, xlj, xlk);

    if (res)
    {
        answer = If (Odd (s), (res > 0), (res < 0));
#ifdef SOS_TEST
        li_inSphereP++;
        converter_.truncMulToLi(xo, 1, lo_);
        converter_.truncMulToLi(xi, 1, li_);
        converter_.truncMulToLi(xj, 1, lj_);
        converter_.truncMulToLi(xk, 1, lk_);
        converter_.truncMulToLi(xl, 1, ll_);
  
        lambda5_.eval (lo_, li_, lj_, lk_, ll_, o, i, j, k, l, result_);
	  
        int d2 = result_.sign ();
        int answer1 = If (Odd (s), (1 == d2), (1 != d2));
        Assert(answer==answer1);
#endif
    }
    else
    {
        li_inSphereP++;

		//02-Aug, Yan Ke added below five lines.
		rarr.getPoint(l, xl); converter_.truncMul(xl);
		rarr.getPoint(o, xo); converter_.truncMul(xo);
		rarr.getPoint(i, xi); converter_.truncMul(xi); 
		rarr.getPoint(j, xj); converter_.truncMul(xj);
		rarr.getPoint(k, xk); converter_.truncMul(xk);


        converter_.truncMulToLi(xo, 1, lo_);
        converter_.truncMulToLi(xi, 1, li_);
        converter_.truncMulToLi(xj, 1, lj_);
        converter_.truncMulToLi(xk, 1, lk_);
        converter_.truncMulToLi(xl, 1, ll_);
  
        lambda5_.eval (lo_, li_, lj_, lk_, ll_, o, i, j, k, l, result_);
	  
        int d2 = result_.sign ();
        answer = If (Odd (s), (1 == d2), (1 != d2));
    }

    return answer;
}



//--------------------------------------------------------------------------------