// segmenttree.cpp
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

/* Libraries */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "boxes.h"
#include "segmenttree.h"

/* Defaults */
static int gDefaultCutoffs[] = {1500, 5000};

/* Private Globals */
static int gCutoffs[kNumDim];
static boxT gNegInfBox, gPosInfBox;
static Tolerancer *gEvaluator = NULL;
static BoxesIntersectCB gCallbackFn = NULL;

/* Private Functions */
static int SegmentTree(boxT *li, boxT *ri, 
		       boxT *lp, boxT *rp, 
		       boxT ls, boxT rs, int d, BOOL inOrder);
static int OneWaySegmentTree(boxT *li, boxT *ri, 
			     boxT *lp, boxT *rp, 
			     boxT ls, boxT rs, int d);
static boxT *SplitPoints(boxT *lp, boxT *rp, int d, boxT *ms);
static void SplitIntervalsLeft(boxT *li, boxT *ri, 
			       boxT *ls, boxT *ms, boxT *rs,
			       int d, boxT **lli, boxT **lci);
static void SplitIntervalsRight(boxT *lli, 
				boxT *lci, boxT *ms, 
				int d, boxT **lloi);
static boxT *MedianOfThree(boxT *a1, boxT *a2, boxT *a3, int d);
static int CountUsingScanAlg(boxT *li, boxT *ri, 
			     boxT *lp, boxT *rp, 
			     int lastDim, BOOL inOrder);
static int CountUsingTwoWayScanAlg(boxT *li, boxT *ri, 
				   boxT *lp, boxT *rp, 
				   int lastDim, BOOL inOrder);

/* Implementation of Exported Function */

int CountUsingSegmentTreeAlg(boxesT *points, boxesT *intervals, 
														 int cutoffs[2], Tolerancer *evaluator, 
														 BoxesIntersectCB callback,
														 BOOL isOneWay)
{
  int numPairs;

	/* Setting module private globals */
  gCallbackFn = callback;
	gEvaluator = evaluator;
  if(cutoffs == NULL) {
    gCutoffs[2] = gDefaultCutoffs[0];
    gCutoffs[1] = gDefaultCutoffs[1];
  } else {
    gCutoffs[2] = cutoffs[0]; /* first level is at dim 2*/
    gCutoffs[1] = cutoffs[1]; 
  }
  gNegInfBox = *(intervals->boxes);
  gPosInfBox = *(intervals->boxes + intervals->numBoxes - 1);
  if(isOneWay) {
    numPairs = OneWaySegmentTree(intervals->boxes + 1, 
		intervals->boxes + intervals->numBoxes -2, 
		points->boxes + 1, 
		points->boxes + points->numBoxes - 2, 
		gNegInfBox, gPosInfBox,
		kNumDim-1); 
  } else {
    numPairs = 
      SegmentTree(intervals->boxes + 1, 
				intervals->boxes + intervals->numBoxes - 2, 
				points->boxes + 1, 
				points->boxes + points->numBoxes-2, 
				gNegInfBox, gPosInfBox,
				kNumDim-1, TRUE)
      + SegmentTree(points->boxes + 1, 
					points->boxes + points->numBoxes-1, 
					intervals->boxes + 1, 
					intervals->boxes + intervals->numBoxes - 2, 
					gNegInfBox, gPosInfBox,
					kNumDim-1, FALSE); 
  }
  return numPairs;
}

/* Implementation of Private Functions */

static int SegmentTree(boxT *li, boxT *ri, 
		       boxT *lp, boxT *rp, 
		       boxT ls, boxT rs, int d, BOOL inOrder)
{
  boxT *mp, ms, *lloi, *lli, *lci;
  int pairs;

  pairs = 0;
  
  /* Base Cases */
  if(li > ri || lp > rp) return 0;        /* Empty Intervals OR Points */
  if(d == 0) {
    return CountUsingScanAlg(li, ri, lp, rp, d, inOrder);
  }
  if(IsLoLessLo(&rs, &ls, d)) return 0;     /* Empty Segment */  
  if((rp - lp + 1) <= 2 || ri-li <= gCutoffs[d] || rp-lp <= gCutoffs[d]) {
    return CountUsingTwoWayScanAlg(li, ri, lp, rp, d, inOrder);
  }

  /* Split segment and sets */
  mp = SplitPoints(lp, rp, d, &ms);
  SplitIntervalsLeft(li, ri, &ls, &ms, &rs, d, &lli, &lci);

  /* Recurse left and right */
  pairs += SegmentTree(lli, lci-1, lp, mp-1, ls, ms, d, inOrder);
  SplitIntervalsRight(lli, lci, &ms, d, &lloi);
  pairs += SegmentTree(li, lloi-1, mp, rp, ms, rs, d, inOrder);

  /* Recurse for Ispan, points with roles exchanged */
  pairs += 
    SegmentTree(lci, ri, lp, rp, gNegInfBox, gPosInfBox, d-1, inOrder);
  pairs += 
    SegmentTree(lp, rp, lci, ri, gNegInfBox, gPosInfBox, d-1, !inOrder);
  return pairs;
}

static int OneWaySegmentTree(boxT *li, boxT *ri, 
			     boxT *lp, boxT *rp, 
			     boxT ls, boxT rs, int d)
{
  boxT *mp, ms, *lloi, *lli, *lci;
  int pairs;

  pairs = 0;
  
  /* Base Cases */
  if(li > ri || lp > rp) return 0;        /* Empty Intervals OR Points */
  if(d == 0) {
    return CountUsingScanAlg(li, ri, lp, rp, d, TRUE);
  }
  if(IsLoLessLo(&rs, &ls, d)) return 0;     /* Empty Segment */  
  if((rp - lp + 1) <= 2 || ri-li <= gCutoffs[d] || rp-lp <= gCutoffs[d]) {
    return CountUsingScanAlg(li, ri, lp, rp, d, TRUE);
  }

  /* Split segment and sets */
  mp = SplitPoints(lp, rp, d, &ms);
  SplitIntervalsLeft(li, ri, &ls, &ms, &rs, d, &lli, &lci);
  
  /* Recurse left and right */
  pairs += OneWaySegmentTree(lli, lci-1, lp, mp-1, ls, ms, d);
  SplitIntervalsRight(lli, lci, &ms, d, &lloi);
  pairs += OneWaySegmentTree(li, lloi-1, mp, rp, ms, rs, d);
  
    /* Recurse for Ispan no exchanging of roles */
  pairs += OneWaySegmentTree(lci, ri, lp, rp, gNegInfBox, gPosInfBox, d-1);
  return pairs;
}

/* Split without keeping order! */
/* simple partition algorithm like CLR's */
/* split into [lp..l-1] and [l..rp].*/
static boxT *SplitPoints(boxT *lp, boxT *rp, int d, boxT *ms)
{
  boxT *random;
  boxT *mid;
  boxT temp;
  boxT *l, *r;
  
  random = lp + 1 + rand() % (rp - lp - 1); /* leave space for 3 */
  mid = MedianOfThree(random-1, random, random+1, d);
  *ms = *mid;

  /* Partition in place */
  l = lp - 1;
  r = rp + 1;
  while(TRUE) {
    do { r--; } while(IsLoLessLo(ms, r, d));
    do { l++; } while(IsLoLessLo(l, ms, d));
    if(l < r) {
      temp = *l;
      *l = *r;
      *r = temp;
    } else {
      return r+1;
    }
  }
}

/* SplitIntervalsLeft
 *
 * first case:  lo of interval is higher than middle segment, so interval
 * could only intersect in the right segment (doesn't intersect left);
 * second case: means that it doesn't contain segment.  therefore, it
 * must intersect left.
 * third case:  contains (otherwise.)
 * fill in third from back!
 */ 
static void SplitIntervalsLeft(boxT *li, boxT *ri, 
			       boxT *ls, boxT *ms, boxT *rs,
			       int d, boxT **lli, boxT **lci) 
{
  /* intervals, left interval point, left temp pointer, right temp pointer */
  boxT *l, *r, temp;

  /* first find out all the intervals that only intersect right segment */
  l = li - 1;
  r = ri + 1;
  while(TRUE) {
    do { r--; } while(l < r && IsLoLessLo(r, ms, d));
    do { l++; } while(l < r && !IsLoLessLo(l, ms, d));
    if(l < r) {
      temp = *l;
      *l = *r;
      *r = temp;
    } else {
      *lli = r+1;
      break;
    }
  }

  /* Now separate the intervals that contain the segment from others */
  l = r;
  r = ri + 1;

  while(TRUE) {
    do { r--; } while(l < r && 
		      !(IsLoLessLo(ls, r, d) || IsHiLessLo(r, rs, d)));
    do { l++; } while(l < r && 
		      (IsLoLessLo(ls, l, d) || IsHiLessLo(l, rs, d)));
    if(l < r) {
      temp = *l;
      *l = *r;
      *r = temp;
    } else {
      *lci = r+1;
      break;
    }
  }
}

static void SplitIntervalsRight(boxT *lli, boxT *lci, boxT *ms, 
				int d, boxT **lloi)
{
  boxT *l, *r, temp;

  l = lli - 1;
  r = lci;  /* lci IS the next one */
  while(TRUE) {
    do { r--; } while(l < r && IsHiLessLo(r, ms, d));
    do { l++; } while(l < r && !IsHiLessLo(l, ms, d));
    if(l < r) {
      temp = *l;
      *l = *r;
      *r = temp;
    } else {
      *lloi = r+1;
      break;
    }
  }
}

static boxT *MedianOfThree(boxT *a1, boxT *a2, boxT *a3, int d)
{
  boxT *temp;

  if(IsLoLessLo(a3, a1, d)) {           
    temp = a1;
    a1 = a3;    
    a3 = temp;
  }
  if(IsLoLessLo(a2, a1, d)) return a1;       
  if(IsLoLessLo(a2, a3, d)) return a2;  
  return a3;
}

static int CountUsingScanAlg(boxT *li, boxT *ri, boxT *lp, boxT *rp, 
			     int lastDim, BOOL inOrder)
{
  int d;
  boxT *p;
  int pairs;
  
  pairs = 0;

  qsort((void *)li, ri-li+1, sizeof(boxT), BoxCompare);
  qsort((void *)lp, rp-lp+1, sizeof(boxT), BoxCompare);

	for(; li <= ri; li++) {
		for(; lp <= rp && !IsLoLessLo(li, lp, 0); lp++);
		for(p = lp; p <= rp &&  IsLoLessHi(p, li, 0); p++) {
			for(d = 1; d <= lastDim; d++) {
				if(!Intersects(li, p, d)) break;
      }
      if(d > lastDim) {
				if(inOrder) {
					(gEvaluator->*gCallbackFn)(p->num, li->num);
				} else {
					(gEvaluator->*gCallbackFn)(li->num, p->num);
				}
			pairs++;
			}
		}
	}
  return pairs;
}

static int CountUsingTwoWayScanAlg(boxT *li, boxT *ri, 
				   boxT *lp, boxT *rp, 
				   int lastDim, BOOL inOrder)
{
  int d;
  boxT *p, *i;
  boxT *interval, *point;
  int pairs;
  
  pairs = 0;

  qsort((void *)li, ri-li+1, sizeof(boxT), BoxCompare);
  qsort((void *)lp, rp-lp+1, sizeof(boxT), BoxCompare);

  while(li <= ri && lp <= rp) {
		if(IsLoLessLo(li, lp, 0)) {
			interval = li++;
      for(p = lp; p <= rp &&  IsLoLessHi(p, interval, 0); p++) {
				for(d = 1; d < lastDim; d++) {
					if(!Intersects(interval, p, d)) break;
				}
				if(d == lastDim && ContainsLoPoint(interval, p, d)){ 
					if(inOrder) {
						(gEvaluator->*gCallbackFn)(p->num, interval->num);
					} else {
						(gEvaluator->*gCallbackFn)(interval->num, p->num);
					}
				}
			pairs++;
			}
		} else {
      point = lp++;
      for(i = li; i <= ri && IsLoLessHi(i, point, 0); i++) {
				for(d = 1; d < lastDim; d++) {
					if(!Intersects(i, point, d)) break;
				}
				if(d == lastDim && ContainsLoPoint(i, point, d)) {	
					if(inOrder) {
						(gEvaluator->*gCallbackFn)(point->num, i->num);
					} else {
						(gEvaluator->*gCallbackFn)(i->num, point->num);
					}
				pairs++;
				}
      }
    }
  }
  return pairs;
}
