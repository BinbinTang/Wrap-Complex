#ifndef VECTMAT_H
#define VECTMAT_H

#include <math.h>
#include <basic/basic.h>

#define X_AXIS 1
#define Y_AXIS 2
#define Z_AXIS 3

#define ABS(x) ( (x) > 0 ? (x) : -(x) )
#define VERYSMALL(x) (ABS(x)<0.00000000000001)
#define MEDIUMSMALL(x) (ABS(x)<0.0000001)
#define QUITESMALL(x) (ABS((x))<0.001)
#define BM_SQR(x) ((double)(x)*(double)(x))
#define BM_LENGTH(x,y) \
  sqrt(  BM_SQR((x)[0]-(y)[0]) +  \
         BM_SQR((x)[1]-(y)[1]) +  \
         BM_SQR((x)[2]-(y)[2]) )
#define ONEOVERROOTTWO 0.70710678
#define ROOTTWO 1.4142136


class Matrix33 {
  double m[3][3];

  public:
    Matrix33() {SetToIdentical();};
	Matrix33(double x00,double x01,double x02,
		      double x10,double x11,double x12,
			  double x20,double x21,double x22);
    void SetToIdentical();
    Matrix33 Inverse(bool* sing=NULL);
	Matrix33 Transpose();
    void SetToInverse();
    double operator() (int i,int j) { return m[i][j]; };
    void operator() (int i,int j,double d) { m[i][j] = d;};
    Matrix33 operator*(Matrix33);
    Matrix33 operator=(Matrix33);
    void show();
  friend Matrix33 Rotation(double,int);
};

Matrix33 Rotation(double,int);

//  Vector3 is assumed to be a 1x3 Matrix
class Vector3 {

  void copytofloat();

  public:
    double x[3];

//    Vector3 operator = (float*);
    Vector3 operator = (double*);
    Vector3 operator = (Vector3);
    Vector3 operator - (Vector3);
    Vector3 operator - ();
    Vector3 operator + (Vector3);
    Vector3 operator * (double d);
    Vector3 operator / (double d);
    Vector3 operator * (Matrix33);
	bool    operator == (Vector3);
    inline double  length() { return sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);};
    inline double  lengthsqr() { return x[0]*x[0]+x[1]*x[1]+x[2]*x[2];};
    void operator () (double x1,double x2,double x3) {x[0] = x1;x[1] = x2;x[2] = x3;};
	void operator () (double x_[3]) { x[0] = x_[0]; x[1] = x_[1]; x[2] = x_[2];};;
    void show();
    float* fp();
    void normalize();

    Vector3() {x[0] = x[1] = x[2] = 0;};
    Vector3(double x1,double x2,double x3) { x[0] = x1; x[1] = x2; x[2] = x3;};
	Vector3(double x_[3]) { x[0] = x_[0]; x[1] = x_[1]; x[2] = x_[2];};;
    int InTriangle(Vector3*);

    Vector3 OnLine(Vector3,Vector3);
    double  DistWithLine(Vector3,Vector3);
};


#define SIDE01     0
#define SIDE12     1
#define SIDE20     2

#define SIDE10     SIDE01
#define SIDE21     SIDE12
#define SIDE02     SIDE20

int side(int v1,int v2);

double dot_prod(Vector3,Vector3);
Vector3 cross_prod(Vector3,Vector3);

void intersect(Vector3 p0,Vector3 p1,Vector3 n0,Vector3 n1,
               Vector3 n,double d,Vector3& out,Vector3& outn);
double length(Vector3);
double lengthsqr(Vector3);
Matrix33 AlignWithYAxis(Vector3);
Matrix33 AlignWithYAxis(Vector3,Vector3); // the second point is a point should be moved on
                                          // to the XY plane afterwards
int PtQuiteNear(Vector3,Vector3);  


//double min(double,double);
//double max(double,double);

double atanh(double);

double plane2PtDist2(Vector3,Vector3,Vector3,Vector3);
double line2PtDist2(Vector3 a,Vector3 b,Vector3 v);
double tri2PtDist2(Vector3 a,Vector3 b,Vector3 c,Vector3 v);

// first 3 points define a plane and the third one define the point to measure

Vector3 tetraCenter(Vector3,Vector3,Vector3,Vector3);
Vector3 circumCenter(Vector3,Vector3,Vector3,bool* sing=NULL);
Vector3 intersect3Planes(Vector3,double,Vector3,double,Vector3,double,bool* sing=NULL);
Bool    lineIntersectTri(Vector3,Vector3,Vector3,Vector3,Vector3); 
        // check whether a line intersect the triangle with the 3 vertices
Vector3 lineIntersectPlane(Vector3 lv,Vector3 ln,Vector3 pn,double d);
		// find the intersection of line lv+t*ln with plane <pn,x> = d
double  segmentIntersectPlanes(Vector3 p1,Vector3 p2,Vector3 pn,double d);
		// find the intersection of segment p1p2 and return the t with is p1+t(p2-p1)
double  angle(Vector3 a,Vector3 b,Vector3 c);
		// find angle abc; (angle at b)
double  Orient4(Vector3,Vector3,Vector3,Vector3);
double  Orient3(Vector3,Vector3,Vector3);

double cosDihedralAngle(Vector3 a,Vector3 b,Vector3 c,Vector3 d);
// return the cosine of dihedral angle of triangle abc and bad
#endif
