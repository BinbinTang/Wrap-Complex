#include <malloc.h>
#include <string.h>

#ifdef __DEBUG__
	#include <iostream>
#endif

#include <basic/callbackobject.h>
#include <basic/callbacklist.h>
#include <basic/data.h>


void memStat (void)
{
#ifdef __DEBUG__
#ifdef WIN32
  MEMORYSTATUS memStat;
  GlobalMemoryStatus (&memStat);
  double memUsageBytes = memStat.dwTotalPageFile - memStat.dwAvailPageFile;
  double memUsageKs =   memUsageBytes/1024.0;
  double memUsageMegs = memUsageBytes/1048576.0;
#endif
#endif
}

RgiObject::
~RgiObject (void)
{

}


// Private utility class

class  CallbackObjectList
 {    
  private:  
     CallbackObject  **_objects;
    int                 _numObjects;

  public:
      CallbackObjectList ( );
     virtual ~ CallbackObjectList ( );
     virtual void add( CallbackObject *);
     virtual void remove( CallbackObject *);
     virtual void removeFirst( CallbackObject *);
     int size() const { return _numObjects; }
      CallbackObject *operator[]  (int index)   const;    
};

 CallbackObjectList::
 CallbackObjectList ( )
{
    _objects    = NULL;
    _numObjects = 0;
}

 CallbackObjectList::
~ CallbackObjectList ( )
{
    if(_objects)
		delete _objects;

    _objects    = NULL;
    _numObjects = 0;
}

void  CallbackObjectList::
add( CallbackObject *comp)
{
    int i;

	CallbackObject **newlist = new  CallbackObject*[_numObjects +1];

    for(i = 0; i < _numObjects; i++)
	newlist[i] = _objects[i];

    newlist[_numObjects] = comp;

    delete [] _objects;
    _objects = newlist;

    _numObjects++;
}

void  CallbackObjectList::
remove( CallbackObject *comp)
{
    int numFound;
    int i;
    numFound = 0;

    for(i = 0; i < _numObjects; i++)
	if(comp == _objects[i])
	    numFound++;

    if(numFound == 0)
		return;

    if((_numObjects - numFound) <= 0)
    {
		delete _objects;
		_objects = NULL;
		_numObjects = 0;
		return;
    }
    else
    {
		 CallbackObject **newlist = new  CallbackObject*[_numObjects - numFound];

		int count  = 0;
		for(i = 0; i < _numObjects; i++)
			if(comp != _objects[i])
				newlist[count++] = _objects[i];

		delete _objects;
		_objects = newlist;

		_numObjects -= numFound;
    }
}

void  CallbackObjectList::
removeFirst( CallbackObject *comp)
{
    int numFound;
    int i;
    numFound = 0;

    for(i = 0; i < _numObjects; i++)
	if(comp == _objects[i])
	    numFound++;

    if(numFound == 0)
		return;


    if(_numObjects <= 1)
    {
		delete _objects;
		_objects = NULL;
		_numObjects = 0;
		return;
    }
    else
    {
		for(i = 0; i < _numObjects; i++)
		{
			if(comp == _objects[i])
			{
				for(int j = i; j < _numObjects - 1; j++)
					_objects[j] = _objects[j+1];
		
				break;
			}
		}

		_numObjects -= 1;
	}
}

 CallbackObject * CallbackObjectList::
operator[] (int index) const
{
    if (index < 0 || index >= _numObjects)
		return NULL;
    return ( _objects [index] );
}



/////////////////////////////////////////////////
// CallbackObject class
//////////////////////////////////////////////////


CallbackObject::
CallbackObject(void)
 : _callbacks(NULL), _senders(NULL)
{
  
}

CallbackObject::
CallbackObject(CallbackObject& other)
 : _callbacks(NULL), _senders(NULL)
{
  
}

CallbackObject& CallbackObject::
operator= (CallbackObject& other)
{
  cleanUp_();
  return *this;
}


 CallbackObject::
~ CallbackObject()
{
  cleanUp_();
}

void CallbackObject::
cleanUp_ (void)
{
   // Remove any callbacks we have registered with other objects

    if(_senders)
    {
		for(int i = 0; i < _senders->size(); i++)
		{
			 CallbackObject *sender = (*_senders)[i];

			if(sender->_callbacks)
			sender->_callbacks->remove(this); 
		}
	
		delete _senders;
		_senders = NULL;
    }

    // Remove any callbacks others have registered with us

    removeAllCallbacks();

    if(_callbacks)
		delete _callbacks;
    _callbacks = NULL;
}



const char*  CallbackObject::
className()
{
    return " CallbackObject";
}


void  CallbackObject::
addCallback(int name,  CallbackFunction func, Data *clientData)
{
    if(!_callbacks)
		_callbacks = new  CallbackList();

    _callbacks->add(name, func, clientData);

}

void  CallbackObject::
addCallback(int name,  CallbackObject *obj,  CallbackMethod method, 
			Data *clientData)
{
    if(!_callbacks)
		_callbacks = new  CallbackList();

    _callbacks->add(name, obj, method, clientData);

    obj->registerSender(this);
}

void  CallbackObject::
removeCallback(int name,  CallbackFunction func, Data *clientData)
{
    if(_callbacks)
		_callbacks->remove(name, func, clientData); 

}

void  CallbackObject::
removeAllCallbacks(int name)
{
    if(!_callbacks)
      return;

	//	_callbacks->remove(name); 
    for(int i =  _callbacks->size() - 1; i >= 0 ; i--)
		{
			 CallbackItem *item = (*_callbacks)[i];

       if (item->_name != name)
         continue;
			if(item->_obj == NULL)	  
				removeCallback(name, item->_func, item->_clientData);
			else
				removeCallback(name, item->_obj, item->_method, item->_clientData);
		}

}

void   CallbackObject::
removeCallback(int name,  CallbackObject *obj,  CallbackMethod method, 
			   Data *clientData)
{
    if(_callbacks)
		_callbacks->remove(name, obj, method, clientData);

    obj->unregisterSender(this);
}

void  CallbackObject::
removeAllCallbacks()
{
    if(!_callbacks)
      return;

		for(int i =  _callbacks->size() - 1; i >= 0 ; i--)
		{
			 CallbackItem *item = (*_callbacks)[i];

			if(item->_obj == NULL)	  
				removeCallback(item->_name, item->_func, item->_clientData);
			else
				removeCallback(item->_name, item->_obj, item->_method, item->_clientData);
		}

    delete _callbacks;
    _callbacks = NULL;
}

void  CallbackObject::
removeAllCallbacks( CallbackObject *obj)
{
    if(_callbacks)
		_callbacks->remove(obj); 

    obj->unregisterSenders(this);
}

void  CallbackObject::
callCallbacks (int  name, Data * callData )
{
    if(_callbacks)
		_callbacks->invoke(this, name,  callData);
}


void  CallbackObject::
registerSender( CallbackObject *obj)
{
    if(!_senders)
		_senders = new  CallbackObjectList();

    _senders->add(obj);
}

void  CallbackObject::
unregisterSender( CallbackObject *obj)
{
    if(_senders)
		_senders->removeFirst(obj);
}

void  CallbackObject::
unregisterSenders( CallbackObject *obj)
{
    if(_senders)
		_senders->remove(obj);
}

int  CallbackObject::
hasCallbacks(int name) const
{
    if(_callbacks)
		return _callbacks->hasCallbacks(name);
    else
		return 0;
}
/*
void  CallbackObject::
cloneCallbacks( CallbackObject *clone)
{
    // Assign all callbacks currently registered with this object 
    // to the "clone"

    if(_callbacks && _callbacks->size() > 0)
		for(int i = 0; i < _callbacks->size(); i++)
		{
		     CallbackItem *item = (*_callbacks)[i];
	    
			if(item->_obj == NULL)
				clone->addCallback(item->_name, item->_func, item->_clientData);
			else
				clone->addCallback(item->_name, item->_obj, item->_method, item->_clientData);
		}
}

void  CallbackObject::
cloneCallback(int  name,  CallbackObject *clone)
{
    cloneCallback(name, name, clone);
}
*/
void  CallbackObject::
cloneCallback(int  name, int  clone_name,
				      CallbackObject *clone)
{
    // Assign all callbacks currently registered with this object 
    // to the "clone"

    if(_callbacks)
		for(int i = 0; i < _callbacks->size(); i++)
		{
			 CallbackItem *item = (*_callbacks)[i];
	    
			if(name ==  item->_name)
			if(item->_obj == NULL)
				clone->addCallback(clone_name, item->_func, item->_clientData);
			else
				clone->addCallback(clone_name, item->_obj, item->_method, item->_clientData);
		}
}


/*-------------------------------------------------------------------------------*/
/*
ProgressCallbackObject::
ProgressCallbackObject (void) :
                 valBegin_(0), 
                 valEnd_(1000),
                 valDone_(0)
{ 

}

ProgressCallbackObject::
ProgressCallbackObject (ProgressCallbackObject &other) :
                 valBegin_(0), 
                 valEnd_(1000),
                 valDone_(0)
{ 

}

ProgressCallbackObject& ProgressCallbackObject::
operator= (ProgressCallbackObject& other)
{
  valBegin_ = 0, valEnd_ = 1000, valDone_ = 0;
  return *this;
}
*/

/*-------------------------------------------------------------------------------*/
/*
void ProgressCallbackObject::
setProgressBounds (int percentBegin, int percentEnd)
{ 
  valBegin_ = (percentBegin >= 0)? 10*percentBegin: valBegin_;
  valEnd_ = (percentEnd >= 0)? 10*percentEnd: valEnd_;
  if (valBegin_ > 1000) valBegin_ = 1000;
  if (valEnd_ > 1000) valEnd_ = 1000;
  if (valBegin_ > valEnd_) valBegin_ = valEnd_;
}

/*-------------------------------------------------------------------------------*/
/*
void ProgressCallbackObject::
getProgressBounds (int& percentBegin, int& percentEnd)
{
  percentBegin = valBegin_/10;
  percentEnd = valEnd_/10;
}

/*-------------------------------------------------------------------------------*/
/*
void ProgressCallbackObject::
setProgressCB (CallbackObject * obj, CallbackMethod method)
{
  removeAllCallbacks (PROGRESS_CB);
  addCallback(PROGRESS_CB, obj, method);
}

/*-------------------------------------------------------------------------------*/
/*
void ProgressCallbackObject::
setProgressCBto (ProgressCallbackObject * to)
{
  if (to == NULL)
    return;

  to->removeAllCallbacks (PROGRESS_CB);
  cloneCallback(PROGRESS_CB, PROGRESS_CB, to);
  to->valBegin_ = valBegin_;
  to->valEnd_ = valEnd_;
  to->valDone_ = 0;
}

/*-------------------------------------------------------------------------------*/
/*
void ProgressCallbackObject::
setProgressCBfrom (ProgressCallbackObject * from)
{
  if (from == NULL)
    return;

  from->setProgressCBto (this);
}

/*-------------------------------------------------------------------------------*/
/*
Bool ProgressCallbackObject::
progressUpdate (float fractionDone)
{
  fractionDone = (fractionDone < 0)? 0: ((fractionDone > 1)? 1: fractionDone);
  int valDone = valBegin_ + 
      (int) ((valEnd_ - valBegin_)*fractionDone);
  if (valDone == valDone_)
    return TRUE;
  valDone_ = valDone;
  ProgressData data(valDone_/10);
  callCallbacks (PROGRESS_CB, &data);
  return !data.cancelled();
}

/*-------------------------------------------------------------------------------*/
/*
Bool ProgressCallbackObject::
progressBegin (void)
{
  valDone_ = 0;
  ProgressData data(0);
  callCallbacks (PROGRESS_CB, &data);
  return !data.cancelled();
}

/*-------------------------------------------------------------------------------*/
/*
Bool ProgressCallbackObject::
progressEnd (void)
{
  valDone_ = 0;
  ProgressData data(0);
  callCallbacks (PROGRESS_CB, &data);
  return !data.cancelled();
}

*/