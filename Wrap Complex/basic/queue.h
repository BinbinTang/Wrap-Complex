
#ifndef __RGI_QUEUE_H__
#define __RGI_QUEUE_H__

#include <iostream>
#include <basic/iterstack.h>


/************************************************************************/

/*
 * Class IStackQueue
 *
 *
 *
 */

template <class T>
class StackQueue
{
public:
  StackQueue (int size = 10);
  ~StackQueue (void);
  T top (void);
  T pop (void);
  T bot (void);
  Bool empty (void);
  void push(T ); 
  T get (void);
  int length (void);
  void reset (void);
private:
  IterStack<T> * stack_;
  IterStack<T> * bottom_;
  int head_;
};

#include <basic/queue.hpp>

#endif // __QUEUE_H__
