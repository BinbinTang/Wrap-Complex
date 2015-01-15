#include <basic/basic.h>

inline double *RgiMatrix::
operator[](int i) 
{ 
	return &rep_[i][0]; 
}

inline const double *RgiMatrix::
operator[](int i) const 
{ 
	return &rep_[i][0]; 
}

inline double *RgiMatrix::
dga(void) const 
{ 
	return ((double*)&this->rep_[0][0]); 
}

inline RgiMatrix::
RgiMatrix(const RgiMatrix &m) 
{
	memcpy(rep_, m.rep_, sizeof(double)*16);
}

inline RgiMatrix::
~RgiMatrix(void) 
{
	/***/
}

inline RgiMatrix &RgiMatrix::
operator=(const RgiMatrix &m) 
{
	memcpy(rep_, m.rep_, sizeof(double)*16);
	return *this;
}

inline double &RgiMatrix::
element(int i, int j) 
{
	Assert (i >= 0 && i < 4 && j >= 0 && j < 4);
	return rep_[i][j];
}

inline double RgiMatrix::
element(int i, int j) const 
{
	Assert (i >= 0 && i < 4 && j >= 0 && j < 4);
	return rep_[i][j];
}

inline int RgiMatrix::
operator!=(const RgiMatrix &m) const 
{ 
	return !operator==(m);
}

inline RgiMatrix RgiMatrix::
operator*(const RgiMatrix &m) const 
{
	return RgiMatrix(*this, m);
}

inline RgiMatrix RgiMatrix::
transposed(void) const
{
	RgiMatrix mat(*this);
	mat.transpose();
	return mat;
}

