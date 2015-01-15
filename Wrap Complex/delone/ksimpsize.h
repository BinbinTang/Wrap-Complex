/*
 * ksimpsize.h					KsimplexSize
 *
 * A wrapper for computing size of Ksimplex, simplex in OrTri representation.
 *
 * This class just hides user from LI (infinitely long integer) results
 * from SoS/SizeEvaluator, where the actual computation take place in
 * terms of LI and SoS.
 * Many people, including me, are not familiar with LI and SoS.
 * This class make things look 'double' instead of LI.
 *
 * The plan is to have subclass SimplexSize derived from KsimplexSize.
 * Yes, SimplexSize is a subclass of KsimplexSize, not vice versa,
 * because Simplex in index representation requires hash tables
 * (except 2-simplex) which is auxiliary compared to OrTri and Trist
 * for Ksimplex.
 * 
 * As a result, Complex without hash tables, e.g. Complex2D,
 * uses KsimplexSize, and Complex with hash tables, e.g. DComplex,
 * uses  SimplexSize.
 *
 *
 * NOTE
 *    Ksimplex, NOT KSimplex. 9)
 *
 * Damrong  May 22, 1998
 *
 */
 
#ifndef __RGI_KSIMPSIZE_H__
#define __RGI_KSIMPSIZE_H__

class Complex;
class SizeEvaluator;
class LI_PArray;

#include "sos/size.h"
#include "li/li.h"
#include "geometry/comp.h"

class KsimplexSize : public RgiObject
{
public:
  KsimplexSize (Complex * comp, SizeEvaluator & se);
  virtual ~KsimplexSize (void) {}

  virtual double size (Ksimplex sigma);
  virtual double rho  (Ksimplex sigma);

protected:
  Complex *      comp_;
  SizeEvaluator & se_;
};

//---------------------------------------------------------------------------

#endif // __KSIMPSIZE_H__
