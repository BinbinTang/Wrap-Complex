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

#include <basic/wfshortestpather.h>


WireFrameShortestPather::
WireFrameShortestPather (IndexWireFrame * wf, WireFrameSameEnds * sameEnds)
: wf_(wf), sameEnds_(sameEnds), mask_(NULL),
  src_(0), dest_(0)
{
  pq_.setCompFunc((PriorityQueue<VIndex>::CompFunc) &WireFrameShortestPather::compareCB_,this);
  lengthFunc_ = (LengthFunc) &WireFrameShortestPather::length_;
  obj_ = this;
}

WireFrameShortestPather::
~WireFrameShortestPather (void)
{

}

void WireFrameShortestPather::
setMask (BitVector * mask)
{
  mask_ = mask;
}

void WireFrameShortestPather::
setLengthFunc (LengthFunc func, RgiObject * obj)
{
  lengthFunc_ = func;
  obj_ = obj;
}

void WireFrameShortestPather::
resetLengthFunc (void)
{
  lengthFunc_ = (LengthFunc) &WireFrameShortestPather::length_;
  obj_ = this;
}

void WireFrameShortestPather::
reset (void)
{
  pq_.reset();
  traversed_.reset();
  usedVx_.reset();
  src_ = dest_ = 0;
}

void WireFrameShortestPather::
resetInternal (void)
{
  pq_.reset();
  traversed_.reset();
  src_ = dest_ = 0;
}

Bool WireFrameShortestPather::
findShortestPath (OrPath orgPath, OrPath destPath, IterStack<OrPath>& opseq)
{
  opseq.reset();
  src_ = wf_->getAtOrg(orgPath), 
  dest_ = wf_->getAtOrg(destPath);

  // traverse from dest to src, since we reverse order at the end
  elist_[dest_] = destPath;
  distance_[dest_] = 0;
  keyLoc_[dest_] = pq_.insert(dest_);
  traversed_.set(dest_);
  Bool Done = FALSE;

  while(!pq_.empty())
  {
    Index currV = pq_.extractMin();
    if(Done = (currV==src_))
      break;
    OrPath op1(elist_[currV]);
    OrPath op(op1);
    double currVdist = distance_[currV];
    do {
      if(mask_ && !mask_->test(op.index()))
        continue;
      OrPath curop(op.sym());
      VIndex Nb = wf_->getAtOrg(curop);
      if(usedVx_.test(Nb))
        continue;
      double edgeLen = (obj_->*lengthFunc_)(curop) + currVdist;
      if(!traversed_.test(Nb))
      {
        elist_[Nb] = curop;
        distance_[Nb] = edgeLen;
        keyLoc_[Nb] = pq_.insert(Nb);
        traversed_.set(Nb);
      }
      else
      {
        if(edgeLen < distance_[Nb])
        {
          distance_[Nb] = edgeLen;
          elist_[Nb]    = curop;
          pq_.decreaseKey(keyLoc_[Nb]);
        }
      }

    } while((op = sameEnds_->next(op)) != op1);
  }

  if(!Done)
    return FALSE;

  VIndex v = src_;
  usedVx_.set(v);
  while(v!=dest_)
  {
    OrPath op(elist_[v]);
    opseq.push(op);
    v = wf_->getAtDest(op);
  }
  return TRUE;

}

int WireFrameShortestPather::
compareCB_(Index& v1,Index& v2)
{
 // return (distance_[v1] < distance_[v2]);
  double diff = distance_[v2] - distance_[v1];
  return diff? (diff > 0) : (v1==src_);
}

double WireFrameShortestPather::
length_ (OrPath op)
{
  return 1.0;
}

