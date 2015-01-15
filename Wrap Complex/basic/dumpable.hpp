

template<class T>
Bool RArrayDumpable<T>::
binInit (BinInput& bi)
{
  int newN = bi.readInt();
  resize(newN);
  float r = newN? 1.0f/newN : 1.0f;
  int skip = newN / 50 + 1;
  for(int i=0; i<newN; i++)
  {
    if(!data_[i].binInit(bi))
      return FALSE;
    if(i % skip == 0) progressUpdate(i*r);
  }
  return TRUE;
}

template<class T>
void RArrayDumpable<T>::
binDump (BinOutput& bo)
{
  bo.writeInt(n_);
  float r = n_? 1.0f/n_ : 1.0f;
  int skip = n_ / 50 + 1;
  for(int i=0; i<n_; i++)
  {
    data_[i].binDump(bo);
    if(i % skip == 0) progressUpdate(i*r);
  }
}


template<class T>
Bool RObjArrayDumpable<T>::
binInit (BinInput& bi)
{
  int newN = bi.readInt();
  resize(newN);
  float r = newN? 1.0f/newN : 1.0f;
  int skip = newN / 50 + 1;
  for(int i=0; i<newN; i++)
  {
    if(!data_[i].binInit(bi))
      return FALSE;
    if(i % skip == 0) progressUpdate(i*r);
  }
  return TRUE;
}

template<class T>
void RObjArrayDumpable<T>::
binDump (BinOutput& bo)
{
  bo.writeInt(n_);
  float r = n_? 1.0f/n_ : 1.0f;
  int skip = n_ / 50 + 1;
  for(int i=0; i<n_; i++)
  {
    data_[i].binDump(bo);
    if(i % skip == 0) progressUpdate(i*r);
  }
}

//----------------------------------------------------
template<class T>
Bool IterStackDumpable<T>::
binInit (BinInput& bi)
{
  reset();
  T elem;
  int n = bi.readInt();
  float r = n? 1.0f/n : 1.0f;
  int skip = n / 50 + 1;
  for(int i=1; i<=n; i++)
  {
    if(!elem.binInit(bi))
      return FALSE;
    push(elem);
    if(i % skip == 0) progressUpdate(i*r);
  }
  return TRUE;
}

template<class T>
void IterStackDumpable<T>::
binDump (BinOutput& bo)
{
  int n = size();
  float r = n? 1.0f/n : 1.0f;
  int skip = n / 50 + 1;
  bo.writeInt(n);
  for(int i=1; i<=n; i++)
  {
    (*this)[i].binDump(bo);
    if(i % skip == 0) progressUpdate(i*r);
  }
}
//----------------------------------------------------

template<class T>
Bool IterObjStackDumpable<T>::
binInit (BinInput& bi)
{
  reset();
  T elem;
  int n = bi.readInt();
  float r = n? 1.0f/n : 1.0f;
  int skip = n / 50 + 1;
  for(int i=1; i<=n; i++)
  {
    if(!elem.binInit(bi))
      return FALSE;
    push(elem);
    if(i % skip == 0) progressUpdate(i*r);
  }
  return TRUE;
}

template<class T>
void IterObjStackDumpable<T>::
binDump (BinOutput& bo)
{
  int n = size();
  float r = n? 1.0f/n : 1.0f;
  int skip = n / 50 + 1;
  bo.writeInt(n);
  for(int i=1; i<=n; i++)
  {
    (*this)[i].binDump(bo);
    if(i % skip == 0) progressUpdate(i*r);
  }
}

//----------------------------------------------------

template<class T>
Bool FreeArrayDumpable<T>::
binInit (BinInput& bi)
{
  int prog1, prog3;
  getProgressBounds(prog1, prog3);
  int prog2 = (10*prog1 + 90*prog3)/100;
  setProgressBounds(prog1, prog2);
  nextReusable_ = bi.readInt();
  int newN = bi.readInt();
  arr_.resize(newN);
  float r = newN? 1.0f/newN : 1.0f;
  int skip = newN / 50 + 1;
  for(int i=0; i<newN; i++)
  {
    if(!arr_[i].binInit(bi))
      return FALSE;
    if (i%skip == 0) progressUpdate(i*r);
  }
  setProgressBounds(prog2, prog3);
  progressUpdate(0.1f);
  int res =  exists_.binInit(bi);
  progressUpdate(0.9f);
  setProgressBounds(prog1, prog3);
  return res;
}

template<class T>
void FreeArrayDumpable<T>::
binDump (BinOutput& bo)
{
  int prog1, prog3;
  getProgressBounds(prog1, prog3);
  int prog2 = (10*prog1 + 90*prog3)/100;
  setProgressBounds(prog1, prog2);
  bo.writeInt(nextReusable_);
  int n = arr_.size();
  bo.writeInt(n);
  float r = n? 1.0f/n : 1.0f;
  int skip = n / 50 + 1;
  for(int i=0; i<n; i++)
  {
    arr_[i].binDump(bo);
    if (i%skip == 0) progressUpdate(i*r);
  }
  setProgressBounds(prog2, prog3);
  progressUpdate(0.1f);
  exists_.binDump(bo);
  progressUpdate(0.9f);
  setProgressBounds(prog1, prog3);
}

