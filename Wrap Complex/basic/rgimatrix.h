#ifndef __RGI_RGIMATRIX_H__
#define __RGI_RGIMATRIX_H__

#include <iostream>

#include <basic/rgivector.h>

//- RgiMatrix
class RgiMatrix {
//. 4x4 Matrices
public:
    //- identity
    static const RgiMatrix &identity();

    //- RgiMatrix
	RgiMatrix();
	RgiMatrix(const RgiMatrix &m);
	RgiMatrix(const RgiMatrix &m1, const RgiMatrix &m2);
	RgiMatrix(double *m);
	RgiMatrix(
		double m00, double m01, double m02, double m03,
		double m10, double m11, double m12, double m13,
		double m20, double m21, double m22, double m23,
		double m30=0, double m31=0, double m32=0, double m33=1
	);
	~RgiMatrix();
		//. Default constructor: identity matrix.

    //- operator=
    RgiMatrix& operator=(const RgiMatrix &m);

    //- operator==, operator!=
    int operator==(const RgiMatrix &m) const;
    int operator!=(const RgiMatrix &m) const;

    //- element, operator[]
    double &element(int i, int j);
    double element(int i, int j) const;
    double *operator[](int i);
    double const *operator[](int i) const;

    //- operator*
    RgiMatrix operator*(const RgiMatrix &m) const;
    RgiVector operator*(const RgiVector &v) const;
	RgiMatrix multRot(const RgiMatrix &m) const;
	RgiVector multRot(const RgiVector &v) const;

    //- transpose, inverse
    void transpose();
	RgiMatrix transposed(void) const;
    int inverse(RgiMatrix &m) const;

	// conversion operators
	operator double* ();

	void binDump (BinOutput &);
	int binInit (BinInput &);

    //- dga
    double *dga() const;
private:

    double rep_[4][4];
};
typedef RgiMatrix *RgiMatrixPtr;

extern ostream& operator<<(ostream &s, const RgiMatrix &m);
extern istream& operator>>(istream &s, RgiMatrix &m);

#include <basic/rgimatrix.hpp>

#endif
