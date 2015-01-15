#ifndef __RGI_RGIMESSAGE_H__
#define __RGI_RGIMESSAGE_H__

#include <basic/rgistring.h>

class RgiMessage
{
public:
	RgiMessage ();
	RgiMessage (const RgiString &text);
	RgiMessage (const RgiString &text, int flags);
	~RgiMessage ();

	const RgiString &text () const;
	int flags () const;

private:
	RgiString text_;
	int flags_;
};

#endif
