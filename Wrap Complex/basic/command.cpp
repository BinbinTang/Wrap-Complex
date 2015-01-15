#include "command.h"

Command::
Command (void)
{
}

Command::
~Command (void)
{

}

int Command::
canUndo (void)
{
  return 0;
}

void Command::
undo (void)
{

}

int Command::
canRedo (void)
{
  return 0;
}

void Command::
redo (void)
{

}

const RgiString &Command::
getMessage()
{
	static RgiString empty;
	return empty;
}

/*------------------------------------------------------------------*/

const RgiString &Command::
undoName()
{
	static RgiString empty;
	return empty;
}

const RgiString &Command::
description () const 
{
  return RgiString::null; 
}

const char* Command::
getCmdName() const 
{ 
  return NULL; 
}

VarLenTypedData* Command::
getData() 
{ 
  return NULL; 
}

int Command::
execute (VarLenTypedData *data) 
{ 
  return 0; 
}

