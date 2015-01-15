#include <basic/basic.h>
#include <basic/rgimessage.h>

RgiMessage::
RgiMessage ()
{
	flags_ = 0;
}

RgiMessage::
RgiMessage (const RgiString &text) :
	text_ (text)
{
	flags_ = 0;
}

RgiMessage::
RgiMessage (const RgiString &text, int flags) :
	text_ (text), flags_ (flags)
{
}

RgiMessage::
~RgiMessage ()
{
}

const RgiString &RgiMessage::
text () const
{
	return text_;
}

int RgiMessage::
flags () const
{
	return flags_;
}

