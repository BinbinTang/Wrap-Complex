#ifndef __RGI_CMDLOOKUP_H__
#define __RGI_CMDLOOKUP_H__

#include <basic/rgicstring.h>
#include <basic/map.h>

class Command;
class VarLenTypedData;

class RgiCStringHash
{
public:
	int operator () (const RgiCString &) const;
};

class CmdLookUp
{
	Map<RgiCString,Command*,RgiCStringHash> cmdmap_;
public:
	CmdLookUp();
	~CmdLookUp();

	void addCommand(Command *cmd);
	Command* findCommand(const RgiCString &);
};

extern CmdLookUp theCmdLookUp;


#endif //CMDLOOKUP_H
