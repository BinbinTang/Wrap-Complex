//
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
//
#ifndef __RGI_WIREFRAME_H__
#define __RGI_WIREFRAME_H__

#include <basic/orindex.h>
// Internal wireframe structures

class HeadTail
{
public:
  int head;
  int tail;
  HeadTail (void) : head(0), tail(0) { }
  inline void binDump (BinOutput& bo);
  inline Bool binInit (BinInput& bi);
};

template <class T>
class WireFrameNode
{
public:
  T data;
  Index next;
  Index prev;
  WireFrameNode (void) : next(0), prev(0) { }
  void binDump (BinOutput& bo);
  Bool binInit (BinInput& bi);
};

// Generic wireframe

template <class T>
class WireFrame : public RgiObject
{
public:
  WireFrame (int nOfPaths = 20, int nOfEdges = 20);
  WireFrame (const WireFrame<T> &other);
  ~WireFrame (void);
  const WireFrame<T> &operator=(const WireFrame<T> &other);
  Bool isEmpty (void);
  int nOfEdges (void);
  int nOfPaths (void);
  Index addPath (void);
  Index addSimplePath (T& data1, T& data2);
  Bool removePath (Index pathId);
  int pathSize (Index pathId);
  inline Index getHeadPos (Index pathId);
  inline Index getTailPos (Index pathId);
  inline Bool isValidPos (Index pos);
  inline Bool isValidPath (Index pathId);
  inline T& getAt(Index pos);
  inline T& getHead (Index pathId);
  inline T& getTail (Index pathId);
  inline Index getNextPos (Index pos);
  inline Index getPrevPos (Index pos);
  Index getPath (Index pos);
  Index getNextPath (Index pathId);
  Index addHead(Index pathId, T& data);
  Index addTail(Index pathId, T& data);
  Index insertAfter(Index pos, T& data);
  Index insertBefore(Index pos, T& data);
  // oriented versions
  inline OrPos addOrg(OrPath orPath, T& data);
  inline OrPos addDest(OrPath orPath, T& data);
  inline OrPos insertAfter(OrPos orPos, T& data);
  inline OrPos insertBefore(OrPos orPos, T& data);
  void removePos (Index pos);
  inline void removePos (OrPos pos);
  BitVector& getPosBV (void);
  BitVector& getPathBV (void);
  void binDump (BinOutput& bo);
  Bool binInit (BinInput& bi);
  // oriented functions
  inline T& getAt (OrPos orPos);
  inline OrPos enext(OrPos orPos);
  inline OrPos org(OrPath orPath);
  inline OrPos dest(OrPath orPath);
  inline T& getAtOrg(OrPath orPath);
  inline T& getAtDest(OrPath orPath);
  inline OrPath getPath (OrPos orPos);
  void reset (void);
  IMap * packPositions (void);
  IMap * packPaths (void);

	Bool reverseDir (Index pathId);

protected:
  FreeArrayDumpable<HeadTail> parr_;
  FreeArrayDumpable< WireFrameNode<T> > arr_;
};

// iterator over wireframe

template <class T>
class WireFrameIter
{
public:
  WireFrameIter (WireFrame<T>& wf, BitVector * pathBV = NULL);
  ~WireFrameIter (void);
  inline void first (void);
  inline Bool isDone (void);
  void next (void);
  void nextPath (void);
  inline Index curPos (void); // returns current position
  inline Index curPath (void); // returns current path id
  inline T& curItem (void); // returns current element
private:
  WireFrame<T>& wf_;
  Index curPath_;
  Index curPos_;
  BitVector * pathBV_;
};

class IndexWireFrame : public WireFrame<IntDumpable>
{
public:
  IndexWireFrame (int nOfPaths = 20, int nOfEdges = 20);
  IndexWireFrame (const IndexWireFrame &other);
  const IndexWireFrame &operator=(const IndexWireFrame &other);
  ~IndexWireFrame (void);
  void binDump (BinOutput& bo);
  Bool binInit (BinInput& bi);
  Bool splitPath (Index pathId, Index pos, Index &pathId1, Index &pathId2);
  Bool mergePaths (Index pathId1, Index pathId2, Index &pathId);
private:
  Magic magic_;
};

class WireFrameSameEnds : public RgiObject
{
public:
  WireFrameSameEnds (int size = 20);
  WireFrameSameEnds (WireFrameSameEnds& other);
  ~WireFrameSameEnds (void);
  // navigation
  inline OrPath next (OrPath op);
  OrPath prev (OrPath op);
  Bool same (OrPath op1, OrPath op2);
  Bool getStar (OrPath op, IterStack<OrPath>& ops);
  Bool markStar (OrPath op, BitVector& bv, Bool mark); // bv uses integer representation of paths
  Bool markPathStar (OrPath op, BitVector& bv, Bool mark); // bv uses indices of paths
  Bool enumerateStar (OrPath path, OrPathCB cbFunc, RgiObject * obj);
  int getDegree (OrPath op);
  // modifications and setup
  void reset (void);
  Bool add (OrPath op);
  Bool del (OrPath op);
  inline Bool addPath (int pathId);
  inline Bool delPath (int pathId);
  Bool separate (OrPath op);
  Bool merge (OrPath op1, OrPath op2, Bool versionOrient = FALSE);
  Bool separateRing (OrPath op1, OrPath op2, Bool versionOrient = FALSE);
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
	inline int maxSize (void);
protected:
  Magic magic_;
  RArrayDumpable<IntDumpable> arr_;
};

#include <basic/wireframe.hpp>
#endif // __WIREFRAME_H__
