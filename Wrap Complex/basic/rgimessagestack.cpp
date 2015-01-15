#include <basic/rgimessagestack.h>

static MessageStack globalMessageStack;
MessageStack *theMessage = &globalMessageStack;

MessageStack::
MessageStack ()
{
	level_.push (1);
}

MessageStack::
~MessageStack ()
{
}

int MessageStack::
messages () const
{
	return message_.size() - level_.top();
}

RgiMessage &MessageStack::
message (int idx)
{
	Assert ((idx >= 0) && (idx < message_.size() - level_.top()));
	return message_[level_.top() + idx];
}

void MessageStack::
add (const RgiMessage &str)
{
	message_.push (str);
}

void MessageStack::
pushMark ()
{
	level_.push (message_.size());
}

void MessageStack::
popMark ()
{
	int count = level_.top ();
	level_.pop ();
	count -= level_.top ();
	while (count > 0) {
		message_.pop();
	}
}

int MessageStack::
marks () const
{
	return level_.size ();
}

