/*
 * listack.h -- This header file defines the interface for a long integer
 *              stack.  This stack provides basic long integer operations on
 *              the elements at the top of the stack.
 */

#ifndef __RGI_LISTACK_H__
#define __RGI_LISTACK_H__

#include <basic/iterstack.h>
#include <li/li.h>

class LI_Stack : public IterObjStack<LI>
{
public:
  LI_Stack (int size = 10) : IterObjStack<LI> (size) {}

  ~LI_Stack (void) {}

  // push, pop, etc are the same

  void pushtop (void);
  void plus (void);
  void minus (void);
  void times (void);
  void ipower (int p);
  void negtop (void);

private:
  // Should never be used...
  // LI_Stack (const LI_Stack &other) { Touch_args(other); }    
  // const LI_Stack &operator= (const LI_Stack &other) { Touch_args(other); }

};


#endif  // __LI_STACK_H__
