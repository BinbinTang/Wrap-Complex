#ifndef __RGI_GENMATRIX_H__
#define __RGI_GENMATRIX_H__

#include <iostream>
#include <fstream>
#include <strstream>

using namespace std;

//- GenMatrix
class GenMatrix {
//. mxn Matrices
public:
    //- GenMatrix
    GenMatrix(int m, int n);
    GenMatrix(const GenMatrix&);
    GenMatrix(const GenMatrix&, const GenMatrix&);
    ~GenMatrix();
        //. Default constructor: identity matrix.

    //- operator=
    GenMatrix& operator=(const GenMatrix&);

    //- get matrix size
    void getSize (int & m, int & n) const;

    //- operator==, operator!=
    int operator==(const GenMatrix&) const;
    int operator!=(const GenMatrix&) const;

    //- element, operator[]
    double & element(int,int);
    double element(int,int) const;
    double * operator[](int);
    double const* operator[](int) const;

    //- operator*
    GenMatrix * operator*(const GenMatrix&) const;
// Later --    Vector operator*(const Vector&) const;

    //- transpose, inverse
    void transpose();
		void identity();
		int inverse(GenMatrix &result) const;
    
		//- Arithmetic
		GenMatrix& operator*= (double num);
    
		//- Solving linear systems
    GenMatrix * adjoinMatrix (GenMatrix &) const;
    GenMatrix * extractSubmatrix (int m1, int m2, int n1, int n2) const;
		int solveLinearVectorSystem(int numRightHandVectors, GenMatrix &rightHandMatrix);
    void solveSystem ();
    void testSolveSystem ();
    void randomFill ();

    //- dga
    double * dga() const;
private:
    int m_, n_;
    double **rep_;

    void initArray_ (void);
    void deleteArray_ (void);
};

inline double * GenMatrix::operator[] (int i) { return &rep_[i][0]; }
inline const double * GenMatrix::operator[] (int i) const { return &rep_[i][0]; }
inline double * GenMatrix::dga() const {return ((double *)&this->rep_[0][0]); }

extern ostream& operator<<(ostream&, const GenMatrix&);
extern istream& operator>>(istream&, GenMatrix&);

#endif
