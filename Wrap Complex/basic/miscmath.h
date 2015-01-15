
#ifndef __RGI_MISCMATH_H__
#define __RGI_MISCMATH_H__

#include <basic/basic.h>
#include <fstream>

/*--------------------------------------------------------------------------*/
#ifdef MIN
#undef MIN
#endif

#ifdef MAX
#undef MAX
#endif

#ifdef Abs
#undef Abs
#endif

#ifdef Sign
#undef Sign
#endif

#ifdef PI
#undef PI 
#endif

#ifdef PI_2
#undef PI_2
#endif

#ifdef PI_4
#undef  PI_4
#endif

const double PI = 3.14159265358979323846;
const double PI_2 = 1.57079632679489661923;
const double PI_4 = 0.78539816339744830962;
const unsigned long GET_RANDOM_MAX = (1 << 30) - 1;

int solve3 (double a[3], double b[3], double c[3], double d[3], 
            double x[3]);
double tetraOrVolume(double a[3], double b[3], double c[3], double d[3]);
double tetraVolume (double a[3], double b[3], double c[3], double d[3]);
void computeNormal (double a[3], double b[3], double c[3], double normal[3]);
void normalNormalize (double normal[3]);
void computeNormalizedNormal (double a[3], double b[3], double c[3], 
			       double normal[3]);
double computeDotProduct (double a[3], double b[3]);

void setSeed (unsigned seed);
unsigned getSeed (void);

int getRandom (void);

/*--------------------------------------------------------------------------*/
// compute middle point Z equally far from a,b,c,d using solve3()
// return 0 on failure

int mid2 (double a[3], double b[3],                           double z[3]);
int mid3 (double a[3], double b[3], double c[3],              double z[3]);
int mid4 (double a[3], double b[3], double c[3], double d[3], double z[3]);
/*--------------------------------------------------------------------------*/
// general operations
template <class T>
inline T MIN( T a, T b) {return (a < b)? a : b; }

template <class T>
inline T MAX( T a, T b) {return (a > b)? a : b; }

template <class T>
inline T MIN( T a, T b, T c) {return MIN(MIN(a, b), c); }

template <class T>
inline T MAX( T a, T b, T c) {return MAX(MAX(a, b), c); }

template <class T>
inline T MIN( T a, T b, T c, T d) {return MIN(MIN(a, b), MIN(c, d)); }

template <class T>
inline T MAX( T a, T b, T c, T d) {return MAX(MAX(a, b), MAX(c, d)); }

template <class T>
inline T Abs (T a) { return (a >= 0)? a : -a; }

template <class T>
inline int Sign (T a) { return (a < 0)? -1 : (a > 0); }

template <class T>
inline T clamp (T a, T m, T M) { return (a < m) ? m : (a < M) ? a : M; }

inline Bool Odd(unsigned int v) { return (v & 1); }

/*--------------------------------------------------------------------------*/

inline float * vdtof(double x[3], float y[3])
{ y[0] = (float) x[0]; y[1] = (float) x[1]; y[2] = (float) x[2]; return y; }

inline double * vftod(float x[3], double y[3])
{ y[0] = x[0]; y[1] = x[1]; y[2] = x[2]; return y; }
// basic vector operations with doubles ot floats
template <class T>
inline T minAbsCoord(const T x[3])
{ return MIN(fabs(x[0]),MIN(fabs(x[1]), fabs(x[2]))); }

template <class T>
inline T maxAbsCoord(const T x[3])
{ return MAX(fabs(x[0]),MAX(fabs(x[1]), fabs(x[2]))); }


template <class T>
inline T *vcopy (const T x[3], T r[3])
  { r[0]=x[0]; r[1]=x[1]; r[2]=x[2]; return r; }

template <class T>
inline T *vcopyw (const T x[4], T r[4])
  { r[0]=x[0]; r[1]=x[1]; r[2]=x[2]; r[3]=x[3]; return r; }

template <class T>
inline T *vzero (T x[3])
  { x[0] = 0; x[1] = 0; x[2] = 0; return x; }

template <class T>
inline T *vzerow (T x[3])
  { x[0] = 0; x[1] = 0; x[2] = 0; x[3] = 0; return x; }

template <class T>
inline T *vsub (const T x[3], const T y[3], T r[3])
  { r[0]=x[0]-y[0]; r[1]=x[1]-y[1]; r[2]=x[2]-y[2]; return r; }

template <class T>
inline T *vsubw (const T x[4], const T y[4], T r[4])
  { r[0]=x[0]-y[0]; r[1]=x[1]-y[1]; r[2]=x[2]-y[2]; r[3]=x[3]-y[3]; return r; }

template <class T>
inline T *vsub (T x[3], const T y[3])
  { x[0]-=y[0]; x[1]-=y[1]; x[2]-=y[2]; return x;}

template <class T>
inline T *vsubw (T x[4], const T y[4])
  { x[0]-=y[0]; x[1]-=y[1]; x[2]-=y[2]; x[3]-=y[3];return x;}

template <class T>
inline T *vsub (T x[3], const T y[3], T c)
  { x[0]-=y[0]*c; x[1]-=y[1]*c; x[2]-=y[2]*c; return x;}

template <class T>
inline T *vadd (const T x[3], const T y[3], T r[3])
  { r[0]=x[0]+y[0]; r[1]=x[1]+y[1]; r[2]=x[2]+y[2]; return r; }

template <class T>
inline T *vadd (T x[3], const T y[3])
  { x[0]+=y[0]; x[1]+=y[1]; x[2]+=y[2]; return x;}

template <class T>
inline T *vadd (T x[3], const T y[3], T c)
  { x[0]+=y[0]*c; x[1]+=y[1]*c; x[2]+=y[2]*c; return x;}


template <class T>
inline T *vmul (const T x[3], T c, T r[3])
  { r[0] = c*x[0]; r[1] = c*x[1]; r[2] = c*x[2]; return r; }


template <class T>
inline T *vmul (T x[3], T c)
  { x[0]*=c; x[1]*=c; x[2]*=c; return x; }


template <class T>
inline T *vmulw (T x[4], T c)
  { x[0]*=c; x[1]*=c; x[2]*=c; x[3]*=c; return x; }


template <class T>
inline T *vdiv (const T x[3], T c, T r[3])
{ return vmul (x, c? 1.0/c : 0.0, r); }

template <class T>
inline T *vdiv (T x[3], T c)
  { return vmul (x, c? 1.0/c : 0.0); }


template <class T>
inline T *vdivw (T x[4], T c)
  { return vmulw (x, c? 1.0/c : 0.0); }


template <class T>
inline T vdot (const T x[3], const T y[3])
  { return (x[0]*y[0] + x[1]*y[1] + x[2]*y[2]); }

template <class T>
inline T vlength2 (const T x[3])
  { return vdot (x, x); }

template <class T>
inline T vlength (const T x[3])
  { return sqrt(vlength2(x)); }

template <class T>
inline T * vnorm (const T x[3], T r[3])
  { T tmp = vlength(x); tmp = tmp? 1.0/tmp : 1.0; return vmul(x, tmp, r); }

template <class T>
inline T * vnorm (T x[3])
  { T tmp = vlength(x); tmp = tmp? 1.0/tmp : 1.0; return vmul(x, tmp); }

template <class T>
inline T vdist2 (const T x[3], const T y[3])
  { T tmp[3]; return vlength2(vsub(x, y, tmp)); }

template <class T>
inline T vdist (const T x[3], const T y[3])
  { T tmp[3]; return vlength(vsub(x, y, tmp)); }

template <class T>
inline T * vcross (const T x[3], const T y[3], T r[3])
  { 
    r[0]=x[1]*y[2]-x[2]*y[1]; 
    r[1]=x[2]*y[0]-x[0]*y[2]; 
    r[2]=x[0]*y[1]-x[1]*y[0];
    return r;
  }

// angle between two vectors
template <class T>
double vangle (const T x[3], const T y[3])
{ 
  T a[3],b[3]; 
  double dot = vdot(vnorm(x, a),vnorm(y, b));
  if (dot > 1.0) dot = 1.0;
  if (dot < -1.0) dot = -1.0;
  return acos(dot); 
}

// angle XYZ
template <class T>
inline T vangle (const T x[3], const T y[3], const T z[3])
{ T a[3], b[3]; return vangle(vsub(x, y, a), vsub(z, y, b)); }

inline double det2 (double x0, double x1,
                    double y0, double y1, double * modul = NULL)
{
  double x0y1 = x0*y1, x1y0 = x1*y0;
  if (modul) *modul = Abs(x0y1) + Abs(x1y0);
  return x0y1 - x1y0;
}

inline double det3 (double x0, double x1, double x2,
                    double y0, double y1, double y2,
                    double z0, double z1, double z2, double * modul = NULL)
{
   double y1z2 = y1*z2, y2z1 = y2*z1,
          y2z0 = y2*z0, y0z2 = y0*z2,
          y0z1 = y0*z1, y1z0 = y1*z0;
   double det0 = y1z2 - y2z1,  
          det1 = y2z0 - y0z2, 
          det2 = y0z1 - y1z0;
   if (modul)
   {
     double da0 = Abs(y1z2) + Abs(y2z1),
            da1 = Abs(y2z0) + Abs(y0z2),
            da2 = Abs(y0z1) + Abs(y1z0);
     *modul = Abs(x0*da0) + Abs(x1*da1) + Abs(x2*da2);
   }
   return x0*det0 + x1*det1 + x2*det2;
}

inline double det4 (double x0, double x1, double x2, double x3,
                    double y0, double y1, double y2, double y3,
                    double z0, double z1, double z2, double z3,
                    double w0, double w1, double w2, double w3, double * modul = NULL)
{
   double minor0,minor1,minor2,minor3;
   if (modul)
   {
       double m1,m2,m3,m4;
       minor0=det3(y1,y2,y3,
                   z1,z2,z3,
                   w1,w2,w3,&m1);
       minor1=det3(y0,y2,y3,
                   z0,z2,z3,
                   w0,w2,w3,&m2);
       minor2=det3(y0,y1,y3,
                   z0,z1,z3,
                   w0,w1,w3,&m3);
       minor3=det3(y0,y1,y2,
                   z0,z1,z2,
                   w0,w1,w2,&m4);
       *modul = Abs(x0*m1)+Abs(x1*m2)+Abs(x2*m3)+Abs(x3*m4);
   } else {
       minor0=det3(y1,y2,y3,
                   z1,z2,z3,
                   w1,w2,w3);
       minor1=det3(y0,y2,y3,
                   z0,z2,z3,
                   w0,w2,w3);
       minor2=det3(y0,y1,y3,
                   z0,z1,z3,
                   w0,w1,w3);
       minor3=det3(y0,y1,y2,
                   z0,z1,z2,
                   w0,w1,w2);
   }
   return x0*minor0-x1*minor1+x2*minor2-x3*minor3;
}


template <class T>
inline double det3 (const T x[3], const T y[3], const T z[3])
  {
    return ( x[0]*(y[1]*z[2] - y[2]*z[1]) +
             x[1]*(y[2]*z[0] - y[0]*z[2]) +
             x[2]*(y[0]*z[1] - y[1]*z[0]) );
  }

double * project (double x[3], double y[3], double z[3], double w[3], double r[3]);

// n[3] is assumed to be normalized normal to projective plane
template <class T>
inline T * vproject(const T x[3], const T n[3], T r[3])
{
  T tmp[3];
  return vsub(x, vmul(n, vdot(x, n), tmp), r);
}

template <class T>
inline T * vproject(T x[3], const T n[3])
{
  T tmp[3];
  return vsub(x, vmul(n, vdot(x, n), tmp));
}

double * projectToLine (double vx[3], double ax[3], double bx[3], double wx[3]);
double pointToLineDist2 (double vx[3], double ax[3], double bx[3]);

void movePoint (
				double x, double y, double z, 
				double n1, double n2, double n3, 
				double length,
				double &newX, double &newY, double &newZ);

inline void movePoint (
				double x, double y, double z, 
				double unitNormal[3], 
				double length,
				double &newX, double &newY, double &newZ)
{
	movePoint (
			x, y, z, 
			unitNormal[0], unitNormal[1], unitNormal[2], 
			length,
			newX, newY, newZ);
}

inline void movePoint (
				double origP[3],  
				double unitNormal[3], 
				double length,
				double newP[3])
 {
	movePoint (
			origP[0], origP[1], origP[2], 
			unitNormal[0], unitNormal[1], unitNormal[2], 
			length,
			newP[0], newP[1], newP[2]);
 }

/****************************************************************************\
*  The following function is added by Tianjun (TJ) WANG on 01/17/99          *
\****************************************************************************/
template <class T>
inline void ptLocalToGlobal (T localOrigin[3], T localX[3], T localY[3], 
	T localZ[3], T localPt[3], T retGlobalPt[3])
{
	retGlobalPt[0] = localOrigin[0] + 
		localX[0] * localPt[0] + localY[0] * localPt[1] + localZ[0] * localPt[2];
  retGlobalPt[1] = localOrigin[1] + 
		localX[1] * localPt[0] + localY[1] * localPt[1] + localZ[1] * localPt[2];
	retGlobalPt[2] = localOrigin[2] + 
		localX[2] * localPt[0] + localY[2] * localPt[1] + localZ[2] * localPt[2];
}


// Scans a string to determine in which range its digits are. If
// the string is in an invalid format, 0 is returned, otherwise 1.
// "left" returns the decadic logarithm of the most significant
// digit, rounded down to the next integer. "right" does the same
// for the least significant digit. If the number 0 was passed in,
// the corresponding flag is set. If this function returns 0, the
// values in "left", "right" and "zero" are undefined.

Bool findDigits (const char *, int &left, int &right, int &zero);

/*--------------------------------------------------------------------------*/

#endif /* __MISCMATH_H__ */
