#include <assert.h>
#include <string.h>
#include <malloc.h>

#include "callbackobject.h"
#include "callbacklist.h"
#include "data.h"


#if defined (DEC)
	extern "C"  
	{
		void free(void *);
		 void *malloc(size_t);
	}
#endif

#ifndef NULL
#define NULL 0
#endif

CallbackItem:: 
CallbackItem ( CallbackObject *obj,  int name, 
	CallbackMethod func,  Data *clientData)
{
    _obj        = obj;
    _name       = name;
    _method     = func;
    _func       = NULL;
    _clientData = clientData;
}

CallbackItem:: 
CallbackItem(int name,  CallbackFunction func, Data *clientData)
{
    _obj        = NULL;
    _name       = name;
    _func       = func;
    _method     = NULL;
    _clientData = clientData;
}

CallbackItem::
~ CallbackItem()
{
}

void  CallbackItem::
invoke ( CallbackObject *self, int name, Data *callData)
{
	if (_name !=  name)
		return;
       
    if(_func)
		(*_func)(self, _clientData, callData);
    else if(_obj != 0 && _method != 0)
		(_obj->*_method)(self, _clientData, callData);
}

void  CallbackList::
add( CallbackItem * item)
{
    if(!_callbacks)
    {
		_callbacks = new  CallbackItem*[1];
		_numCallbacks = 1;
		_callbacks[0] = item;
    }
    else
    {
		CallbackItem   **newlist;

		newlist =  new  CallbackItem*[_numCallbacks + 1];

		for(int i = 0; i< _numCallbacks; i++)
			newlist[i] = _callbacks[i];

		newlist[_numCallbacks] = item;

		_numCallbacks++;

		delete [] _callbacks;

		_callbacks = newlist;
    }
}

void  CallbackList::
add( CallbackList *list)
{
  for(int i = 0;  i < list->_numCallbacks;  i++)
    add(list->_callbacks[i]);
}

void  CallbackList::
add(int name,  CallbackObject *obj,  CallbackMethod m, Data *clientData)
{
  if (find(name, obj, m, clientData) == -1)
	  add(new  CallbackItem(obj, name, m, clientData));
}

void  CallbackList::
add(int name,  CallbackFunction f, Data *clientData)
{
  if (find (name, f, clientData) == -1)
    add(new  CallbackItem(name, f, clientData));
}


void  CallbackList::
remove( CallbackItem *item)
{
   CallbackItem   **newlist;
	int found = 0;
    int i;

	for(i = 0; i < _numCallbacks; i++)
		 if(_callbacks[i] == item)	  
			found = 1;

	assert(found != 0);

	if(_numCallbacks <= 1)
	{
		delete _callbacks[0];
	    delete [] _callbacks;
	    _callbacks = NULL;
	    _numCallbacks = 0;
	    return;
	}
	else
	{
	    newlist =  new  CallbackItem*[_numCallbacks - 1];
		CallbackItem *c = 0;

	    int count = 0;
	    for(i = 0; i < _numCallbacks; i++)
		{
			if(_callbacks[i] != item)
				newlist[count++] = _callbacks[i];
			else
				c = _callbacks[i];
		}

	    _numCallbacks--;

		delete c;
	    delete [] _callbacks;

	    _callbacks = newlist;    
	}
}

void  CallbackList::
remove( CallbackObject *callee)
{
    for(int i = _numCallbacks -1; i >= 0; i--)
	if(_callbacks[i]->_obj == callee )	  
	    remove(_callbacks[i]);
}


void  CallbackList::
remove(int name,  CallbackObject *callee,  
	   CallbackMethod method, Data *clientData)
{
  /*
    for(int i = _numCallbacks - 1; i >= 0; i--)
	if ((_callbacks[i]->_obj  == callee) &&
	    (_callbacks[i]->_name == name) &&
	    (_callbacks[i]->_method == method)  &&
	    (_callbacks[i]->_clientData == clientData))
			remove(_callbacks[i]);
      */
  int i = find (name, callee, method, clientData);
  if (i >= 0)
    remove(_callbacks[i]);

}

int CallbackList::
find (int name,  CallbackObject *callee,  
	   CallbackMethod method, Data *clientData)
{
  for(int i = _numCallbacks - 1; i >= 0; i--)
	if ((_callbacks[i]->_obj  == callee) &&
	    (_callbacks[i]->_name == name) &&
	    (_callbacks[i]->_method == method)  &&
	    (_callbacks[i]->_clientData == clientData))
			return i;

  return -1;
}

void  CallbackList::
remove(int name)
{
  for(int i = _numCallbacks - 1; i >= 0; i--)
	if (_callbacks[i]->_name == name)
			 remove(_callbacks[i]);
}


void  CallbackList::
remove(int name,  CallbackFunction func, Data *clientData)
{
  /*
    for(int i = _numCallbacks - 1; i >= 0; i--)
	if((_callbacks[i]->_obj == 0)     &&
	   (_callbacks[i]->_name == name) &&
	   (_callbacks[i]->_func == func)  &&
	   (_callbacks[i]->_clientData == clientData))
			remove(_callbacks[i]);
      */
  int i = find (name, func, clientData);
  if (i >= 0)
    remove(_callbacks[i]);
}

int CallbackList::
find (int name,  CallbackFunction func, Data *clientData)
{
  for(int i = _numCallbacks - 1; i >= 0; i--)
	if((_callbacks[i]->_obj == 0)     &&
	   (_callbacks[i]->_name == name) &&
	   (_callbacks[i]->_func == func)  &&
	   (_callbacks[i]->_clientData == clientData))
			return i;

  return -1;
}


void  CallbackList::
invoke( CallbackObject *caller, int name, Data * callData)
{
    for(int i=0; i< _numCallbacks; i++)
		_callbacks[i]->invoke(caller, name, callData);
}

 CallbackList:: 
 CallbackList()
{
    _callbacks = NULL;
    _numCallbacks = 0;
}

 CallbackList::
~ CallbackList()
{
	int j;
  for(int i=0;i< _numCallbacks; i++)
	{
		j = _numCallbacks;
		delete _callbacks[i];
	}

    delete [] _callbacks;
}

int  CallbackList::
hasCallbacks(int name)
{
    if(_callbacks)
	for(int i=0; i< _numCallbacks; i++)
	    if(_callbacks[i]->_name ==  name)
		return 1;

    return 0;
}
