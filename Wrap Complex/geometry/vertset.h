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

#ifndef __RGI_VERTSET_H__
#define __RGI_VERTSET_H__

#include <geometry/comp.h>

class ComplexEdgeSet;

class ComplexVertexSet : public TristModifier
{
public:
  ComplexVertexSet (Complex *comp);
  ComplexVertexSet (ComplexVertexSet &other);
  ComplexVertexSet & operator= (ComplexVertexSet & other);
  ComplexVertexSet (void) { }
  void setComplex (Complex *comp);
  Complex * getComplex (void);
  void set (OrTri ef, Bool onOff = TRUE);
  void clear (OrTri ef);
  void reset (void);
  Bool test (OrTri ef);
  Bool test (VIndex v);
  Bool getMarkedOrTri (VIndex v, OrTri& ef);
  void pack (IMap * map = NULL);
  BitVector &getVertexBV (void);
  int getRep (OrTri ef);
  OrTri getOrTri (int rep);
  void invalidate (void); // in case something was changed externally
  void validate (void);

  // low level maintaining
  void setI (OrTri ef, Bool onOff = TRUE);
  void clearI (OrTri ef);
  BitVector &getVertexBVI (void);
	Bool binInit(BinInput &bi);
	void binDump(BinOutput &bo);
protected:
	Magic magic_;
  void recomputeVRep_ (void);
  Complex * comp_;
  Bool vValid_;
  BitVector vRep_, efRep_;
  StackBV tmp_;
};


inline Complex * ComplexVertexSet::getComplex (void) 
{ return comp_; }
inline int ComplexVertexSet::getRep (OrTri ef)
{ return ef.index()*3 + (((ef.version()+1)%6) >> 1); }
inline OrTri ComplexVertexSet::getOrTri (int rep)
{ return OrTri(rep/3, (rep%3) << 1); }
inline void ComplexVertexSet::clear (OrTri ef) { set(ef, FALSE); }


inline void ComplexVertexSet::setI (OrTri ef, Bool onOff) { efRep_.set(getRep(ef), onOff); }
inline void ComplexVertexSet::clearI (OrTri ef) { setI(ef, FALSE); }
inline BitVector &ComplexVertexSet::getVertexBVI (void) { return vRep_; }


// this iterator knows only about vertices, can't get back OrTri information,
// which is OK for plain iteration.
class ComplexVertexSetIter  : public BitVectorIter
{
public:
  ComplexVertexSetIter (ComplexVertexSet & vSet)
    : BitVectorIter (vSet.getVertexBV()) { }
  ~ComplexVertexSetIter (void) { }
};

#endif // __RGI_VERTSET_H__
