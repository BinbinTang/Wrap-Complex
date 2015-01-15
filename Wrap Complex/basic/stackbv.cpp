//
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
//

#include <basic/stackbv.h>

StackBV::
~StackBV (void)
{
  delete bv_;
}

StackBV::
StackBV (StackBV &other)
: bv_(new BitVector(*other.bv_)), s_(other.s_)
{

}

StackBV& StackBV::
operator= (StackBV& other)
{
  reset();
  StackBVIter iter(other);
  for(iter.first(); !iter.isDone(); iter.next())
    push(iter.curItem());
  return *this;
}

//---------------------------------------------------------------

int StackBV::
pop (void)
{
  int i; 
  while (!s_.empty()) 
  {  
	i = s_.pop();
    if (test(i))
	{
      clear(i);
	  return i;
	}
  } 
  return -1;
}

//--------------------------------------------------------------

int StackBV::
top (void)
{
 if (s_.empty())
   return -1;

 int i = s_.top();
 if (test(i))
	 return i;
 
 while (!s_.empty())
 {
   i = s_.pop();
   if (test(i))
	 return i;
 }
 
  return -1;
}


//--------------------------------------------------------------

void StackBV::
merge (StackBV& other)
{
  removeDuplicates();
  other.removeDuplicates();
  IndexStack& os = other.getStack();
  int osize = os.size();
  for (int i=1; i<=osize; i++)
  {
    set(os[i]);
  }
}

//--------------------------------------------------------------

void StackBV::
removeDuplicates (void)
{
  if (s_.size() > bv_->getNumSet ())
    sort();
}

//--------------------------------------------------------------

void StackBV::
sort (void)
{
  int ssize = s_.size();
  int tsize = bv_->getNumSet ();
  int ind = -1;
  Assert (ssize >= tsize);
  if (ssize > tsize)
  {
    for (int i=1; i<=ssize; i++)
    {
      if (!bv_->test(s_[i]))
        s_[i] = MAXINT;
    }
  }

  s_.removeDuplicates();
  ssize = s_.size();
  Assert (ssize >= tsize);
  if (ssize > tsize)
  {
    ind = s_.pop();
    ssize--;
    Assert (ind == MAXINT);
  }
  Assert (ssize == tsize);
}

//--------------------------------------------------------------

void StackBV::
remap (IMap& map)
{
  removeDuplicates();
  bv_->reset();
  int sSize = s_.size();
  int mapSize = map.size();
  for(int i=1; i<=sSize; i++)
  {
    Assert (mapSize > s_[i]);
    s_[i] = map[s_[i]];
    bv_->set(s_[i]);
  }
}

//--------------------------------------------------------------


StackBVIter::
StackBVIter (StackBV& sbv)
: sbv_(sbv), curIndex_(sbv.getStack().size()+1)
{

}

//-------------------------------------------------------------

void StackBVIter::
first (void)
{
  IndexStack &s = sbv_.getStack();
  BitVector& bv = sbv_.getBitVector();
  for(curIndex_ = 1; curIndex_ <= s.size(); curIndex_++)
  {
    if (bv.test(s[curIndex_]))
      return;
  }
}

//-------------------------------------------------------------

void StackBVIter::
next (void)
{
  IndexStack &s = sbv_.getStack();
  BitVector& bv = sbv_.getBitVector();
  for(curIndex_++; curIndex_ <= s.size(); curIndex_++)
  {
    if (bv.test(s[curIndex_]))
      return;
  }
}

//-------------------------------------------------------------

