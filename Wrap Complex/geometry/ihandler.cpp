/*
 * ihandler.cpp -- C++ Implementation file for the Index Handler class
 *
 * author -- Mike Facello, Damrong
 *
 *
 * date -- 12/17/96
 *      --  2/27/98  Damrong
 *              Pass tr_ instead of comp_ to hash tables.
 *              Use SimpHTable instead of SimpHashTable.
 *		Note '#ifdef _DYNAMIC_'
 */
#include <geometry/simph.h>
#include <geometry/ihandler.h>
#include <geometry/geomutil.h>
#include <basic/binio.h>
#include <iostream>
#include <basic/miscmath.h>
#include <basic/comment.h>

/*--------------------------------------------------------------------------*/
/*--------------------------- CLASS INDEXHANDLER ---------------------------*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/


IndexHandler::
IndexHandler (Complex *comp, BitVector * triangles, 
              unsigned int dimFlag, Bool closed, Bool isRandom)
  : TristModifier (comp->getTrist()),
    comp_ (comp),
    dimFlag_ (dimFlag),
    closed_ (closed),
    isInitialized_ (FALSE),
    triangles_ (triangles),
    numV_ (0),
    numE_ ((dimFlag & INDEX_E)? comp->numEdges (triangles):0),
    numF_ (comp->getTrist()->maxTri()),
    numT_ (((dimFlag & INDEX_T) && !triangles)? comp->numTetra ():0 ),
    usedVert_(comp->getPArray()->size()+1)

{
#ifdef _DYNAMIC_
    vertTable_ =(new OrTri [comp->getPArray()->size() +1]);
    edgeTable_ =(new RSimpHTable (tr_, 1, numE_, isRandom));
    tetraTable_ =(new RSimpHTable (tr_, 3, numT_, isRandom));
#else
    vertTable_ =(new OrTri [ comp->getPArray()->size() +1]);
    edgeTable_ =(new FSimpHTable (tr_, 1, numE_, isRandom));
    tetraTable_ =(new FSimpHTable (tr_, 3, numT_, isRandom));
#endif
    

}

/*-----------------------------------------------------------------------*/

IndexHandler::
~IndexHandler (void)
{
#ifdef _DYNAMIC_
  COMMENT("edgeTable : insert&del resize, avgProbeLen&numSearch ");
  COMMENT(edgeTable_->resizeCount1 () <<","
    << edgeTable_->resizeCount2 () <<"     "
    << edgeTable_->avgProbeLen  () <<","
    << edgeTable_->numSearch    () <<);
  COMMENT("tetraTable : insert&del resize, avgProbeLen&numSearch ");
  COMMENT(tetraTable_->resizeCount1 () <<","
    << tetraTable_->resizeCount2 () <<"     "
    << tetraTable_->avgProbeLen  () <<","
    << tetraTable_->numSearch    () <<endl);

	delete     edgeTable_;
	delete    tetraTable_;

#else
  delete     edgeTable_;
  delete    tetraTable_;
  delete []  vertTable_;
#endif
}


/*--------------------------------------------------------------------------*/

Bool IndexHandler::
binInit (BinInput& bi)
{
  if (!edgeTable_->binInit (bi))
    return FALSE;
  if (!tetraTable_->binInit (bi))
    return FALSE;
  return TRUE;
}


/*--------------------------------------------------------------------------*/

int IndexHandler::
init (void)
{
  int counter = 0;

  BitVector * triBV = triangles_;
  if (!triBV)
  {
    // hull case
    if (dimFlag_ & INDEX_T)
      triBV = &tr_->getTriangleBV();
    else if (closed_)
      triBV = &comp_->getFaceBV(CLOSED_TRI);
    else
      triBV = &comp_->getFaceBV(COMPLETE_TRI);
  }

  int progInterval = triBV->getNumSet() / 50;
  if (progInterval < 2) progInterval = 2;
  float r = (float) triBV->getNumSet();
  r = r? 1.0f/r : 0.0f;

  BitVector usedEdge (numF_ * 4);
  BitVector usedTetra(tr_->getHullBV());

  BitVectorIter tIter(*triBV);
  for (tIter.first(); !tIter.isDone (); tIter.next(), counter++)
    {
      Assert (!tr_->deleted(tIter.curItem()));
      if (!(counter % progInterval))
        if(!progressUpdate (r * counter))
          return FALSE;

      OrTri curEF(tIter.curItem(), 0);

      int i, vertSet = 0;
      for (i=0; i<3; i++, curEF = Enext_(curEF))
      {
        if (usedEdge.test (ERep_(curEF)))
          continue;
        edgeTable_->insert (curEF);
        OrTri ef = curEF;
        do {
          usedEdge.set(ERep_(ef));
          ef = Fnext_(ef);
        } while (ef != curEF);
        if (!vertSet)
        {
					insert (curEF,           0);         // insert 3 vertices
					insert (Enext_ (curEF),  0);
					insert (Enext2_ (curEF), 0);
          vertSet = 1;
        }
      }

      if (!(dimFlag_ & INDEX_T))
        continue;

      for(i=0; i<2; i++, curEF = Sym_(curEF))
      {
        if (usedTetra.test(FRep_(curEF)))
          continue;
        tetraTable_->insert(curEF);
        usedTetra.set (FRep_(curEF));
        usedTetra.set (FRep_(Sym_(Fnext_(curEF))));
        usedTetra.set (FRep_(Sym_(Fnext_(Enext_(curEF)))));
        usedTetra.set (FRep_(Sym_(Fnext_(Enext2_(curEF)))));
      }

    }
   isInitialized_ = TRUE;
   return TRUE;
}

/*-----------------------------------------------------------------------*/

void IndexHandler::
binDump (BinOutput& bo)
{
   edgeTable_->binDump(bo);
  tetraTable_->binDump(bo); 
}

/*-----------------------------------------------------------------------*/

int IndexHandler::
ix (OrTri ef, Dim d)
{
  switch (d)
  {
  case 0: return toVIX (ef);
  case 1: return toEIX (ef);
  case 2: return toFIX (ef);
  case 3: return toTIX (ef);
  }
  return -1;
}

/*-----------------------------------------------------------------------*/

EIndex IndexHandler::
ix_v (VIndex a, VIndex b)
{
  Assert (dimFlag_ & INDEX_E);
  VertArray v(a, b);
  v.sort();
  SimpHTable::VA_SimpKey key(&v);
  return edgeTable_->searchKey (key);
}

/*-----------------------------------------------------------------------*/

FIndex IndexHandler::
ix_v (VIndex a, VIndex b, VIndex c)

{
  EIndex ix = ix_v (a, b);
  if (!ixIsSimplexIndex (ix, 1))
    return SimpHTable::NIL;

  OrTri abd = fromEIX (ix);
  OrTri abc = abd;

  do {
    if (tr_->dest(tr_->enext(abc)) == c)
      return abc.index();
    abc = tr_->fnext (abc);
  } while (abd != abc);

  return SimpHTable::NIL;
}


/*-----------------------------------------------------------------------*/

TIndex IndexHandler::
ix_v (VIndex a, VIndex b, VIndex c, VIndex d)
{
  Assert (dimFlag_ & INDEX_T);
  VertArray v(a, b, c, d); 
  v.sort();
  SimpHTable::VA_SimpKey key(&v);
  return tetraTable_->searchKey (key);
}

/*-----------------------------------------------------------------------*/

OrTri IndexHandler::
ref (Index ix, Dim d)

{
  Assert (ixMaxIndex(d) >= ix);
  switch (d)
  {
    case 0:
       Assert_always (usedVert_.test(ix));
       return vertTable_[ix];
    case 1:
       Assert_always (edgeTable_->isValidIndex (ix));
       return (*edgeTable_) [ix];   
    case 2:
       return OrTri(ix, 0);
    case 3:
       Assert_always (tetraTable_->isValidIndex (ix));   
       return (*tetraTable_) [ix];
    default: Assert_always (0);
  }
  return OrTri(ix, 0);
}

/*-----------------------------------------------------------------------*/

int IndexHandler::
ixMaxIndex (Dim d)
{
  switch (d)
  {
  case 0: return comp_->getPArray()->size();
  case 1: return edgeTable_->size ();
  case 2: return tr_->maxTri();
  case 3: return tetraTable_->size ();
  default: Assert_always(0);
  }

  return -1;
}

/*-----------------------------------------------------------------------*/

Bool IndexHandler::
ixIsSimplexIndex (int ix, Dim d)
{
  if (ix < 0) 
    return FALSE;

  if (ix > ixMaxIndex(d)) 
    return FALSE;

  switch (d)
  {
    case 0: return usedVert_.test (ix);
    case 1: return edgeTable_->isValidIndex (ix);
    case 2: return !tr_->deleted (ix);
    case 3: return tetraTable_->isValidIndex (ix);
  }

  return FALSE;
}

/*-----------------------------------------------------------------------*/

int IndexHandler::
insert (OrTri ef, Dim d)
{
  VIndex a;
  Assert(!tr_->deleted(ef.index()));

  switch (d)
  {
  case 0: a = tr_->org(ef);
          if (! usedVert_.test (a))
          {
						  usedVert_.set (a);
						  numV_ ++;
					}
          vertTable_[a] = ef;
          return a;
  case 1: return edgeTable_->insert (ef);
  case 2: return ef.index();
  case 3: return tetraTable_->insert (ef);
  default: Assert_always(0);
  }
  return SimpHTable::NIL;
}

/*-----------------------------------------------------------------------*/

int IndexHandler::
substitute ( OrTri ef, int ix, Dim d)
{
	Assert (ixIsSimplexIndex (ix, d));

  switch (d)
  {
  case 0:
    vertTable_[ix] = ef;
    break;
  case 1:
    Assert (dimFlag_ & INDEX_E);
    (*edgeTable_) [ix] = ef;
    break;
  case 2:
    break;
  case 3:
    Assert (dimFlag_ & INDEX_T);
    (*tetraTable_) [ix] = ef;
    break;
  default:
    Assert_always(0);
  }
  return TRUE;
}

/*-----------------------------------------------------------------------*/

Bool IndexHandler::
del (int ix, Dim d)
{
  switch (d)
  {
    case 0:
			if (! usedVert_.test(ix))
			{
				  return FALSE;
			}
			else
			{
          usedVert_.clear(ix);
					numV_ --;
          return TRUE;
			}
    case 1:
      return edgeTable_->del (ix);
    case 2:
      return FALSE;
    case 3:
      return tetraTable_->del (ix);
    default: Assert_always (0);
  }
  return FALSE;
}

/*-----------------------------------------------------------------------*/

Bool IndexHandler::
del (OrTri ef, Dim d)
{
  int ind = ix( ef, d);
  if (!ixIsSimplexIndex (ind, d))
    return FALSE;

  return del (ind, d);
}

/*-----------------------------------------------------------------------*/

BitVector& IndexHandler::
getBitVector (Dim d)
{
  switch (d)
  {
  case 0: return   usedVert_;
  case 1: return  edgeTable_->getBV ();
  case 2: return         tr_->getTriangleBV ();
  case 3: return tetraTable_->getBV ();
  }
  return usedVert_;
}

/*-----------------------------------------------------------------------*/

ostream&
operator<< (ostream& s, IndexHandler& iHandler)
{
  
  // Do something here...

  s << "Index Handler:\n\n";
  s << "numV: " << iHandler.numV_ << endl;
  s << "numE: " << iHandler.numE_ << endl;
  s << "numF: " << iHandler.numF_ << endl;
  s << "numT: " << iHandler.numT_ << endl;

  s << "\nEdges: \n\n";
  s << (*(iHandler.edgeTable_));
  s << "\nThedra: \n\n";
  s << (*(iHandler.tetraTable_));

  return s;
}

/*--------------------------------------------------------------------------*/

void IndexHandler::
addSimplices (SimplexSet * s)
{
  Dim                   d;
  IStackIter <OrTri> * it;

  for (d = 0; d <= 3; d ++)
  {
    it = s->getIter (d);
    for (it->first (); !it->isDone (); it->next ())
    {
      insert (it->curItem (), d);
    }
    delete it;
  }
}

/*--------------------------------------------------------------------------*/

void IndexHandler::
delSimplices (SimplexSet * s)
{
  Dim                   d;
  IStackIter <OrTri> * it;

  for (d = 0; d <= 3; d ++)
  {
    it = s->getIter (d);
    for (it->first (); !it->isDone (); it->next ())
    {
      del (it->curItem (), d);
    }
    delete it;
  }
}

/*--------------------------------------------------------------------------*/

void IndexHandler::
updSimplices (SimplexSet *s)
{
	Dim                   d;
  IStackIter <OrTri> * it;

  for (d = 0; d <= 3; d++)
  {
		it = s->getIter (d);
    for (it->first (); !it->isDone (); it->next ())
		{
        substitute (it->curItem (), ix (it->curItem (), d), d);
		}
    delete it;
	}
}

/*--------------------------------------------------------------------------*/

void IndexHandler::
reMap (IMap *m)
{
  int	       i;
  OrTri	 ot0,ot1;
  Dim	       d;
  IndexIter * it;

  for (d = 0; d <= 3; d++)
  {
    if (d == 2) continue;			// do nothing for Triangle

    it = new IndexIter (*this, d);
    for (it->first (); !it->isDone (); it->next ())
    {
      i   = it->curIndex ();
      ot0 = it->curOrTri ();
      ot1 = OrTri ((*m) [ot0.index()], ot0.version ());
      substitute (ot1, i, d);
    }
    delete it;
  }
}

/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*---------------------------- CLASS INDEXITER -----------------------------*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

IndexIter::
IndexIter (IndexHandler& iHandler, Dim d)
: iHandler_ (iHandler),
  d_ (d),
  iter_(iHandler.getBitVector (d))
{

}

/*--------------------------------------------------------------------------*/

