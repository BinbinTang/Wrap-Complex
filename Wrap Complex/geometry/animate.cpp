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


#include "animate.h"

GeomAnimator theGeomAnimator;


void AnimateData::
addTriBV (BitVector * bv)
{
	markedTriSets_.push (bv);
}

void AnimateData::
addEdgeBV (OrEdgeBV * bv)
{
	markedEdgeSets_.push (bv);
}

void AnimateData::
addVertBV (BitVector * bv)
{
	markedVertSets_.push (bv);
}

void AnimateData::
setMessage (char * message)
{
	//sprintf (message_, "%s", message);
	sprintf_s(message, 100, "%s", message);
}

void AnimateData::
resetBVs (void)
{
	markedTriSets_.reset ();
	markedEdgeSets_.reset ();
	markedVertSets_.reset ();
}

void AnimateData::
reset (void)
{
	resetBVs ();
	triMask_ = NULL;
	edgeMask_ = NULL;
	vertMask_ = NULL;
}


ostream& 
operator<< (ostream & s, AnimateData & data)
{
	int i;

	s << "\nAnimateData:" << endl << endl;
	s << "Mode = ";
	switch (data.getMode ())
	{
	case (Default):
		s << "Default";
		break;
	case (Animate):
		s << "Animate";
		break;
	case (Pause):
		s << "Pause";
		break;
	default:
		break;
	}
	s << endl;
	s << "Message: " << data.getMessage () << endl;

	s << "Number of triangle BVs:  " << data.markedTriSets_.size () << endl;
	for (i = 1; i <= data.markedTriSets_.size (); i++)
		s << "\nBitVector " << i << ": " << endl << *data.markedTriSets_[i];

	s << "Number of edge BVs:  " << data.markedEdgeSets_.size () << endl;
	for (i = 1; i <= data.markedEdgeSets_.size (); i++)
		s << "\nBitVector " << i << ": " << endl << *data.markedEdgeSets_[i];

	s << "Number of vertex BVs:  " << data.markedVertSets_.size ();
	for (i = 1; i <= data.markedVertSets_.size (); i++)
		s << "\nBitVector " << i << ": " << endl << *data.markedVertSets_[i];

	return s;
}

//------------------------------------------------------------------------------------------------------

void GeomAnimator::
addTriBV (BitVector * bv)
{
	data_.addTriBV (bv);
}

void GeomAnimator::
addEdgeBV (OrEdgeBV * bv)
{
	data_.addEdgeBV (bv);
}

void GeomAnimator::
addVertBV (BitVector * bv)
{
	data_.addVertBV (bv);
}

void GeomAnimator::
setMessage (char * message)
{
	data_.setMessage (message);
}

void GeomAnimator::
beginAnimate (void)
{
  data_.setMode (BeginAnimate);
	callCallbacks (AnimateCB, &data_);
	data_.reset ();
}

void GeomAnimator::
endAnimate (void)
{
  data_.setMode (EndAnimate);
	callCallbacks (AnimateCB, &data_);
	data_.reset ();
}

Data * GeomAnimator::
animate (AnimateMode mode)
{
	data_.setMode (mode);
	callCallbacks (AnimateCB, &data_);
	data_.reset ();
	return NULL;
}
