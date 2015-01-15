/*
* size.C -- Method Implementations for the SizeEvaluator classes.
*
* author -- Mike Facello
*
* date -- 12/17/96
*/

#include "sos/size.h"
#include "li/lipoints.h"
#include "li/liminor.h"
#include "li/listack.h"

// static ofstream os("loglog.txt");

static double hidden1Bound = 1.0e-14;
static double hidden2Bound = 1.0e-14;
static double hidden3Bound = 1.0e-12;

/*-----------------------------------------------------------------------*/
/*------------------------ CLASS SIZE EVALUATOR -------------------------*/
/*-----------------------------------------------------------------------*/

SizeEvaluator::
SizeEvaluator(void)
{
    mEval_ = new LI_VertMinorEvaluator;
    tmpPoint_[0].alloc(); tmpPoint_[1].alloc(); 
    tmpPoint_[2].alloc(); tmpPoint_[3].alloc(); 

    int i;
    for(i=0;i<wSize;i++)
        wspace_[i].alloc();
}

/*-----------------------------------------------------------------------*/

SizeEvaluator::
~SizeEvaluator(void)
{
    delete mEval_;
    int i;
    for(i=0;i<4;i++)
        tmpPoint_[i].free();
    for(i=0;i<wSize;i++)
        wspace_[i].free();
}

Bool SizeEvaluator::hidden (RealPArray *r, LI_Converter& c, int v1, int v2, int vp) 
{
    double p1[4], p2[4], pp[4];
    r->getPoint(v1, p1);
    r->getPoint(v2, p2);
    r->getPoint(vp, pp);
    return hidden_ (p1, p2, pp, c);
}

Bool SizeEvaluator::hidden (RealPArray *r, LI_Converter& c, int v1, int v2, int v3, int vp) 
{
    double p1[4], p2[4], p3[4], pp[4];
    r->getPoint(v1, p1);
    r->getPoint(v2, p2);
    r->getPoint(v3, p3);
    r->getPoint(vp, pp);
    return hidden_ (p1, p2, p3, pp, c);
}


/*-----------------------------------------------------------------------*/
/*---------------------- CLASS UW SIZE EVALUATOR ------------------------*/
/*-----------------------------------------------------------------------*/

UWSizeEvaluator::
UWSizeEvaluator(void)
{
    
}

UWSizeEvaluator::
~UWSizeEvaluator(void)
{
    
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

double UWSizeEvaluator::
size (RealPArray * rArray, int v1)
{
    return 0.0;
}

void UWSizeEvaluator::
size (LI_Point & p1,
      LI & numer, LI & denom)
{
    Touch_args (&p1);
    numer = LI(0);
    denom = LI(1);
}

/*-----------------------------------------------------------------------*/

double UWSizeEvaluator::
size (RealPArray * rArray, int v1, int v2)
{
    double p1[4], p2[4];
    rArray->getPoint(v1, p1);
    rArray->getPoint(v2, p2);
    return 0.25*vlength2(vsub(p1, p2));
}

void UWSizeEvaluator::
size (LI_Point & p1, LI_Point & p2, 
      LI & numer, LI & denom)
{
    mEval_->minor2 (p1, p2, 1, 0, tmpLI_[0]);
    mEval_->minor2 (p1, p2, 2, 0, tmpLI_[1]);
    mEval_->minor2 (p1, p2, 3, 0, tmpLI_[2]);
    
    wspace_[0].mult(tmpLI_[0], tmpLI_[0]);
    wspace_[1].mult(tmpLI_[1], tmpLI_[1]);
    wspace_[2].mult(tmpLI_[2], tmpLI_[2]);
    wspace_[3].add(wspace_[1],wspace_[2]);	
    numer.add(wspace_[0], wspace_[3]);
    
    Assert (numer.sign () > 0);
    
    denom = LI(4);
    Assert (denom.sign () > 0);
}

/*-----------------------------------------------------------------------*/

double UWSizeEvaluator::
size (RealPArray *rArray, int v1, int v2, int v3)
{
    double p1[4], p2[4], p3[4];
    rArray->getPoint(v1, p1);
    rArray->getPoint(v2, p2);
    rArray->getPoint(v3, p3);
    // convert to differences
    vsub(p2, p1); vsub(p3, p1); vsub(p3, p2, p1);
    p1[3] = vlength2(p1); p2[3] = vlength2(p2); p3[3] = vlength2(p3);
    double axb = vlength2(vcross(p2, p3, p1));
    return axb? (0.25*p1[3]*(p2[3]*p3[3]/axb)) : MAXFLOAT;
}

void UWSizeEvaluator::
size (LI_Point & p1, LI_Point & p2, LI_Point & p3,
      LI & numer, LI & denom)
{
    LI li4(4);
    mEval_->minor2 (p1, p2, 1, 0, tmpLI_[0]); // 0
    mEval_->minor2 (p1, p2, 2, 0, tmpLI_[1]); // 0
    mEval_->minor2 (p1, p2, 3, 0, tmpLI_[2]); // 0
    mEval_->minor2 (p1, p3, 1, 0, tmpLI_[3]); 
    mEval_->minor2 (p1, p3, 2, 0, tmpLI_[4]);
    mEval_->minor2 (p1, p3, 3, 0, tmpLI_[5]);
    mEval_->minor2 (p2, p3, 1, 0, tmpLI_[6]);
    mEval_->minor2 (p2, p3, 2, 0, tmpLI_[7]);
    mEval_->minor2 (p2, p3, 3, 0, tmpLI_[8]);
    mEval_->minor3 (p1, p2, p3, 1, 2, 0, tmpLI_[9]);
    mEval_->minor3 (p1, p2, p3, 1, 3, 0, tmpLI_[10]);
    mEval_->minor3 (p1, p2, p3, 2, 3, 0, tmpLI_[11]);
    
    wspace_[0].mult(tmpLI_[0], tmpLI_[0]);
    wspace_[1].mult(tmpLI_[1], tmpLI_[1]);
    wspace_[2].mult(tmpLI_[2], tmpLI_[2]);
    wspace_[3].add(wspace_[0],wspace_[1]);
    wspace_[10].add(wspace_[2],wspace_[3]); // keep sum of squares in 10
    
    wspace_[0].mult(tmpLI_[3], tmpLI_[3]);
    wspace_[1].mult(tmpLI_[4], tmpLI_[4]);
    wspace_[2].mult(tmpLI_[5], tmpLI_[5]);
    wspace_[3].add(wspace_[0],wspace_[1]);
    wspace_[11].add(wspace_[2],wspace_[3]); // keep sum of squares in 11
    
    wspace_[0].mult(tmpLI_[6], tmpLI_[6]);
    wspace_[1].mult(tmpLI_[7], tmpLI_[7]);
    wspace_[2].mult(tmpLI_[8], tmpLI_[8]);
    wspace_[3].add(wspace_[0],wspace_[1]);
    wspace_[12].add(wspace_[2],wspace_[3]); // keep sum of squares in 12
    
    wspace_[13].mult(wspace_[10],wspace_[11]);
    numer.mult(wspace_[12], wspace_[13]);
    
    Assert (numer.sign() > 0);
    
    wspace_[0].mult(tmpLI_[9], tmpLI_[9]);
    wspace_[1].mult(tmpLI_[10], tmpLI_[10]);
    wspace_[2].mult(tmpLI_[11], tmpLI_[11]);
    wspace_[3].add(wspace_[0],wspace_[1]);
    wspace_[4].add(wspace_[2],wspace_[3]);
    denom.mult(wspace_[4], li4);
    
    Assert (denom.sign() > 0);
}

/*-----------------------------------------------------------------------*/

double UWSizeEvaluator::
size (RealPArray * rArray, int v1, int v2, int v3, int v4)
{
    double p1[4], p2[4], p3[4], p4[4];
    rArray->getPoint(v1, p1);
    rArray->getPoint(v2, p2);
    rArray->getPoint(v3, p3);
    rArray->getPoint(v4, p4);
    // shift p1 to origin
    vsub(p2, p1); vsub(p3, p1); vsub(p4, p1);
    p2[3] = vlength2(p2); p3[3] = vlength2(p3); p4[3] = vlength2(p4);
    
    double d0 = -det3(p2[0], p2[1], p2[2],
        p3[0], p3[1], p3[2],
        p4[0], p4[1], p4[2]);
    
    if (!d0) return MAXFLOAT;
    
    double d2 = det3(p2[0], p2[1], p2[3],
        p3[0], p3[1], p3[3],
        p4[0], p4[1], p4[3]);
    double d3 = det3(p2[0], p2[2], p2[3],
        p3[0], p3[2], p3[3],
        p4[0], p4[2], p4[3]);
    double d4 = det3(p2[1], p2[2], p2[3],
        p3[1], p3[2], p3[3],
        p4[1], p4[2], p4[3]);
    
    return 0.25*(d2*d2 + d3*d3 + d4*d4)/(d0*d0);
}

void UWSizeEvaluator::
size (LI_Point & p1, LI_Point & p2, LI_Point & p3, LI_Point & p4,
      LI & numer, LI & denom)
{
    LI li4(4);
    
    mEval_->minor4 (p1, p2, p3, p4, 1, 2, 3, 0, tmpLI_[0]);
    mEval_->minor4 (p1, p2, p3, p4, 1, 2, 3, 4, tmpLI_[1]);
    mEval_->minor4 (p1, p2, p3, p4, 1, 2, 4, 0, tmpLI_[2]);
    mEval_->minor4 (p1, p2, p3, p4, 1, 3, 4, 0, tmpLI_[3]);
    mEval_->minor4 (p1, p2, p3, p4, 2, 3, 4, 0, tmpLI_[4]);
    mEval_->minor4 (p1, p2, p3, p4, 1, 2, 3, 0, tmpLI_[5]);
    
    wspace_[0].mult(tmpLI_[0], tmpLI_[1]);
    wspace_[1].mult(wspace_[0], li4);
    wspace_[2].mult(tmpLI_[2], tmpLI_[2]);
    wspace_[3].mult(tmpLI_[3], tmpLI_[3]);
    wspace_[4].mult(tmpLI_[4], tmpLI_[4]);
    wspace_[5].add(wspace_[1],wspace_[2]);
    wspace_[6].add(wspace_[5],wspace_[3]);
    numer.add(wspace_[6], wspace_[4]);
    
    Assert (numer.sign() > 0);
    wspace_[5].mult(tmpLI_[5], tmpLI_[5]);
    denom.mult(wspace_[5], li4);
    Assert (denom.sign () > 0);
}

/*-----------------------------------------------------------------------*/

Bool UWSizeEvaluator::hidden (RealPArray *r, LI_Converter& c, int v1, int vp) 
{ 
    return false; 
}

/*-----------------------------------------------------------------------*/

Bool UWSizeEvaluator::
hidden_ (LI_Point & p1, LI_Point & p2, LI_Point & pp)
{
    wspace_[0].sub(p1[1], pp[1]);
    wspace_[1].sub(p1[2], pp[2]);
    wspace_[2].sub(p1[3], pp[3]);
    wspace_[3].sub(pp[1], p2[1]);
    wspace_[4].sub(pp[2], p2[2]);
    wspace_[5].sub(pp[3], p2[3]);
    
    wspace_[7].mult(wspace_[0], wspace_[3]);
    wspace_[8].mult(wspace_[1], wspace_[4]);
    wspace_[9].mult(wspace_[2], wspace_[5]);
    wspace_[10].add(wspace_[7], wspace_[8]);
    wspace_[6].add(wspace_[10], wspace_[9]);
    
    //*------------------------------------------------
    switch (wspace_[6].sign() /* liStack_.pop ().sign () */)
    {
    case  1: return (TRUE);
    case  0: return (TRUE + TRUE);
    case -1: return (FALSE);
    default:
        Assert_always (FALSE);
        return (FALSE);  /* just to silent up lint */
    }
}

Bool UWSizeEvaluator::
hidden_ (double p1[4], double p2[4], double pp[4], LI_Converter& converter)
{
    // make pp the origin, so hidden is p1*p2
    double x1[4], x2[4], xx[4];
    vsub(p1, pp, x1);
    vsub(pp, p2, x2);
    vzero(xx);
    converter.truncMul(x1);
    converter.truncMul(x2);
    double d0 = x1[0]*x2[0];
    double d1 = x1[1]*x2[1];
    double d2 = x1[2]*x2[2];
    double res = d0 + d1 + d2;
    double modul = Abs(d0) + Abs(d1) +  Abs(d2);
    
    if (Abs(res) < modul*hidden2Bound)
    {
        LI_Point& lp1 = tmpPoint_[0];
        LI_Point& lp2 = tmpPoint_[1];
        LI_Point& lpp = tmpPoint_[2];
        converter.toLi(p1, 1, lp1);
        converter.toLi(p2, 1, lp2);
        converter.toLi(pp, 1, lpp);
        return hidden_(lp1, lp2, lpp);
    }
    
    return (res > 0);
    
}

/*-----------------------------------------------------------------------*/

Bool UWSizeEvaluator::
hidden_ (double p1[4], double p2[4], double p3[4], double pp[4], LI_Converter& converter)
{
    // shift to make p1 origin
    double x2[4], x3[4], xp[4]; // x1 is by definition - zero
    vsub(p2, p1, x2); vsub(p3, p1, x3); vsub(pp, p1, xp);
    converter.truncMul(x2); converter.truncMul(x3); converter.truncMul(xp); 
    x2[3] = vlength2(x2); x3[3] = vlength2(x3); xp[3] = vlength2(xp);
    // three points are set up
    double modul40, modul41, modul42;
    double minor40 = -det3(x2[1], x2[2], x2[3],
        x3[1], x3[2], x3[3],
        xp[1], xp[2], xp[3], &modul40);
    double minor41 = -det3(x2[0], x2[2], x2[3],
        x3[0], x3[2], x3[3],
        xp[0], xp[2], xp[3], &modul41);
    double minor42 = -det3(x2[0], x2[1], x2[3],
        x3[0], x3[1], x3[3],
        xp[0], xp[1], xp[3], &modul42);
    
    double modul30, modul31, modul32;
    double minor30 = det2(x2[1], x2[2], 
        x3[1], x3[2], &modul30);
    double minor31 = det2(x2[0], x2[2], 
        x3[0], x3[2], &modul31);
    double minor32 = det2(x2[0], x2[1], 
        x3[0], x3[1], &modul32);
    
    double res =   minor40*minor30 + minor41*minor31 + minor42*minor32;
    double modul = modul40*modul30 + modul41*modul31 + modul42*modul32;
    
    if (Abs(res) < modul*hidden3Bound)
    {
        LI_Point& lp1 = tmpPoint_[0];
        LI_Point& lp2 = tmpPoint_[1];
        LI_Point& lp3 = tmpPoint_[2];
        LI_Point& lpp = tmpPoint_[3];
        converter.toLi(p1, 1, lp1);
        converter.toLi(p2, 1, lp2);
        converter.toLi(p3, 1, lp3);
        converter.toLi(pp, 1, lpp);
        return hidden_(lp1, lp2, lp3, lpp);
    }
    
    return (res > 0);
}

/*-----------------------------------------------------------------------*/

Bool UWSizeEvaluator::
hidden_ (LI_Point & p1, LI_Point & p2, LI_Point & p3, LI_Point & pp)
{
    LI li2(2);
    mEval_->minor4 (p1, p2, p3, pp, 2, 3, 4, 0, tmpLI_[0]);
    mEval_->minor3 (p1, p2, p3, 2, 3, 0, tmpLI_[1]);
    mEval_->minor4 (p1, p2, p3, pp, 1, 3, 4, 0, tmpLI_[2]);
    mEval_->minor3 (p1, p2, p3, 1, 3, 0, tmpLI_[3]);
    mEval_->minor4 (p1, p2, p3, pp, 1, 2, 4, 0, tmpLI_[4]);
    mEval_->minor3 (p1, p2, p3, 1, 2, 0, tmpLI_[5]);
    mEval_->minor4 (p1, p2, p3, pp, 1, 2, 3, 0, tmpLI_[6]);
    mEval_->minor3 (p1, p2, p3, 1, 2, 3, tmpLI_[7]);
    
    wspace_[0].mult(tmpLI_[0], tmpLI_[1]);
    wspace_[2].mult(tmpLI_[2], tmpLI_[3]);
    wspace_[4].mult(tmpLI_[4], tmpLI_[5]);
    wspace_[1].add(wspace_[0], wspace_[2]);
    wspace_[3].add(wspace_[1], wspace_[4]); // wspace_[3] is busy
    
    wspace_[6].mult(tmpLI_[6], tmpLI_[7]);
    wspace_[7].mult(wspace_[6], li2);
    wspace_[0].sub(wspace_[3], wspace_[7]);
    
    switch (wspace_[0].sign() /* liStack_.pop ().sign () */ )
    {
    case  1: return (TRUE);
    case  0: return (TRUE + TRUE);
    case -1: return (FALSE);
    default:
        Assert_always (FALSE);
        return (FALSE);  /* just to silent up lint */
    }
}


/*-----------------------------------------------------------------------*/
/*---------------------- CLASS W SIZE EVALUATOR -------------------------*/
/*-----------------------------------------------------------------------*/


WSizeEvaluator::
WSizeEvaluator(void)
{
    int i,j;
    for(i=0;i<12;i++)
        tmpLI_[i].alloc();
    for(i=0;i<5;i++)
        for(j=0;j<5;j++)
            results_[i][j].alloc();
};

WSizeEvaluator::
~WSizeEvaluator(void)
{
    int i,j;
    for(i=0;i<12;i++)
        tmpLI_[i].free();
    for(i=0;i<5;i++)
        for(j=0;j<5;j++)
            results_[i][j].free();
    
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

void WSizeEvaluator::
size (LI_Point & p1,
      LI & numer, LI & denom)
{
}

/*-----------------------------------------------------------------------*/

void WSizeEvaluator::
size (LI_Point & p1, LI_Point & p2, 
      LI & numer, LI & denom)
{
}

/*-----------------------------------------------------------------------*/

void WSizeEvaluator::
size (LI_Point & p1, LI_Point & p2, LI_Point & p3,
      LI & numer, LI & denom)
{
}

/*-----------------------------------------------------------------------*/

void WSizeEvaluator::
size (LI_Point & p1, LI_Point & p2, LI_Point & p3, LI_Point & p4,
      LI & numer, LI & denom)
{
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

Bool WSizeEvaluator::hidden (RealPArray *r, LI_Converter& c, int v1, int vp) 
{
    // not done
    double p1[4], pp[4];
    r->getPoint(v1, p1);
    r->getPoint(vp, pp);
    return hidden_ (p1, pp, c);
}

/*-----------------------------------------------------------------------*/

Bool WSizeEvaluator::
hidden_ (double p1[4], double pp[4], LI_Converter& converter)
{
    double p1p[4],wp1,wpp,l2;
    vsubw(p1,pp,p1p);
    wp1 = Sign(p1[3])*p1[3]*p1[3];
    wpp = Sign(pp[3])*pp[3]*pp[3];
    l2 = vlength2(p1p);
    double res = l2-wpp+wp1;
    double modul = l2 + Abs(wpp) + Abs(wp1);
    
    if (Abs(res) < modul*hidden1Bound)
    {
        LI_Point& lp1 = tmpPoint_[0];
        LI_Point& lpp = tmpPoint_[1];
        converter.toLi(p1, 0, lp1);
        converter.toLi(pp, 0, lpp);
        return hidden_(lp1, lpp);
    }
    
    return (res <= 0);
}

Bool WSizeEvaluator::
hidden_ (LI_Point & p1, LI_Point & pp)
{
    wspace_[0].sub(p1[1], pp[1]);
    wspace_[1].sub(p1[2], pp[2]);
    wspace_[2].sub(p1[3], pp[3]);
    
    wspace_[3].mult(wspace_[0],wspace_[0]);
    wspace_[4].mult(wspace_[1],wspace_[1]);  
    wspace_[5].mult(wspace_[2],wspace_[2]);
    
    wspace_[6].mult(p1[4],p1[4]);
    if(p1[4].sign()==-1)
        wspace_[6].negate();
    wspace_[7].mult(pp[4],pp[4]);
    if(pp[4].sign()==-1)
        wspace_[7].negate();
    
    wspace_[0].add(wspace_[3],wspace_[4]);
    wspace_[1].add(wspace_[0],wspace_[5]);
    wspace_[2].add(wspace_[1],wspace_[6]);
    wspace_[0].sub(wspace_[2],wspace_[7]);
    
    //*------------------------------------------------
    switch (wspace_[0].sign())
    {
    case -1: return (TRUE);
    case  0: return (TRUE + TRUE);
    case  1: return (FALSE);
    default:
        Assert_always (FALSE);
        return (FALSE);  /* just to silent up lint */
    }
    
}



Bool WSizeEvaluator::
hidden_ (double p0[4], double p1[4], double pp[4], LI_Converter& converter)
{ 

    double M[4][4];
    double p0w[4],p1w[4],ppw[4];
    int i;
    double m1,m2,m3,m4,m5,m6,m7;
    double t1,t2,t3,t4;
    double res;

    vcopyw(p0,p0w);
    vcopyw(p1,p1w);
    vcopyw(pp,ppw);
/*
    converter.truncMul(p0w);
    converter.truncMul(p1w);	
    converter.truncMul(ppw);
*/
    p0w[3] = p0w[0]*p0w[0]+p0w[1]*p0w[1]+p0w[2]*p0w[2]-Sign(p0w[3])*p0w[3]*p0w[3];
    p1w[3] = p1w[0]*p1w[0]+p1w[1]*p1w[1]+p1w[2]*p1w[2]-Sign(p1w[3])*p1w[3]*p1w[3];
    ppw[3] = ppw[0]*ppw[0]+ppw[1]*ppw[1]+ppw[2]*ppw[2]-Sign(ppw[3])*ppw[3]*ppw[3];

	for(i=0;i<4;i++)
	{
		M[0][i] = p1w[i]-p0w[i];
		M[3][i] = ppw[i]-p0w[i];
	}

    for(i=0;i<3;i++)
    {
        M[0][i]*=2;
        M[3][i]*=2;
    }

    M[1][0] = p0w[1]-p1w[1];
    M[1][1] = p1w[0]-p0w[0];
    M[1][2] = 0;
    M[1][3] = p1w[0]*p0w[1]-p0w[0]*p1w[1];
    M[2][0] = p0w[2]-p1w[2];
    M[2][1] = 0;
    M[2][2] = p1w[0]-p0w[0];
    M[2][3] = p1w[0]*p0w[2]-p0w[0]*p1w[2];

    t1 = 2*det3(M[1][1],M[1][2],M[1][3],
                M[2][1],M[2][2],M[2][3],
                M[3][1],M[3][2],M[3][3],&m1);

    t2 = - M[0][1] * ( det2(M[1][0],M[1][3],
                            M[3][0],M[3][3],&m2) 
                     - M[3][2]* det2(p0w[1],p0w[2],
                                     p1w[1],p1w[2],&m3));

    t3 = -M[0][2]* ( det2(M[2][0],M[2][3],
                          M[3][0],M[3][3],&m4)+
                    M[3][1]*det2(p0w[1],p0w[2],
                                 p1w[1],p1w[2],&m5));
    t4 =  M[0][3] * ( M[1][0]*M[3][1]+det2(M[2][0],M[2][2],
                                           M[3][0],M[3][2],&m6));

    res = t1+t2+t3+t4;

    m7 = 2*m1+Abs(M[0][1])*(m2+m3)+Abs(M[0][2]*(m4+Abs(M[3][1])*m5))+
         Abs(M[0][3])*(Abs(M[1][0]*M[3][1])+m6);



    Assert(m7>=0);
	
	if(res==0)
		return 2;

    if (Abs(res) < m7*hidden2Bound)
    {
        LI_Point& lp0 = tmpPoint_[0];
        LI_Point& lp1 = tmpPoint_[1];
        LI_Point& lpp = tmpPoint_[2];
        converter.toLi(p0, 1, lp0);
        converter.toLi(p1, 1, lp1);
        converter.toLi(pp, 1, lpp);
        return hidden_(lp0, lp1, lpp);
    }
    return (res <= 0);
    
}



Bool WSizeEvaluator::
hidden_ (LI_Point & p0, LI_Point & p1, LI_Point & pp)
{

    LI li2(2);
    // set up matrix
    int i;
    for(i=0;i<3;i++)
    {
        wspace_[0].sub(p1[i+1],p0[i+1]);
        results_[0][i].mult(li2,wspace_[0]);
    }
    results_[0][3].sub(p1[4],p0[4]);

    results_[1][0].sub(p0[2],p1[2]);
    results_[1][1].sub(p1[1],p0[1]);
    wspace_[0].mult(p1[1],p0[2]);
    wspace_[1].mult(p0[1],p1[2]);
    results_[1][3].sub(wspace_[0],wspace_[1]);
    results_[2][0].sub(p0[3],p1[3]);
    results_[2][2].sub(p1[1],p0[1]);
    wspace_[0].mult(p1[1],p0[3]);
    wspace_[1].mult(p0[1],p1[3]);
    results_[2][3].sub(wspace_[0],wspace_[1]);

    for(i=0;i<3;i++)
    {
        wspace_[0].sub(pp[i+1],p0[i+1]);
        results_[3][i].mult(li2,wspace_[0]);
    }
    results_[3][3].sub(pp[4],p0[4]);

    // term 1
    
    // minor11:
    wspace_[0].mult(results_[2][2],results_[3][3]);
    wspace_[1].mult(results_[3][2],results_[2][3]);
    wspace_[2].sub(wspace_[0],wspace_[1]);
    wspace_[3].mult(results_[1][1],wspace_[2]);

    wspace_[4].mult(results_[3][1],results_[2][2]);
    wspace_[4].negate();
    wspace_[5].mult(results_[1][3],wspace_[4]);
    wspace_[6].add(wspace_[3],wspace_[5]);

    wspace_[0].mult(li2,wspace_[6]); // term 1

    // term 2
    
    wspace_[1].mult(results_[1][0],results_[3][3]);
    wspace_[2].mult(results_[1][3],results_[3][0]);
    wspace_[3].sub(wspace_[1],wspace_[2]);

    wspace_[4].mult(p0[2],p1[3]);
    wspace_[5].mult(p1[2],p0[3]);
    wspace_[6].sub(wspace_[4],wspace_[5]);
    wspace_[7].mult(results_[3][2],wspace_[6]);

    wspace_[8].sub(wspace_[3],wspace_[7]);
    wspace_[1].mult(wspace_[8],results_[0][1]);
    
    wspace_[1].negate(); // term 2;

    // term 3;

    wspace_[2].mult(results_[2][0],results_[3][3]);
    wspace_[3].mult(results_[3][0],results_[2][3]);
    wspace_[4].sub(wspace_[2],wspace_[3]);
    wspace_[4].negate();

    wspace_[5].mult(p0[2],p1[3]);
    wspace_[6].mult(p1[2],p0[3]);
    wspace_[7].sub(wspace_[5],wspace_[6]);
    wspace_[8].mult(results_[3][1],wspace_[7]);

    wspace_[9].sub(wspace_[4],wspace_[8]);
    wspace_[2].mult(wspace_[9],results_[0][2]); // term 3
    
    // term 4

    wspace_[3].mult(results_[1][0],results_[3][1]);
    
    wspace_[4].mult(results_[2][0],results_[3][2]);
    wspace_[5].mult(results_[3][0],results_[2][2]);
	wspace_[6].sub(wspace_[4],wspace_[5]);
    
    wspace_[7].add(wspace_[3],wspace_[6]);
    wspace_[3].mult(results_[0][3],wspace_[7]);  // term 4

    wspace_[4].add(wspace_[0],wspace_[1]);
    wspace_[5].add(wspace_[2],wspace_[3]);
    wspace_[6].add(wspace_[4],wspace_[5]);
    
    //*------------------------------------------------
    switch (wspace_[6].sign() /* liStack_.pop ().sign () */)
    {
    case  -1: return (TRUE);
    case  0: return (TRUE + TRUE);
    case  1: return (FALSE);
    default:
        Assert_always (FALSE);
        return (FALSE);  /* just to silent up lint */
    }
}

/*-----------------------------------------------------------------------*/

Bool WSizeEvaluator::
hidden_ (double p0[4], double p1[4], double p2[4], double pp[4], LI_Converter& converter)
{

    // shift to make p1 origin

    double p0w[4],p1w[4],p2w[4],ppw[4];

    vcopyw(p0,p0w);
    vcopyw(p1,p1w);
    vcopyw(p2,p2w);
    vcopyw(pp,ppw);

    converter.truncMul(p2w); converter.truncMul(p1w); converter.truncMul(p0w); converter.truncMul(ppw); 

    p0w[3] = p0w[0]*p0w[0]+p0w[1]*p0w[1]+p0w[2]*p0w[2]-Sign(p0w[3])*p0w[3]*p0w[3];
    p1w[3] = p1w[0]*p1w[0]+p1w[1]*p1w[1]+p1w[2]*p1w[2]-Sign(p1w[3])*p1w[3]*p1w[3];
    p2w[3] = p2w[0]*p2w[0]+p2w[1]*p2w[1]+p2w[2]*p2w[2]-Sign(p2w[3])*p2w[3]*p2w[3];
    ppw[3] = ppw[0]*ppw[0]+ppw[1]*ppw[1]+ppw[2]*ppw[2]-Sign(ppw[3])*ppw[3]*ppw[3];

    double mN1,mN2,mN3,mN0;
    double NI1 =  det3(1,p0w[1],p0w[2],
                       1,p1w[1],p1w[2],
                       1,p2w[1],p2w[2],&mN1);
    double NI2 = -det3(1,p0w[0],p0w[2],
                       1,p1w[0],p1w[2],
                       1,p2w[0],p2w[2],&mN2);
    double NI3 =  det3(1,p0w[0],p0w[1],
                       1,p1w[0],p1w[1],
                       1,p2w[0],p2w[1],&mN3);
    double NI0 = -det3(p0w[0],p0w[1],p0w[2],
                       p1w[0],p1w[1],p1w[2],
                       p2w[0],p2w[1],p2w[2],&mN0);

    Assert(det4(2*p0w[0],2*p0w[1],2*p0w[2],-1,
           2*p1w[0],2*p1w[1],2*p1w[2],-1,
           2*p2w[0],2*p2w[1],2*p2w[2],-1,
           NI1     ,NI2     ,NI3     , 0)>=0);

    double ml1,ml2,ml3,ml4;
    double lam1 = det4(2*p0w[0],2*p0w[1],2*p0w[2],-1,
                       2*p1w[0],2*p1w[1],2*p1w[2],-1,
                       2*p2w[0],2*p2w[1],2*p2w[2],-1,
                       2*ppw[0],2*ppw[1],2*ppw[2],-1,&ml1);
    double lam2=  det4(p0w[3],2*p0w[1],2*p0w[2],-1,
                       p1w[3],2*p1w[1],2*p1w[2],-1,
                       p2w[3],2*p2w[1],2*p2w[2],-1,
                       ppw[3],2*ppw[1],2*ppw[2],-1,&ml2);
    double lam3=  det4(p0w[3],2*p0w[0],2*p0w[2],-1,
                       p1w[3],2*p1w[0],2*p1w[2],-1,
                       p2w[3],2*p2w[0],2*p2w[2],-1,
                       ppw[3],2*ppw[0],2*ppw[2],-1,&ml3);
    double lam4=  det4(p0w[3],2*p0w[0],2*p0w[1],-1,
                       p1w[3],2*p1w[0],2*p1w[1],-1,
                       p2w[3],2*p2w[0],2*p2w[1],-1,
                       ppw[3],2*ppw[0],2*ppw[1],-1,&ml4);

    double res =(NI0*lam1+NI1*lam2-NI2*lam3+NI3*lam4);
    double modul = mN0*ml1+mN1*ml2+mN2*ml3+mN3*ml4;
    if (Abs(res) < modul*hidden3Bound)
    {
        LI_Point& lp0 = tmpPoint_[0];
        LI_Point& lp1 = tmpPoint_[1];
        LI_Point& lp2 = tmpPoint_[2];
        LI_Point& lpp = tmpPoint_[3];
        converter.toLi(p0, 1, lp0);
        converter.toLi(p1, 1, lp1);
        converter.toLi(p2, 1, lp2);
        converter.toLi(pp, 1, lpp);
        return hidden_(lp0, lp1, lp2, lpp);
    }
    
    return (res < 0);


}

/*-----------------------------------------------------------------------*/

Bool WSizeEvaluator::
hidden_ (LI_Point & p1, LI_Point & p2, LI_Point & p3, LI_Point & pp)
{
    
    // not done
    LI li2(2);
    mEval_->minor4 (p1, p2, p3, pp, 2, 3, 4, 0, tmpLI_[0]);
    mEval_->minor3 (p1, p2, p3, 2, 3, 0, tmpLI_[1]);
    mEval_->minor4 (p1, p2, p3, pp, 1, 3, 4, 0, tmpLI_[2]);
    mEval_->minor3 (p1, p2, p3, 1, 3, 0, tmpLI_[3]);
    mEval_->minor4 (p1, p2, p3, pp, 1, 2, 4, 0, tmpLI_[4]);
    mEval_->minor3 (p1, p2, p3, 1, 2, 0, tmpLI_[5]);
    mEval_->minor4 (p1, p2, p3, pp, 1, 2, 3, 0, tmpLI_[6]);
    mEval_->minor3 (p1, p2, p3, 1, 2, 3, tmpLI_[7]);
    
    wspace_[0].mult(tmpLI_[0], tmpLI_[1]);
    wspace_[2].mult(tmpLI_[2], tmpLI_[3]);
    wspace_[4].mult(tmpLI_[4], tmpLI_[5]);
    wspace_[1].add(wspace_[0], wspace_[2]);
    wspace_[3].add(wspace_[1], wspace_[4]); // wspace_[3] is busy
    
    wspace_[6].mult(tmpLI_[6], tmpLI_[7]);
    wspace_[7].mult(wspace_[6], li2);
    wspace_[0].sub(wspace_[3], wspace_[7]);
    // these are computation of -Lambda, but where is Gamma?

    switch (wspace_[0].sign() /* liStack_.pop ().sign () */ )
    {
    case  1: return (TRUE);
    case  0: return (TRUE + TRUE);
    case -1: return (FALSE);
    default:
        Assert_always (FALSE);
        return (FALSE);  /* just to silent up lint */
    }
}
