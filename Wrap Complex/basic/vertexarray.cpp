#include "basic.h"
#include "vertexarray.h"

static inline int realStride (int stride, FloatType type)
{
	if (stride) {
		return stride;
	}
	if (type == ::Float) {
		return 3 * sizeof (float);
	}
	return 3 * sizeof (double);
}

VertexArray::
VertexArray (const void *ptr, FloatType type, int stride, int count)
{
	Assert (stride >= 0);
	Assert (count >= -1);
	ptr_ = ptr;
	type_ = type;
	stride_ = realStride (stride, type_);
	count_ = count;
}

VertexArray::
VertexArray (const double *ptr, int stride, int count)
{
	Assert (stride >= 0);
	Assert (count >= -1);
	ptr_ = (const void *)ptr;
	type_ = ::Double;
	stride_ = realStride (stride, type_);
	count_ = count;
}

VertexArray::
VertexArray (const float *ptr, int stride, int count)
{
	Assert (stride >= 0);
	Assert (count >= -1);
	ptr_ = (const void *)ptr;
	type_ = ::Float;
	stride_ = realStride (stride, type_);
	count_ = count;
}

VertexArray::
VertexArray (const RgiVector *ptr, int stride, int count)
{
	Assert (stride >= 0);
	Assert (count >= -1);
	ptr_ = (const void *)ptr;
	type_ = ::Double;
	stride_ = realStride (stride, type_);
	count_ = count;
}

VertexArray::
VertexArray (const RgiColor *ptr, int stride, int count)
{
	Assert (stride >= 0);
	Assert (count >= -1);
	ptr_ = (const void *)ptr;
	type_ = ::Float;
	stride_ = realStride (stride, type_);
	count_ = count;
}

VertexArray::
VertexArray (const VertexArray &other)
{
	ptr_ = other.ptr_;
	type_ = other.type_;
	stride_ = other.stride_;
	count_ = other.count_;
}

VertexArray::
VertexArray ()
{
	ptr_ = (const void *)0;
}

BOOL VertexArray::
isEmpty () const
{
	return ptr_ == (const void *)0;
}

const void *VertexArray::
data() const
{
	return ptr_;
}

FloatType VertexArray::
type () const
{
	return type_;
}

int VertexArray::
stride () const
{
	return stride_;
}

int VertexArray::
count () const
{
	return count_;
}

void VertexArray::
lookUp (int idx, float *result)
{
	Assert ((idx >= 0) && ((count_ == -1) || (idx < count_)));
	Assert (ptr_);
	if (type_ == ::Float) {
		const float *src = (const float *) ((const char *)ptr_ + idx*stride_);
		result[0] = src[0];
		result[1] = src[1];
		result[2] = src[2];
	} else {
		const double *src = (const double *) ((const char *)ptr_ + idx*stride_);
		result[0] = (float) src[0];
		result[1] = (float) src[1];
		result[2] = (float) src[2];
	}
}

void VertexArray::
lookUp (int idx, double *result)
{
	Assert ((idx >= 0) && ((count_ == -1) || (idx < count_)));
	Assert (ptr_);
	if (type_ == ::Float) {
		const float *src = (const float *) ((const char *)ptr_ + idx*stride_);
		result[0] = src[0];
		result[1] = src[1];
		result[2] = src[2];
	} else {
		const double *src = (const double *) ((const char *)ptr_ + idx*stride_);
		result[0] = src[0];
		result[1] = src[1];
		result[2] = src[2];
	}
}

VertexArray &VertexArray::
operator = (const VertexArray &other)
{
	ptr_ = other.ptr_;
	type_ = other.type_;
	stride_ = other.stride_;
	count_ = other.count_;
	return *this;
}

