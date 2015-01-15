#include "basic/stringtable.h"


StringTable::
StringTable (char **strings, int numStrings)
{
  for (int i = 0; i < numStrings; i++)
    addString (strings[i]);
}


int StringTable::
addString (char *newString)
{
  stringStack_.push (newString);
  return stringStack_.size ();
}

int StringTable::
findString (const char *myString)
{
  int index = 1;
      
  while ((index <= stringStack_.size ()) &&
	 (strcmp (myString, stringStack_[index])))
    index++;

  if (index >  stringStack_.size ())
    return 0;
  else
    return index;
}
	     
