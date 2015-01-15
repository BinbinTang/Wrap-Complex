#include <basic/wireframe.h>


//--------------------------------------------------------------------

const int IndexWireFrameMagic = 330;
const int IndexWireFrameVersion = 1;

IndexWireFrame::
IndexWireFrame (int nOfPaths, int nOfEdges)
: WireFrame<IntDumpable>(nOfPaths, nOfEdges),
  magic_(IndexWireFrameMagic, IndexWireFrameVersion)
{

}

IndexWireFrame::
IndexWireFrame (const IndexWireFrame &other)
: WireFrame<IntDumpable>(other),
  magic_(IndexWireFrameMagic, IndexWireFrameVersion)
{

}

IndexWireFrame::
~IndexWireFrame (void)
{

}

const IndexWireFrame &IndexWireFrame::
operator=(const IndexWireFrame &other)
{
  WireFrame<IntDumpable>::operator=(other);
  return *this;
}


void IndexWireFrame::
binDump (BinOutput& bo)
{
  magic_.binDump(bo);
  WireFrame<IntDumpable>::binDump(bo);
}

Bool IndexWireFrame::
binInit (BinInput& bi)
{
  if(!magic_.binInit(bi))
    return FALSE;
  return WireFrame<IntDumpable>::binInit(bi);
}

//--------------------------------------------------------------------

Bool IndexWireFrame::
splitPath (Index pathId, Index pos, Index &pathId1, Index &pathId2)
{
  if(!getNextPos(pos) || !getPrevPos(pos))
    return FALSE;
  Assert(pathId==getPath(pos));
  pathId1 = addPath();
  pathId2 = addPath();
  Index head1pos = getHeadPos(pathId);
  Index tail1pos = pos;
  IntDumpable pIndex = getAt(pos);
  Index head2pos = insertAfter(pos, pIndex);
  Index tail2pos = getTailPos(pathId);
  HeadTail &ht1 = parr_[pathId1], &ht2 = parr_[pathId2];
  ht1.head = head1pos, ht1.tail = tail1pos,
  ht2.head = head2pos, ht2.tail = tail2pos;
  arr_[head1pos].prev = arr_[tail1pos].next = -pathId1;
  arr_[head2pos].prev = arr_[tail2pos].next = -pathId2;
  parr_.delElt(pathId);
  return TRUE;
}

Bool IndexWireFrame::
mergePaths (Index pathId1, Index pathId2, Index &pathId)
{
  Assert(getTail(pathId1)==getHead(pathId2));
  pathId = addPath();
  removePos(getHeadPos(pathId2));
  
  Index head1pos = getHeadPos(pathId1);
  Index tail1pos = getTailPos(pathId1);
  Index head2pos = getHeadPos(pathId2);
  Index tail2pos = getTailPos(pathId2);
  HeadTail &ht = parr_[pathId];
  ht.head = head1pos;
  ht.tail = tail2pos;
  arr_[head1pos].prev = arr_[tail2pos].next = -pathId;
  arr_[tail1pos].next = head2pos;
  arr_[head2pos].prev = tail1pos;
  parr_.delElt(pathId2);
  parr_.delElt(pathId1);
  return TRUE;
}


//--------------------------------------------------------------------

const int WireFrameSameEndsMagic = 332;
const int WireFrameSameEndsVersion = 1;

WireFrameSameEnds::
WireFrameSameEnds (int size)
: magic_(WireFrameSameEndsMagic, WireFrameSameEndsVersion),
  arr_(size)
{

}

WireFrameSameEnds::
WireFrameSameEnds (WireFrameSameEnds& other)
: magic_(WireFrameSameEndsMagic, WireFrameSameEndsVersion),
  arr_(other.arr_)
{

}


WireFrameSameEnds::
~WireFrameSameEnds (void)
{

}

void WireFrameSameEnds::
reset (void)
{
  arr_.reset();
}

Bool WireFrameSameEnds::
binInit (BinInput& bi)
{
  return magic_.binInit(bi) && arr_.binInit(bi);
}

void WireFrameSameEnds::
binDump (BinOutput& bo)
{
  magic_.binDump(bo);
  arr_.binDump(bo);
}

OrPath WireFrameSameEnds::
prev (OrPath op)
{
  int rep = op.getIntRep();
  int nextrep = rep;
  while(arr_[nextrep] != rep)
    nextrep = arr_[nextrep];
  return op.setIntRep(nextrep), op;
}

Bool WireFrameSameEnds::
same (OrPath op1, OrPath op2)
{
  int rep1 = op1.getIntRep(), rep2 = op2.getIntRep();
  int currep = rep1;
  do {
    currep = arr_[currep];
  } while (currep && currep != rep1 && currep != rep2);
  return (currep == rep2);
}

Bool WireFrameSameEnds::
getStar (OrPath op, IterStack<OrPath>& s)
{
  OrPath curop(op);
  do {
    curop.setIntRep(arr_[curop.getIntRep()]);
    s.push(curop);
  } while (curop != op);
  return TRUE;
}

Bool WireFrameSameEnds::
markStar (OrPath op, BitVector& bv, Bool mark)
{
  int oprep = op.getIntRep();
  int currep = oprep;
  do {
    bv.set(currep = arr_[currep], mark);
  } while (currep != oprep);
  return TRUE;
}

Bool WireFrameSameEnds::
markPathStar (OrPath op, BitVector& bv, Bool mark)
{
  int oprep = op.getIntRep();
  int currep = oprep;
  do {
    bv.set((currep = arr_[currep]) >> 1, mark);
  } while (currep != oprep);
  return TRUE;
}

int WireFrameSameEnds::
getDegree (OrPath op)
{
  int rep = op.getIntRep();
  if(!arr_[rep])
    return 0;
  int degree = 0, currep = rep;
  do {
    currep = arr_[currep], degree++;
  } while (currep != rep);
  return degree;
}

Bool WireFrameSameEnds::
enumerateStar (OrPath op, OrPathCB cbFunc, RgiObject * obj)
{
  int rep = op.getIntRep();
  if(!arr_[rep])
    return FALSE;
  int currep = rep;
  do {
    op.setIntRep(currep);
    if(!(obj->*cbFunc) (op))
        return FALSE;
    currep = arr_[currep];
  } while (currep != rep);
  return TRUE;
}

//------------------------------------------------------------

Bool WireFrameSameEnds::
add (OrPath op)
{
  int rep = op.getIntRep();
  if(arr_[rep])
    return FALSE;
  arr_[rep] = rep;
  return TRUE;
}

Bool WireFrameSameEnds::
del (OrPath op)
{
  int rep = op.getIntRep();
  int nextrep = arr_[rep];
  if(!nextrep)
    return FALSE;
  if(nextrep==rep)
    return arr_[rep] = 0, TRUE;

  int prevrep = nextrep, currep = nextrep;
  while (currep != rep)
  {
    prevrep = currep, currep = arr_[currep];
  }

  // now next and previos are known, repointer them
  arr_[prevrep] = nextrep;
  arr_[rep] = 0;
  return TRUE;
}

Bool WireFrameSameEnds::
separate (OrPath op)
{
  int rep = op.getIntRep();
  int nextrep = arr_[rep];
  if(!nextrep)
    return FALSE;
  if(nextrep==rep)
    return TRUE;
  int prevrep = nextrep, currep = nextrep;
  while (currep != rep)
  {
    prevrep = currep, currep = arr_[currep];
  }
  arr_[prevrep] = nextrep;
  arr_[rep] = rep;
  return TRUE;
}

Bool WireFrameSameEnds::
merge (OrPath op1, OrPath op2, Bool versionOrient)
{
  int rep1 = (versionOrient? op2.getIntRep() : op1.getIntRep()), 
      rep2 = (versionOrient? op1.getIntRep() : op2.getIntRep());
  int nextrep1 = arr_[rep1], nextrep2 = arr_[rep2];
  if(!nextrep1 || !nextrep2) // not valid
    return FALSE;
  int prevrep2 = nextrep2, currep2 = nextrep2;
  while (currep2 != rep2) // cycle over second ring
  {
    prevrep2 = currep2, currep2 = arr_[currep2];
    if(currep2 == rep1)
      return FALSE;
  }

  arr_[rep1] = rep2;
  arr_[prevrep2] = nextrep1;
  return TRUE;
}

// result of operation - op1->op2 skipping other ring
Bool WireFrameSameEnds::
separateRing (OrPath op1, OrPath op2, Bool versionOrient)
{
  Assert(same(op1, op2));
  int rep1 = (versionOrient? op2.getIntRep() : op1.getIntRep()), 
      rep2 = (versionOrient? op1.getIntRep() : op2.getIntRep());

  int nextrep = arr_[rep1];
  Assert(nextrep);
  if(nextrep == rep2)
    return TRUE;

  int prevrep = 0, currep = nextrep;
  do {
    prevrep = currep;
    currep = arr_[currep]; 
  } while(currep != rep2);
  arr_[rep1] = rep2;
  arr_[prevrep] = nextrep;
  return TRUE;
}