#include <basic/dumpable.h>

Bool IntRArray::
binInit (BinInput& bi)
{
  n_ = bi.readInt();
  if (n_ < 0)
    return FALSE;
  data_ = (int *) realloc(data_, n_ * sizeof(int));
  return bi.readInts(data_, n_);
}

void IntRArray::
binDump (BinOutput& bo)
{
  bo.writeInt(n_);
  bo.writeInts(data_, n_);
}

Bool DoubleRArray::
binInit (BinInput& bi)
{
  n_ = bi.readInt();
  if (n_ < 0)
    return FALSE;
  data_ = (double *) realloc(data_, n_ * sizeof(double));
  return bi.readDoubles(data_, n_);
}

void DoubleRArray::
binDump (BinOutput& bo)
{
  bo.writeInt(n_);
  bo.writeDoubles(data_, n_);
}

Bool FloatRArray::
binInit (BinInput& bi)
{
  n_ = bi.readInt();
  if (n_ < 0)
    return FALSE;
  data_ = (float *) realloc(data_, n_ * sizeof(float));
  return bi.readFloats(data_, n_);
}

void FloatRArray::
binDump (BinOutput& bo)
{
  bo.writeInt(n_);
  bo.writeFloats(data_, n_);
}

Bool IndexStack::
binInit (BinInput& bi)
{
  numElts_ = bi.readInt();
  int n = numElts_ + 1;
  arr_.setData(NULL, 0);
  int * data = (int *) malloc (sizeof(int)*n);
  data[0] = 0;
  arr_.setData(data, n);
  return bi.readInts(data+1, numElts_);
}

void IndexStack::
binDump (BinOutput& bo)
{
  bo.writeInt(numElts_);
  int * data = arr_.getData();
  bo.writeInts(data+1, numElts_);
}