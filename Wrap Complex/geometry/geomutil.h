#ifndef __RGI_GEOMUTIL_H__
#define __RGI_GEOMUTIL_H__


#include <basic/stackbv.h> 
#include <li/lipoints.h>
#include <geometry/trist.h>
#include <geometry/bvtag.h>
#include <geometry/comp.h>
#include <geometry/simplex.h>

class Complex;
class Ksimplex;


/************************************************************************
*
*
*     class GenVertexInfo
*
*/

class GenVertexInfo
{
public:
  VIndex a;
  OrTri  abc;
  double coords[4];
  GenVertexInfo (void) { }
  GenVertexInfo (VIndex v, double x, double y, double z)
	  : a(v)
  {
    coords[0] = x; coords[1] = y; coords[2] = z;
  }
  ~GenVertexInfo (void) { }
  int loadPoint(VIndex v, double x, double y, double z)
  { a = v; coords[0] = x; coords[1] = y; coords[2] = z; return a; }
};

/************************************************************************
*
*
*    class VertexInfo
*
*/

class VertexInfo : public GenVertexInfo
{
public:
  IndexStack nblist;
};

/************************************************************************
*
*
*     class FIndexIterator
*
*/

class FIndexIterator
{

public:
  FIndexIterator (Trist& tr);
  FIndexIterator (BitVector& bv);
  FIndexIterator (BVTag& bvtag);
  FIndexIterator (StackBV& sbvtag);
  FIndexIterator (BitVector * bv, Trist * tr);
  void first (void) { iter_.first(); }
  void next (void) { iter_.next(); }
  Bool isDone (void) { return iter_.isDone(); }
  FIndex curItem (void) { return iter_.curItem(); }
private:
  BitVectorIter iter_;
};

/****************************************************************************
 *
 *
 *        class EFDisc
 *
 */

class EFDisc : public IterStack<OrTri>
{
public:
  Bool isDisc;
  EFDisc (void) : isDisc(FALSE) { }
  ~EFDisc (void) { }
};

/****************************************************************************
 *
 *
 *        class VertexEFDisc
 *
 */

class VertexEFDiscSet : public IterStack<EFDisc *>
{
public:
  VertexEFDiscSet (void);
  ~VertexEFDiscSet (void);

};

class StackBVTag : public StackBV
{
public:
	StackBVTag (BitVectorPool& pool);
  StackBVTag (StackBVTag& other);
  StackBVTag& operator= (StackBV& other);
	virtual ~StackBVTag (void);
  BVTag &getBVTag (void);
  BitVectorPool& getBVPool (void);
protected:
  BVTag tag_;
};

inline StackBVTag::StackBVTag (BitVectorPool& pool)
		: StackBV(FALSE, (BitVector *) NULL) , tag_(pool) { bv_ = &tag_.getBitVector(); }
inline StackBVTag::StackBVTag (StackBVTag& other)
: StackBV(FALSE, &other.tag_.getBitVector(), other.s_), tag_ (other.tag_){  }
inline BVTag &StackBVTag::getBVTag (void) { return tag_; }
inline BitVectorPool& StackBVTag::getBVPool (void) { return tag_.getBVPool(); }

#endif // __GEOMUTIL_H__



