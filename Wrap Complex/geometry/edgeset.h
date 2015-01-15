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

#ifndef __RGI_EDGESET_H__
#define __RGI_EDGESET_H__

#include <geometry/comp.h>

class EdgeSet : public TristModifier
{
public:
  EdgeSet (Trist * tr) : 
    TristModifier(tr), bv_(tr->maxTri()*4) { }
  virtual ~EdgeSet (void) { }
  EdgeSet (EdgeSet & other);
  EdgeSet & operator= (EdgeSet & other);

  virtual void set (OrTri ef);
  void clear (OrTri ef);
  Bool test (OrTri ef);
  void reset (void);
  Bool orSets(EdgeSet &set);
  Bool subSets (EdgeSet &set);
	OrTri getNextSet (OrTri ef);

  BitVector& getBitVector (void);
  Bool verify (void);
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
protected:
  BitVector bv_;
};

inline BitVector& EdgeSet::getBitVector (void) { return bv_; }
inline Bool EdgeSet::test (OrTri ef) { return bv_.test(ERep_(ef)); }

class ComplexEdgeSet : public EdgeSet
{
public:
  friend class EdgeSetClassifier;
  ComplexEdgeSet (Complex * comp) : 
    EdgeSet(comp->getTrist()), comp_(comp) { }
  ~ComplexEdgeSet (void) { }
  ComplexEdgeSet (ComplexEdgeSet & other);
  ComplexEdgeSet & operator= (ComplexEdgeSet & other);
  void set (OrTri ef);
  void set (OrTri ef, BitVector &triBV, Bool useSet = TRUE);
  Bool markVertices (BitVector &vertBV);
	Bool clearNonBd (void);
  void setComplex (Complex *comp) { comp_ = comp, tr_ = comp->getTrist(); }
  Complex * getComplex (void) { return comp_; }
  void pack (IMap * map = NULL);
protected:
  Complex * comp_;
};

class BoundaryEdgeSet : public ComplexEdgeSet
{
public:
  enum Degree { Degree_1 = 1, Degree_3_plus = 2, Degree_non_2 = 3};
  BoundaryEdgeSet (Complex * comp) : ComplexEdgeSet (comp) { }
  Bool init (Degree degree, BitVector * triangles);
  Bool markBdCycle (OrTri ef);
};

//-------------------------------------------------------------------------------

class ClassifiedEdgeStack : public OrTriStack
{
public:
  ClassifiedEdgeStack (int size) :OrTriStack(size), flag_(0) { }
  ClassifiedEdgeStack (OrTriStack &s) : OrTriStack(s), flag_(0) { }
  ClassifiedEdgeStack &operator= (ClassifiedEdgeStack &other) 
  { OrTriStack::operator=(other), flag_ = other.flag_; return *this; }
  ClassifiedEdgeStack &operator= (OrTriStack &other) 
  { OrTriStack::operator=(other); return *this; }
  void setClosed (Bool set);
  void setBd (Bool set);
  Bool isClosed (void);
  Bool isBd (void);
private:
  unsigned char flag_;
};

class EdgeSetClassifier : public TristModifier
{
public:
  EdgeSetClassifier (ComplexEdgeSet &eSet, BitVector * markedVert = NULL);
  ~EdgeSetClassifier (void);
  Bool classifyEdges (IterStack<ClassifiedEdgeStack *>& cycleSet);
  Bool getClassifiedEdgeStack (OrTri ef, ClassifiedEdgeStack &stack);
private:
  ComplexEdgeSet& eSet_;
  ComplexEdgeSet visited_;
  BitVector * markedVert_;
  Bool isBd_(OrTri ef);
  Bool sameEdge_ (OrTri ef1, OrTri ef2);
  OrTri getNextEdge_ (OrTri ef, Bool &isBd, Bool &isSel, Bool &openCycle);
  Bool setBdCycle_(OrTri ef);
  Bool setIntCycle_(OrTri ef);
  void createCycle_(ClassifiedEdgeStack &newCycle);

  OrTriStack sf_, sb_;

};

//-------------------------------------------------------------------------------

class ComplexEdgeSetIter : public TristModifier
{
public:
  ComplexEdgeSetIter (ComplexEdgeSet& set);
  ~ComplexEdgeSetIter (void) { }
  void first (void);
  Bool isDone (void);
  void next (void);
  OrTri curItem (void);
protected:
  ComplexEdgeSet& set_;
  Complex * comp_;
  BitVector bv_;
  int lastIndex_;
};

class ComplexEdgeSetLayerMarker : public TristModifier
{
public:
  ComplexEdgeSetLayerMarker (Complex * comp);
  ~ComplexEdgeSetLayerMarker (void);
  Bool mark (ComplexEdgeSet &eSet, int nLayers, BitVector * triBV);
  Bool mark (OrTriStack &eStack, int nLayers, BitVector * triBV);
protected:
  Bool markLayer_(StackBVTag * s1, StackBVTag * s2, BitVector * triBV);
  Bool markFirstLayerOrTri_(OrTri ef, BitVector * triBV);
  Complex *comp_;
  StackBVTag fStar_, F1_, F2_, usedStarF_, usedV_;
};

#endif // __SETS_H__