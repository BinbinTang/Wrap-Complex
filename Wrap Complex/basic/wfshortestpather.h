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

#ifndef __RGI_WFSHORTESTPATHER_H__
#define __RGI_WFSHORTESTPATHER_H__

#include <basic/wireframe.h>
#include <basic/stackbv.h>
#include <basic/pqueue.h>

class WireFrameShortestPather : public RgiObject
{
public:
  typedef double (RgiObject::*LengthFunc) (OrPath);
  WireFrameShortestPather (IndexWireFrame *wf, WireFrameSameEnds * sameEnds);
  ~WireFrameShortestPather (void);
  Bool findShortestPath (OrPath orgPath, OrPath destPath, IterStack<OrPath>& opseq);
  void setMask (BitVector * mask);
  void setLengthFunc (LengthFunc func, RgiObject * obj);
  void resetLengthFunc (void);
  void reset (void);
  void resetInternal (void);
protected:
  int compareCB_ (Index &v1, Index &v2);
  double length_ (OrPath op);
  IndexWireFrame * wf_;
  WireFrameSameEnds * sameEnds_;
  BitVector * mask_;
  StackBV usedVx_;
  StackBV traversed_;
  Index src_, dest_;
  BinaryHeap<Index> pq_;
  RArray<double> distance_;
  RArray<int>  keyLoc_; 
  RArray<OrPath> elist_;
  LengthFunc lengthFunc_;
  RgiObject * obj_;
};


#endif // __RGI_WFSHORTESTPATHER_H__