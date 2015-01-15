#ifndef __RGI_STRINGTABLE_H__
#define __RGI_STRINGTABLE_H__

#include <string.h>
#include <basic/iterstack.h>

class StringTable
{
public:
  StringTable (void) { }
  StringTable (char **strings, int numStrings);
  ~StringTable (void) { }

  int size (void) { return stringStack_.size (); }
  int addString (char *newString);
  int findString (const char *myString);

private:
  IterStack <char *> stringStack_;
};



#endif // __STRINGTABLE_H__
