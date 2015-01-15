//
// Copyright 1996-97. Raindrop Geomagic, Inc.
// All Rights Reserved.
//
// This is unpublished proprietary source code of Raindrop Geomagic, Inc.; 
// the contents of this file may not be disclosed to third parties, copied 
// or duplicated in any form, in whole or in part, without the prior written 
// permission of Raindrop Geomagic, Inc.  
//
// This copyright notices may not be removed from this file at any time.
// 
// RESTRICTED RIGHTS LEGEND:
// Use, duplication or disclosure by the Government is subject to restrictions
// as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
// and Computer Software clause at DFARS 252.227-7013, and/or in similar or
// successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished-
// rights reserved under the Copyright Laws of the United States.
// 
//                 Raindrop Geomagic, Inc
//                 206 N. Randolph Street, Suite 520
//                 Champaign IL 61820-3898
//                 Phone: 217-239-2551
//                 Fax:   217-239-2556
//                 Email: inquiry@geomagic.com
// 
// 

#ifndef __RGI_HISTORY_H__
#define __RGI_HISTORY_H__

#include <basic/iterstack.h>
#include <basic/freearray.h>

//----------------------------------------------------------------------------

class HistoryAttribute;
class HistoryList;
class ProgressCallbackObject;

//----------------------------------------------------------------------------

#define GET_ATTRIBUTE(he, className) \
        (className *) he->getAttribute (className::getClassAttribId())

#define SET_ATTRIBUTE(he, attr) \
         he->setAttribute (attr)


class HistoryAttribute
{
public:
  HistoryAttribute (void) { }
  virtual ~HistoryAttribute (void);
  virtual int getAttribId (void);
  static  int getClassAttribId (void);
};



//----------------------------------------------------------------------------

class HistoryElement
{
public:

  HistoryElement (void) { time_ = -1; id_ = 0; attribPtr_ = NULL; };
  HistoryElement (HistoryList * hl, FilePointer & s);
  virtual ~HistoryElement (void);

  virtual void apply (void);
  virtual void updateStatus (void);
  virtual void unApply (void);
  virtual void eraseFromHistory (void);

  virtual int binWrite (FilePointer & s);

  int getTime (void) { return time_; }
  void setTime (int time) { time_ = time; }
  int isInstance (void) { return isInstance_; }
  int getId (void) { return id_; }

  void * getAttribute (int attribId); 
  void setAttribute (HistoryAttribute * attribPtr);

  // every subclass HESub of HistoryElement should define the following 
  // static method (it is required if the element can be read from file):
  //
  //   static HistoryElement * construct (void * data, FilePointer & s)
  //                                     { return new HESub (data, s); }
  //
  // where data is either NULL, a pointer to a HistoryList, a pointer to
  // a subclass of HistoryList, or a pointer to some data element; 
  // depending on how the specific implementation of HistoryList and
  // the elements stored in the history list handle commom data.

protected:

  int isInstance_;
  int time_;
  int id_;
  HistoryAttribute * attribPtr_;
};

//----------------------------------------------------------------------------

class HistoryInstance : public HistoryElement
{
public:

  HistoryInstance (void) { isInstance_ = 1; };
  HistoryInstance (HistoryList * hl, FilePointer & s) 
    : HistoryElement (hl, s) {};
};

//----------------------------------------------------------------------------

class HistoryOperation : public HistoryElement
{
public:

  HistoryOperation (void) { isInstance_ = 0; };
  HistoryOperation (HistoryList * hl, FilePointer & s) 
    : HistoryElement (hl, s) {};
};

//----------------------------------------------------------------------------

typedef struct
{
  int id;

  // the following is a substitute for:
  //
  //     HistoryElement (*hep) (MyHistoryList *, FilePointer &);
  //
  // where MyHistoryList is a subclass of HistoryList. This solution
  // avoids downcasting in the "construct" methods of history elements.

  HistoryElement * (*construct) (void *, FilePointer &);
}
HistoryConstructorType;

//----------------------------------------------------------------------------

class HistoryList
{
public:

  HistoryList (void);
  virtual ~HistoryList (void);
  virtual void append (HistoryElement * he);
  virtual int gotoTime (int time);
  Bool undo (void);
  Bool redo (void);
  Bool canUndo (void);
  Bool canRedo (void);
  virtual int binWrite (FilePointer & s);
  virtual int binRead (void * data, FilePointer & s);

  void deleteAllMoreRecent (void);
  int timeCurrent (void);
  int timeNext (void);
  int timeMax (void);
  Bool playbackMode (void);
  Bool instanceExists (int time);
  Bool isModified (void);
  int nofOperations (void);
  void reset (void);

  Bool isHistoryActive (void);
  void setHistoryActive (Bool mode);
  virtual void clearHistory (void);

  int getCurrentReadId (void) { return currentReadId_; }

protected:

  IterStack<HistoryElement *> hOpList_;
  FreeArray<HistoryElement *> hInstList_;
  Bool playBack_;

  void addConstructor (int id, HistoryElement * (*construct) 
		                                (void *, FilePointer &));

protected:

  int currentIndex_;
  Bool isModified_;
  int currentReadId_;
  Bool historyActive_;
  IterStack<HistoryConstructorType> Constructors_;
  ProgressCallbackObject * progressObject_;

  void gotoTimeInstance_ (int time);
  void deleteAllMoreRecentOperations_ (void);
  void deleteAllMoreRecentInstances_ (void);
  void appendOperation_ (HistoryElement * op);
  void appendInstance_ (HistoryElement * inst);
  int findLargestInstance_ (int time);
  int findIdIndex_ (int id);
  HistoryElement * getNextOperation_ (void);
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif // __HISTORY_H__
