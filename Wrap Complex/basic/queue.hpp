/* clib/queue.cpp --- Unbounded int stack/queue with dynamic re-allocation. */

/*--------------------------------------------------------------------------*/

#include "basic/queue.h"
#include "basic/comment.h"

/*--------------------------------------------------------------------------*/

template <class T>
StackQueue<T>::
StackQueue (int minsize)
           : 
             stack_(new IterStack<T>(minsize)),
             bottom_(new IterStack<T>(1)),
             head_(1)
{
 Assert (minsize >= 1); 
}

/*--------------------------------------------------------------------------*/

template <class T>
StackQueue<T>::
~StackQueue (void)
{
  delete bottom_;
  delete stack_;
}

/*--------------------------------------------------------------------------*/

template <class T>
void StackQueue<T>::
reset (void)
{
 stack_->reset();
 bottom_->reset();
 head_ = 1;
}

/*--------------------------------------------------------------------------*/

template <class T>
Bool StackQueue<T>::
empty (void)
{
  return (stack_->empty() && (bottom_->size() < head_));
}

/*--------------------------------------------------------------------------*/

template <class T>
T StackQueue<T>::
top (void)
{
  if (! stack_->empty()) return stack_->top();
  else if (! bottom_->empty()) return bottom_->top();
  else Assert_always(0);
  return bottom_->top(); // never reached
}


/*--------------------------------------------------------------------------*/

template <class T>
T StackQueue<T>::
pop (void)
{
 if (! stack_->empty()) return stack_->pop();
 else if (bottom_->size() >= head_) return bottom_->pop();
 else Assert_always(0); 
 return bottom_->pop(); //never reached
}

/*--------------------------------------------------------------------------*/

/* Adds a new top element (stack and queue operation!). */
template <class T>
void StackQueue<T>::
push (T value)  
{
 stack_->push(value);
 // COMMENT("PUSH:"<<value);
 // COMMENT("  stack: " << *stack_);
 // COMMENT("  bottom: " << *bottom_);
}

/*--------------------------------------------------------------------------*/

template <class T>
T StackQueue<T>::
bot (void)
{
  if (bottom_->size() >= head_)
    return (*bottom_)[head_];
  else if (! stack_->empty())
    return ((*stack_)[1]);
  else
    Assert_always(0);
   return (*bottom_)[head_];
}

/*--------------------------------------------------------------------------*/

/* Returns the bottom element (queue operation!) and removes it. */
template <class T>
T StackQueue<T>::
get (void)
{
  T result = bot ();
  if (bottom_->size() >= head_) head_++;
  else if (! stack_->empty())
    { /* move stack to bottom */
      IterStack<T> * tmp = bottom_;
      bottom_ = stack_;
      stack_ = tmp;
      stack_->reset();
      head_ = 2;
    }
  else
    Assert_always(0); // queue_get: underflow

 // COMMENT("GET:"<< result);
 // COMMENT("  stack: " << *stack_);
 // COMMENT("  bottom: " << *bottom_);

  return (result);
}

/*--------------------------------------------------------------------------*/

/* Returns the number of elements in s. */
template <class T>
int StackQueue<T>::
length (void)
{
  int len = 0;
  if (! stack_->empty())
    len += stack_->size();
  if (! bottom_->empty())
    len += bottom_->size() - (head_ - 1);
  return (len);
}

/*--------------------------------------------------------------------------*/
