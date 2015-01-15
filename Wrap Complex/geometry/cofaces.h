
#ifndef __RGI_COFACES_H__
#define __RGI_COFACES_H__


/***********************************************************************
 *
 *
 *        class Cofaces
 *        base for all cofaces
 *
 */

#include "basic/index.h"
#include "geometry/tristmodifier.h"
#include "geometry/comp.h"

class Ksimplex;
class Simplex;

class Cofaces : public TristModifier
{
public:
  Cofaces (void) { }
	Cofaces (Complex * comp) : TristModifier (comp->getTrist()) { }
  ~Cofaces (void) { }
  virtual Bool cofaces (Ksimplex sigma, Dim k, 
               KsimplexCB, RgiObject * obj) 
												{ Assert_always(0); return TRUE;}
	virtual Bool cofaces (Simplex sigma, Dim k, 
               SimplexCB, RgiObject * obj) 
												{ Assert_always(0); return TRUE;}
  virtual Bool enumSimplicies (Dim k, 
               SimplexCB, RgiObject * obj) 
							 { Assert_always(0); return TRUE;}
	virtual Bool enumKsimplicies (Dim k, 
               KsimplexCB, RgiObject * obj) 
							 { Assert_always(0); return TRUE;}
};



#endif // __COFACES_H__

