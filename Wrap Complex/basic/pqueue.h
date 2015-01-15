#ifndef __RGI_PQUEUE_H__
#define __RGI_PQUEUE_H__

#include <iostream>
#include <basic/freearray.h>
#include <basic/callbackobject.h>

/********************************************************************/

/*
 *
 *
 *    class PriorityQueue --
 *    The comparison function should return 1 if first argument is larger.
 *    This heap stores the *minimum* element at the top, as determined by
 *    the comparison function, so for minimum heaps, reverse the direction of
 *    the comparison function.
 *
 *    NOTE:  Because of the debug feature, any class used in this template
 *    needs an operator<< method defined.
 */

template <class T>
class PriorityQueue
{
public:
   typedef int (RgiObject::*CompFunc) (T&, T&);
   PriorityQueue(void);
   virtual ~PriorityQueue (void);
   virtual void setCompFunc ( CompFunc comp, RgiObject * obj);

   virtual int insert (T key) = 0;
   // This method returns the "index" of a key.  This index can be used in
   // operations that require direct reference to the key, such as
   // decreaseKey and, eventually, delete.
   
   virtual void decreaseKey (int index) = 0;
   // This operation requires an index for a key, as returned by the insert
   // method.
   virtual void increaseKey (int index) = 0;
   virtual void changeKey (int index, T newVal) = 0;
   virtual T getKey (int index) = 0;

   
   virtual int empty (void) = 0;
   virtual int minKeyIndex (void) = 0;
          // Returns the "index" of the minimum key.
   virtual T minKey (void) = 0;
   virtual T extractMin (void) = 0;
   virtual void reset (void) = 0;
//   virtual void show(ostream &os = cout) = 0;

protected:
   CompFunc comp_;
   RgiObject * obj_;
};


/*
 *
 * class BheapData -- This class stores a key in a binary heap, along with
 *           its current location in the heap array.
 */

template <class T>
class BheapData
{
public:
   T key;
   int location;
};

/*
 *    class BinaryHeap -- see comments on PriorityQueue above...
 *
 *    NOTE:  this class has the same restrictions as FreeArray, since it
 *    uses that class.  See rarray.h for more details.
 */

template <class T>
class BinaryHeap : public PriorityQueue<T>
{
public:
   BinaryHeap(int minsize = 10);
   virtual ~BinaryHeap (void);

   virtual int insert (T key);
   virtual void decreaseKey (int index);
   virtual void increaseKey (int index);
   virtual void changeKey (int index, T newVal);
   virtual T getKey (int index);

   virtual int empty (void);
   virtual int minKeyIndex (void);
          // Returns the "index" of the minimum key.
   virtual T minKey (void);
   virtual T extractMin (void);
   virtual void heapsort (void);
//   virtual void show (ostream &os);
   void verify (void);
	 int size (void);
   void reset (void);
   Bool isValidIndex (int index);
   int getIndex (T& key);
   inline T &operator[] (int index);
   inline FreeArray< BheapData<T> >& heapData (void);
   inline BitVector& getBitVector (void);
private:
   RArray<int> heap_;
   // A standard heap array, with leaf child = 2i and right child = 2i + 1.
   // The elements store indices into data_ below.

   FreeArray< BheapData<T> > heapData_;
   // This array stores the actual data, along with an index into heap_ above.
   // This method for storing keys allows for a convenient reference index
   // that allows for deletion or update of the key priority (with
   // decreaseKey, for example).

   int heapSize_;
   inline int parent_ (int i);
   inline int left_ (int i);
   inline int right_ (int i);
   inline int larger_ (int i, int j);
   inline void exchange_ (int i, int j);

   inline T & data_ (int i);
   // A convenient "macro" for getting the data corresponding to an index.
   inline int & dataLoc_ (int i);
   // A convenient "macro" for getting the location of an index in heap_.
   
   void heapify_ (int i);
};

#include <basic/pqueue.hpp>

#endif // __PQUEUE_H__
