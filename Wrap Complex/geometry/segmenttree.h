// segmenttree.h
// Afra Zomorodian
// January 13, 1999
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

#ifndef __RGI_SEGMENTTREE_H__
#define __RGI_SEGMENTTREE_H__

#include <geometry/tolerancer.h>
#include <geometry/boxes.h>

/* Callback Function type */
typedef void (Tolerancer::*BoxesIntersectCB)(int pointNum, int intervalNum);

// Function:  CountUsingSegmentTreeAlg
//
// Given boxes 'points' and 'intervals', will compute the
// number of intersections between the boxes (bipartite)
// and return them.  For any intersection, calls the callback
// method of the evaluator class.  The cutoffs may be set to
// NULL for default cutoffs to be used (cutoffs determine when
// segment tree using scanning algorithm.)  The boolean 
// oneWay finds all intersection of the boxes 'points' considered
// as real points with the boxes intervals.  (For more information
// on this, look at how Segment Tree works in general.)

int CountUsingSegmentTreeAlg(boxesT *points, boxesT *intervals, 
														 int cutoffs[2], Tolerancer *evaluator, 
														 BoxesIntersectCB callback,
														 BOOL isOneWay);

#endif /* _SEGMENTTREE_H_ */
