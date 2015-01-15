template <class T>
ListNode<T>::ListNode(T dt)
{
	data = dt;
	next = NULL;
}

template <class T>
List<T>::List (void)
{
	nElements_ = 0;
	head_ = NULL;
	tail_ = NULL;
}

template <class T>
List<T>::~List(void)
{
	removeAll();
}

template <class T>
int List<T>::isEmpty()
{
	return (nElements_ == 0);
}

template <class T>
int List<T>::size()
{
	return nElements_;
}

template <class T>
T& List<T>::getHead()
{
	Assert(nElements_ > 0 && head_ != NULL);
	return head_->data;
}

template <class T>
T& List<T>::getTail()
{
	Assert(nElements_ > 0 && tail_ != NULL);
	return tail_->data;
}

template <class T>
LISTPOS List<T>::addHead(T data)
{
	if (nElements_ == 0)
	{
		head_ = tail_ = new ListNode<T>(data);
	}
	else
	{
		ListNode<T> *temp = new ListNode<T>(data);
		temp->next = head_;
		head = temp;
	}
	nElements_++;

	return head_;
}

template <class T>
LISTPOS List<T>::addTail(T data)
{
	if (nElements_ == 0)
	{
		head_ = tail_ = new ListNode<T>(data);
	}
	else
	{
		ListNode<T> *temp = new ListNode<T>(data);
		tail_->next = temp;
		tail_ = temp;
	}
	nElements_++;

	return tail_;
}

template <class T>
LISTPOS List<T>::insertAfter(LISTPOS pos, T data)
{
	Assert(nElements_ > 0 &&  pos != NULL);
	
	ListNode<T> *temp = new ListNode<T>(data);
	ListNode<T> *cur = (ListNode<T>*) pos;
	temp->next = cur->next;
	cur->next = temp;
	nElements_++;

	return temp;
}

template <class T>
LISTPOS List<T>::getHeadPos()
{
	return head_;
}

template <class T>
LISTPOS List<T>::getTailPos()
{
	return tail_;
}



template <class T>
T& List<T>::getNext(LISTPOS &pos)
{
	Assert(nElements_ > 0 &&  pos != NULL);
	
	ListNode<T> *cur = (ListNode<T> *) pos;
	pos = cur->next;
	return cur->data;
}

template <class T>
T& List<T>::getAt(LISTPOS pos)
{
	Assert(nElements_ > 0 &&  pos != NULL);
	
	ListNode<T> *cur = (ListNode<T> *) pos;
	return cur->data;
}

template <class T>
T List<T>::removeHead()
{
	Assert(nElements_ > 0 && head_ != NULL);

	ListNode<T> *temp = head_;
	T tmpData = head_->data;
	head_ = head_->next;
	if (head_ == NULL)
		tail_ = NULL;
	nElements_--;
	delete temp;
	return tmpData;
}

template <class T>
T List<T>::removeAfter(LISTPOS pos)
{
	Assert(nElements_ > 0);

	if (pos == NULL)
		return removeHead();

	ListNode<T> *temp = (ListNode<T> *)pos;
	Assert(temp != tail_);

	T tmpData = temp->next->data;
	if (temp->next == tail_)
		tail_ = temp;
	ListNode<T>  *temp2 = temp->next;
	temp->next = temp->next->next;
	nElements_--;
	delete temp2;
	return tmpData;
}

template <class T>
void List<T>::removeAll()
{
	if (nElements_ == 0)
		return;

	Assert(nElements_ > 0 && head_ != NULL);

	ListNode<T> *temp;
	while (head_)
	{
		temp = head_;
		head_ = head_->next;
		delete temp;
	}
	nElements_ = 0;
	head_ = tail_ = NULL;
}

//----------------------------------------------------------------

template <class T>
ListIter<T>::
ListIter (List<T>& list) :
list_(list), pos_(NULL)
{

}

//----------------------------------------------------------------

template <class T>
ListIter<T>::
~ListIter (void) 
{

}

//----------------------------------------------------------------

template <class T>
void ListIter<T>::
first (void)
{
   pos_ = list_.getHeadPos();
}

//----------------------------------------------------------------

template <class T>
Bool ListIter<T>::
isDone (void)
{
   return (pos_ == NULL);
}

//----------------------------------------------------------------

template <class T>
void ListIter<T>::
next (void)
{
   list_.getNext(pos_);
}

//----------------------------------------------------------------

template <class T>
T& ListIter<T>::
curItem (void)
{
   return list_.getAt(pos_);
}

//-----------------------------------------------------------------
//                       class FreeList           
//-----------------------------------------------------------------


template <class T>
FreeList<T>::FreeList (int size)
: arr_(size),
 head_(0),
 tail_(0)
{
	
}

template <class T>
FreeList<T>::~FreeList(void)
{
	
}

template <class T>
int FreeList<T>::isEmpty()
{
	return (arr_.numUsed() == 0);
}

template <class T>
int FreeList<T>::size()
{
	return arr_.numUsed();
}

template <class T>
T& FreeList<T>::getHead()
{
	return arr_[head_].data;
}

template <class T>
T& FreeList<T>::getTail()
{
	return arr_[tail_].data;
}

template <class T>
Index FreeList<T>::addHead(T data)
{
	if (!arr_.numUsed())
	{
		head_ = tail_ = arr_.newLoc();
    arr_[head_].next = 0;
	}
	else
	{
		Index temp = arr_.newLoc();
		arr_[temp].next = head_;
		head_ = temp;
	}
  arr_[head_].data = data;
	return head_;
}

template <class T>
Index FreeList<T>::addTail(T data)
{
	if (!arr_.numUsed())
	{
		head_ = tail_ = arr_.newLoc();
    arr_[tail_].next = 0;
	}
	else
	{
		Index temp = arr_.newLoc();
		arr_[tail_].next = temp;
		tail_ = temp;
	}
  arr_[tail_].data = data;
	return tail_;
}

template <class T>
Index FreeList<T>::insertAfter(Index pos, T data)
{
  if (pos == 0)
    return addHead(data);
  if (pos == tail_)
    return addTail(data);

	Assert (arr_.isValidIndex(pos));
	Index temp = arr_.newLoc();
  arr_[temp].data = data;
	arr_[temp].next = arr_[pos].next;
	arr_[pos].next = temp;
  
	return temp;
}

template <class T>
Index FreeList<T>::getHeadPos()
{
	return head_;
}

template <class T>
Index FreeList<T>::getTailPos()
{
	return tail_;
}

template <class T>
T& FreeList<T>::getNext(Index &pos)
{
	Assert (arr_.isValidIndex(pos));
	Index cur = pos;
	pos = arr_[cur].next;
	return arr_[cur].data;
}

template <class T>
Index FreeList<T>::getNextPos(Index pos)
{
  return pos? arr_[pos].next : head_;
}

template <class T>
T& FreeList<T>::getAt(Index pos)
{
	Assert (arr_.isValidIndex(pos));
	return arr_[pos].data;
}

template <class T>
void FreeList<T>::removeHead()
{
	Index temp = head_;
	head_ = arr_[head_].next;
	if (head_ == 0)
		tail_ = 0;
  arr_.delElt(temp);
}

template <class T>
void FreeList<T>::removeAfter(Index pos)
{
  if (!arr_.numUsed() || pos == tail_)
    return;

	if (pos == 0)
  {
		removeHead();
    return;
  }

  Index cur = arr_[pos].next;
	if (cur == tail_)
		tail_ = pos;
  arr_[pos].next = arr_[cur].next;
  arr_.delElt(cur);
}

template <class T>
Bool FreeList<T>::isValidIndex (Index pos)
{
  return arr_.isValidIndex(pos);
}

template <class T>
void FreeList<T>::removeAll()
{
	arr_.getBitVector().reset();
  arr_.resetReusable();
  head_ = tail_ = 0;
}

template <class T>
void FreeList<T>::convertFrom(List<T>& list)
{
	removeAll();
  ListIter<T> iter(list);
  for (iter.first(); !iter.isDone(); iter.next())
  {
    addTail(iter.curItem());
  }
}

template <class T>
void FreeList<T>::convertTo(List<T>& list)
{
  list.removeAll();
  FreeListIter<T> iter(*this);
  for (iter.first(); !iter.isDone(); iter.next())
  {
    list.addTail(iter.curItem());
  }
}



//----------------------------------------------------------------

template <class T>
FreeListIter<T>::
FreeListIter (FreeList<T>& list) :
list_(list), pos_(0)
{

}

//----------------------------------------------------------------

template <class T>
FreeListIter<T>::
~FreeListIter (void) 
{

}

//----------------------------------------------------------------

template <class T>
void FreeListIter<T>::
first (void)
{
   pos_ = list_.getHeadPos();
}

//----------------------------------------------------------------

template <class T>
Bool FreeListIter<T>::
isDone (void)
{
   return (pos_ == 0);
}

//----------------------------------------------------------------

template <class T>
void FreeListIter<T>::
next (void)
{
   pos_ = list_.getNextPos(pos_);
}

//----------------------------------------------------------------

template <class T>
T& FreeListIter<T>::
curItem (void)
{
   return list_.getAt(pos_);
}

//----------------------------------------------------------------

template <class T>
Index FreeListIter<T>::
curIndex (void)
{
   return pos_;
}

//----------------------------------------------------------------







