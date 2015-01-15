#include <math.h>

// inlines
inline RgiVector::
RgiVector() 
{ 
	v_[0] = v_[1] = v_[2] = 0.0; 
}

inline RgiVector::
RgiVector(double x, double y, double z) 
{
    v_[0] = x; 
	v_[1] = y; 
	v_[2] = z; 
}

inline RgiVector::
RgiVector(const double *v) 
{
    v_[0] = v[0]; 
	v_[1] = v[1]; 
	v_[2] = v[2]; 
}

inline RgiVector::
RgiVector(const RgiVector& w) 
{ 
    v_[0] = w.v_[0]; 
	v_[1] = w.v_[1]; 
	v_[2] = w.v_[2];
}

inline RgiVector::
~RgiVector() 
{
	/***/
}

inline void RgiVector::
set(const double x, const double y, const double z)
{
	v_[0] = x; 
	v_[1] = y;
	v_[2] = z;
}

inline void RgiVector::
scale(const double xs, const double ys, const double zs)
{
	v_[0] *= xs;
	v_[1] *= ys;
	v_[2] *= zs;
}

inline void RgiVector::
scale(const RgiVector &v)
{
	v_[0] *= v.v_[0];
	v_[1] *= v.v_[1];
	v_[2] *= v.v_[2];
}

inline void RgiVector::
negate(void)
{
	v_[0] = -v_[0];
	v_[1] = -v_[1];
	v_[2] = -v_[2];
}

inline void RgiVector::
sub(const RgiVector &v1, const RgiVector &v2)
{
	v_[0] = v1.v_[0] - v2.v_[0];
	v_[1] = v1.v_[1] - v2.v_[1];
	v_[2] = v1.v_[2] - v2.v_[2];
}

inline void RgiVector::
cross(const RgiVector &v1, const RgiVector &v2)
{
	v_[0] = v1.v_[1]*v2.v_[2] - v1.v_[2]*v2.v_[1];
	v_[1] = v1.v_[2]*v2.v_[0] - v1.v_[0]*v2.v_[2];
	v_[2] = v1.v_[0]*v2.v_[1] - v1.v_[1]*v2.v_[0];
}

inline double RgiVector::
magnitude(void) const
{
	return sqrt(v_[0]*v_[0] + v_[1]*v_[1] + v_[2]*v_[2]);
}

inline double RgiVector::operator[](int i) const { return v_[i]; }

inline double& RgiVector::operator[](int i) { return v_[i]; } 

inline double* RgiVector::rep() const { return &(((RgiVector*)this)->v_[0]); }

inline RgiVector& RgiVector::
operator=(const RgiVector& u) 
{
    v_[0]=u.v_[0];
    v_[1]=u.v_[1];
    v_[2]=u.v_[2];
    return *this;
}

inline RgiVector& RgiVector::
operator=(double p[3]) 
{
    v_[0]=p[0];
    v_[1]=p[1];
    v_[2]=p[2];
    return *this;
}

inline int RgiVector::
operator==(const RgiVector& u) const 
{
	return(    v_[0]==u.v_[0]
	        && v_[1]==u.v_[1]
		&& v_[2]==u.v_[2]);
}

inline int RgiVector::
operator!=(const RgiVector& u) const 
{
	return(    v_[0]!=u.v_[0]
	        || v_[1]!=u.v_[1]
		|| v_[2]!=u.v_[2]);
}

inline int RgiVector::
operator<(const RgiVector& u) const 
{
        return(v_[0]<u.v_[0] || (v_[0]==u.v_[0] && v_[1]<u.v_[1])
                || (v_[0]==u.v_[0] && v_[1]==u.v_[1] && v_[2]<u.v_[2]));
}

inline RgiVector& RgiVector::
operator+=(const RgiVector& u) 
{
    v_[0] += u.v_[0];
    v_[1] += u.v_[1];
    v_[2] += u.v_[2];
    return *this;
}

inline RgiVector& RgiVector::
operator-=(const RgiVector& u) 
{
    v_[0] -= u.v_[0];
    v_[1] -= u.v_[1];
    v_[2] -= u.v_[2];
    return *this;
}


inline RgiVector& RgiVector::
operator*=(const RgiVector& u) 
{
    // cross product
    double x = v_[0];
    double y = v_[1];
    double z = v_[2];
    v_[0] = y*u.v_[2] - z*u.v_[1];
    v_[1] = z*u.v_[0] - x*u.v_[2];
    v_[2] = x*u.v_[1] - y*u.v_[0];
    return *this;
}

inline RgiVector& RgiVector::
operator*=(double t) 
{
    v_[0] *= t;
    v_[1] *= t;
    v_[2] *= t;
    return *this;
}

inline RgiVector RgiVector::
operator+(const RgiVector& u) const 
{
	return RgiVector(v_[0]+u.v_[0],v_[1]+u.v_[1],v_[2]+u.v_[2]);
}

inline RgiVector RgiVector::
operator-(const RgiVector& u) const 
{
	return RgiVector(v_[0]-u.v_[0],v_[1]-u.v_[1],v_[2]-u.v_[2]);
}

inline RgiVector RgiVector::
operator*(const RgiVector& w) const 
{
	return RgiVector(v_[1]*w.v_[2]-v_[2]*w.v_[1],
		      v_[2]*w.v_[0]-v_[0]*w.v_[2],
		      v_[0]*w.v_[1]-v_[1]*w.v_[0]);
}

inline RgiVector RgiVector::
operator/(double s) const 
{
	return RgiVector(v_[0]/s, v_[1]/s, v_[2]/s);
}

inline RgiVector RgiVector::
operator-() const 
{
	return RgiVector(-v_[0], -v_[1], -v_[2]);
}

inline RgiVector RgiVector::
operator* (double f) const 
{
    return RgiVector(v_[0]*f, v_[1]*f, v_[2]*f);
}

inline RgiVector operator*(double s, const RgiVector& u) 
{
	return RgiVector(u[0]*s, u[1]*s, u[2]*s);
}

inline double scp(const RgiVector& u1, const RgiVector& u2) 
{
	return( u1[0]*u2[0]
		+ u1[1]*u2[1]
		+ u1[2]*u2[2]);
}

inline double abs(const RgiVector& u) 
{
	return(sqrt(scp(u,u)));
}

inline double dot(const RgiVector& u1, const RgiVector& u2) 
{
	return( u1[0]*u2[0]
		+ u1[1]*u2[1]
		+ u1[2]*u2[2]);
}

inline RgiVector cross (const RgiVector &x, const RgiVector &y)
{
	RgiVector cp;

    cp[0]=x[1]*y[2]-x[2]*y[1]; 
    cp[1]=x[2]*y[0]-x[0]*y[2]; 
    cp[2]=x[0]*y[1]-x[1]*y[0];
    return cp;
}


inline RgiVector RgiVector::
normalized() const 
{
	double L = sqrt(v_[0]*v_[0] + v_[1]*v_[1] + v_[2]*v_[2]);
	if(L==0.0) {
	    return *this;
	} else {
	    return RgiVector(v_[0]/L, v_[1]/L, v_[2]/L);
        }
}

inline int RgiVector::
normalize() 
{
    double L = sqrt(v_[0]*v_[0] + v_[1]*v_[1] + v_[2]*v_[2]);
    if(L==0.0) {
	return 0;
    }
    v_[0] /= L;
    v_[1] /= L;
    v_[2] /= L;
    return 1;
}
