#ifndef __RGI_VERTEXARRAY_H__
#define __RGI_VERTEXARRAY_H__

class RgiVector;
class RgiColor;

#include <basic/basic.h>
#include <basic/rgitypes.h>

class VertexArray
{
public:
	VertexArray (const void *ptr, FloatType type, int stride=0, int count=-1);
	VertexArray (const double *ptr, int stride=0, int count=-1);
	VertexArray (const float *ptr, int stride=0, int count=-1);
	VertexArray (const RgiVector *ptr, int stride=0, int count=-1);
	VertexArray (const RgiColor *ptr, int stride=0, int count=-1);
	VertexArray (const VertexArray &);
	VertexArray ();

	BOOL isEmpty () const;

	const void *data() const;
	FloatType type () const;
	int stride () const;
	int count () const;

	VertexArray &operator= (const VertexArray &);

	void lookUp (int idx, float *);
	void lookUp (int idx, double *);

private:
	const void *ptr_;
	FloatType type_;
	int stride_;
	int count_;
};

#endif
