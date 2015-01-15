//
// ortribv.h -- A bitvector-type structure for oriented triangles


#ifndef __RGI_ORTRIBV_H__
#define __RGI_ORTRIBV_H__

#include <basic/ortri.h>
#include <basic/index.h>
#include <basic/bitvector.h>

// class OrTriBV -- an oriented triangle "bitvector"
//
// This class keeps track of a set of oriented triangles using
// the orientation of the triangle only, meaning that if a triangle
// OrTri (t, 0) is set, then OrTri (t, 2) and OrTri (t, 4) are both set
// also.  Likewise with the odd orientations.

class OrTriBV
{
public:
	OrTriBV (int numTriangles = 20);
	~OrTriBV (void);
	OrTriBV (const OrTriBV & other);

	const OrTriBV & operator= (const OrTriBV & other);

	//  The following functions will modify the internal bitvectors.
	//  The "test" functions will test if the bitvectors were changed,
	//  invalidate temp BVs accordingly, and indicate if a change
	//  occurred.  The non-test functions are meant for efficiency.
	//  WARNING:  If you use them, you are responsible for invalidating
	//  BVs when necessary.

	void set (OrTri ef);
	int setTest (OrTri ef);   // returns TRUE if not already set
	void set (Index ix);
	int setTest (Index ix);   // returns TRUE if not already set

	void setAll (BitVector & other);

	void clear (OrTri ef);
	int clearTest (OrTri ef);   // returns TRUE if not already clear
	void clear (Index ix);
	int clearTest (Index ix);   // returns TRUE if not already clear

	int test (OrTri ef);
	int test (Index ix);

	void reset ();

	void and_op (BitVector& other);
	void or_op (BitVector& other);
	void xor_op (BitVector& other);
	void sub_op (BitVector& other);
	void neg_op (void);
	
	int getNumSet (void);

	void remap (IMap& map);
	
	void getBVs (BitVector* &bv0, BitVector* &bv1);
	BitVector * getOrientedBV (int orientation);
	BitVector * getIndexBV (void);

	void invalidateBVs (void);

  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);

private:
	enum { MaxOrientation_ = 1 };
	BitVector *bv_[MaxOrientation_ + 1];

	enum { MaxTemp_ = 0 };
	BitVector *tempBV_[MaxTemp_ + 1];
};

// class OrEdgeBV -- an oriented edge "bitvector"
//
// This class keeps track of a set of oriented edges using
// the orientation of the triangle only, meaning that if a triangle
// OrTri (t, 0) is set, then OrTri (t, 1) is set also.  Likewise with
// other orientations.

class OrEdgeBV
{
public:
	OrEdgeBV (int numTriangles = 20);

	void set (OrTri ef);
	void clear (OrTri ef);
	int test (OrTri ef);

	void reset ();

	void and_op (OrEdgeBV& other);
	void or_op (OrEdgeBV& other);
	void xor_op (OrEdgeBV& other);
	void sub_op (OrEdgeBV& other);
	void neg_op (void);
	
	int getNumSet (void);
	
	BitVector &getBV (void) { return bv_; }

	friend ostream& operator<< (ostream & s, OrEdgeBV & bv)
	{ s << bv.bv_ << endl;  return s; }
  void binDump (BinOutput& bo);
  Bool binInit (BinInput& bi);
private:
	BitVector bv_;
};

#endif // _ORTRI_BV_H_
