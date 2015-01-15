
#include <string.h>

#include "binio.h"
#include "rgimatrix.h"

static double unitMatrix[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
};

static RgiMatrix id(
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
);

const RgiMatrix &RgiMatrix::
identity(void) 
{ 
	return id; 
}

RgiMatrix::
RgiMatrix() 
{
	memcpy(rep_, unitMatrix, sizeof(double)*16);
}

RgiMatrix::
RgiMatrix(const RgiMatrix &m1, const RgiMatrix &m2) 
{
	rep_[0][0] =
		  m1.rep_[0][0]*m2.rep_[0][0] 
		+ m1.rep_[0][1]*m2.rep_[1][0]
		+ m1.rep_[0][2]*m2.rep_[2][0] 
		+ m1.rep_[0][3]*m2.rep_[3][0];

	rep_[0][1] = 
		  m1.rep_[0][0]*m2.rep_[0][1] 
		+ m1.rep_[0][1]*m2.rep_[1][1]
		+ m1.rep_[0][2]*m2.rep_[2][1] 
		+ m1.rep_[0][3]*m2.rep_[3][1];

	rep_[0][2] = 
		  m1.rep_[0][0]*m2.rep_[0][2] 
		+ m1.rep_[0][1]*m2.rep_[1][2]
		+ m1.rep_[0][2]*m2.rep_[2][2] 
		+ m1.rep_[0][3]*m2.rep_[3][2];

	rep_[0][3] = 
		  m1.rep_[0][0]*m2.rep_[0][3] 
		+ m1.rep_[0][1]*m2.rep_[1][3]
		+ m1.rep_[0][2]*m2.rep_[2][3] 
		+ m1.rep_[0][3]*m2.rep_[3][3];

	/***/

	rep_[1][0] =
		  m1.rep_[1][0]*m2.rep_[0][0] 
		+ m1.rep_[1][1]*m2.rep_[1][0]
		+ m1.rep_[1][2]*m2.rep_[2][0] 
		+ m1.rep_[1][3]*m2.rep_[3][0];

	rep_[1][1] = 
		  m1.rep_[1][0]*m2.rep_[0][1] 
		+ m1.rep_[1][1]*m2.rep_[1][1]
		+ m1.rep_[1][2]*m2.rep_[2][1] 
		+ m1.rep_[1][3]*m2.rep_[3][1];

	rep_[1][2] = 
		  m1.rep_[1][0]*m2.rep_[0][2] 
		+ m1.rep_[1][1]*m2.rep_[1][2]
		+ m1.rep_[1][2]*m2.rep_[2][2] 
		+ m1.rep_[1][3]*m2.rep_[3][2];

	rep_[1][3] = 
		  m1.rep_[1][0]*m2.rep_[0][3] 
		+ m1.rep_[1][1]*m2.rep_[1][3]
		+ m1.rep_[1][2]*m2.rep_[2][3] 
		+ m1.rep_[1][3]*m2.rep_[3][3];

	/***/

	rep_[2][0] =
		  m1.rep_[2][0]*m2.rep_[0][0] 
		+ m1.rep_[2][1]*m2.rep_[1][0]
		+ m1.rep_[2][2]*m2.rep_[2][0] 
		+ m1.rep_[2][3]*m2.rep_[3][0];

	rep_[2][1] = 
		  m1.rep_[2][0]*m2.rep_[0][1] 
		+ m1.rep_[2][1]*m2.rep_[1][1]
		+ m1.rep_[2][2]*m2.rep_[2][1] 
		+ m1.rep_[2][3]*m2.rep_[3][1];

	rep_[2][2] = 
		  m1.rep_[2][0]*m2.rep_[0][2] 
		+ m1.rep_[2][1]*m2.rep_[1][2]
		+ m1.rep_[2][2]*m2.rep_[2][2] 
		+ m1.rep_[2][3]*m2.rep_[3][2];

	rep_[2][3] = 
		  m1.rep_[2][0]*m2.rep_[0][3] 
		+ m1.rep_[2][1]*m2.rep_[1][3]
		+ m1.rep_[2][2]*m2.rep_[2][3] 
		+ m1.rep_[2][3]*m2.rep_[3][3];

	/***/

	rep_[3][0] =
		  m1.rep_[3][0]*m2.rep_[0][0] 
		+ m1.rep_[3][1]*m2.rep_[1][0]
		+ m1.rep_[3][2]*m2.rep_[2][0] 
		+ m1.rep_[3][3]*m2.rep_[3][0];

	rep_[3][1] = 
		  m1.rep_[3][0]*m2.rep_[0][1] 
		+ m1.rep_[3][1]*m2.rep_[1][1]
		+ m1.rep_[3][2]*m2.rep_[2][1] 
		+ m1.rep_[3][3]*m2.rep_[3][1];

	rep_[3][2] = 
		  m1.rep_[3][0]*m2.rep_[0][2] 
		+ m1.rep_[3][1]*m2.rep_[1][2]
		+ m1.rep_[3][2]*m2.rep_[2][2] 
		+ m1.rep_[3][3]*m2.rep_[3][2];

	rep_[3][3] = 
		  m1.rep_[3][0]*m2.rep_[0][3] 
		+ m1.rep_[3][1]*m2.rep_[1][3]
		+ m1.rep_[3][2]*m2.rep_[2][3] 
		+ m1.rep_[3][3]*m2.rep_[3][3];
}

RgiMatrix::
RgiMatrix(
	double f00, double f01, double f02, double f03,
	double f10, double f11, double f12, double f13,
	double f20, double f21, double f22, double f23,
	double f30, double f31, double f32, double f33)
{
	rep_[0][0] = f00; rep_[0][1] = f01; rep_[0][2] = f02; rep_[0][3] = f03;
	rep_[1][0] = f10; rep_[1][1] = f11; rep_[1][2] = f12; rep_[1][3] = f13;
	rep_[2][0] = f20; rep_[2][1] = f21; rep_[2][2] = f22; rep_[2][3] = f23;
	rep_[3][0] = f30; rep_[3][1] = f31; rep_[3][2] = f32; rep_[3][3] = f33;
}

RgiMatrix::
RgiMatrix(double *m)
{
	rep_[0][0]=m[ 0]; rep_[0][1]=m[ 1]; rep_[0][2]=m[ 2]; rep_[0][3]=m[ 3];
	rep_[1][0]=m[ 4]; rep_[1][1]=m[ 5]; rep_[1][2]=m[ 6]; rep_[1][3]=m[ 7];
	rep_[2][0]=m[ 8]; rep_[2][1]=m[ 9]; rep_[2][2]=m[10]; rep_[2][3]=m[11];
	rep_[3][0]=m[12]; rep_[3][1]=m[13]; rep_[3][2]=m[14]; rep_[3][3]=m[15];
}


int RgiMatrix::
operator==(const RgiMatrix &m) const 
{
	int i;
	for(i = 0; i < 4; i++) {

		int j;
		for(j = 0; j < 4; j++) {

			if(rep_[i][j] != m.rep_[i][j]) return 0;
		}
	}

	return 1;
}

RgiVector RgiMatrix::
operator*(const RgiVector &v) const 
{
	const double w = 
		  rep_[3][0]*v[0] 
		+ rep_[3][1]*v[1] 
		+ rep_[3][2]*v[2] 
		+ rep_[3][3];

	return RgiVector(
		(rep_[0][0]*v[0] + rep_[0][1]*v[1] + rep_[0][2]*v[2] + rep_[0][3])/w,
		(rep_[1][0]*v[0] + rep_[1][1]*v[1] + rep_[1][2]*v[2] + rep_[1][3])/w,
		(rep_[2][0]*v[0] + rep_[2][1]*v[1] + rep_[2][2]*v[2] + rep_[2][3])/w
	);
}

RgiMatrix RgiMatrix::
multRot(const RgiMatrix &m) const
{
	RgiMatrix mat(*this);

	mat[0][3] = 0.0;
	mat[1][3] = 0.0;
	mat[2][3] = 0.0;
	mat[3][3] = 1.0;

	return RgiMatrix(mat ,m);
}

RgiVector RgiMatrix::
multRot(const RgiVector &v) const
{
	return RgiVector(
		(rep_[0][0]*v[0] + rep_[0][1]*v[1] + rep_[0][2]*v[2]),
		(rep_[1][0]*v[0] + rep_[1][1]*v[1] + rep_[1][2]*v[2]),
		(rep_[2][0]*v[0] + rep_[2][1]*v[1] + rep_[2][2]*v[2])
	);
}

#define SWAP(a,b,t) (t=a,a=b,b=t)
#define ABS(x) (x>=0?x:-(x))

void RgiMatrix::
transpose() 
{
	double t;

	SWAP(rep_[0][1], rep_[1][0], t);
	SWAP(rep_[0][2], rep_[2][0], t);
	SWAP(rep_[0][3], rep_[3][0], t);
	SWAP(rep_[1][2], rep_[2][1], t);
	SWAP(rep_[1][3], rep_[3][1], t);
	SWAP(rep_[2][3], rep_[3][2], t);
}

int RgiMatrix::
inverse(RgiMatrix &result) const 
{
	RgiMatrix m4tmp(*this);
	RgiMatrix inverse;

	int i;
	for (i = 0; i < 4; i++) {

		/* Look for row with largest pivot and swap rows */

		double temp = 0.0; 
		int j = -1;

		int k;
		for (k = i; k < 4; k++) {

			const double absVal = ABS(m4tmp.rep_[k][i]);

			if (absVal > temp) {

				temp = absVal;
				j = k;
			}
		}

		if (j == -1)     /* No replacing row -> no inverse */
			return(0);

		double *m4tmpI = m4tmp.rep_[i];
		double *invI = inverse.rep_[i];

		if (j != i) {

			double *m4tmpJ = m4tmp.rep_[j];
			double *invJ = inverse.rep_[j];

			SWAP(m4tmpI[0], m4tmpJ[0], temp);
			SWAP(invI[0], invJ[0], temp);

			SWAP(m4tmpI[1], m4tmpJ[1], temp);
			SWAP(invI[1], invJ[1], temp);

			SWAP(m4tmpI[2], m4tmpJ[2], temp);
			SWAP(invI[2], invJ[2], temp);

			SWAP(m4tmpI[3], m4tmpJ[3], temp);
			SWAP(invI[3], invJ[3], temp);
		}

		temp = m4tmp.rep_[i][i];

		m4tmpI[0] /= temp;
		invI[0] /= temp;

		m4tmpI[1] /= temp;
		invI[1] /= temp;

		m4tmpI[2] /= temp;
		invI[2] /= temp;

		m4tmpI[3] /= temp;
		invI[3] /= temp;

		for (j = 0; j < 4; j++) {

			if (j != i) {

				temp = m4tmp.rep_[j][i];

				m4tmp.rep_[j][0] -= m4tmp.rep_[i][0]*temp;
				inverse.rep_[j][0] -= inverse.rep_[i][0]*temp;

				m4tmp.rep_[j][1] -= m4tmp.rep_[i][1]*temp;
				inverse.rep_[j][1] -= inverse.rep_[i][1]*temp;

				m4tmp.rep_[j][2] -= m4tmp.rep_[i][2]*temp;
				inverse.rep_[j][2] -= inverse.rep_[i][2]*temp;

				m4tmp.rep_[j][3] -= m4tmp.rep_[i][3]*temp;
				inverse.rep_[j][3] -= inverse.rep_[i][3]*temp;
			}
		}
	}

	result = inverse;
	return(1);
}

RgiMatrix::
operator double *()
{
	return (double *) rep_;
}
    
ostream& operator<<(ostream &s, const RgiMatrix &m)
{
	int i;
	for (i = 0; i < 4; i++) {

		int j;
		for (j = 0; j < 4; j++) {

			s << m.element(i,j) << " ";
		}
	}

	return s;
}

istream& operator>>(istream &s, RgiMatrix &m)
{
	double f;

	int i;
	for(i = 0; i < 4; i++) {

		int j;
		for(j = 0; j < 4; j++) {

			s >> f; 
			m.element(i,j) = f;
		}
	}

	return s;
}

void RgiMatrix::
binDump (BinOutput &bo)
{
	bo.writeDoubles (rep_[0], 4);
	bo.writeDoubles (rep_[1], 4);
	bo.writeDoubles (rep_[2], 4);
	bo.writeDoubles (rep_[3], 4);
}

Bool RgiMatrix::
binInit (BinInput &bi)
{
	return
		bi.readDoubles (rep_[0], 4) &&
		bi.readDoubles (rep_[1], 4) &&
		bi.readDoubles (rep_[2], 4) &&
		bi.readDoubles (rep_[3], 4);
}

