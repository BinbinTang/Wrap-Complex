#include <basic/command.h>
#include <basic/cmdlookup.h>

/////////////////////////////////////////////////////////////////////////////
// CmdLookUp

CmdLookUp::CmdLookUp()
{
}

CmdLookUp::~CmdLookUp()
{
}

void CmdLookUp::addCommand(Command *cmd)
{
	if (!cmd)
		return;

	const char *name = cmd->getCmdName();
	if (!name)
		return;

	RgiCString str(name);
	cmdmap_[str] = cmd;
}

Command* CmdLookUp::findCommand(const RgiCString &str)
{
	Command *cmd = NULL;
	cmdmap_.lookUp(str,cmd);
	return cmd;
}

CmdLookUp theCmdLookUp;

int RgiCStringHash::
operator () (const RgiCString &str) const
{
	const char *buf = str.data();
	unsigned int num = 0;
	if (buf)
	{
		// TODO: no error handling !?
		for (const char *ptr = buf; *ptr; ptr++)
		{
			num += (unsigned int)(*ptr);
			// TODO: not 10 !?
			num *= 2;
		}
	}
	return num;
}

