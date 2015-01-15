/*                         
// Copyright 1996-97. Raindrop Geomagic, Inc.
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


#ifndef __RGI_ANIMATE_H__
#define __RGI_ANIMATE_H__

#include <basic/basic.h>
#include <basic/data.h>
#include <basic/rarray.h>
#include <basic/callbackobject.h>
#include <basic/bitvector.h>
#include <basic/tritype.h>
#include "ortribv.h"
#include "geomutil.h"

class AnimateData : public Data
{
public:
	AnimateData (void) { mode_ = Default; triMask_ = NULL; edgeMask_ = NULL;
					vertMask_ = NULL; message_[0] = '\0'; }
	~AnimateData (void) { }

	void addTriBV (BitVector * bv);
	void addEdgeBV (OrEdgeBV * bv);
	void addVertBV (BitVector * bv);

	IterStack<BitVector *> * getTriBVs (void) { return &markedTriSets_; }
	IterStack<OrEdgeBV *> * getEdgeBVs (void) { return &markedEdgeSets_; }
	IterStack<BitVector *> * getVertBVs (void) { return &markedVertSets_; }

	void setMode (AnimateMode mode)    { mode_ = mode; }
	AnimateMode getMode (void)          { return mode_; }

	void setMessage (char * message);
	char * getMessage (void) { return message_; }

	void resetBVs (void);	 // resets BVs only
	void reset (void);			// resets BVs and masks

	void setTriMask (BitVector * mask)  { triMask_ = mask;  }
	void setEdgeMask (OrEdgeBV * mask)  { edgeMask_ = mask;  }
	void setVertMask (BitVector * mask)  { vertMask_ = mask;  }
	BitVector * getTriMask (void)  { return triMask_; }
	OrEdgeBV * getEdgeMask (void)  { return edgeMask_; }
	BitVector * getVertMask (void)  { return vertMask_; }

	friend ostream& operator<< (ostream & s, AnimateData & data);

private:
	AnimateMode mode_;
	IterStack<BitVector *> markedTriSets_, markedVertSets_;
	IterStack<OrEdgeBV *> markedEdgeSets_;
	BitVector *triMask_, *vertMask_;
	OrEdgeBV *edgeMask_;
	char message_[100];
};

class GeomAnimator : public CallbackObject
{
public:
	GeomAnimator () {}
	~GeomAnimator (void) {}

	enum { AnimateCB };

	void addTriBV (BitVector * bv);
	void addEdgeBV (OrEdgeBV * bv);
	void addVertBV (BitVector * bv);
	void setMessage (char * message);
	void beginAnimate (void);
  Data * animate (AnimateMode mode = Default);
  void endAnimate (void);
	
private:
	AnimateData data_;
};


// A global object that should be used for algorithm animation.
extern GeomAnimator theGeomAnimator;

#endif   // __ANIMATE_H__
