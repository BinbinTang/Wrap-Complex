#include <assert.h>
#include <ctype.h>
#include <iostream>
#include <stdlib.h>
#include <basic/basic.h>
#include "rgitranslator.h"

RgiTranslator *theTranslator = 0;

RgiTranslator::
RgiTranslator ()
{
}

RgiString RgiTranslator::
get (const char *id)
{
	RgiString result = lookUp (id);
	expectArgs (result, 0);
	return result;
}

RgiString RgiTranslator::
get (const char *id, const RgiStringAdaptor &arg1)
{
	RgiString result = lookUp (id);
	expectArgs (result, 1);
	result = replaceArg (result, 1, arg1.string());
	return result;
}

RgiString RgiTranslator::
get (const char *id, const RgiStringAdaptor &arg1, const RgiStringAdaptor &arg2)
{
	RgiString result = lookUp (id);
	expectArgs (result, 2);
	result = replaceArg (result, 1, arg1.string());
	result = replaceArg (result, 2, arg2.string());
	return result;
}

RgiString RgiTranslator::
get (const char *id, const RgiStringAdaptor &arg1, const RgiStringAdaptor &arg2,
	const RgiStringAdaptor &arg3)
{
	RgiString result = lookUp (id);
	expectArgs (result, 3);
	result = replaceArg (result, 1, arg1.string());
	result = replaceArg (result, 2, arg2.string());
	result = replaceArg (result, 3, arg3.string());
	return result;
}

RgiString RgiTranslator::
get (const char *id, const RgiStringAdaptor &arg1, const RgiStringAdaptor &arg2,
	const RgiStringAdaptor &arg3, const RgiStringAdaptor &arg4)
{
	RgiString result = lookUp (id);
	expectArgs (result, 4);
	result = replaceArg (result, 1, arg1.string());
	result = replaceArg (result, 2, arg2.string());
	result = replaceArg (result, 3, arg3.string());
	result = replaceArg (result, 4, arg4.string());
	return result;
}

RgiString RgiTranslator::
get (const char *id, int size, const RgiStringAdaptor *args)
{
	RgiString result = lookUp (id);
	expectArgs (result, size);
	for (int i=0; i<size; i++) {
		result = replaceArg (result, i+1, args[i].string());
	};
	return result;
}

RgiString RgiTranslator::
lookUp (const char *id)
{
	return RgiString (id);
}

void RgiTranslator::
expectArgs (const RgiString &string, int count) const
{
	int len = string.length();
	for (int i=0; i<len-1; i++) {
		if ((string[i] == '%') && isdigit (string[i+1])) {
			count--;
		}
	}
	assert (count == 0);
}

RgiString RgiTranslator::
replaceArg (const RgiString &string, int idx, const RgiString &arg) const
{
	int i, key = '0' + idx, len = string.length();
	for (i=0; i<len-1; i++) {
		if ((string[i] == '%') && (string[i+1] == key)) {
			break;
		}
	}

	assert (i < (len-1));
	RgiString result = string.replace (i, i+2, arg);
	return result;
}

