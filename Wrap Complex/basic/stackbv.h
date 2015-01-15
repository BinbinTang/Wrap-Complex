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

#ifndef __RGI_STACKBV_H__
#define __RGI_STACKBV_H__

#include <basic/dumpable.h>
/*****************************************************************
 *
 *
 *         class StackBV 
 *
 */

class StackBV
{
public:
  StackBV (void);
  StackBV (StackBV &other);
  virtual ~StackBV (void);
  
  StackBV& operator= (StackBV& other);
  Bool test (int i);
	Bool set (int i);
	Bool clear (int i);
	Bool push (int i);
  Bool set (int i, Bool onOff);
	int pop (void);
	int top (void);
	Bool empty (void);
	void reset (void);
  void sort (void);
  void removeDuplicates(void);
  void remap(IMap& map);
  void merge (StackBV& other);
  IndexStack &getStack (void);
  BitVector& getBitVector (void);
  int size (void);
protected:
  StackBV (Bool noAlloc, BitVector *bv); // no allocation for bitvector
  StackBV (Bool noAlloc, BitVector *bv, IndexStack &s); // no allocation for bitvector
  BitVector * bv_;
  IndexStack s_;
};

// inline functions
inline StackBV::StackBV(void) : bv_(new BitVector()), s_(16) { }
inline StackBV::StackBV(Bool noAlloc, BitVector *bv) : bv_(bv), s_(16) { }
inline StackBV::StackBV(Bool noAlloc, BitVector *bv, IndexStack &s) : bv_(bv), s_(s) { }
inline Bool StackBV::test (int i) { return bv_->test(i); }
inline Bool StackBV::set (int i) { if (!test(i)) { bv_->set(i); s_.push(i); } return TRUE; }
inline Bool StackBV::clear (int i) { return bv_->clear(i); }
inline Bool StackBV::push (int i) {  return set(i); }
inline Bool StackBV::set (int i, Bool onOff) { return onOff? set(i) : clear(i); }
inline Bool StackBV::empty (void) { return (bv_->getNumSet() == 0)? (s_.reset(), TRUE) : FALSE; }
inline IndexStack &StackBV::getStack (void) { return s_; }
inline int StackBV::size (void) { int n = bv_->getNumSet(); return n? n : (s_.reset(), 0); }
inline BitVector& StackBV::getBitVector (void) { return *bv_; }
inline void StackBV::reset (void) { while(!s_.empty()) {int i = s_.pop(); bv_->clear(i); } }

/*****************************************************************
 *
 *
 *         class StackBVTagIter
 *
 */

class StackBVIter
{
public:
  StackBVIter (StackBV& stag);
  ~StackBVIter (void) { }
  void first (void);
  void next (void);
  Bool isDone (void);
  int curItem (void);
  int curIndex (void);
private:
  StackBV &sbv_;
  int curIndex_;
};

inline Bool StackBVIter::isDone (void) { return curIndex_ > sbv_.getStack().size(); }
inline int StackBVIter::curItem (void) { return sbv_.getStack()[curIndex_]; }
inline int StackBVIter::curIndex (void) { return curIndex_; }

#endif // __RGI_STACKBV_H__