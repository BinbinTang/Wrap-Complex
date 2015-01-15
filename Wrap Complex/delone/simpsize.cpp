#ifdef WRAP_DEBUG
#define __DEBUG__
#endif

#include "ksimpsize.h"
#include  "simpsize.h"
#include     "dcomp.h"

/*--------------------------------------------------------------------------*/

SimplexSize::
SimplexSize (DComplex * dc) 
  : KsimplexSize (dc, dc->getSizeEvaluator()),
    dc_ (dc) 
{ 
}

/*--------------------------------------------------------------------------*/

double SimplexSize::
size (Ksimplex sigma) 
{ 
  return KsimplexSize::size (sigma); 
}

/*--------------------------------------------------------------------------*/

double SimplexSize::
rho (Ksimplex sigma) 
{ 
  return KsimplexSize::rho (sigma); 
}

/*--------------------------------------------------------------------------*/

double SimplexSize::
size (Simplex sigma) 
{ 
  return KsimplexSize::size (dc_->createKsimplex (sigma)); 
}

/*--------------------------------------------------------------------------*/

double SimplexSize::
rho (Simplex sigma) 
{ 
  return KsimplexSize::rho (dc_->createKsimplex (sigma)); 
}

/*--------------------------------------------------------------------------*/


