/*
* size.h -- Header file for the SizeEvaluator classes.
*
* author -- Mike Facello
*
* date -- 12/17/96
*/


#ifndef __RGI_SIZE_H__
#define __RGI_SIZE_H__


#include <basic/basic.h>
#include <li/li.h>
#include <li/lipoints.h>

class LI_Point;
class LI_VertMinorEvaluator;

//--------------------------------------------------------------------------

//. CLASS SizeEvaluator
//-
//- This class implements size computations and hidden tests.

class SizeEvaluator
{
public:
    SizeEvaluator (void);
    virtual ~SizeEvaluator (void);
    
    virtual double size (RealPArray *, int v1) = 0;
    virtual double size (RealPArray *, int v1, int v2) = 0;
    virtual double size (RealPArray *, int v1, int v2, int v3) = 0;
    virtual double size (RealPArray *, int v1, int v2, int v3, int v4) = 0;
    
    virtual Bool hidden (RealPArray *, LI_Converter&, int v1, int vp) = 0;
    virtual Bool hidden (RealPArray *, LI_Converter&, int v1, int v2, int vp);
    virtual Bool hidden (RealPArray *, LI_Converter&, int v1, int v2, int v3, int vp);
    
protected:
    virtual Bool hidden_ (double p1[4], double p2[4], double pp[4], LI_Converter&) = 0;
    virtual Bool hidden_ (double p1[4], double p2[4], double p3[4], double pp[4], LI_Converter&) = 0;
    virtual Bool hidden_ (LI_Point& p1, LI_Point& p2, LI_Point& pp) = 0;
    virtual Bool hidden_ (LI_Point& p1, LI_Point& p2, LI_Point& p3, LI_Point& pp) = 0;
    
    LI_VertMinorEvaluator *mEval_;
    //  LI_Stack liStack_;
    enum { wSize = 17 };        // Size of the workspace
    LI wspace_[wSize];          // The workspace 
    LI_Point tmpPoint_[4];
};


//--------------------------------------------------------------------------

//. CLASS UWSizeEvaluator
//-
//- This class implements size computations and hidden tests.

class UWSizeEvaluator : public SizeEvaluator
{
public:
    UWSizeEvaluator (void);
    ~UWSizeEvaluator (void);
    
    double size (RealPArray *, int v1);
    double size (RealPArray *, int v1, int v2);
    double size (RealPArray *, int v1, int v2, int v3);
    double size (RealPArray *, int v1, int v2, int v3, int v4);
    
    void size (LI_Point& p1,
        LI& numer, LI& denom);
    void size (LI_Point& p1, LI_Point& p2, 
        LI& numer, LI& denom);
    void size (LI_Point& p1, LI_Point& p2, LI_Point& p3,
        LI& numer, LI& denom);
    void size (LI_Point& p1, LI_Point& p2, LI_Point& p3, LI_Point& p4,
        LI& numer, LI& denom);
    
    virtual Bool hidden (RealPArray *, LI_Converter&, int v1, int vp);
    
protected:
    Bool hidden_ (double p1[4], double p2[4], double pp[4], LI_Converter& );
    Bool hidden_ (double p1[4], double p2[4], double p3[4], double pp[4], LI_Converter& );
    Bool hidden_ (LI_Point& p1, LI_Point& p2, LI_Point& pp);
    Bool hidden_ (LI_Point& p1, LI_Point& p2, LI_Point& p3, LI_Point& pp);
    
private:
    LI tmpLI_[12];    // Temporary locations for LI values
    
};


//--------------------------------------------------------------------------

//. CLASS WSizeEvaluator
//-
//- This class implements size computations and hidden tests.

class WSizeEvaluator : public SizeEvaluator
{
public:
    WSizeEvaluator (void);
    ~WSizeEvaluator (void);
    double size (RealPArray *, int v1) {return -1.0; }
    double size (RealPArray *, int v1, int v2) {return -1.0; }
    double size (RealPArray *, int v1, int v2, int v3) {return -1.0; }
    double size (RealPArray *, int v1, int v2, int v3, int v4) {return -1.0; }
    
    void size (LI_Point& p1,
        LI& numer, LI& denom);
    void size (LI_Point& p1, LI_Point& p2, 
        LI& numer, LI& denom);
    void size (LI_Point& p1, LI_Point& p2, LI_Point& p3,
        LI& numer, LI& denom);
    void size (LI_Point& p1, LI_Point& p2, LI_Point& p3, LI_Point& p4,
        LI& numer, LI& denom);
    
    virtual Bool hidden (RealPArray *, LI_Converter&, int v1, int vp);
    
protected:
    Bool hidden_ (double p1[3], double pp[3], LI_Converter& ) ;
    Bool hidden_ (double p1[3], double p2[3], double pp[3], LI_Converter& ) ;
    Bool hidden_ (double p1[3], double p2[3], double p3[3], double pp[3], LI_Converter& ) ;
    Bool hidden_ (LI_Point& p1, LI_Point& p2);
    Bool hidden_ (LI_Point& p1, LI_Point& p2, LI_Point& p3);
    Bool hidden_ (LI_Point& p1, LI_Point& p2, LI_Point& p3, LI_Point& p4);
    
private:
    LI results_[5][5];
private:
    LI tmpLI_[12];    // Temporary locations for LI values
};


#endif // __SIZE_H__

