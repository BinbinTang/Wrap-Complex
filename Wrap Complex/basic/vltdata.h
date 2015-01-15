#ifndef __RGI_VLTDATA_H__
#define __RGI_VLTDATA_H__

#include <basic/iterstack.h>
#include <basic/rgicstring.h>

class ParamData
{
public:
	enum VarType { Int, Char, String, Double };

	int param;
	char *name;
	VarType type;
	char *value;

	ParamData(int p, const char *n, VarType t);
	~ParamData();
	void setValue(const char *v);

	friend class VarLenTypedData;
};

class VarLenTypedData
{
	IterStack<ParamData*> params_;
public:
	
	VarLenTypedData();
	~VarLenTypedData();

public:
	int addParam(const char *name, ParamData::VarType type);
	int numParams();
	const char* getName(int param);
	int setParams(int num, IterStack<char*> *vals);
	int setParams(int num, IterObjStack<RgiCString> *vals);
	int getParams(int param, void *data, ParamData::VarType type);
};


#endif //VLTDATA_H
