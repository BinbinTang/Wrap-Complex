#ifndef __RGI_COMMAND_H__
#define __RGI_COMMAND_H__

#include <iostream>
#include <fstream>
#include <basic/rgistring.h>
#include <basic/callbackobject.h>
#include <basic/iterstack.h>

class VarLenTypedData;

//- Command
//. The top level command class whick supports undo, redo,
//. interrupt, execute.  After the command is executed, user
//. can call description() to get some info on what happened.
//. It can be saved on a command history list.
class Command : public CallbackObject
{
public:
	virtual ~Command();

	//- undo, canUndo
	virtual void undo ();
	virtual const RgiString &undoName();
	virtual int canUndo (); 
	//. undo() allow command to undo itself, canUndo() tells the user
	//. of the command if this command can be undone.
	//. default can not undo 

	//- redo, canRedo
	virtual void redo ();
	virtual int canRedo (); 
	//. redo() allow command to redo, canRedo() tells the user
	//. of the command if this command can be redone.
	//. default can not redo 

	//- execute
	virtual int  execute (Data *data) = 0; 
	//. execute the command

	//- description
	virtual const RgiString &description () const;
	//. This function returns a description of the last execution of
	//. the command

public:
	// function for user cmdline input

	virtual const char* getCmdName() const;
	virtual VarLenTypedData* getData();
	virtual int execute (VarLenTypedData *data);
	virtual const RgiString &getMessage();

protected: 
	Command ();

	void beginTime (void);
	void endTime (void);
	void outputTime (const char name []);

	static ofstream timeFile_;
	static int timeUsed_;
	static double realTime_, cpuTime_;
};

//- CompositeCommand
//. allows storage of a list of commands to be executed
class CompositeCommand : public Command
{
public:
	virtual ~CompositeCommand ();

	//- add, remove
	virtual void add (Command *);
	virtual Command * remove ();
	//. add() allows client to add a command to end of command list
	//. remove() removes the last command in the command list

	friend ostream&  operator<< (ostream& s, const CompositeCommand& p) ;

protected:
	 CompositeCommand() {};
	 
	 IterStack <Command*> cmds_;
};

#define SimpleCmdDecl(ClassName)  \
	class ClassName : public Command { public: int execute (Data *);};

#endif
