/****************************************************************************/

//
// Implementations of methods for the templates defined in rarray.h
//

template <class T, class ObjSpec>
FBaseArray<T,ObjSpec>::
FBaseArray (int size)
  : n_(size + 1),
    data_(new T[size+1])
{ 

}


template <class T, class ObjSpec>
FBaseArray<T,ObjSpec>::
~FBaseArray (void)
{ 
  delete [] data_; 
}

template <class T, class ObjSpec>
FBaseArray<T,ObjSpec>::
FBaseArray (FBaseArray<T,ObjSpec> const &other)
{
  n_ = other.n_;
  data_ = new T [n_];
  for (register int i = 0; i < n_; i++)
    data_[i] = other.data_[i];
}

template <class T, class ObjSpec>
FBaseArray<T,ObjSpec> const &FBaseArray<T,ObjSpec>::
operator= (FBaseArray<T,ObjSpec> const &other)
{
  if (this == &other) return *this;

  if(n_ < other.n_) 
  {
    delete [] data_;
    data_ = new T[other.n_];
  }

  n_ = other.n_;
  for (register int i = 0; i < n_; i++)
    data_[i] = other.data_[i];
  return (*this);
}


/*
 * FBaseArray::operator[] -- returns a reference to the appropriate array
 *                       object, also checking for array bound violations
 */
template <class T, class ObjSpec>
T& FBaseArray<T,ObjSpec>::
operator[] (int index)
{
  Assert (index >= 0 && index <= n_ );
  return data_[index];
}


template <class T, class ObjSpec>
T& FBaseArray<T,ObjSpec>::operator[] (int index) const
{
  Assert (index >= 0 && index <= n_ );
  return data_[index];
}

template <class T, class ObjSpec>
int FBaseArray<T,ObjSpec>::
size (void)
{ 
  return n_-1; 
}

template <class T, class ObjSpec>
void FBaseArray<T, ObjSpec>::
resize (int size)
{
  T* newdata = new T[size+1];
  for (int i=0; i<n_; i++)
	  newdata[i] = data_[i];
  delete [] data_;
  data_ = newdata;
  n_ = size+1;
}

