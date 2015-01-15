#include <geometry/geomutil.h>


/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/

FIndexIterator::
FIndexIterator (Trist& tr)
: iter_(tr.getTriangleBV())
{
  
}

/*------------------------------------------------------------------*/

FIndexIterator::
FIndexIterator (StackBV& sbv)
: iter_(sbv.getBitVector())
{
  
}

/*--------------------------------------------------------------------*/

FIndexIterator::
FIndexIterator (BVTag& bvtag)
: iter_(bvtag.getBitVector())
{

}

FIndexIterator::
FIndexIterator (BitVector& bv)
: iter_(bv)
{

}

FIndexIterator::
FIndexIterator (BitVector * bv, Trist * tr)
: iter_(bv? *bv : tr->getTriangleBV())
{

}


/****************************************************************************
 *
 *
 *        class VertexEFDiscSet
 *
 */

VertexEFDiscSet::
VertexEFDiscSet (void)
{
  Assert(empty());
}

VertexEFDiscSet::
~VertexEFDiscSet (void)
{
  while (!empty())
  {
    delete pop();
  }
}

/***************************************************************
 *    
 *           StackBVTag class
 *
 *
 ***************************************************************/




StackBVTag::
~StackBVTag (void)
{
  reset(); bv_ = NULL;
}



//--------------------------------------------------------------

StackBVTag& StackBVTag::
operator= (StackBV& other)
{
  StackBV::operator=(other);
  return *this;
}

