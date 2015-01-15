#include <iostream>
#include <geometry/bvtag.h>


BitVectorPool::
BitVectorPool (void) : IterStack<BitVector *>(10)
{

}

/*---------------------------------------------------------------------*/

ostream&   operator<< (ostream& s, const BVTag& bvtag)
{
 s << *bvtag.bv_;
 return s;
}

/*---------------------------------------------------------------------*/

SimplexBVTag::
SimplexBVTag (BitVectorPool& bvpool, Dim dim, int size)
            : bvpool_(bvpool),
              dim_(dim)
{
  for(register int i=0; i<=dim_;i++)
  {
    bv_[i] = bvpool.get();
  }
}

/*---------------------------------------------------------------------*/

SimplexBVTag::
SimplexBVTag (const SimplexBVTag& bvtag)
            : bvpool_(bvtag.bvpool_),
              dim_(bvtag.dim_)
{
  for(register int i=0; i<=dim_;i++)
  {  
    bv_[i] = bvpool_.get(); *bv_[i] = *bvtag.bv_[i];
  }
}

/*---------------------------------------------------------------------*/

ostream&   operator<< (ostream& s, const SimplexBVTag& bvtag)
{
 for(int d=0; d<= bvtag.dim_; d++)
 {
   s <<"Dim="<<d<<endl;
   s <<*bvtag.bv_[d]<<endl;
 }
 return s;
}

/*---------------------------------------------------------------------*/

SimplexBVTag& SimplexBVTag::
operator= (SimplexBVTag& bvtag)
{
 int i;
 int dim = dim_;
 bvtag.getDim();
 for(i=dim+1; i<=bvtag.dim_;i++) bv_[i] = bvpool_.get();
 for(i=bvtag.dim_+1; i<=dim;i++) bvpool_.put(bv_[i]);
 dim_ = bvtag.dim_;
 for(i=0;i<=dim;i++)
  *bv_[i] = *bvtag.bv_[i];
 return *this;
}

/*---------------------------------------------------------------------*/

SimplexBVTag::
~SimplexBVTag (void)
{
  for(int i=dim_; i>=0; i--)
  {
    if (bv_[i]->getNumSet())
      bv_[i]->reset();
    bvpool_.put(bv_[i]);
  }
}

/*---------------------------------------------------------------------*/


