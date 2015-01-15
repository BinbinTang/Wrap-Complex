/*
 * simpsize.h					SimplexSize
 *
 * A wrapper for computing size of Simplex, simplex represented by
 * index from auxiliary hash table. 
 * SimplexSize is a subclass of KsimplexSize.
 * Basically SimplexSize just converts Simplex to Ksimplex,
 * and pass control to KsimplexSize.
 *
 * Note that Ksimplex (OrTri) is more general than Simplex (index),
 * eventhough the names suggest not.
 * For example, Complex2D knows Ksimplex, but not Simplex;
 * DComplex knows both Ksimplex and Simplex.
 *
 * member functions  : size (Simplex),  rho (Simplex)
 * derived functions : size (Ksimplex), rho (Ksimplex)
 *
 * NOTE
 *   SimplexSize (DComplex *)
 *   but KsimplexSize (Complex *, SizeEvaluator &)
 *   because Complex does not have SizeEvaluator.
 *   Strange ?
 *   Future new design is coming....
 */

#ifndef __RGI_SIMPSIZE_H__
#define __RGI_SIMPSIZE_H__

class DComplex;
class KsimplexSize;

#include "ksimpsize.h"

class SimplexSize : public KsimplexSize
{
public:
  SimplexSize (DComplex * dc);
  virtual ~SimplexSize (void) {}

  virtual double size (Simplex sigma);
  virtual double rho  (Simplex sigma);

  virtual double size (Ksimplex sigma);
  virtual double rho  (Ksimplex sigma);

protected:
  DComplex * dc_;
};

//---------------------------------------------------------------------------

#endif // __SIMPSIZE_H__
