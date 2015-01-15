
#ifndef __RGI_DCOFACES_H__
#define __RGI_DCOFACES_H__

/*--------------------------------------------------------------------------*/


#include <iostream>
#include <basic/rarray.h>
#include <geometry/trist.h>
#include <geometry/cofaces.h>

#include <delone/dcomp.h>
#include <geometry/ksimplex.h>
#include <geometry/geomutil.h>

class DCofaces;




/***********************************************************************
 *
 *
 *        class DCofaces
 *
 *
 */

class 
DCofaces : public Cofaces
{
public:
  DCofaces (DComplex * dc);
  ~DCofaces (void);

  Bool cofaces (Ksimplex sigma, Dim k, KsimplexCB, RgiObject * obj);
  Bool cofaces (Simplex sigma, Dim k, SimplexCB, RgiObject * obj);
  Bool enumKsimplicies (Dim k, KsimplexCB, RgiObject * obj);
  Bool enumSimplicies (Dim k, SimplexCB, RgiObject * obj);
	class Arg
	{
   public:
	   Arg (void) { }
	   Arg (KsimplexCB, SimplexCB, RgiObject *);
	   SimplexCB simpCB;
		 KsimplexCB ksimpCB;
		 RgiObject * obj;
	};

	class DFSArg
	{
   public:
	  DFSArg(BitVectorPool& , OrTriCB);
	  StackBVTag triTag;
		StackBVTag custTag;
		OrTriStack s;
		OrTriCB report;
	};
	
private:
  DComplex * dc_;
	IndexHandler * iHandler_;
	Arg arg_;
	DFSArg * dfs_;

  // inline conversion functions
	int map_ (Dim d1, Dim d2);
  Simplex makeSimplex_ (Ksimplex sigma);
	Simplex makeSimplex_ (OrTri ef, Dim d);
	Ksimplex makeKsimplex_ (Simplex sigma);
	Ksimplex makeKsimplex_ (Index ix, Dim d);
	Index getIndex_ (OrTri ef, Dim d);
  OrTri getFace_ (Simplex sigma);
	OrTri getFace_ (Ksimplex sigma);
  Bool isFiniteSimplexFacet_ (OrTri ef);

  // function to be called in a callback
	int doTheCallback_ (Ksimplex sigma);
  
/*----------------------------------------------------------------------------*/
 OrTriCB cofacesFunc_(Dim d, Dim k);
// special report functions
 Bool report01_ (OrTri ef);
 Bool report02_ (OrTri ef);
 Bool report03_ (OrTri ef);
// depth-first search function with given plug-in
 Bool cofaces0_ (OrTri ef, OrTriCB reportFunc);
 Bool cofaces0i_ (OrTri ef);
// enumerators
 Bool cofaces01_ (OrTri f);
 Bool cofaces02_ (OrTri f);
 Bool cofaces03_ (OrTri f);
 Bool cofaces12_ (OrTri f);
 Bool cofaces13_ (OrTri f);
 Bool cofaces23_ (OrTri f);
};

//------------------------------------------------------------------------
inline DCofaces::Arg::
Arg (KsimplexCB k, SimplexCB s, RgiObject * o)
  : simpCB(s), ksimpCB(k), obj(o) { }

inline DCofaces::DFSArg::
DFSArg (BitVectorPool& pool, OrTriCB crep)
 : triTag(pool), custTag(pool), s(20), report(crep) { }

inline Simplex DCofaces::
makeSimplex_ (Ksimplex sigma)
{ return Simplex(iHandler_->ix(sigma), sigma.getDim()); }

inline Simplex DCofaces::
makeSimplex_ (OrTri ef, Dim d)
{ return Simplex(iHandler_->ix(ef, d), d); }

inline Ksimplex DCofaces::
makeKsimplex_ (Simplex sigma)
{ return Ksimplex(iHandler_->ref(sigma), sigma.getDim()); }

inline Ksimplex DCofaces::
makeKsimplex_ (Index ix, Dim d)
{ return Ksimplex(iHandler_->ref(ix, d), d); }

inline Index DCofaces::
getIndex_ (OrTri ef, Dim d)
{ return iHandler_->ix(ef, d); }

inline OrTri DCofaces::
getFace_ (Simplex sigma)
{ return iHandler_->ref(sigma); }

inline OrTri DCofaces::
getFace_ (Ksimplex sigma)
{ return sigma.getFace(); }

inline Bool DCofaces::
isFiniteSimplexFacet_ (OrTri ef)
{ return !tr_->isOnHull (ef); }

#endif // __DCOFACES_H__

