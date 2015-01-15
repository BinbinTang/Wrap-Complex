// comment.h -- A class for commenting
//
// Usage:
// Whenever you want to "print" some information, use
// the stream notation
//
//			COMMENT (... << ... << ... << ...);
//
// An endl will be inserted at the end of this sequence of
// objects.
//
// Those who want to receive comment strings that are
// completed here should used register for the CommentCB
// callback as so:
//
//			theComment->addCallback (CommentHandler::CommentCB, ...)

#ifndef __RGI_COMMENT_H__
#define __RGI_COMMENT_H__

//#include "isdk/general/import.h"

#ifdef WIN32
//#include <strstrea.h>
#include <strstream>
#else
#include <strstream.h>
#endif

#include <basic/basic.h>
#include <basic/callbackobject.h>


class CommentHandler : public CallbackObject, public ostrstream
{
public:
	CommentHandler (void);
	~CommentHandler (void);

	enum { 
		CommentCB,
		LogCB,

		MaxCBs
	};

	void beginComment (void);
	ostream & getStream (void);    // Use this for your output
	void endComment (void);
	void endCommentAndLog (void);

	int doComments (void) { return commentsOn_; }
	void commentsOn (void) { commentsOn_ = TRUE; }
	void commentsOff (void) {commentsOn_ = FALSE; }

private:
	enum { MaxOutputSize=100000 };

	char * output_;
	int commentsOn_;
};

// This is actually defined in wrapgeni.cpp due to linking problems...
// --Mike
//extern INTERNAL_DLL CommentHandler theComment;

/*
#ifdef __DEBUG__
#ifdef WIN32
#define COMMENT(s) \
    do { \
	if (theComment.doComments ()) \
	{ \
	    theComment.beginComment (); \
	    theComment << s; \
	    theComment.endComment (); \
	} \
    } while (0)
#else
#define COMMENT(s) \
    do { \
	if (theComment.doComments ()) { \
	    cout << s << endl; \
	} \
    } while (0)
#endif

#define LOG(s) \
	do { \
	    theComment.beginComment (); \
	    theComment << s; \
	    theComment.endCommentAndLog (); \
	} while (0)

#define BEGINCOMMENT() \
	if (theComment.doComments ()) \
		theComment.beginComment ();

#define PRINTCOMMENT(s) \
	if (theComment.doComments ()) \
		theComment << s;

#define ENDCOMMENT() \
	if (theComment.doComments ()) \
		theComment.beginComment ();

#else
#define COMMENT(s)
#define LOG(s)
#define BEGINCOMMENT()
#define PRINTCOMMENT(s)
#define ENDCOMMENT()
#endif
*/

/* New since win32 is now a console app when it counts. -swp */
#ifdef __DEBUG__

#define COMMENT(s) \
	do { \
		/*if (theComment.doComments ()) */{ \
			cout << s << endl; \
		} \
	} while (0)
#define BEGINCOMMENT()
#define PRINTCOMMENT(s) COMMENT(s)
#define ENDCOMMENT()

#else

#define COMMENT(s)
#define LOG(s)
#define BEGINCOMMENT()
#define PRINTCOMMENT(s)
#define ENDCOMMENT()

#endif /* __DEBUG__ */

#endif
