#ifndef __RGI_RGISTRINGTABLE_H__
#define __RGI_RGISTRINGTABLE_H__

#include "rgistring.h"

class RgiTranslator {
public:
	RgiTranslator ();

	RgiString get (const char *);
	RgiString get (const char *, const RgiStringAdaptor &);
	RgiString get (const char *, const RgiStringAdaptor &,
		const RgiStringAdaptor &);
	RgiString get (const char *, const RgiStringAdaptor &,
		const RgiStringAdaptor &, const RgiStringAdaptor &);
	RgiString get (const char *, const RgiStringAdaptor &,
		const RgiStringAdaptor &, const RgiStringAdaptor &,
		const RgiStringAdaptor &);

	RgiString get (const char *, int, const RgiStringAdaptor *);

protected:
	virtual RgiString lookUp (const char *);

	void expectArgs (const RgiString &, int) const;
	RgiString replaceArg (const RgiString &, int idx, const RgiString &) const;
};

extern RgiTranslator *theTranslator;

inline RgiString i18n (const char *id)
{
	return theTranslator->get (id);
}

inline RgiString i18n (const char *id, const RgiStringAdaptor &arg1)
{
	return theTranslator->get (id, arg1);
}

inline RgiString i18n (const char *id, const RgiStringAdaptor &arg1,
	const RgiStringAdaptor &arg2)
{
	return theTranslator->get (id, arg1, arg2);
}

inline RgiString i18n (const char *id, const RgiStringAdaptor &arg1,
	const RgiStringAdaptor &arg2, const RgiStringAdaptor &arg3)
{
	return theTranslator->get (id, arg1, arg2, arg3);
}

inline RgiString i18n (const char *id, const RgiStringAdaptor &arg1,
	const RgiStringAdaptor &arg2, const RgiStringAdaptor &arg3,
	const RgiStringAdaptor &arg4)
{
	return theTranslator->get (id, arg1, arg2, arg3, arg4);
}

inline RgiString i18n (const char *id, int size, const RgiStringAdaptor *args)
{
	return theTranslator->get (id, size, args);
}

#endif
