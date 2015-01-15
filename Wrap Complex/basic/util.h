#ifndef __RGI_UTIL_H__
#define __RGI_UTIL_H__

#include <basic/basic.h>
#include <iostream>

/*--------------------------------------------------------------------------*/

#ifndef WIN32 // swp
#  include <assert.h>
#  include <stdio.h>
#  ifdef __DEBUG__
#    define TRACE printf
#  else
#    define TRACE
#  endif
#  define RgiAssert(A) assert(A)
#else
#  define RgiAssert(A) Assert(A)
#endif

#define CheckReturnValue_M(X,Y)  if (! X) {cout << Y <<endl;	Assert(0);} 

#define CopyVector(X, Y)      \
	    X[0] = Y[0];      \
	    X[1] = Y[1];      \
	    X[2] = Y[2];      

#define DeleteUtil(X) if (X) delete X; X = NULL;

template <class T> 
inline T minUtil (T a, T b) { return (a<b)?a:b; }

template <class T> 
inline T maxUtil (T a, T b) { return (a>b)?a:b; }

template <class T> 
inline Bool isPosUtil (T a) { return (a>0); }

template <class T> 
inline Bool isNegUtil (T a) { return (a<0); }

template <class T> 
inline void swapUtil (T &a, T &b) { T tmp = b; b = a; a = tmp;   }

inline Bool betweenUtil (double x, double r1, double r2)
{
  return (((r1 <= x) && (x <= r2)) || ((r2 <= x) && (x <= r1)));
}

inline void copyColorUtil (float x[4], const float y[4])
{
  x[0] = y[0]; x[1] = y[1]; x[2] = y[2]; x[3] = y[3];
}

inline void copyVectorUtil (float x[3], const float y[3])
{
  x[0] = y[0]; x[1] = y[1]; x[2] = y[2];
}

inline void copyVectorUtil (double x[3], const double y[3])
{
  x[0] = y[0]; x[1] = y[1]; x[2] = y[2];
}

inline void copyVectorUtil (int x[3], const int y[3])
{
  x[0] = y[0]; x[1] = y[1]; x[2] = y[2];
}

// Handy little defines
#define IS_SET(bit, var)	((var) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))

// Some guards. I find them handy to avoid code duplication;

class CloseGuard {
public:
	CloseGuard (FILE *&file) : file_ (file) {}
	~CloseGuard () { if (file_) { fclose (file_); file_ = (FILE *)0; } }
private:
	FILE *&file_;
};

class FreeGuard {
public:
	FreeGuard (void *&ptr) : ptr_ (ptr) {}
	~FreeGuard () { free (ptr_); ptr_ = (void *)0; }
private:
	void *&ptr_;
};

template<class T>
class DeleteGuard {
public:
	DeleteGuard (T *&ptr) : ptr_ (ptr) {}
	~DeleteGuard () { delete ptr_; ptr_ = (T *)0; }
private:
	T *&ptr_;
};

template<class T>
class VDeleteGuard {
public:
	VDeleteGuard (T *&ptr) : ptr_ (ptr) {}
	~VDeleteGuard () { delete[] ptr_; ptr_ = (T *)0; }
private:
	T *&ptr_;
};

#endif
