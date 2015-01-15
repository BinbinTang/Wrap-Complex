#include "genmatrix.h"
#include <basic/basic.h>
#include <basic/comment.h>
#include <basic/miscmath.h>

#define NUMERICAL_ZERO 1.0E-06


//--------------------------------------------------------------

GenMatrix::
GenMatrix(int m, int n) 
{
  m_ = m;
  n_ = n;
  initArray_ ();
}

//--------------------------------------------------------------

GenMatrix::
GenMatrix(const GenMatrix& m) 
{
  m.getSize (m_, n_);
  initArray_ ();
  for (int i = 0; i < m_; i++)
    for (int j = 0; j < n_; j++)
      element (i, j) = m.element (i, j);
}

//--------------------------------------------------------------

GenMatrix::
GenMatrix(const GenMatrix& m1, const GenMatrix& m2) 
{
  int m1_n, m2_m;
  m1.getSize (m_, m1_n);
  m2.getSize (m2_m, n_);
  Assert (m1_n == m2_m);
  initArray_ ();

  for(int i = 0; i < m_; i++) {
    for(int j = 0; j < n_; j++) {
	    rep_[i][j] = 0.0;
	    for(int k = 0; k < m1_n; k++) {
		    rep_[i][j] += m1.rep_[i][k]*m2.rep_[k][j];
	    }
	  }
  }
}

//--------------------------------------------------------------

GenMatrix::
~GenMatrix() 
{
  deleteArray_ ();
}

//--------------------------------------------------------------

void GenMatrix::
getSize (int & m, int & n) const
{
  m = m_; n = n_;
}

//--------------------------------------------------------------

GenMatrix& GenMatrix::
operator=(const GenMatrix& m) {
    int m2, n2;
    m.getSize (m2, n2);
    if ((m_ != m2) || (n_ != n2))
    {
      for (int i = 0; i < m_; i++)
        delete [] rep_[i];
      delete [] rep_;
      m_ = m2;
      n_ = n2;
      initArray_ ();
    }

  for(int i=0; i < m_; i++)
    for(int j=0; j < n_; j++)
  	  rep_[i][j] = m.rep_[i][j];
            
    return *this;
}

//--------------------------------------------------------------

double & GenMatrix::
element(int i, int j) {
    Assert(i>=0 && i<m_ && j>=0 && j<n_);
    return rep_[i][j];
}

//--------------------------------------------------------------

double GenMatrix::
element(int i, int j) const 
{
    Assert(i>=0 && i < m_ && j>=0 && j<n_);
    return rep_[i][j];
}

//--------------------------------------------------------------

int GenMatrix::
operator==(const GenMatrix& m) const 
{
  for(int i=0; i < m_; i++) {
    for(int j=0; j < n_; j++) {
  	  if(fabs (rep_[i][j] - m.rep_[i][j])> 0.5) return 0;
	  }
  }
  return 1;
}

//--------------------------------------------------------------

int GenMatrix::
operator!=(const GenMatrix& m) const 
{ 
    return !operator==(m);
}

//--------------------------------------------------------------

GenMatrix * GenMatrix::
operator*(const GenMatrix& m) const 
{
    return new GenMatrix(*this, m);
}

//--------------------------------------------------------------

/*
 * Not now...

Vector GenMatrix::
operator*(const Vector& v) const {
    double * vp = v.rep();
    double w = rep_[3][0]*vp[0]+rep_[3][1]*vp[1]+rep_[3][2]*vp[2]+rep_[3][3];
    return Vector(
      (rep_[0][0]*vp[0]+rep_[0][1]*vp[1]+rep_[0][2]*vp[2]+rep_[0][3]) / w,
      (rep_[1][0]*vp[0]+rep_[1][1]*vp[1]+rep_[1][2]*vp[2]+rep_[1][3]) / w,
      (rep_[2][0]*vp[0]+rep_[2][1]*vp[1]+rep_[2][2]*vp[2]+rep_[2][3]) / w
    );
}
*/

//--------------------------------------------------------------

#define SWAP(a,b,t) (t=a,a=b,b=t)
#define ABS(x) (x>=0?x:-(x))

void GenMatrix::
transpose() {
    double t;
    Assert (m_ == n_);  // handle m_ != n_ case later...
    for (int i = 0; i < m_; i++)
      for (int j = i+1; j < n_; j++)
        SWAP(rep_[i][j], rep_[j][i], t);
}

//--------------------------------------------------------------
void GenMatrix::
identity()
{
	Assert (m_ == n_);
	for (int i = 0; i < m_; i++)
	{
		for (int j = 0; j < m_; j++)
		{
			if (i == j)
			{
			  rep_[i][j] = 1.0;
			}
			else
			{
				rep_[i][j] = 0.0;
			}
		}
	}
}

//--------------------------------------------------------------

int GenMatrix::
inverse(GenMatrix& inverse) const 
{
	Assert(n_==m_ && inverse.m_==inverse.n_ && n_==inverse.n_);
	int answer = 1;
	int i, j, k;
  GenMatrix matrix(*this);
	int numRow = matrix.m_;
  int numCol = matrix.n_;
  inverse.identity();

	for (i = 0; i < numRow; i++)
	{
		int rowIndex = i;
		double tmpValue = fabs (matrix.rep_[i][i]);
		for (j = i + 1; j < numRow; j++)
		{
			if (tmpValue < fabs (matrix.rep_[j][i]))
			{
				tmpValue = fabs (matrix.rep_[j][i]);
				rowIndex = j;
			}
		}

		// Exchange two rows.
		if (rowIndex > i)
		{
      for (j = i; j < numCol; j++)
			{
		  	tmpValue = matrix.rep_[rowIndex][j];
        matrix.rep_[rowIndex][j] = matrix.rep_[i][j];
        matrix.rep_[i][j] = tmpValue;
      }

			for (j = 0; j < numCol; j++)
			{
        tmpValue = inverse.rep_[rowIndex][j];
        inverse.rep_[rowIndex][j] = inverse.rep_[i][j];
        inverse.rep_[i][j] = tmpValue;
			}
		}

		if (fabs (matrix.rep_[i][i]) < NUMERICAL_ZERO)
		{
			answer = 0;
			return (answer);
		}

		// Gaussian elimination.
    for (k = i + 1; k < numRow; k++)
		{
			if (fabs (matrix.rep_[k][i]) > 0.0)
			{
		  	tmpValue = matrix.rep_[k][i] / matrix.rep_[i][i];
			  for (j = i + 1; j < numCol; j++)
				{
			    matrix.rep_[k][j] += - matrix.rep_[i][j] * tmpValue;
				}
  
			  for (j = 0; j < numCol; j++)
				{
			    inverse.rep_[k][j] += - inverse.rep_[i][j] * tmpValue;
				}
      }

      matrix.rep_[k][i] = 0.0;
		}
	}

  if (fabs (matrix.rep_[numRow - 1][numRow - 1]) < NUMERICAL_ZERO)
	{
		answer = 0;
		return (answer);
	}

  // Backward elimination
	for (i = numRow - 1; i >= 0; i--)
	{
		// elimination.
		for (j = 0; j < numCol; j++)
		{
			inverse.rep_[i][j] /= matrix.rep_[i][i];
		}

    for (k = i - 1; k >= 0; k--)
		{
			if (fabs (matrix.rep_[k][i]) > 0.0)
			{
			  for (j = 0; j < numCol; j++)
				{
			    inverse.rep_[k][j] += - inverse.rep_[i][j] * matrix.rep_[k][i];
				}
      }
		}
	}

	return (answer);
}

//--------------------------------------------------------------

GenMatrix& GenMatrix::
operator*= (double num)
{
    for (int i = 0; i < m_; i++)
      for (int j = 0; j < n_; j++)
				element (i, j) *= num;
		return *this;
}

//--------------------------------------------------------------

/*
 * Not now...
 
int GenMatrix::
inverse(GenMatrix& result) const {
    int i,j,k;
    double temp;

    GenMatrix m4tmp(*this);
    GenMatrix inverse;

    for(i = 0; i < 4; i++) {
	// Look for row with largest pivot and swap rows
	temp = 0.0f; 
	j = -1;
	for(k = i; k < 4; k++)
            if(ABS(m4tmp.rep_[k][i]) > temp) {
                temp = ABS(m4tmp.rep_[k][i]);
                j = k;
            }
        if(j == -1)     // No replacing row -> no inverse
            return(0);
        if (j != i)
            for(k = 0; k < 4; k++) {
                SWAP(m4tmp.rep_[i][k],m4tmp.rep_[j][k],temp);
                SWAP(inverse.rep_[i][k],inverse.rep_[j][k],temp);
            }
        temp = m4tmp.rep_[i][i];
        for(k = 0; k < 4; k++) {
            m4tmp.rep_[i][k] /= temp;
            inverse.rep_[i][k] /= temp;
        }
        for(j = 0; j < 4; j++) {
            if(j != i) {
                temp = m4tmp.rep_[j][i];
                for(k = 0; k < 4; k++) {
                    m4tmp.rep_[j][k] -= m4tmp.rep_[i][k]*temp;
                    inverse.rep_[j][k] -= inverse.rep_[i][k]*temp;
                }
            }
        }
    }
    result = inverse;
    return(1);
}
*/

//--------------------------------------------------------------
    
ostream& operator<<(ostream& s, const GenMatrix& m) 
{
  int m_m, m_n;
  m.getSize (m_m, m_n);

  s << "Size: " << m_m << " x " << m_n << "." << endl;
  for(int i=0; i<m_m; i++) {
	  for(int j=0; j<m_n; j++) {
	     s << ((GenMatrix*)&m)->element(i,j) << " ";
	  }
    s << endl;
  }
  return s;
}

//--------------------------------------------------------------

istream& operator>>(istream& s, GenMatrix& m) 
{
  int m_m, m_n;
  m.getSize (m_m, m_n);

  double f;
  for(int i=0; i<m_m; i++) {
	  for(int j=0; j<m_n; j++) {
	      s >> f; m.element(i,j) = f;
	  }
  }
  return s;
}

//--------------------------------------------------------------

void GenMatrix::
initArray_ (void)
{
  // Assumes m_ and n_ have been set.
  rep_ = new double * [m_];
  for (int i = 0; i < m_; i++)
    rep_[i] = new double [n_];
}

//--------------------------------------------------------------

void GenMatrix::
deleteArray_ (void)
{
  for (int i = 0; i < m_; i++)
    delete [] rep_[i];
  delete [] rep_;
}

/*----------------------------------------------------------------------------*\
* the right-hand matrix is the returned solution matrix                        *
\*----------------------------------------------------------------------------*/
int GenMatrix::
solveLinearVectorSystem(int numRightHandVectors, GenMatrix &rightHandMatrix) 
{
	int answer = 1;
	int i, j, k;
	GenMatrix coeffMatrix(*this);
	int numRow = coeffMatrix.m_;
  int numCol = coeffMatrix.n_;
  double tmpValue;

	Assert (numRightHandVectors > 0);

	for (i = 0; i < numRow; i++)
	{
		int rowIndex = i;
		tmpValue = fabs (coeffMatrix.rep_[i][i]);
		for (j = i + 1; j < numRow; j++)
		{
			if (tmpValue < fabs (coeffMatrix.rep_[j][i]))
			{
				tmpValue = fabs (coeffMatrix.rep_[j][i]);
				rowIndex = j;
			}
		}

		// Exchange two rows.
		if (rowIndex > i)
		{
      for (j = i; j < numCol; j++)
			{
		  	tmpValue = coeffMatrix.rep_[rowIndex][j];
        coeffMatrix.rep_[rowIndex][j] = coeffMatrix.rep_[i][j];
        coeffMatrix.rep_[i][j] = tmpValue;
      }

      for (j = 0; j < numRightHandVectors; j++)
			{
		  	tmpValue = rightHandMatrix.rep_[rowIndex][j];
        rightHandMatrix.rep_[rowIndex][j] = rightHandMatrix.rep_[i][j];
        rightHandMatrix.rep_[i][j] = tmpValue;
      }
		}

		if (fabs (coeffMatrix.rep_[i][i]) < NUMERICAL_ZERO)
		{
			answer = 0;
			return (answer);
		}

		// Gaussian elimination.
    for (k = i + 1; k < numRow; k++)
		{
			if (fabs (coeffMatrix.rep_[k][i]) > 0.0)
			{
		  	tmpValue = coeffMatrix.rep_[k][i] / coeffMatrix.rep_[i][i];
			  for (j = i + 1; j < numCol; j++)
				{
			    coeffMatrix.rep_[k][j] -= coeffMatrix.rep_[i][j] * tmpValue;
				}
  
				for (j = 0; j < numRightHandVectors; j++)
				{
          rightHandMatrix.rep_[k][j] -= rightHandMatrix.rep_[i][j] * tmpValue; 
				}

				coeffMatrix.rep_[k][i] = 0.0;
      }
		}
	}

  // Backward substitution
	for (j = 0; j < numRightHandVectors; j++)
	{
	  for (i = numRow - 1; i >= 0; i--)
		{
      tmpValue = 0.0;
			for (k = i+1; k < numRow; k++)
			{
				tmpValue += coeffMatrix.rep_[i][k] * rightHandMatrix.rep_[k][j];
			}

			// solution <- rightHandVectors
      rightHandMatrix.rep_[i][j] = (rightHandMatrix.rep_[i][j] - tmpValue) / 
				coeffMatrix.rep_[i][i];
    }
	}

	return (answer);
}

//--------------------------------------------------------------

void GenMatrix::
solveSystem (void)
{
  Assert (m_ < n_);
  int i, j, i2;
  double pivot, pivot2;
  
  for (i = 0; i < m_; i++)
  {
    // Reduce row i to all 0's for j < m_ except for rep_[i][i] = 1

    pivot = rep_[i][i];
    Assert (pivot != 0);
    for (j = i+1; j < n_; j++)
      rep_[i][j] /= pivot;
    rep_[i][i] = 1.0;

    for (i2 = 0; i2 < m_; i2++)
    {
      if (i2 == i) continue;
      pivot2 = rep_[i2][i];
      if (pivot2 == 0.0) continue;
      rep_[i2][i] = 0.0;
      for (j = i+1; j < n_; j++)
        rep_[i2][j] -= pivot2 * rep_[i][j];
    }

    COMMENT ("After pivot #" << i << ":");
    COMMENT (*this);
  }

  // Sanity check...
  for (i = 0; i < m_; i++)
    for (j = 0; j < m_; j++)
      if (i != j)
        Assert (rep_[i][j] == 0.0);
      else
        Assert (rep_[i][j] == 1.0);
}

//--------------------------------------------------------------

void GenMatrix::
testSolveSystem (void)
{
  int size1 = (getRandom() % 20) + 1;
  int size2 = (getRandom() % 5) + 1;

  GenMatrix A (size1, size1);
  GenMatrix b (size1, size2);
  GenMatrix *Ab;

  A.randomFill ();
  b.randomFill ();

  COMMENT ("Testing SolveSystem.");
  COMMENT ("Matrix A:");
  COMMENT (A);
  COMMENT (endl << "Matrix b:");
  COMMENT (b);

  Ab = A.adjoinMatrix (b);

  COMMENT (endl << "Matrix Ab:");
  COMMENT (*Ab);

  Ab->solveSystem ();

  COMMENT (endl << "Matrix Ab after Solving System:");
  COMMENT (*Ab);

  GenMatrix *x;

  // Load up the solution x

  x = Ab->extractSubmatrix (0, size1 - 1, size1, size1 + size2 - 1);
  int x_m, x_n;
  x->getSize (x_m, x_n);
  Assert ((x_m == size1) && (x_n == size2));

  COMMENT (endl << "Matrix x:");
  COMMENT (*x);

  GenMatrix * b2 = A * *x;
  int b2_m, b2_n;
  b2->getSize (b2_m, b2_n);
  Assert ((b2_m == size1) && (b2_n == size2));

  COMMENT (endl << "Matrix b2 = A * x (should be b):");
  COMMENT (*b2);

  Assert (*b2 == b);
}

//--------------------------------------------------------------

GenMatrix * GenMatrix::
adjoinMatrix (GenMatrix &m2) const
{
  int m, n, i, j;
  m2.getSize (m, n);
  Assert (m_ == m);

  GenMatrix * newMatrix = new GenMatrix (m_, n_ + n);
    
  for (i = 0; i < m_; i++)
  {
    for (j = 0; j < n_; j++)
      newMatrix->element (i, j) = element (i, j);
    for (j = 0; j < n; j++)
      newMatrix->element (i, n_ + j) = m2.element (i, j);
  }

  return newMatrix;
}

//--------------------------------------------------------------

void GenMatrix::
randomFill (void)
{
  for (int i = 0; i < m_; i++)
    for (int j = 0; j < n_; j++)
      element (i, j) = getRandom () % 10000;
}

//--------------------------------------------------------------

GenMatrix * GenMatrix::
extractSubmatrix (int m1, int m2, int n1, int n2) const
{
  int size1 = m2 - m1 + 1;
  int size2 = n2 - n1 + 1;
  int i, j;

  Assert ((size1 > 0) && (size2 > 0));
  Assert ((0 <= m1) && (m2 < m_));
  Assert ((0 <= n1) && (n2 < n_));

  GenMatrix * newMatrix = new GenMatrix (size1, size2);
  
  for (i = 0; i < size1; i++)
    for (j = 0; j < size2; j++)
      newMatrix->element (i, j) = element (m1 + i, n1 + j);

    return newMatrix;
}
