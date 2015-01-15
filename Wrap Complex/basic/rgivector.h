#ifndef __RGI_RGIVECTOR_H__
#define __RGI_RGIVECTOR_H__

#include <cstdlib>
#include <iostream>

class BinInput;
class BinOutput;

class RgiVector 
{
public:
    //- RgiVector 
    RgiVector();
    RgiVector(const RgiVector&);
    RgiVector(double, double, double);
    RgiVector(const double *v);
    ~RgiVector();

	// Set
	void set(const double x, const double y, const double z);
		// Sets the components of the vector.

    //- operator=
    RgiVector& operator=(const RgiVector&);
	RgiVector & operator =(double p[3]);

    //- operator==, operator!=, operator<
    int operator==(const RgiVector&) const;
    int operator!=(const RgiVector&) const;
    int operator<(const RgiVector&) const;  // lexiographic order

    //- operator[]
    double operator[](int i) const;
    double& operator[](int i);

    //- operator+=, operator-=, operator*=
    RgiVector& operator+=(const RgiVector&);
    RgiVector& operator-=(const RgiVector&);
    RgiVector& operator*=(const RgiVector&);
    RgiVector& operator*=(double);

    //- operator+, operator-, operator*, operator/, operator-, 
    //- operator*
    RgiVector operator+(const RgiVector&) const;
    RgiVector operator-(const RgiVector&) const;
    RgiVector operator*(const RgiVector&) const;
    RgiVector operator/(double) const;
    RgiVector operator-() const;
    RgiVector operator*(double) const;
    friend RgiVector operator*(double, const RgiVector&);

    //- normalized
    RgiVector normalized() const;
    int normalize();

	void shift (double dx, double dy, double dz);

	void scale(const double xs, const double ys, const double zs);
	void scale(const RgiVector &v);
	void negate(void);
	void sub(const RgiVector &v1, const RgiVector &v2);
	void cross(const RgiVector &v1, const RgiVector &v2);
	double magnitude(void) const;

	operator double* () { return v_;};
	operator const double* () const { return v_; }
    
	void binDump (BinOutput &);
	int binInit (BinInput &);

    //- rep
    double * rep() const;
private:
    double v_[3];
};
typedef RgiVector *RgiVectorPtr;

// streams
extern const char* MAM_NAME(RgiVector*);
extern ostream& operator<<(ostream&, const RgiVector&);
extern istream& operator>>(istream&, RgiVector&);

// (eukl.) vector length
double abs(const RgiVector&);

// scalar product
double scp(const RgiVector&, const RgiVector&);
double dot(const RgiVector&, const RgiVector&);

//cross product
RgiVector cross (const RgiVector&, const RgiVector&);

#include <basic/rgivector.hpp>

#endif
