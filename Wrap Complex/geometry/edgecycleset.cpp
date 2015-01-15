#include "edgecycleset.h"
#include "trist.h"
#include <basic/util.h>


OrEdgeInfo OrEdgeInfo::
sym(void)
{
  Assert(isEdge());
  return OrEdgeInfo(abc_.sym(), flag_ ^ IsUp);
}

OrEdgeInfo OrEdgeInfo::
fnext (Complex * comp)
{
  Assert(isEdge());
  return (flag_ & IsManifold)? 
    OrEdgeInfo(comp->FnextComplete(abc_), flag_ ^ IsUp) : *this;
}

OrEdgeInfoSeq::OrEdgeInfoSeq(VIndex src, VIndex dest)
: src_(src) ,dest_(dest),
  destTri_(0, 0)
{
  
}

void OrEdgeInfoSeq::
reset (void)
{
  edges_.reset();
  src_ = dest_ = 0;
  destTri_.setIntRep(0);
}

IterStack<OrEdgeInfo> &OrEdgeInfoSeq::
getStack (void)
{
  return edges_;
}

OrEdgeInfo OrEdgeInfoSeq::lastEdge()
{
  return edges_.top();
}

VIndex& OrEdgeInfoSeq::src()
{
  return src_;
}

VIndex& OrEdgeInfoSeq::dest()
{
  return dest_;
}

OrTri& OrEdgeInfoSeq::destAsOrTri()
{
  return destTri_;
}

void OrEdgeInfoSeq::addEdge(OrEdgeInfo e, Trist * tr)
{
  edges_.push(e);
  dest_ = tr->dest(e.getOrTri());
}

OrEdgeInfoSeqList::OrEdgeInfoSeqList(VIndex src,OrTri srctri)
: closed_(FALSE), src_(src), dest_(src), srcTri_(srctri)
{
  
}



void OrEdgeInfoSeqList::
addSeq(OrEdgeInfoSeq *es,OrTri destTri)
{
  edgeSeq_.push(es);
  Assert(es->src() == dest());
  if(es->dest() == src_)
    closed_ = TRUE;
  dest_ = es->dest();
}

void OrEdgeInfoSeqList::undo()
{
  Assert(edgeSeq_.size());
  closed_ = FALSE;
  delete edgeSeq_.pop();
  dest_ = edgeSeq_.size()? edgeSeq_.top()->dest() : src_;
}

OrEdgeInfoSeqList::~OrEdgeInfoSeqList()
{
  while(!edgeSeq_.empty())
    delete edgeSeq_.pop();
}


OrEdgeInfoCycleSet::
OrEdgeInfoCycleSet (void)
{
  
}

OrEdgeInfoCycleSet::
~OrEdgeInfoCycleSet (void)
{
  while(!edgeCycle_.empty())
    delete edgeCycle_.pop();
}

void OrEdgeInfoCycleSet::addSeq(OrEdgeInfoSeq* es,OrTri destTri)
{
  Assert(lastLoopClosed()==FALSE);
  edgeCycle_.top()->addSeq(es,destTri);
}

void OrEdgeInfoCycleSet::addVertex(VIndex src,OrTri srctri)
{
//  Assert(lastLoopClosed()==TRUE);
  OrEdgeInfoSeqList *newList = new OrEdgeInfoSeqList(src,srctri);
  edgeCycle_.push(newList);
}

Bool OrEdgeInfoCycleSet::undo()
{
  if(!edgeCycle_.size())
  {
    return FALSE;
  }

  if(edgeCycle_.top()->numSeq() > 0)
    edgeCycle_.top()->undo();
  else
  {
    delete edgeCycle_.pop();
  }

  return TRUE;
}

Bool OrEdgeInfoCycleSet::lastLoopClosed()
{
  return edgeCycle_.size()? edgeCycle_.top()->closed() : TRUE;
}

VIndex OrEdgeInfoCycleSet::lastVertex()
{
  return edgeCycle_.size()? edgeCycle_.top()->dest() : 0;
}

OrTri OrEdgeInfoCycleSet::lastVertexAsOrTri()
{
  Assert(edgeCycle_.size());
  return edgeCycle_.top()->destAsOrTri();
}

Bool OrEdgeInfoCycleSet::lastPathHasEdge (void)
{
  if(!edgeCycle_.size())
    return FALSE;
  return !edgeCycle_.top()->singlePtEdge();
}

VIndex OrEdgeInfoCycleSet::lastStartingVertex()
{
  //  Assert(!LastLoopClosed());
  return edgeCycle_.size()? edgeCycle_.top()->src() : 0;
}

OrTri OrEdgeInfoCycleSet::lastStartingVertexAsOrTri()
{
  Assert(!lastLoopClosed());
  return edgeCycle_.top()->srcAsOrTri();
}

//--------------------------------------------------------------------

//------------------------------------------------------------------------

OrEdgeInfoCycleIter::OrEdgeInfoCycleIter(OrEdgeInfoCycleSet *ecs)
{
  ecs_ = ecs;
  itr_ = NULL;
}

void OrEdgeInfoCycleIter::start()
{
  delete itr_;
  itr_ = new IStackIter<OrEdgeInfoSeqList*> (ecs_->edgeCycle_);
  itr_->first();
}

void OrEdgeInfoCycleIter::next()
{
  itr_->next();
}

OrEdgeInfoSeqList* OrEdgeInfoCycleIter::currCycle()
{
  return itr_->curItem();
}

Bool OrEdgeInfoCycleIter::isDone()
{
  return itr_->isDone();
}

OrEdgeInfoCycleIter::~OrEdgeInfoCycleIter()
{
  delete itr_;
}

//---------------------------------------------------------------------------------



//--------------------------------------------------------------------------------

OrEdgeInfoSeqIter::OrEdgeInfoSeqIter(OrEdgeInfoSeqList* esl)
{
  esl_ = esl;
  esItr_ = NULL;
  eItr_ = NULL;
}

OrEdgeInfoSeqIter::~OrEdgeInfoSeqIter()
{
  delete esItr_;
  delete eItr_;
}

OrEdgeInfo OrEdgeInfoSeqIter::currEdge_()
{
  return eItr_->curItem();
}

Bool OrEdgeInfoSeqIter::isDone()
{
  if(singlePtEdge())
    return TRUE;
  if(esItr_->isDone())
    return TRUE;

  return FALSE;
}


VIndex OrEdgeInfoSeqIter::src()
{
  return esl_->src_;
}

Bool OrEdgeInfoSeqIter::singlePtEdge()
{
  return esl_->singlePtEdge();
}

void OrEdgeInfoSeqIter::start()
{
  if(singlePtEdge())
    return;

  if(esItr_)
    delete esItr_;
  if(eItr_)
    delete eItr_;

  esItr_ = new IStackIter<OrEdgeInfoSeq*> (esl_->edgeSeq_);
  esItr_->first();

  if(esl_->numSeq()>0)
  {
    eItr_ = new IStackIter<OrEdgeInfo> (esItr_->curItem()->edges_) ;
    eItr_->first();
  }
}

void OrEdgeInfoSeqIter::next_()
{
  if(singlePtEdge())
    return;

  Assert(!singlePtEdge());

  eItr_->next();

  if(eItr_->isDone())
  {
    esItr_->next();
    if(!esItr_->isDone())
    {
      delete eItr_;
      eItr_ = new IStackIter<OrEdgeInfo> (esItr_->curItem()->edges_) ;
      eItr_->first();
    }
  }
}

int OrEdgeInfoSeqIter::getNextEdge(OrEdgeInfo &e)
{
  if(!isDone())
  {
    e = currEdge_();
    next_();
    return 1;
  }

  return 0;
}

//----------------------------------------------------------------------------
