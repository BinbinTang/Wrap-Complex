#ifndef __RGI_RGIMESSAGESTACK_H__
#define __RGI_RGIMESSAGESTACK_H__

#include <basic/iterstack.h>
#include <basic/rgimessage.h>

class MessageStack
{
public:
	MessageStack ();
	~MessageStack ();

	int messages () const;
	RgiMessage &message (int);

	void add (const RgiMessage &);

	void pushMark ();
	void popMark ();
	int marks () const;

private:
	IterStack<RgiMessage> message_;
	IterStack<int> level_;
};

extern MessageStack *theError;

#endif
