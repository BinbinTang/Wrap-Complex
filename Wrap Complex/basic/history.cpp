//
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
// 


#include <basic/history.h>
#include <basic/callbackobject.h>

//----------------------------------------------------------------------------

HistoryAttribute::
~HistoryAttribute (void)
{

}

int HistoryAttribute::
getAttribId (void)
{
  return 0; 
}

int HistoryAttribute::
getClassAttribId (void) 
{
  return 0; 
}


//----------------------------------------------------------------------------

HistoryElement::
HistoryElement (HistoryList * hl, FilePointer & s)
{
  id_ = hl->getCurrentReadId ();
  BinInput bi(s);
  bi.readInts (&time_, 1);
  attribPtr_ = NULL;
}

//----------------------------------------------------------------------------

HistoryElement::
~HistoryElement (void)
{
  if (attribPtr_) delete attribPtr_;
}

//----------------------------------------------------------------------------

void HistoryElement::
apply (void) 
{ 
}

//----------------------------------------------------------------------------

void HistoryElement::
updateStatus (void) 
{ 
}

//----------------------------------------------------------------------------

void HistoryElement::
unApply (void) 
{ 
}

//----------------------------------------------------------------------------

void HistoryElement::
eraseFromHistory (void) 
{ 
}

//----------------------------------------------------------------------------

int HistoryElement::
binWrite (FilePointer & s) 
{
  BinOutput bo(s);
  bo.writeInts (&id_, 1);
  bo.writeInts (&time_, 1);

  return 1;
}

//----------------------------------------------------------------------------

void * HistoryElement::
getAttribute (int attribId)
{
  if (attribPtr_ == NULL)
     return NULL;

  if (attribId == attribPtr_->getAttribId())
     return (void *) attribPtr_;
  else
     return NULL;
}

//----------------------------------------------------------------------------

void HistoryElement::
setAttribute (HistoryAttribute * attribPtr)
{
  attribPtr_ = attribPtr;
}

//----------------------------------------------------------------------------

HistoryList::
HistoryList (void) 
        : playBack_ (0),
          currentIndex_(0),
          isModified_(FALSE),
          historyActive_(TRUE),
          progressObject_(NULL)
{ 
  
}

//----------------------------------------------------------------------------

HistoryList::
~HistoryList (void)
{
  reset ();
}

//----------------------------------------------------------------------------

int HistoryList::
findIdIndex_ (int id)
{ 
  IStackIter<HistoryConstructorType> hcpi (Constructors_);
  for (hcpi.first (); ! hcpi.isDone (); hcpi.next ())
  {
    int index = hcpi.curIndex ();

    if (Constructors_[index].id == id)
      return index;
  }

  return -1;
}

//----------------------------------------------------------------------------

#define HISTORY_MAGIC 845686

int HistoryList::
binRead (void * data, FilePointer & s) 
{ 
  BinInput bi (s);
  int index;
  int historyMagic;
  bi.readInts (&historyMagic, 1);
  Assert_always (historyMagic == HISTORY_MAGIC);

  int lastIndex;
  bi.readInts (&lastIndex, 1);

  for (index = 1; index <= lastIndex; index++)
  {
    bi.readInts (&currentReadId_, 1);
    int idIndex = findIdIndex_ (currentReadId_);

    Assert_always (idIndex != -1);

    HistoryElement * he = Constructors_[idIndex].construct (data, s);
    appendInstance_ (he);
  }

  bi.readInts (&historyMagic, 1);
  Assert_always (historyMagic == HISTORY_MAGIC);

  bi.readInts (&currentIndex_, 1);
  bi.readInts (&lastIndex, 1);

  for (index = 1; index <= lastIndex; index++)
  {
    bi.readInts (&currentReadId_, 1);
    int idIndex = findIdIndex_ (currentReadId_);

    Assert_always (idIndex != -1);

    HistoryElement * he = Constructors_[idIndex].construct (data, s);
    hOpList_.push (he);
  }
 
  bi.readInts (&historyMagic, 1);
  Assert_always (historyMagic == HISTORY_MAGIC);

  return 1;
}

//----------------------------------------------------------------------------

int HistoryList::
binWrite (FilePointer & s)
{ 
  BinOutput bo(s);

  int historyMagic = HISTORY_MAGIC;

  int lastIndex = hInstList_.numUsed();

  bo.writeInts (&historyMagic, 1);
  bo.writeInts (&lastIndex, 1);

  FArrayIter<HistoryElement *> hii (hInstList_);
  for (hii.first (); ! hii.isDone (); hii.next ())
  {
    int index = hii.curIndex ();

    int id = hInstList_[index]->getId ();
    bo.writeInts (&id, 1);
    hInstList_[index]->binWrite(s);
  }

  bo.writeInts (&historyMagic, 1);

  lastIndex = hOpList_.size();

  bo.writeInts (&historyMagic, 1);
  bo.writeInts (&currentIndex_, 1);
  bo.writeInts (&lastIndex, 1);

  IStackIter<HistoryElement *> hoi (hOpList_);
  for (hoi.first (); ! hoi.isDone (); hoi.next ())
  {
    int index = hoi.curIndex ();

    int id = hOpList_[index]->getId ();
    bo.writeInts (&id, 1);
    hOpList_[index]->binWrite(s);
  }

  bo.writeInts (&historyMagic, 1);

  return 1;
}

#undef HISTORY_MAGIC

//----------------------------------------------------------------------------

void HistoryList::
addConstructor (int id, HistoryElement * (*construct) (void *, FilePointer &))
{ 
  HistoryConstructorType hc;
  hc.id = id;
  hc.construct = construct;

  Constructors_.push (hc);
}

//----------------------------------------------------------------------------

void HistoryList::
reset (void)
{
  /*
  while (currentIndex_ > 0)
  {
    hOpList_[currentIndex_--]->unApply ();
  }
  */
  currentIndex_ = 0;
  deleteAllMoreRecentInstances_ ();
  while (hOpList_.size () > currentIndex_)
  {
    delete hOpList_.pop ();
  }
  
}

//----------------------------------------------------------------------------

int HistoryList::
nofOperations (void) 
{ 
  return currentIndex_;
}

//----------------------------------------------------------------------------

int HistoryList::
timeCurrent (void) 
{ 
  if (currentIndex_ == 0) return -1;
  
  return hOpList_[currentIndex_]->getTime(); 
}

//----------------------------------------------------------------------------

int HistoryList::
timeMax (void) 
{ 
  if (hOpList_.size() == 0) return -1;
  
  return hOpList_[hOpList_.size()]->getTime(); 
}

//----------------------------------------------------------------------------

int HistoryList::
timeNext (void) 
{ 
  if (hOpList_.size() == 0) return timeCurrent () + 1;

  if (currentIndex_ == hOpList_.size())
    return timeCurrent () + 1; 
  
  return timeCurrent ();
}

//----------------------------------------------------------------------------

Bool HistoryList::
playbackMode (void)

{
  return playBack_;
}

//----------------------------------------------------------------------------

void HistoryList::
deleteAllMoreRecentInstances_ (void)
{
  IterStack<int> delList;
  FArrayIter<HistoryElement *> hii (hInstList_);

  for (hii.first (); ! hii.isDone (); hii.next ())
  {
    int curInstIndex = hii.curIndex ();

    if (hInstList_[curInstIndex]->getTime() > timeCurrent ())
      delList.push (curInstIndex);
  }

  while (! delList.empty ()) 
  {
    delete hInstList_[delList.top ()];
    hInstList_.delElt (delList.pop ());
  }
}

//----------------------------------------------------------------------------

void HistoryList::
deleteAllMoreRecentOperations_ (void)
{
  while (hOpList_.size () > currentIndex_)
  {
    hOpList_[hOpList_.size ()]->eraseFromHistory ();
    delete hOpList_.pop ();
  }
}

//----------------------------------------------------------------------------

void HistoryList::
deleteAllMoreRecent (void)
{
  deleteAllMoreRecentInstances_ ();
  deleteAllMoreRecentOperations_ ();
}

//----------------------------------------------------------------------------

void HistoryList::
appendOperation_ (HistoryElement * op)
{
  deleteAllMoreRecent ();

  hOpList_.push (op);
  currentIndex_++;

  isModified_ = 1;

  Assert (currentIndex_ == hOpList_.size());
}

//----------------------------------------------------------------------------

void HistoryList::
appendInstance_ (HistoryElement * inst)
{
  if (instanceExists (inst->getTime ())) 
  {
    delete inst;
    return;
  }

  hInstList_[hInstList_.newLoc ()] = inst;
}

//----------------------------------------------------------------------------

void HistoryList::
append (HistoryElement * he)
{
  Assert (! playBack_);

  if (he->isInstance ())
  {
    appendInstance_ (he);
  }
  else
  {
    appendOperation_ (he);
  }
}

//----------------------------------------------------------------------------

Bool HistoryList::
instanceExists (int time)
{
  FArrayIter<HistoryElement *> hii (hInstList_);

  for (hii.first (); ! hii.isDone (); hii.next ())
  {
    int curInstIndex = hii.curIndex ();
    if (hInstList_[curInstIndex]->getTime() == time)
      return 1;
  }
 
  return 0;
}

//----------------------------------------------------------------------------

Bool HistoryList::
isModified (void)
{
  Bool tmp = isModified_;
  isModified_ = 0;

  return tmp;
}

//----------------------------------------------------------------------------

int HistoryList::
findLargestInstance_ (int time)
{
  FArrayIter<HistoryElement *> hii (hInstList_);

  int instIndex = -1;
  int instTime = -1;
  for (hii.first (); ! hii.isDone (); hii.next ())
  {
    int curInstIndex = hii.curIndex ();
    int curInstTime = hInstList_[curInstIndex]->getTime();

    Assert (hInstList_[curInstIndex]->isInstance ());

    if ((curInstTime > instTime) && (curInstTime <= time))
    {
      instIndex = curInstIndex;
      instTime = curInstTime;
    }
  }

  return instIndex;
}

//----------------------------------------------------------------------------

void HistoryList::
gotoTimeInstance_ (int time)
{
  Assert (time < timeCurrent ());

  int instIndex = findLargestInstance_ (time);

  if (instIndex != -1) 
  {
    int instTime = hInstList_[instIndex]->getTime();
  
    while (timeCurrent () > instTime)
    {
      hOpList_[currentIndex_--]->unApply ();
    }

    Assert (hOpList_[currentIndex_]->getTime() == instTime);
    Assert (currentIndex_ >= 1);

    while (currentIndex_ > 1)
    {
      if (hOpList_[currentIndex_ - 1]->getTime() < instTime)
	break;

      hOpList_[currentIndex_--]->unApply ();
    }

    Assert (currentIndex_ >= 1);

    playBack_ = 1;
    hInstList_[instIndex]->apply ();
    playBack_ = 0;

    return;
  }

  while (currentIndex_ >= 1)
  {
    hOpList_[currentIndex_--]->unApply ();
  }
}

//----------------------------------------------------------------------------

int HistoryList::
gotoTime (int time)
{
  if (hOpList_.size () == 0) 
    return 0;

  if (time > timeMax() || time < 0)
    return 0;
  
  Assert (currentIndex_ <= hOpList_.size());

  if (time > hOpList_[hOpList_.size()]->getTime()) 
    time = hOpList_[hOpList_.size()]->getTime();

  if (time == timeCurrent ())
    return 1;

  if (time < timeCurrent ()) 
    gotoTimeInstance_ (time);
  else 
  {
    int instIndex = findLargestInstance_ (time);

    if (instIndex != -1) 
    {
      int instTime = hInstList_[instIndex]->getTime();

      if (instTime > timeCurrent ())
      {
	do 
	{
	  if (currentIndex_ == hOpList_.size()) 
	    break;
	  if (hOpList_[currentIndex_ + 1]->getTime() > instTime) 
	    break;

	  currentIndex_++;

	  hOpList_[currentIndex_]->updateStatus ();
	} 
	while (1);

	playBack_ = 1;
	hInstList_[instIndex]->apply ();
	playBack_ = 0;
      }
    }
  }

  Assert (currentIndex_ <= hOpList_.size());
  Assert (timeCurrent () <= time);

  if (time == timeCurrent ()) 
    return 1;

  // figure out the number of operations, which should be performed
  int prog_begin, prog_end;
  int n_op = 0, done_op = 0;
  if (progressObject_) 
  {
    progressObject_->getProgressBounds(prog_begin, prog_end);
    int tmpIndex = currentIndex_;
    do {
      if (tmpIndex == hOpList_.size()) 
        break;
      if (hOpList_[tmpIndex + 1]->getTime() > time) 
        break;
      tmpIndex++;
      n_op++;
      if (hOpList_[tmpIndex]->getTime() == time) 
       break;
    } while (1);
    // now n_oper gives the number of operations
  }
  


  do 
  {
    if (currentIndex_ == hOpList_.size()) 
      break;

    if (hOpList_[currentIndex_ + 1]->getTime() > time) 
      break;

    currentIndex_++;

    if (progressObject_)
    {
      int prog1 = (prog_begin*(n_op-done_op)+prog_end*done_op)/n_op;
      int prog2 = (prog_begin*(n_op-done_op-1)+prog_end*(done_op+1))/n_op;
      progressObject_->setProgressBounds (prog1, prog2);
    }

    playBack_ = 1;
    hOpList_[currentIndex_]->apply ();
    playBack_ = 0;
    done_op++;
    if (hOpList_[currentIndex_]->getTime() == time) 
      break;
  } 
  while (1);

  if (progressObject_) 
  {
    progressObject_->setProgressBounds(prog_begin, prog_end);
    progressObject_->progressEnd();
  }
  return 1;
}

//----------------------------------------------------------------------------

Bool HistoryList::
undo (void)
{
  return gotoTime(timeCurrent() - 1);
}

Bool HistoryList::
redo (void)
{
  return gotoTime(timeCurrent() + 1);
}

Bool HistoryList::
canUndo (void)
{
  return (timeCurrent() > 0);
}

Bool HistoryList::
canRedo (void)
{
  return (timeCurrent() < timeMax());
}

//----------------------------------------------------------------------------

Bool HistoryList::
isHistoryActive (void)
{
  return historyActive_;
}

void HistoryList::
setHistoryActive (Bool mode)
{
  historyActive_ = mode;
  if (!mode)
    clearHistory ();
}

//----------------------------------------------------------------------------

void HistoryList::
clearHistory (void)
{
  reset ();
}

//----------------------------------------------------------------------------

HistoryElement * HistoryList::
getNextOperation_ (void)
{
  if (currentIndex_ >= hOpList_.size()) 
    return NULL;
  return hOpList_[currentIndex_ + 1];
}
