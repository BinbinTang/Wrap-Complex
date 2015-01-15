#ifndef __RGI_CALLBACKOBJECT_H__
#define __RGI_CALLBACKOBJECT_H__

#include <basic/basic.h>
class  CallbackObject;
class  CallbackObjectList;
class  CallbackList;
class Data;

class RgiObject
{
public:
  virtual ~RgiObject (void);
};

typedef void (* CallbackFunction) ( CallbackObject *caller, Data *clientData, Data *callData);
typedef void ( CallbackObject::* CallbackMethod) ( CallbackObject *caller, Data *clientData, Data *callData);

#define  AddCallbackFunction(name, you, func, clientData) ( (you)->addCallback(name, func, clientData))
#define  AddCallbackMethod(name, you, me, func, clientData) ( (you)->addCallback(name, (me), ( CallbackMethod) func, clientData))

#define  RemoveCallbackFunction(name, you, func, clientData) ( (you)->removeCallback(name, func, clientData))
#define  RemoveCallbackMethod(name, you, me, func, clientData) ( (you)->removeCallback(name, (me), ( CallbackMethod) func, clientData))

class  CallbackObject : public RgiObject
{
 public:

    virtual ~CallbackObject();
    virtual const char *className();

    void addCallback(int , 
		      CallbackFunction, 
		     Data *clientData = 0);
    void addCallback(int , 
		      CallbackObject *, 
		      CallbackMethod, 
		     Data *clientData = 0);

    void removeCallback(int , 
			 CallbackFunction, 
			Data *clientData = 0);
    void removeCallback(int , 
			 CallbackObject *, 
			 CallbackMethod,
			Data *clientData = 0);
    void removeAllCallbacks (int);
    void removeAllCallbacks();
    void removeAllCallbacks( CallbackObject *);

    int hasCallbacks(int) const;

//    void cloneCallbacks( CallbackObject *);
//    void cloneCallback(int ,  CallbackObject *);
    void cloneCallback(int , int ,
		        CallbackObject *);

  protected:

    void callCallbacks (int, Data *);
     CallbackObject( );     // Protected constructor forces subclasses to redefine
     // avoid implicit pointer copying
     CallbackObject(CallbackObject& other);
     CallbackObject& operator= (CallbackObject& other);

  private:

     CallbackList        *_callbacks;
     CallbackObjectList  *_senders;

    void registerSender( CallbackObject *); 
    void unregisterSender( CallbackObject *); 
    void unregisterSenders( CallbackObject *); 
    void cleanUp_ (void);
};

class ProgressCallbackObject : public CallbackObject
{
public:
  enum { PROGRESS_CB = 5000 };
  ProgressCallbackObject (void) {};
  ProgressCallbackObject (ProgressCallbackObject &other) {};
  ProgressCallbackObject& operator= (ProgressCallbackObject &other) {return *this;};
  void setProgressBounds (int percentBegin, int percentEnd) {};
  void getProgressBounds (int& percentBegin, int& percentEnd) {};
  void setProgressCB (CallbackObject * obj, CallbackMethod method) {};
  void setProgressCBto (ProgressCallbackObject * to) {};
  void setProgressCBfrom (ProgressCallbackObject * from) {};
  Bool progressBegin (void) { return true;};
  Bool progressUpdate (float fractionDone) { return true;};
  Bool progressEnd (void) { return true;};
private:
  int valBegin_, valEnd_, valDone_;
};

#endif

