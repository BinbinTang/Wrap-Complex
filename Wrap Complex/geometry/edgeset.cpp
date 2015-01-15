/*
// Copyright 1996-98. Raindrop Geomagic, Inc.
// All Rights Reserved.
//
// This is unpublished proprietary source code of Raindrop Geomagic, Inc.; 
// the contents of this file may not be disclosed to third parties, copied 
// or duplicated in any form, in whole or in part, without the prior written 
// permission of Raindrop Geomagic, Inc.  
//
// This copyright notices may not be removed from this file at any time.
// 
// RESTRICTED RIGHTS LEGEND:
// Use, duplication or disclosure by the Government is subject to restrictions
// as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
// and Computer Software clause at DFARS 252.227-7013, and/or in similar or
// successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished-
// rights reserved under the Copyright Laws of the United States.
// 
//                 Raindrop Geomagic, Inc
//                 206 N. Randolph Street, Suite 520
//                 Champaign IL 61820-3898
//                 Phone: 217-239-2551
//                 Fax:   217-239-2556
//                 Email: inquiry@geomagic.com
// 
*/

#include <geometry/edgeset.h>

EdgeSet::
EdgeSet (EdgeSet & other)
  : TristModifier (other),
    bv_ (other.bv_)
{

}

EdgeSet & EdgeSet::
operator= (EdgeSet & other)
{
  if (this != &other)
  {
    TristModifier::operator= (other);
    bv_ = other.bv_;
  }
  return *this;
}

void EdgeSet::
set (OrTri ef)
{
  OrTri ef1 = ef;
  do {
    bv_.set(ERep_(ef1));
    ef1 = Fnext_(ef1);
  } while (ef1 != ef);
}

void EdgeSet::
clear (OrTri ef)
{
  OrTri ef1 = ef;
  do {
    bv_.clear(ERep_(ef1));
    ef1 = Fnext_(ef1);
  } while (ef1 != ef);
}

void EdgeSet::
reset (void)
{
  bv_.reset ();
}

Bool EdgeSet::
orSets(EdgeSet &set)
{
  bv_.or_op(set.bv_);
  return TRUE;
}

Bool EdgeSet::
subSets (EdgeSet &set)
{
  bv_.sub_op(set.bv_);
  return TRUE;
}

OrTri EdgeSet::
getNextSet (OrTri ef)
{
	int nextRep = bv_.getNextSet (ERep_(ef));
	OrTri nextEF (0, 0);
	if (nextRep > 0)
		nextEF.setIntRep (nextRep << 1);
	return nextEF;
}

Bool EdgeSet::
verify (void) // if this 
{
  BitVector bv(bv_);
  BitVectorIter iter(bv);
  OrTri ef, ef1;
  VIndex p = 0, q = 0, r = 0, p1, q1 = 0, r1 = 0;
  for (iter.first(); !iter.isDone(); iter.next())
  {
    Index rep = iter.curItem();
    ef.setIntRep(rep << 1);
    if (!Fvalid_(ef))
    {
      bv.clear(rep);
      continue;
    }
    Triangle_(ef, p, q, r);
    ef1 = ef;
    do {
      rep = ERep_(ef1);
      if(!bv.test(rep))
      {
        Triangle_(ef1, p1, q1, r1);
        Assert(0);
        return FALSE;
      }
      bv.clear(rep);
    } while ((ef1 = Fnext_(ef1)) != ef);
  }
  return TRUE;
}

Bool EdgeSet::
binInit (BinInput& bi)
{
  return bv_.binInit(bi);
}

void EdgeSet::
binDump (BinOutput& bo)
{
  bv_.binDump(bo);
}

//-------------------------------------------------------------------------

ComplexEdgeSet::
ComplexEdgeSet (ComplexEdgeSet & other)
  : EdgeSet (other),
    comp_ (other.comp_)
{

}

//-------------------------------------------------------------------------

ComplexEdgeSet & ComplexEdgeSet::
operator= (ComplexEdgeSet & other)
{
  if (this != &other)
  {
    EdgeSet::operator= (other);
    comp_ = other.comp_;
  }
  return *this;
}

//-------------------------------------------------------------------------

void ComplexEdgeSet::
set (OrTri ef)
{
  OrTri ef1 = ef;
	if (! comp_->inComplex (ef.index ()))
		return;
  do {
    bv_.set(ERep_(ef1));
  } while ((ef1 = comp_->FnextComplete(ef1)) != ef);
}

//-------------------------------------------------------------------------

void ComplexEdgeSet::
set (OrTri ef, BitVector &triBV, Bool useSet)
{
  OrTri ef1 = ef;
  do {
    if (triBV.test(ef1.index()) == useSet)
      bv_.set(ERep_(ef1));
  } while ((ef1 = comp_->FnextComplete(ef1)) != ef);
}

//-------------------------------------------------------------------------

Bool ComplexEdgeSet::
markVertices (BitVector &vertBV)
{
  BitVectorIter iter(bv_);
  OrTri ef;
  for(iter.first(); !iter.isDone(); iter.next())
  {
    ef.setIntRep(iter.curItem() << 1);
    if(!comp_->inComplex(ef.index()))
      continue;
    vertBV.set(Org_(ef)), vertBV.set(Dest_(ef));
  }
  return TRUE;
}

//-------------------------------------------------------------------------

Bool ComplexEdgeSet::
clearNonBd (void)
{
	ComplexEdgeSetIter iter (*this);
	OrTri ef;
	for (iter.first (); ! iter.isDone (); iter.next ())
	{
		ef = iter.curItem ();
    OrTri ef2(comp_->FnextComplete(ef));
    if (ef2.index()!=ef.index())
			clear (ef);
	}
	return TRUE;
}

//-------------------------------------------------------------------------

Bool BoundaryEdgeSet::
init (BoundaryEdgeSet::Degree degree, BitVector * triangles)
{
  bv_.reset();
  int degree1 = degree & Degree_1;
  int degree3 = degree & Degree_non_2;
  Bool result = FALSE;

  if (!triangles || !triangles->getNumSet()) 
    triangles = &comp_->getFaceBV(COMPLETE_TRI);
  BitVector eBV(tr_->maxTri()*4);
  BitVectorIter tIter(*triangles);
  for (tIter.first(); !tIter.isDone(); tIter.next())
  {
    OrTri ef(tIter.curItem(), 0);
    for (int i=0; i<3; i++, ef = ef.enext())
    {
      if (eBV.test(ERep_(ef)))
        continue;
      int count = 0;
      OrTri ef1 = ef;
      do {
        eBV.set(ERep_(ef1));
        ef1 = FnextBV_(ef1, *triangles);
        count++;
      } while (ef1 != ef);

      if (((count == 1) && degree1) ||
         ((count >= 3) && degree3))
      {
          ef1 = ef, result = TRUE;
          do {
          bv_.set(ERep_(ef1));
          ef1 = Fnext_(ef1);
        } while (ef1 != ef);
      }
    }
  }
  return result;
}

//-------------------------------------------------------------------------

void ComplexEdgeSet::
pack (IMap * map)
{
  BitVectorIter iter(bv_);
  for(iter.first(); !iter.isDone(); iter.next())
  {
    Index eIndex = iter.curItem();
    FIndex f = eIndex >> 2;
    Index ev = eIndex & 03;
    if (map)
    {
      FIndex fNew = (*map)[f];
      bv_.clear(eIndex);
      if(comp_->inComplex(Simplex(fNew, 2)))
        bv_.set((fNew << 2) | ev);
    }
    else
    {
      if(!comp_->inComplex(Simplex(f, 2)))
        bv_.clear(eIndex);
    }
  }
}

//-------------------------------------------------------------------------

Bool BoundaryEdgeSet::
markBdCycle (OrTri ef)
{
  OrTri efBd1, efBd2;
  if(!comp_->getVertBoundary(ef, efBd1, efBd2))
    return FALSE;
  set(efBd1), set(efBd2);
  OrTri ef1 = efBd1.sym();
  
  while (ERep_(ef1) != ERep_(efBd2))
  {
    Bool isBd = FALSE;
    do {
      OrTri ef2(comp_->FnextComplete(ef1.sym().enext()));
      isBd = (ef2.index()==ef1.index() || comp_->FnextComplete(ef2).index()!=ef1.index());
      ef1 = ef2;
    } while (!isBd);
    set(ef1); 
    ef1 = ef1.sym();
  }
  return TRUE;
}

//-------------------------------------------------------------------------

ComplexEdgeSetIter::
ComplexEdgeSetIter (ComplexEdgeSet& set)
: TristModifier (set.getTrist()),
  set_ (set), 
  comp_(set.getComplex()),
  bv_ (set.getBitVector().size()), 
  lastIndex_ (-1)
{

}

//-------------------------------------------------------------------------

void ComplexEdgeSetIter::
first (void)
{
  bv_ = set_.getBitVector();
  bv_.getNumSet();
  lastIndex_ = 0;
  do {
    lastIndex_ = bv_.getNextSet(lastIndex_);
  } while (lastIndex_ >= 0 && !comp_->inComplex(lastIndex_ >> 2));
}

//-------------------------------------------------------------------------

Bool ComplexEdgeSetIter::
isDone (void)
{
  return (lastIndex_ == -1);
}

//-------------------------------------------------------------------------

void ComplexEdgeSetIter::
next (void)
{
  if (lastIndex_ == -1)
    return;

  OrTri ef; 
  ef.setIntRep(lastIndex_ << 1);
  OrTri ef1 = ef;
  do {
    bv_.clear (ERep_(ef1));
    ef1 = Fnext_(ef1);
  } while (ef1 != ef);

  do {
    lastIndex_ = bv_.getNextSet(lastIndex_);
  } while (lastIndex_ >= 0 && !comp_->inComplex(lastIndex_ >> 2));
}

//------------------------------------------------------------------------

OrTri ComplexEdgeSetIter::
curItem (void)
{
  OrTri ef;
  ef.setIntRep (lastIndex_ << 1);
  return ef;
}

//---------------------------------------------------------------------------

Bool ClassifiedEdgeStack::
isClosed (void)
{
  return (flag_ & 02);
}

Bool ClassifiedEdgeStack::
isBd (void)
{
  return (flag_ & 01);
}

void ClassifiedEdgeStack::
setClosed (Bool set)
{
  if(set)
    flag_ |= 02;
  else
  {
    if(flag_ & 02)
      flag_ ^= 02;
  }
}

void ClassifiedEdgeStack::
setBd (Bool set)
{
  if(set)
    flag_ |= 01;
  else
  {
    if(flag_ & 01)
      flag_ ^= 01;
  }
}

EdgeSetClassifier::
EdgeSetClassifier (ComplexEdgeSet &eSet, BitVector * markedVert)
: TristModifier(eSet.getComplex()->getTrist()),
  eSet_(eSet), visited_(eSet.getComplex()), markedVert_(markedVert)
{

}

EdgeSetClassifier::
~EdgeSetClassifier (void)
{

}

inline Bool EdgeSetClassifier::
isBd_ (OrTri ef)
{
  Complex * comp = eSet_.getComplex();
  OrTri ef1 = comp->FnextComplete(ef);
  return ef1.index()==ef.index() || comp->FnextComplete(ef1).index() != ef.index();
}

Bool EdgeSetClassifier::
classifyEdges (IterStack<ClassifiedEdgeStack *> &cycleSet)
{
  OrTri ef;
  BitVectorIter eIter(eSet_.getBitVector());
  Complex * comp = eSet_.getComplex();
  for(eIter.first(); !eIter.isDone(); eIter.next())
  {
    Index eRep = eIter.curItem();
    ef.setIntRep(eRep << 1);
    ef = comp->FnextComplete(ef);
    if(visited_.test(ef))
      continue;
    sf_.reset(), sb_.reset();
    Bool isBd =isBd_(ef); 
    isBd? setBdCycle_(ef) : setIntCycle_(ef);
    ClassifiedEdgeStack * newCycle = new ClassifiedEdgeStack(10);
    createCycle_(*newCycle);
    newCycle->setBd(isBd);
    cycleSet.push(newCycle);
    int size = newCycle->size();
    for(int i=1; i<=size; i++)
      visited_.set((*newCycle)[i]);
  }

  return TRUE;
}

Bool EdgeSetClassifier::
getClassifiedEdgeStack (OrTri ef, ClassifiedEdgeStack &newCycle)
{
  Complex * comp = eSet_.getComplex();
  if (!eSet_.test(ef))
    return FALSE;
  
  sf_.reset(), sb_.reset();
  Bool isBd =isBd_(ef); 
  isBd? setBdCycle_(ef) : setIntCycle_(ef);
  createCycle_(newCycle);
  newCycle.setBd(isBd);
  return TRUE;
}

void EdgeSetClassifier::
createCycle_ (ClassifiedEdgeStack &newCycle)
{
  Bool isBd = isBd_(sf_[1]);
  if(sb_.empty()) // real cycle
  {
    newCycle = sf_;
    newCycle.setClosed(TRUE);
    return;
  }

  // open cycle:
  // sf_ and sb_ share first edge, but in symmetric way
  newCycle.resize(sf_.size()+sb_.size()-1);
  while (!sb_.empty())
    newCycle.push(sb_.pop().sym());
  for(int i=2; i<=sf_.size(); i++)
    newCycle.push(sf_[i]);
	Index a, b;

	// A cycle may look like it is open due to non-manifold vertices.  If this
	// is the case, make it closed.
	a = Org_ (newCycle[1]);
	b = Dest_ (newCycle[newCycle.size ()]);
  newCycle.setClosed(a == b);
  return;
}

Bool EdgeSetClassifier::
sameEdge_ (OrTri ef1, OrTri ef2)
{
  if(ERep_(ef1)==ERep_(ef2))
    return TRUE;
  VIndex a1 = Org_(ef1), b1 = Dest_(ef1),
         a2 = Org_(ef2), b2 = Dest_(ef2);
  Assert(a1==a2);
  if(b1 != b2)
    return FALSE;
  // make sure they stitched

  Complex * comp = eSet_.getComplex();
  OrTri ef = ef1;
  do {
    ef = comp->FnextComplete(ef);
    if (ef == ef2)
      return TRUE;
  } while(ef != ef1);

  return FALSE;
}

OrTri EdgeSetClassifier::
getNextEdge_ (OrTri ef, Bool &isBd, Bool &isSel, Bool &openCycle)
{
  OrTri ef1 = ef = ef.sym();
  if (markedVert_ && markedVert_->test(Org_(ef1)))
  {
    openCycle = TRUE;
    return ef1;
  }

  Complex * comp = eSet_.getComplex();
  while(1) {
    ef1 = ef1.sym().enext();
    isBd = isBd_(ef1), isSel = eSet_.test(ef1);
    if(isBd || isSel)
      break;
    ef1 = comp->FnextComplete(ef1);
  }
  openCycle = sameEdge_(ef, ef1);
  return ef1;
}

Bool EdgeSetClassifier::
setBdCycle_ (OrTri ef)
{
  Complex * comp = eSet_.getComplex();
  Assert(isBd_(ef));
  Bool openCycle = FALSE, isBd = FALSE, isSel = FALSE;
  VIndex p, q, r;
  OrTri ef1(ef);
  do {
    sf_.push(ef1);
    Triangle_(ef1, p, q, r);
    ef1 = getNextEdge_(ef1, isBd, isSel, openCycle);
    openCycle |= (isBd != isSel);
    Triangle_(ef1, p, q, r);
  } while(!openCycle && (ef1 != ef));

  if(!openCycle)
    return FALSE; // no need to go backwards

  openCycle = FALSE;
  ef1 = ef = ef.sym();
  do {
    sb_.push(ef1);
    Triangle_(ef1, p, q, r);
    ef1 = getNextEdge_(ef1, isBd, isSel, openCycle);
    openCycle |= (isBd != isSel);
    Triangle_(ef1, p, q, r);
  } while(!openCycle && (ef1 != ef));
  Assert(openCycle);

  return TRUE;
}

Bool EdgeSetClassifier::
setIntCycle_ (OrTri ef)
{
  Complex * comp = eSet_.getComplex();
  Assert(!isBd_(ef));
  VIndex p, q, r;
  Triangle_(ef, p, q, r);
  Bool openCycle = FALSE, isBd = FALSE, isSel = FALSE;

  OrTri ef1(ef);
  do {
    sf_.push(ef1);
    Triangle_(ef1, p, q, r);
    OrTri ef2 = getNextEdge_(ef1, isBd, isSel, openCycle), 
          ef3;
    Triangle_(ef2, p, q, r);
    openCycle |= isBd;
    if(!openCycle)
    {
      ef3 = getNextEdge_(comp->FnextComplete(ef2.sym()), isBd, isSel, openCycle);
      Triangle_(ef3, p, q, r);
      openCycle |= isBd || !sameEdge_(ef1.sym(), ef3);
    }
    ef1 = ef2;
  } while(!openCycle && (ef1 != ef));
  if(!openCycle)
    return FALSE;

  ef1 = ef = ef.sym();
  do {
    sb_.push(ef1);
    Triangle_(ef1, p, q, r);
    OrTri ef2 = getNextEdge_(ef1, isBd, isSel, openCycle), 
          ef3;
    Triangle_(ef2, p, q, r);
    openCycle |= isBd;
    if(!openCycle)
    {
      ef3 = getNextEdge_(comp->FnextComplete(ef2.sym()), isBd, isSel, openCycle);
      Triangle_(ef3, p, q, r);
      openCycle |= isBd || !sameEdge_(ef1.sym(), ef3);
    }
    ef1 = ef2;
  } while(!openCycle && (ef1 != ef));
  Assert(openCycle);

  return TRUE;
}

//-----------------------------------------------------------------------------------------

ComplexEdgeSetLayerMarker::
ComplexEdgeSetLayerMarker (Complex * comp)
: TristModifier(comp->getTrist()), comp_(comp),
  usedV_(comp->getBVPool()), fStar_(comp->getBVPool()),
  F1_(comp->getBVPool()), F2_(comp->getBVPool()), 
  usedStarF_(comp->getBVPool())
{

}
 
ComplexEdgeSetLayerMarker::
~ComplexEdgeSetLayerMarker (void)
{

}

Bool ComplexEdgeSetLayerMarker::
mark (ComplexEdgeSet &eSet, int nLayers, BitVector * triBV)
{
  ComplexEdgeSetIter iter(eSet);

  for(iter.first(); !iter.isDone(); iter.next())
  {
    markFirstLayerOrTri_(iter.curItem(), triBV);
  }

  StackBVTag * s1 = &F1_, *s2 = &F2_, *tmp = NULL;
  while(--nLayers)
  {
    markLayer_(s1, s2, triBV);
    tmp = s1, s1 = s2, s2 = tmp;
  }
  return TRUE;
}

Bool ComplexEdgeSetLayerMarker::
mark (OrTriStack &eStack, int nLayers, BitVector * triBV)
{
  int i, n = eStack.size();

  for(i=1; i<=n; i++)
  {
    markFirstLayerOrTri_(eStack[i], triBV);
  }

  StackBVTag * s1 = &F1_, *s2 = &F2_, *tmp = NULL;
  while(--nLayers)
  {
    markLayer_(s1, s2, triBV);
    tmp = s1, s1 = s2, s2 = tmp;
  }
  return TRUE;
}

Bool ComplexEdgeSetLayerMarker::
markFirstLayerOrTri_(OrTri ef, BitVector * triBV)
{
  for(int i = 0; i<2; i++, ef = ef.sym())
  {
    VIndex v = Org_(ef);
    if(usedV_.test(v))
      continue;
    usedV_.set(v);
    comp_->getPartialDisk(ef, fStar_);
    while(!fStar_.empty())
    {
      FIndex f = fStar_.pop();
      triBV->set(f);
      F1_.push(f);
    }
  }
  return TRUE;
}

Bool ComplexEdgeSetLayerMarker::
markLayer_ (StackBVTag * s1, StackBVTag * s2, BitVector * triBV)
{
  while(!s1->empty())
  {
    FIndex f = s1->pop();
    if(usedStarF_.test(f))
      continue;
    usedStarF_.set(f);
    OrTri ef(f, 0);
    for(int i=0; i<3; i++, ef = ef.enext())
    {
      VIndex v = Org_(ef);
      if(usedV_.test(v))
        continue;
      usedV_.set(v);
      comp_->getPartialDisk(ef, fStar_);
      while(!fStar_.empty())
      {
        FIndex f = fStar_.pop();
        if(usedStarF_.test(f))
          continue;
        triBV->set(f);
        s2->push(f);
      }
    }
  }
  return TRUE;
}

