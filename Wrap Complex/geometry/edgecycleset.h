#ifndef __RGI_EDGECYCLESET_H__
#define __RGI_EDGECYCLESET_H__

#include <geometry/comp.h>

class OrEdgeInfoCycleIter;
class OrEdgeInfoIter;

enum OrEdgeInfoStatus { IsEdge = 1, // is it edge or just the vertex
                IsManifold = 2, // is manifold edge
                IsUp = 4, // is triangle oriented inside
  };

class OrEdgeInfo
{
private:
  OrTri abc_;
  unsigned char flag_;
public:
  
  OrEdgeInfo (void) : abc_(0, 0), flag_(0) { }
  OrEdgeInfo (OrTri abc, unsigned char flag) : abc_(abc), flag_(flag) { }
  OrEdgeInfo sym (void);
  OrEdgeInfo fnext (Complex * comp);
  Bool isUp (void) { return (flag_ & IsUp); }
  Bool isManifold (void) { return (flag_ & IsManifold); }
  Bool isEdge (void) { return (flag_ & IsEdge); }
  OrTri getOrTri (void) { return abc_; }
};

class OrEdgeInfoSeq 
{
public:
  OrEdgeInfoSeq (VIndex src = 0,VIndex dest = 0);
  VIndex& src (void);
  VIndex& dest (void);
  OrTri&  destAsOrTri (void);
  OrEdgeInfo lastEdge (void);
  void addEdge (OrEdgeInfo, Trist *tr);
  void reset (void);
  IterStack<OrEdgeInfo>& getStack (void);
  
  friend class OrEdgeInfoSeqIter;  
protected:
  VIndex src_,dest_;
  OrTri  destTri_;
  IterStack<OrEdgeInfo> edges_;
};

class OrEdgeInfoSeqList 
{
public:
  OrEdgeInfoSeqList(VIndex,OrTri);
  ~OrEdgeInfoSeqList (void);
  Bool singlePtEdge (void);
  void addSeq(OrEdgeInfoSeq*,OrTri destTri);
  int  numSeq (void);
  void undo (void);
  Bool closed (void);
  VIndex src (void);
  VIndex dest (void);
  OrTri  srcAsOrTri (void);
  OrTri  destAsOrTri (void);
  friend class OrEdgeInfoSeqIter;  
  
protected:
  unsigned char closed_;
  VIndex src_,dest_;
  OrTri  srcTri_;
  IterStack<OrEdgeInfoSeq*> edgeSeq_;
};

inline int OrEdgeInfoSeqList::numSeq (void) { return edgeSeq_.size(); }
inline Bool OrEdgeInfoSeqList::closed (void) { return closed_; }
inline VIndex OrEdgeInfoSeqList::src (void) { return src_; }
inline OrTri OrEdgeInfoSeqList::srcAsOrTri (void) { return srcTri_; }
inline Bool OrEdgeInfoSeqList::singlePtEdge (void) { return (edgeSeq_.size() == 0); }
inline VIndex OrEdgeInfoSeqList::dest (void) { return (singlePtEdge() ? src_ : dest_); }
inline OrTri OrEdgeInfoSeqList::destAsOrTri (void) 
{ return (singlePtEdge() ? srcTri_ : edgeSeq_.top()->destAsOrTri()); }


class OrEdgeInfoCycleSet 
{
public:
  OrEdgeInfoCycleSet (void);
  ~OrEdgeInfoCycleSet (void);
  int  numCycle (void);
  void addSeq (OrEdgeInfoSeq*,OrTri);
  void addVertex (VIndex,OrTri);
  Bool undo (void);
  Bool lastLoopClosed (void);
  Bool lastPathHasEdge (void);
  VIndex lastVertex (void);
  OrTri  lastVertexAsOrTri (void);
  VIndex lastStartingVertex (void);
  OrTri  lastStartingVertexAsOrTri (void);
  
  friend class OrEdgeInfoCycleIter;
protected:
  IterStack<OrEdgeInfoSeqList*> edgeCycle_;
};

inline int OrEdgeInfoCycleSet::numCycle (void) { return edgeCycle_.size(); }


//--------------------------------------------------------------------------------

class OrEdgeInfoCycleIter 
{
public:
  OrEdgeInfoCycleIter(OrEdgeInfoCycleSet*);
  void start();
  void next();
  OrEdgeInfoSeqList* currCycle();
  Bool isDone();
  ~OrEdgeInfoCycleIter();
protected:
  IStackIter<OrEdgeInfoSeqList*> *itr_;
  OrEdgeInfoCycleSet *ecs_;
};

class OrEdgeInfoSeqIter 
{
  
public:
  OrEdgeInfoSeqIter(OrEdgeInfoSeqList*);
  Bool singlePtEdge();
  VIndex src();
  void start();
  int getNextEdge (OrEdgeInfo &e);
  Bool isDone();
  ~OrEdgeInfoSeqIter(); 
protected:
  IStackIter<OrEdgeInfoSeq*> *esItr_;
  IStackIter<OrEdgeInfo>     *eItr_;
  OrEdgeInfoSeqList          *esl_;
  void next_();
  OrEdgeInfo currEdge_();
};


#endif
