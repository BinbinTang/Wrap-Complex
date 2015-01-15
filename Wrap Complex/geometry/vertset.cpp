/*
// Copyright 1996-99. Raindrop Geomagic, Inc.
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

#include <geometry/vertset.h>
#include <geometry/edgeset.h>

const int ComplexVertexSetMagic = 5559;
const int ComplexVertexSetVersion = 1;

ComplexVertexSet::
ComplexVertexSet (Complex *comp)
: TristModifier(comp->getTrist()),
  magic_(ComplexVertexSetMagic, ComplexVertexSetVersion),
	comp_(comp), vValid_(TRUE)
{

}

ComplexVertexSet::
ComplexVertexSet (ComplexVertexSet &other)
: TristModifier (other.getComplex()->getTrist()),
	magic_(ComplexVertexSetMagic, ComplexVertexSetVersion),
  comp_(other.comp_), vValid_(other.vValid_),
  efRep_(other.efRep_), vRep_(other.vRep_)
{

}

void ComplexVertexSet::
setComplex (Complex *comp) 
{ 
  comp_ = comp;
  tr_ = comp->getTrist(); 
}


ComplexVertexSet & ComplexVertexSet::
operator= (ComplexVertexSet & other)
{
  comp_ = other.comp_;
  tr_ = other.tr_;
  vValid_ = other.vValid_;
  efRep_ = other.efRep_;
  vRep_ = other.vRep_;
  tmp_.reset();
  return *this;
}


void ComplexVertexSet::
invalidate (void)
{
  vValid_ = FALSE;
}

void ComplexVertexSet::
validate (void)
{
  vValid_ = TRUE;
}

void ComplexVertexSet::
set (OrTri ef, Bool onOff)
{
  Assert(tr_ == comp_->getTrist());
  Assert (tmp_.empty());
  if (test(ef)==onOff)
    return;

  comp_->getCompleteStar(ef, tmp_);
  VIndex v = Org_(ef);
  while(!tmp_.empty())
  {
    ef = Orient_(tmp_.pop(), v);
    efRep_.set(getRep(ef), onOff);
  }
  vRep_.set(v, onOff);
}

void ComplexVertexSet::
reset (void)
{
  vValid_ = TRUE;
  efRep_.reset();
  vRep_.reset();
}

Bool ComplexVertexSet::
test (OrTri ef)
{
  Assert(tr_ == comp_->getTrist());
  return efRep_.test(getRep(ef)) && comp_->inComplex(ef.index());
}

Bool ComplexVertexSet::
test (VIndex v)
{
  Assert(tr_ == comp_->getTrist());
  return getVertexBV().test(v);
}

Bool ComplexVertexSet::
getMarkedOrTri (VIndex v, OrTri& ef)
{
  if(!vRep_.test(v))
    return FALSE;
  BitVectorIter iter(efRep_);
  for(iter.first(); !iter.isDone(); iter.next())
  {
    ef = getOrTri(iter.curItem());
    if (comp_->inComplex(ef.index()) && Org_(ef)==v)
      return TRUE;
  }
  return FALSE;
}

BitVector &ComplexVertexSet::
getVertexBV (void)
{
  if(!vValid_) pack();
  return vRep_;
}

void ComplexVertexSet::
pack (IMap * map)
{
  Assert(tmp_.empty());
  vRep_.reset(); // it will be recomputed from efRep_;
  OrTri ef, ef1;
  Index rep = 0;
  while((rep = efRep_.getNextSet(rep)) > 0)
  {
    efRep_.clear(rep);
    ef = getOrTri(rep);
    ef1 = (map)? OrTri((*map)[ef.index()], ef.version()) : ef;
    if(!comp_->inComplex(ef1.index()))
      continue;
    efRep_.set(getRep(ef1));
    vRep_.set(Org_(ef1));
  }

  vValid_ = TRUE;
}

void ComplexVertexSet::
binDump (BinOutput &bo)
{
	magic_.binDump(bo);
	efRep_.binDump(bo);
}

Bool ComplexVertexSet::
binInit (BinInput &bi)
{
	vValid_ = FALSE;
	return magic_.binInit(bi) &&
		efRep_.binInit(bi);
}
