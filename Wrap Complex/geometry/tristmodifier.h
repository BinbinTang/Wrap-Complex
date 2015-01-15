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

//  NOTE the use of '#ifdef _DYNAMIC_' for dynamic triangulation --Damrong

#ifndef __RGI_TRISTMODIFIER_H__
#define __RGI_TRISTMODIFIER_H__

#include <basic/callbackobject.h>
#include <geometry/trist.h>

#ifdef _DYNAMIC_
class SimplexSet;
class TristModifier;
typedef void (TristModifier::* TristModCB) (SimplexSet * s);
#endif

class Simplex;
class Ksimplex;
class CompDB;

/************************************************************************
 *                                                                      *
 *   class TristModifier - base class for all Trist manipulators.       *
 *                                                                      *
 *                                                                      *
 ************************************************************************/

class TristModifier : public ProgressCallbackObject
{
public:
  TristModifier (void) : tr_(NULL) { }
  TristModifier (Trist * tr) : tr_(tr) { }
  Trist * getTrist (void) { return tr_; }
  Trist * cloneTrist (void) { return tr_? new Trist(*tr_) : NULL; }
  inline int enumVertices (BitVector * triangles = NULL, 
	                OrTriCB cbFunc = NULL, RgiObject * obj = NULL);
  inline int enumEdges (BitVector * triangles = NULL, 
	                OrTriCB cbFunc = NULL, RgiObject * obj = NULL);
  inline int enumTriangles (BitVector * triangles = NULL, 
	                OrTriCB cbFunc = NULL, RgiObject * obj = NULL);
  inline int enumTetra (OrTriCB cbFunc = NULL, RgiObject * obj = NULL);

#ifdef _DYNAMIC_
  enum                ModType { Add, Del, Upd, MaxModType };
  inline void   addTristModCB (ModType mode, TristModCB f, TristModifier * obj);
  inline void  callTristModCB (ModType mode, SimplexSet * s);
  inline void resetTristModCB (void);
  inline Bool  haveTristModCB (ModType mode);
private:
  class ModCallBack 					// nested class
  { 
      public:  TristModifier * obj;   TristModCB f; 
  };							
  IterStack <ModCallBack> cbs_ [MaxModType];
#endif

protected:
  VIndex Org_(OrTri ef) { return tr_->org(ef); }
  VIndex Dest_(OrTri ef) { return tr_->dest(ef); }
  OrTri Enext_(OrTri ef) { return ef.enext(); }
  OrTri Enext2_(OrTri ef) { return ef.enext2(); }
  OrTri Sym_(OrTri ef) { return ef.sym();  }
  OrTri Fnext_(OrTri ef) { return tr_->fnext(ef); }
  OrTri Orient_ (OrTri ef, VIndex a) { return tr_->orient(ef, a); }
  OrTri Orient_ (FIndex f, VIndex a) { return tr_->orient(f, a); }
	OrTri FnextBV_ (OrTri ef, BitVector& bv, Bool rev = FALSE, int useIntRep = FALSE)
	  { return tr_->fnextBV (ef, bv, rev, useIntRep); }
	void Flip_ (FIndex f) { tr_->flip(f); }
	void Flip_ (OrTri ef) { tr_->flip(ef); }
  void MatchOrg_ (OrTri ef1, OrTri ef2) { Assert(Org_(ef1)==Org_(ef2)); }
  void MatchDest_ (OrTri ef1, OrTri ef2) { Assert(Dest_(ef1)==Dest_(ef2)); }
  void MatchEdge_ (OrTri ef1, OrTri ef2) { MatchOrg_(ef1, ef2); MatchDest_(ef1, ef2); }
  void Fsplice_(OrTri ef1, OrTri ef2) { MatchEdge_(ef1, ef2); tr_->fsplice(ef1, ef2); }
  void Fmerge_(OrTri ef1, OrTri ef2) { MatchEdge_(ef1, ef2); tr_->fmerge(ef1, ef2); }
  Bool Fvalid_ (OrTri ef) { return !tr_->deleted(ef.index()); }
  Bool Fvalid_ (FIndex f) { return !tr_->deleted(f); }
  void DelTri_ (OrTri ef) { tr_->delTri(ef.index()); }
  void DelTri_ (FIndex f) { tr_->delTri(f); }
	void DelTri_ (BitVector& bv) { tr_->delTri(bv); }
	void DelTriReset_ (BitVector& bv) { tr_->delTriReset(bv); }
  Bool outside_ (OrTri ef) { return !(ef.getIntRep() & 1); }
  OrTri MakeTri_ (VIndex a, VIndex b, VIndex c, int version = 0) 
	 { return tr_->makeTri(a, b, c, version); }
  void Triangle_ (OrTri ef, VIndex& a, VIndex& b, VIndex& c)
     { tr_->triangle(ef, a, b, c); }
  // edge representation (for marking)
  int ERep_ (OrTri ef) { return (ef.getIntRep() >> 1) ; }
  // triangle side representation (for marking)
  int FRep_ (OrTri ef) { return ((ef.index() << 1) | (ef.getIntRep() & 1)); }
  // Convert an ERep to an OrTri
  OrTri OrTriFromERep_(int eRep) { return OrTri((eRep >> 2), (eRep << 1) & 0x07); }
  // Convert an FRep to an OrTri
  OrTri OrTriFromFRep_(int fRep) { return OrTri((fRep >> 1), fRep & 0x01); }
protected:
  Trist * tr_;
};

// inline functions

int TristModifier::
enumVertices (BitVector * triangles, OrTriCB cbFunc, RgiObject * obj)
{ if (!obj) obj = this;
  return tr_->enumVertices(triangles, cbFunc, obj); }

int TristModifier::
enumEdges (BitVector * triangles, OrTriCB cbFunc, RgiObject * obj)
{ if (!obj) obj = this;
  return tr_->enumEdges(triangles, cbFunc, obj); }

int TristModifier::
enumTriangles (BitVector * triangles, OrTriCB cbFunc, RgiObject * obj)
{ if (!obj) obj = this;
  return tr_->enumTriangles(triangles, cbFunc, obj); }

int TristModifier::
enumTetra (OrTriCB cbFunc, RgiObject * obj)
{ if (!obj) obj = this;
  return tr_->enumTetra(cbFunc, obj); }

#ifdef _DYNAMIC_
void TristModifier::
addTristModCB (ModType mode, TristModCB f, TristModifier * obj)
{
  ModCallBack m;  m.obj = obj;  m.f = f;
  cbs_ [mode].push (m); 
}
void TristModifier::
callTristModCB (ModType mode, SimplexSet * s)
{
  IStackIter <ModCallBack> it (cbs_ [mode]);
  for (it.first (); ! it.isDone (); it.next ())
  {
      ModCallBack & m = it.curItem ();
      ((m.obj)->*(m.f)) (s);
  }
}
void TristModifier::
resetTristModCB (void)
{
  cbs_[Add].reset ();
  cbs_[Del].reset ();
  cbs_[Upd].reset ();
}
Bool TristModifier::
haveTristModCB (ModType mode)
{
  return ! (cbs_[mode].empty ());
}
#endif		// _DYNAMIC_

/************************************************************************
 *                                                                      *
 *   class TristModifierAdmin - base class for all direct               *
 *   Trist manipulators, should be used only by advanced trist hackers. *
 *                                                                      *
 *                                                                      *
 ************************************************************************/


class TristModifierAdmin : public TristModifier
{
protected:
 TristModifierAdmin (Trist * tr)
  : TristModifier(tr), 
    triArr_(tr->triArr_), 
	exists_(tr->getTriangleBV()) { }
protected:
  FreeArray<Trist::TrImp>& triArr_; 
  BitVector& exists_;
  static const int * vo;
  static const int * ve;
};


#endif // __TRISTMODIFIER_H__
