





//-----------------------------------------------------------------

template <class T>
WireFrame<T>::
WireFrame (int nOfPaths, int nOfEdges)
: parr_(nOfPaths), arr_(nOfEdges)
{

}

template <class T>
WireFrame<T>::
WireFrame (const WireFrame<T> &other)
: parr_(other.parr_),
arr_(other.arr_)
{
 
}

template <class T>
const WireFrame<T> &WireFrame<T>::
operator=(const WireFrame<T> &other)
{
  parr_ = other.parr_;
  arr_ = other.arr_;
  return *this;
}

template <class T>
WireFrame<T>::
~WireFrame (void)
{

}

//-----------------------------------------------------------------

template <class T>
void WireFrameNode<T>::
binDump (BinOutput& bo)
{
  data.binDump(bo);
  bo.writeInt(next);
  bo.writeInt(prev);
}

template <class T>
Bool WireFrameNode<T>::
binInit (BinInput& bi)
{
  return data.binInit(bi) &&
  bi.readInts(&next, 1) &&
  bi.readInts(&prev, 1);
}

void HeadTail::
binDump (BinOutput& bo)
{
  bo.writeInts((int *) this, 2);
}

Bool HeadTail::
binInit (BinInput& bi)
{
  return bi.readInts((int *) this, 2);
}


template <class T>
void WireFrame<T>::
binDump (BinOutput& bo)
{
  parr_.binDump(bo);
  arr_.binDump(bo);
}

template <class T>
Bool WireFrame<T>::
binInit (BinInput& bi)
{
  return parr_.binInit(bi) && arr_.binInit(bi);
}

//-----------------------------------------------------------------

template <class T>
Bool WireFrame<T>::
isEmpty (void)
{
  return (parr_.numUsed() == 0);
}

template <class T>
int WireFrame<T>::
nOfEdges (void)
{
  return arr_.numUsed();
}

template <class T>
int WireFrame<T>::
nOfPaths (void)
{
  return parr_.numUsed();
}

template <class T>
Index WireFrame<T>::
addPath (void)
{
  int pathId = parr_.newLoc();
  HeadTail& ht = parr_[pathId];
  ht.head = ht.tail = 0;
  return pathId;
}

template <class T>
Index WireFrame<T>::
addSimplePath (T& data1, T& data2)
{
  int pathId = addPath();
  addTail(pathId, data1);
  addTail(pathId, data2);
  return pathId;
}

template <class T>
Bool WireFrame<T>::
removePath (Index pathId)
{
  if (!parr_.isValidIndex(pathId))
    return FALSE;

  HeadTail& ht = parr_[pathId];

  while (ht.head > 0)
  {
    Index temp = ht.head;
    ht.head = arr_[ht.head].next;
    arr_.delElt(temp);
  }

  parr_.delElt(pathId);
  return TRUE;
}

template <class T>
Index WireFrame<T>::
getHeadPos (Index pathId)
{
  return parr_[pathId].head;
}

template <class T>
Index WireFrame<T>::getTailPos (Index pathId)
{
  return parr_[pathId].tail;
}

template <class T>
Bool WireFrame<T>::isValidPos (Index pos)
{
  return arr_.isValidIndex(pos);
}

template <class T>
Bool WireFrame<T>::isValidPath (Index pathId)
{
  return parr_.isValidIndex(pathId);
}

template <class T>
T& WireFrame<T>::getAt(Index pos)
{
  return arr_[pos].data;
}

template <class T>
T& WireFrame<T>::getHead (Index pathId)
{
  return arr_[parr_[pathId].head].data;
}

template <class T>
T& WireFrame<T>::getTail (Index pathId)
{
  return arr_[parr_[pathId].tail].data;
}

template <class T>
Index WireFrame<T>::getNextPos (Index pos)
{
  int next = arr_[pos].next; 
  return (next > 0)? next : 0;
}

template <class T>
Index WireFrame<T>::getPrevPos (Index pos)
{
  int prev = arr_[pos].prev; 
  return (prev > 0)? prev : 0;
}

template <class T>
Index WireFrame<T>::getPath (Index pos)
{
  Assert(isValidPos(pos));
  do {
    pos = arr_[pos].next;
  } while (pos > 0);
  return -pos;
}

template <class T>
OrPath WireFrame<T>::getPath (OrPos opos)
{
  return OrPath(getPath(opos.index()), opos.version());
}

template <class T>
Index WireFrame<T>::getNextPath (Index pathId)
{
  BitVector& bv = parr_.getBitVector();
  pathId = bv.getNextSet(pathId);
  return (pathId > 0)? pathId : 0;
}

template <class T>
Index WireFrame<T>::addHead(Index pathId, T& data)
{
  HeadTail& ht = parr_[pathId];
  if (ht.head <= 0) // no elements in the path
  {
    Assert(ht.tail <= 0);
    ht.head = ht.tail = arr_.newLoc();
    arr_[ht.head].next = -pathId;
  }
  else
  {
    Index temp = arr_.newLoc();
    arr_[temp].next = ht.head;
    arr_[ht.head].prev = temp;
    ht.head = temp;
  }

  arr_[ht.head].prev = -pathId;
  arr_[ht.head].data = data;
  return ht.head;
}

template <class T>
Index WireFrame<T>::addTail(Index pathId, T& data)
{
  HeadTail& ht = parr_[pathId];
  if (ht.tail <= 0) // no elements in the path
  {
    Assert(ht.head <= 0);
    ht.tail = ht.head = arr_.newLoc();
    arr_[ht.tail].prev = -pathId;
  }
  else
  {
    Index temp = arr_.newLoc();
    arr_[temp].prev = ht.tail;
    arr_[ht.tail].next = temp;
    ht.tail = temp;
  }

  arr_[ht.tail].next = -pathId;
  arr_[ht.tail].data = data;
  return ht.tail;
}

template <class T>
Index WireFrame<T>::insertAfter(Index pos, T& data)
{
  Assert(isValidPos(pos));
  
  Index cur = arr_.newLoc();
  Index next = arr_[pos].next; 
  WireFrameNode<T>& curNode = arr_[cur];
  curNode.next = next;
  curNode.prev = pos;
  curNode.data = data; 
  arr_[pos].next = cur;
  if (next > 0)
    arr_[next].prev = cur;
  else // tail, next is encoded pathId
    parr_[-next].tail = cur;

  return cur;
}

template <class T>
Index WireFrame<T>::insertBefore(Index pos, T& data)
{
  Assert(isValidPos(pos));
  Index cur = arr_.newLoc();
  Index prev = arr_[pos].prev;
  WireFrameNode<T>& curNode = arr_[cur];
  curNode.next = pos;
  curNode.prev = prev;
  curNode.data = data;
  arr_[pos].prev = cur;
  if (prev > 0)
    arr_[prev].next = cur;
  else
    parr_[-prev].head = cur;

  return cur;
}

template <class T>
void WireFrame<T>::removePos (Index pos)
{
  Assert(isValidPos(pos));
  Index next = arr_[pos].next,
        prev = arr_[pos].prev;

  arr_.delElt(pos);

  if (next > 0)
    arr_[next].prev = prev;
  else
    parr_[-next].tail = prev;

  if (prev > 0)
    arr_[prev].next = next;
  else
    parr_[-prev].head = next;
}

template <class T>
BitVector& WireFrame<T>::getPosBV (void)
{
  return arr_.getBitVector();
}

template <class T>
BitVector& WireFrame<T>::getPathBV (void)
{
  return parr_.getBitVector();
}

//--------------------------------------------------

template <class T>
int WireFrame<T>::
pathSize (Index pathId)
{
  if(!isValidPath(pathId))
    return 0;
  int sz = 0;
  for(int pos = getHeadPos(pathId); pos > 0; pos = getNextPos(pos))
    sz++;
  return sz;
}

//--------------------------------------------------

template <class T>
T& WireFrame<T>::getAt (OrPos orPos)
{
  return getAt(orPos.index());
}

template <class T>
OrPos WireFrame<T>::enext (OrPos orPos)
{
  return OrPos (orPos.version()? 
    getPrevPos(orPos.index()) : 
    getNextPos(orPos.index()), orPos.version());
}

template <class T>
OrPos WireFrame<T>::org(OrPath orPath)
{
  return OrPos(orPath.version()?
    getTailPos(orPath.index()) :
    getHeadPos(orPath.index()), orPath.version());
}

template <class T>
OrPos WireFrame<T>::dest(OrPath orPath)
{
  return OrPos(orPath.version()?
    getHeadPos(orPath.index()) :
    getTailPos(orPath.index()), orPath.version());
}

template <class T>
T& WireFrame<T>::getAtOrg(OrPath orPath)
{
  return orPath.version()?
    getTail(orPath.index()) :
    getHead(orPath.index());
}

template <class T>
T& WireFrame<T>::getAtDest(OrPath orPath)
{
  return orPath.version()?
    getHead(orPath.index()) :
    getTail(orPath.index());
}

//--------------------------------------------------

template <class T>
OrPos WireFrame<T>::
addOrg(OrPath orPath, T& data)
{
  return orPath.version()?
    OrPos(addTail(orPath.index(), data), 1) :
    OrPos(addHead(orPath.index(), data), 0);
}

template <class T>
OrPos WireFrame<T>::
addDest(OrPath orPath, T& data)
{
  return orPath.version()?
    OrPos(addHead(orPath.index(), data), 1) :
    OrPos(addTail(orPath.index(), data), 0);
}

template <class T>
OrPos WireFrame<T>::
insertAfter(OrPos orPos, T& data)
{
  return orPos.version()?
    OrPos(insertBefore(orPos.index(), data), 1) :
    OrPos(insertAfter(orPos.index(), data), 0);
}

template <class T>
OrPos WireFrame<T>::
insertBefore(OrPos orPos, T& data)
{
  return orPos.version()?
    OrPos(insertAfter(orPos.index(), data), 1) :
    OrPos(insertBefore(orPos.index(), data), 0);
}

template <class T>
void WireFrame<T>::
removePos (OrPos orPos)
{
  removePos(orPos.index());
}

template <class T>
void WireFrame<T>::
reset (void)
{
  arr_.reset();
  parr_.reset();
}

template <class T>
IMap * WireFrame<T>::
packPositions (void)
{
  IMap * map = arr_.pack();
  int n = arr_.numUsed();
  arr_.resize(n);
  for(int i=1; i<=n; i++)
  {
    WireFrameNode<T>& node = arr_[i];
    if(node.next > 0) node.next = (*map)[node.next];
    if(node.prev > 0) node.prev = (*map)[node.prev];
  }
  BitVectorIter pathIter(parr_.getBitVector());
  for(pathIter.first(); !pathIter.isDone(); pathIter.next())
  {
    HeadTail& hd = parr_[pathIter.curItem()];
    hd.head = (*map)[hd.head];
    hd.tail = (*map)[hd.tail];
  }
  return map;
}

template <class T>
IMap * WireFrame<T>::
packPaths (void)
{
  IMap * map = parr_.pack();
  int n = parr_.numUsed();
  parr_.resize(n);
  for(int i=1; i<=n; i++)
  {
    HeadTail& ht = parr_[i];
    arr_[ht.head].prev = arr_[ht.tail].next = -i;
  }
  return map;
}

template <class T>
Bool WireFrame<T>::
reverseDir (Index pathId)
{
	if(!isValidPath(pathId))
		return FALSE;

	HeadTail& ht = parr_[pathId];
	Index pos = ht.head;
	ht.head = ht.tail; ht.tail = pos; // swap positions
	while(pos > 0)
	{
		WireFrameNode<T> &node = arr_[pos];
		pos = node.next;
		node.next = node.prev;
		node.prev = pos;
	}

	return TRUE;

}


//--------------------------------------------------

template <class T>
WireFrameIter<T>::
WireFrameIter (WireFrame<T>& wf, BitVector * pathBV)
: wf_(wf), curPath_(0), curPos_(0)
{
  pathBV_ = new BitVector (wf_.getPathBV());
  if (pathBV)
    pathBV_->and_op (*pathBV);
  /* int numSet = */ pathBV_->getNumSet ();
}

template <class T>
WireFrameIter<T>::
~WireFrameIter (void)
{
  delete pathBV_;
}

template <class T>
void WireFrameIter<T>::
first (void)
{
  curPath_ = curPos_ = 0;
  next();
}

template <class T>
void WireFrameIter<T>::
next (void)
{
  if (curPos_)
  {
    curPos_ = wf_.getNextPos(curPos_);
  }
 
  while(!curPos_) 
  {
    curPath_ = pathBV_->getNextSet(curPath_);
    if (curPath_ < 0)
    {
      curPath_ = 0;
      return;
    }
    curPos_ = wf_.getHeadPos(curPath_);
  }
}

template <class T>
void WireFrameIter<T>::
nextPath (void)
{
  curPath_ = pathBV_->getNextSet(curPath_);
  curPos_ = (curPath_ > 0)? wf_.getHeadPos(curPath_) : 0;
}

template <class T>
Bool WireFrameIter<T>::
isDone (void)
{
  return (curPos_ <= 0);
}

template <class T>
Index WireFrameIter<T>::
curPos (void) 
{  
  return curPos_;
}

template <class T>
Index WireFrameIter<T>::
curPath (void) 
{  
  return curPath_; 
}

template <class T>
T& WireFrameIter<T>::
curItem (void) 
{ 
  return wf_.getAt(curPos_); 
}

//----------------------------------------------------------
//----------------------------------------------------------

OrPath WireFrameSameEnds::
next (OrPath op)
{
  return op.setIntRep(arr_[op.getIntRep()]), op;
}

Bool WireFrameSameEnds::
addPath (Index pathId)
{
  return add(OrPath(pathId, 0)) && add(OrPath(pathId, 1));
}

Bool WireFrameSameEnds::
delPath (Index pathId)
{
  return del(OrPath(pathId, 0)) && del(OrPath(pathId, 1));
}

int WireFrameSameEnds::
maxSize (void)
{
  return arr_.size();
}


