
#ifdef WRAP_DEBUG
#define __DEBUG__
#endif

#include <math.h>
#include <basic/miscmath.h>
#include <delone/ksimpsize.h>

/*--------------------------------------------------------------------------*/

double KsimplexSize::
size (Ksimplex sigma)
{ return rho (sigma); }

/*--------------------------------------------------------------------------*/
KsimplexSize::
KsimplexSize (Complex * comp, SizeEvaluator & se) 
  : comp_ (comp), se_ (se)
{ 
  
}

/*--------------------------------------------------------------------------*/

double KsimplexSize::
rho (Ksimplex sigma)
{
  VertArray v;
  comp_->getVertices (sigma, v);
  RealPArray * rArray = comp_->getPArray();
  switch (sigma.getDim())
  {
  case 0: return 0.0;
  case 1: return se_.size(rArray, v.getVertex(1), v.getVertex(2));
  case 2: return se_.size(rArray, v.getVertex(1), v.getVertex(2), 
                                   v.getVertex(3));
  case 3: return se_.size(rArray, v.getVertex(1), v.getVertex(2), 
                                   v.getVertex(3), v.getVertex(4));
  }
  return -1.0;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
