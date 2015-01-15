#include "basic/pqueue.h"

/*------------------------------------------------------------------*/

template <class T>
PriorityQueue <T>::
PriorityQueue (void)
: comp_(NULL), obj_(NULL)
{
  
}

/*------------------------------------------------------------------*/

template <class T>
PriorityQueue <T>::
~PriorityQueue (void)
{

}

/*------------------------------------------------------------------*/

template <class T>
void PriorityQueue <T>::
//setCompFunc ( PriorityQueue<T>::CompFunc comp, RgiObject * obj)
setCompFunc ( CompFunc comp, RgiObject * obj)
{
   comp_ = comp;
   obj_ = obj;
}


/*------------------------------------------------------------------*/
/*------------------------------------------------------------------*/

template <class T>
BinaryHeap <T>::
BinaryHeap (int minsize)
   : heap_ (minsize),
     heapData_(minsize),
     heapSize_(0)
{

}

/*------------------------------------------------------------------*/

template <class T>
BinaryHeap <T>::
~BinaryHeap (void)
{

}

/*------------------------------------------------------------------*/

template <class T>
int BinaryHeap <T>::
parent_ (int i)
{
   return (i/2);
}

/*------------------------------------------------------------------*/

template <class T>
int BinaryHeap <T>::
left_ (int i)
{
   return (2*i);
}

/*------------------------------------------------------------------*/

template <class T>
int BinaryHeap <T>::
right_ (int i)
{
   return (2*i + 1);
}

/*------------------------------------------------------------------*/

template <class T>
T & BinaryHeap <T>::
data_ (int i)
{
   return heapData_[i].key;
}

/*------------------------------------------------------------------*/

template <class T>
int & BinaryHeap <T>::
dataLoc_ (int i)
{
   return heapData_[i].location;
}

template <class T>
inline int BinaryHeap <T>::
size (void)
{
  return heapSize_;
}

template <class T>
inline int BinaryHeap <T>::
empty (void)
{
  return (heapSize_ == 0);
}



/*------------------------------------------------------------------*/

template <class T>
int BinaryHeap <T>::
larger_ (int i, int j)
{
  return ((obj_->*comp_)(data_ (heap_[i]), data_ (heap_[j])) > 0);
}

/*------------------------------------------------------------------*/

template <class T>
void BinaryHeap <T>::
exchange_ (int i, int j)
{
   heap_[0] = heap_[i];
   heap_[i] = heap_[j];
   heap_[j] = heap_[0];

   dataLoc_ (heap_[i]) = i;
   dataLoc_ (heap_[j]) = j;
}

/*------------------------------------------------------------------*/

template <class T>
void BinaryHeap <T>::
verify (void)
{
  int i, pi;
  T iKey, piKey;
  for (i = 2; i<=size(); i++)
  {
    pi = parent_(i);
    iKey = data_(heap_[i]);
    piKey = data_(heap_[pi]);
    Assert (larger_ (pi, i));
  }
}

/*------------------------------------------------------------------*/

// non-recursive version
template <class T>
void BinaryHeap <T>::
heapify_ (int i)
{
  int largest = i;
  do {
   int l = left_(i);
   int r = right_(i);
   if(l <= heapSize_ && larger_(l, i))
     largest = l;
   if(r <= heapSize_ && larger_(r, largest))
     largest = r;
   if (largest == i)
     return;
   exchange_(i, largest);
   i = largest;
 } while(1);
}

/*------------------------------------------------------------------*/

template <class T>
void BinaryHeap <T>::
reset (void)
{
  heapData_.reset();
  heapSize_ = 0;
}



/*------------------------------------------------------------------*/

template <class T>
void BinaryHeap <T>::
heapsort (void)
{
  int i, j;
  int temp;
  int saveSize = heapSize_;
  for (i=2; i<=heapSize_; i++)
  {
    temp = heap_[i];
    
    for (j = i; 
	   (j > 1) && ((obj_->*comp_) (data_ (temp), data_ (heap_[parent_(j)])) > 0);
     j = parent_ (j))
     {
       heap_[j] = heap_[parent_ (j)];
       dataLoc_ (heap_[j]) = j;
     }	  
     
     heap_[j] = temp;
     dataLoc_ (heap_[j]) = j;
  }
  
  while (heapSize_ > 1)
  {
    exchange_ (1, heapSize_);
    heapSize_--;
    heapify_ (1);
  }

  heapSize_ = saveSize;
}

/*---------------------------------------------------------------------*/

template <class T>
T BinaryHeap <T>::
extractMin (void)
{
  T minVal = data_ (heap_[1]);
  heapData_.delElt (heap_[1]);
  
  if (heapSize_ != 1)
  {
    heap_[1] = heap_[heapSize_];
    dataLoc_ (heap_[1]) = 1;
    
    heapSize_ --;
    heapify_(1);
  }
  else
  {
    heapSize_ = 0;
  }
  return minVal;   
}

/*---------------------------------------------------------------------*/

template <class T>
int BinaryHeap <T>::
minKeyIndex (void)
{
   return heap_[1];
}

/*---------------------------------------------------------------------*/

template <class T>
T BinaryHeap <T>::
minKey (void)
{
   return data_ (heap_[1]);
}

/*----------------------------------------------------------------------*/

template <class T>
int BinaryHeap <T>::
insert (T key)
{
   heapSize_++;
   int i = heapSize_;
   while((i > 1) && (obj_->*comp_)(key, data_ (heap_[parent_(i)])))
   {
      // NOTE:  The following statements need to be written as they
      //        are.  With RArray, a statement
      //
      //               arr[i] = arr[j];
      // 
      //        is not allowed.
      int tempVal = heap_[parent_(i)];
      heap_[i] = tempVal;
      dataLoc_ (heap_[i]) = i;
      i = parent_(i);
   }
   int loc = heapData_.newLoc ();
   data_(loc) = key;
   dataLoc_ (loc) = i;
   heap_[i] = loc;
   
   return loc;
}

template <class T>
Bool BinaryHeap <T>::
isValidIndex (int index)
{
  return heapData_.isValidIndex (index);
}

template <class T>
int BinaryHeap <T>::
getIndex (T& key)
{
  void * orig =  heapData_.getData();
  void * curr = &key;
  return (curr - orig)/sizeof(BheapData<T>);
}

/*----------------------------------------------------------------------*/

/*
 * decreaseKey () -- This method assumes that the change in the key
 *                   priority occurs somehow within the comparison
 *                   function.
 */

template <class T>
void BinaryHeap <T>::
decreaseKey (int index)
{
  int j;
  for (j = dataLoc_ (index); 
  (j > 1) && ((obj_->*comp_) (data_ (index), data_ (heap_[parent_(j)])) > 0);
  j = parent_ (j))
  {
    heap_[j] = heap_[parent_ (j)];
    dataLoc_ (heap_[j]) = j;
  }
  
  heap_[j] = index;
  dataLoc_ (heap_[j]) = j;
}

/*----------------------------------------------------------------------*/

template <class T>
void BinaryHeap <T>::
increaseKey (int index)
{
  heapify_(dataLoc_(index));
}

/*----------------------------------------------------------------------*/

template <class T>
void BinaryHeap <T>::
changeKey (int index, T newVal)
{
  Assert (heapData_.isValidIndex(index));
  T oldVal = data_(index);
  data_(index) = newVal;
  if ((obj_->*comp_)(oldVal, newVal) > 0)
    increaseKey (index);
  else
    decreaseKey (index);
}

/*----------------------------------------------------------------------*/

template <class T>
FreeArray < BheapData<T> > & BinaryHeap <T>::
heapData (void)
{
  return heapData_;
}

template <class T>
BitVector& BinaryHeap <T>::
getBitVector (void)
{
  return heapData_.getBitVector();
}


/*----------------------------------------------------------------------*/

template <class T>
T BinaryHeap <T>::
getKey (int index)
{
  Assert (heapData_.isValidIndex(index));
  return data_(index);
}

/*----------------------------------------------------------------------*/

template <class T>
T& BinaryHeap <T>::
operator[] (int index)
{
  return heapData_[index].key;
}

/*----------------------------------------------------------------------*/

/*
template <class T>
void BinaryHeap <T>::
show (ostream &os)
{
   for(int i=1; i<=heapSize_; i++) os << data_ (heap_[i]) << " ";

   os << endl;
}
*/

/*----------------------------------------------------------------------*/
