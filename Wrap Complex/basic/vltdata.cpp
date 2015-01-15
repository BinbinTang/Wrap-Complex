#include "basic.h"
#include "vltdata.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////////////
// ParamData

ParamData::ParamData(int p, const char *n, VarType t)
{
	Assert(n != NULL);

	param = p;
	name = _strdup(n);
	type = t;
	value = NULL;
}

ParamData::~ParamData()
{
	free(name);
	free(value);
}

void ParamData::setValue(const char *v)
{
	if (value)
		free(value);
	value = _strdup(v);
}

//////////////////////////////////////////////////////////////////////////////
// VarLenTypedData

VarLenTypedData::VarLenTypedData()
{
}

VarLenTypedData::~VarLenTypedData()
{
	int size = params_.size();
	ParamData *data;
	for (int ndx = 1; ndx <= size; ndx++)
	{
		data = params_[ndx];
		delete data;
	}
	params_.reset();
}


int VarLenTypedData::addParam(const char *name, ParamData::VarType type)
{
	if (!name)
		return 0;

	ParamData *data = new ParamData(params_.size() + 1, name, type);
	params_.push(data);
	return params_.size();
}

int VarLenTypedData::numParams()
{
	return params_.size();
}

const char* VarLenTypedData::getName(int param)
{
	Assert(param <= params_.size());

	ParamData *data = params_[param];

	return data->name;
}

int VarLenTypedData::setParams(int num, IterStack<char*> *vals)
{
	if (num < 1 || vals == NULL)
		return 0;

	int size = params_.size();
	ParamData *data;
	for (int ndx = 1; ndx <= size && ndx <= num; ndx++)
	{
		data = params_[ndx];
		data->setValue((*vals)[ndx]);
	}

	return 1;
}

int VarLenTypedData::setParams(int num, IterObjStack<RgiCString> *vals)
{
	if (num < 1 || vals == NULL)
		return 0;

	int size = params_.size();
	ParamData *data;
	for (int ndx = 1; ndx <= size && ndx <= num; ndx++)
	{
		data = params_[ndx];
		data->setValue((*vals)[ndx].data());
	}

	return 1;
}

int VarLenTypedData::getParams(int param, void *data, ParamData::VarType type)
{
	if (param < 1 || param > params_.size() || data == NULL)
		return 0;

	ParamData *pdata = params_[param];
	if (pdata->type != type || pdata->value == NULL)
		return 0;

	switch (pdata->type)
	{
		case ParamData::Int:
			*((int*)data) = atoi(pdata->value);
			break;
		case ParamData::Char:
			*((char*)data) = pdata->value[0];
			break;
		case ParamData::String:
			strcpy_s((char*)data, strlen(pdata->value), pdata->value);
			//strcpy((char*)data, pdata->value);
			break;
		case ParamData::Double:
			*((double*)data) = atof(pdata->value);
			break;
		default:
			*((int*)data) = 0;
			break;
	}
	return (int)(pdata->type);
}

