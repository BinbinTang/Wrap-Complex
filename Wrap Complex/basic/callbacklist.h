#ifndef __RGI_CALLBACKLIST_H__
#define __RGI_CALLBACKLIST_H__

#include "callbackobject.h"

class Data;

class  CallbackItem 
{
  friend class  CallbackList;
  friend class  CallbackObject;

  public:

     CallbackItem( CallbackObject *obj, int name,  CallbackMethod, Data * clientData);
     CallbackItem(int  name,  CallbackFunction, Data *clientData);
    ~ CallbackItem();

    void invoke( CallbackObject *, int, Data *);

  private:

     CallbackObject   *_obj;
    int               _name;
     CallbackFunction  _func;    
     CallbackMethod    _method;
    Data               *_clientData;
};

class  CallbackList 
{
 friend class  CallbackObject;

  public:

  int find(int  name,   CallbackObject *obj,  CallbackMethod, Data *clientData);
  int find(int  name,  CallbackFunction, Data *clientData);

  void add( CallbackItem *);
  void add( CallbackList *);
  void add(int  name,   CallbackObject *obj,  CallbackMethod, Data *clientData);
  void add(int  name,  CallbackFunction, Data *clientData);

  void invoke( CallbackObject *, int , Data *);
  int hasCallbacks(int );

  int size() { return (_numCallbacks);}

   CallbackItem *operator[] (int index) const { return (_callbacks[index]); }

  void remove( CallbackItem *);
  void remove(int name);
  void remove(int  name,  CallbackObject *obj,  CallbackMethod, Data *clientData);
  void remove( CallbackObject *obj);
  void remove (int  name,  CallbackFunction, Data *clientData);

   CallbackList();
  ~ CallbackList();

  private:

     CallbackItem   **_callbacks;
    int                _numCallbacks;
};


#endif
