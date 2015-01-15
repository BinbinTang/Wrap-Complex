#ifndef __RGI_LIST_H__
#define __RGI_LIST_H__

#include <basic/dumpable.h>

typedef void* LISTPOS;

template <class T>
class ListNode
  {
  public:
    ListNode(T dt);
    T data;
    ListNode<T> *next;
  };

template <class T>
class List
{
public:
	List (void);
	~List (void);

	int isEmpty();
	int size();

	T& getHead();
	T& getTail();

	LISTPOS addHead(T data);
	LISTPOS addTail(T data);
	LISTPOS insertAfter(LISTPOS pos, T data);

	LISTPOS getHeadPos();
  LISTPOS getTailPos();
	T& getNext(LISTPOS &pos);
	T& getAt(LISTPOS pos);
	
	T removeHead();
	T removeAfter(LISTPOS pos); 
	void removeAll();
  
  

private:
	int nElements_;
	ListNode<T> *head_;
	ListNode<T> *tail_;
};

template <class T>
class ListIter
{
public:
  ListIter (List<T>& list);
  ~ListIter (void);
  void first (void);
  Bool isDone (void);
  void next (void);
  T& curItem (void);
private:
  List<T>& list_;
  LISTPOS pos_;
};

//------------------------------------------------------------------------------

template <class T>
class FreeListNode
  {
  public:
    T data;
    Index next;
    FreeListNode (void) : next(0) { }
    FreeListNode(T dt) : data(dt), next(0) { }
  };

template <class T>
class FreeList
{
public:
	FreeList (int size = 20);
	~FreeList (void);

	int isEmpty();
	int size();

	T& getHead();
	T& getTail();

	Index addHead(T data);
	Index addTail(T data);
	Index insertAfter(Index pos, T data);

	inline Index getHeadPos();
  inline Index getTailPos();
  inline Index getNextPos (Index pos);
	inline T& getNext(Index &pos);
	inline T& getAt(Index pos);
  inline Bool isValidIndex (Index pos);
	
	void removeHead();
	void removeAfter(Index pos); 
	void removeAll();
  void convertTo (List<T>& list);
  void convertFrom (List<T>& list);
  
  
private:
	Index head_;
	Index tail_;
  FreeArray< FreeListNode<T> > arr_;
};

template <class T>
class FreeListIter
{
public:
  FreeListIter (FreeList<T>& list);
  ~FreeListIter (void);
  inline void first (void);
  inline Bool isDone (void);
  inline void next (void);
  inline T& curItem (void);
  inline Index curIndex (void);
private:
  FreeList<T>& list_;
  Index pos_;
};

#include <basic/list.hpp>

#endif //LIST_H
