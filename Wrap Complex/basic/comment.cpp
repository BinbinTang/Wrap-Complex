#include "comment.h"
#include "data.h"

#define USE_COUT 1

CommentHandler:: 
CommentHandler (void)
:  ostrstream ()
{
#ifdef __DEBUG__
  commentsOn_ = TRUE;
#else
	commentsOn_ = FALSE;
#endif
}

CommentHandler::
~CommentHandler (void)
{
}

void CommentHandler::
beginComment (void)
{
	seekp (0);
}

void CommentHandler::
endComment (void)
{
	put ('\0');
	char* myString = str();
	CStringData sdata(myString);

#ifdef USE_COUT
  cout << myString;
#else
	callCallbacks(CommentCB, &sdata);
	rdbuf()->freeze(0);
	seekp (0);
#endif
}


void CommentHandler::
endCommentAndLog (void)
{
	put ('\0');
	char* myString = str();
	CStringData sdata(myString);

	callCallbacks(LogCB, &sdata);
	rdbuf()->freeze(0);
	seekp (0);
}
