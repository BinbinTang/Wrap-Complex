/*
 * Implementation of (possibly) inlined functions for long integers
 *
 */

/*-------------------------------------------------------------------*/


// Less expensive implementations of the above operations

template <class LISpec>
void LIGen<LISpec>::
add (const LIBase &other1, const LIBase &other2)
{
#ifdef SAME_OPS
  Lia_obj tmp_obj;
#endif
  
  PRINT ("Entering add\n");
  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  if (lia_obj_ == NULL)
    alloc();
#ifdef SAME_OPS
  if ((other1.lia_obj_ == lia_obj_) || (other2.lia_obj_ == lia_obj_))
    {
      tmp_obj = new Lia [getMaxSize() + 1];
      lia_add (tmp_obj, other1.lia_obj_, other2.lia_obj_);
      delete [] lia_obj_;
      lia_obj_ = tmp_obj;
    }
  else
#endif

    lia_add (lia_obj_, other1.lia_obj_, other2.lia_obj_);
  PRINT ("Exiting add\n");
}

/*-----------------------------------------------------------------------*/

template <class LISpec>
void LIGen<LISpec>::
sub (const LIBase &other1, const LIBase &other2)
{
#ifdef SAME_OPS
  Lia_obj tmp_obj;
#endif
  
  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  if (lia_obj_ == NULL)
    alloc();
#ifdef SAME_OPS
  if ((other1.lia_obj_ == lia_obj_) || (other2.lia_obj_ == lia_obj_))
    {
      tmp_obj = new Lia [getMaxSize() + 1];
      lia_sub (tmp_obj, other1.lia_obj_, other2.lia_obj_);
      delete [] lia_obj_;
      lia_obj_ = tmp_obj;
    }
  else
#endif

    lia_sub (lia_obj_, other1.lia_obj_, other2.lia_obj_);
}

/*-----------------------------------------------------------------------*/

template <class LISpec>
void LIGen<LISpec>::
mult (const LIBase &other1, const LIBase &other2)
{
#ifdef SAME_OPS
  Lia_obj tmp_obj;
#endif
  
  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  if (lia_obj_ == NULL)
    alloc();
#ifdef SAME_OPS
  if ((other1.lia_obj_ == lia_obj_) || (other2.lia_obj_ == lia_obj_))
    {
      tmp_obj = new Lia [getMaxSize() + 1];
      lia_mul (tmp_obj, other1.lia_obj_, other2.lia_obj_);
      delete [] lia_obj_;
      lia_obj_ = tmp_obj;
    }
  else
#endif

    lia_mul (lia_obj_, other1.lia_obj_, other2.lia_obj_);
}

/*-----------------------------------------------------------------------*/

void LIBase::
negate (void)
{
  Assert (lia_obj_ != NULL);
  lia_chs (lia_obj_);
}

/*-----------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/

// Comparison operators

int 
operator== (const LIBase &other1, const LIBase &other2)
{
  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  return lia_eq (other1.lia_obj_, other2.lia_obj_);
}

/*-----------------------------------------------------------------------*/

int 
operator!= (const LIBase &other1, const LIBase &other2)
{
  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  return !lia_eq (other1.lia_obj_, other2.lia_obj_);
}

/*-----------------------------------------------------------------------*/

int 
operator<  (const LIBase &other1, const LIBase &other2)
{
  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  return lia_le (other1.lia_obj_, other2.lia_obj_);
}

/*-----------------------------------------------------------------------*/

int 
operator<= (const LIBase &other1, const LIBase &other2)
{
  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  return lia_leq (other1.lia_obj_, other2.lia_obj_);
}

/*-----------------------------------------------------------------------*/

int 
operator>  (const LIBase &other1, const LIBase &other2)
{
  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  return !lia_leq (other1.lia_obj_, other2.lia_obj_);
}

/*-----------------------------------------------------------------------*/

int 
operator>= (const LIBase &other1, const LIBase &other2)
{
  Assert ((other1.lia_obj_ != NULL) && (other2.lia_obj_ != NULL));
  return !lia_le (other1.lia_obj_, other2.lia_obj_);
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Accessing information

int LIBase::
sign (void)
{
  Assert (lia_obj_ != NULL);
  return lia_sign (lia_obj_);
}

/*-----------------------------------------------------------------------*/

double LIBase::
real (void)
{
  Assert (lia_obj_ != NULL);
  return lia_real (lia_obj_);
}

/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/

// Constructors

/*-----------------------------------------------------------------------*/

template <class LISpec>
LIGen<LISpec>::
LIGen (void) 
: LIBase()
{

}

template <class LISpec>
LIGen<LISpec>::
LIGen (const LIBase &lnum)
: LIBase ()
{
  register int i;

  if (lnum.lia_obj_ != NULL)
    {
      alloc();
      int ind2 = last_of (lnum.lia_obj_);
      Assert (ind2 < getSize());
      for (i=0; i<=ind2; i++)
	    lia_obj_[i] = lnum.lia_obj_[i];
    }
}

/*-----------------------------------------------------------------------*/
template <class LISpec>
LIGen<LISpec>::
LIGen (int inum)
: LIBase ()
{
  alloc();
  lia_load (lia_obj_, inum);
}

/*-----------------------------------------------------------------------*/

template <class LISpec>
LIGen<LISpec>::
LIGen (char *cnum)
: LIBase ()
{
  alloc();
  lia_strload (lia_obj_, cnum, "%d");
}

/*-----------------------------------------------------------------------*/

template <class LISpec>
LIGen<LISpec> const &LIGen<LISpec>::
operator= (const LIBase &lnum)
{
  register int i;
  int ind2;

  if (lnum.lia_obj_ != NULL)    // Don't allocate if the assigned long
                                //      integer is not allocated
    {
      if (lia_obj_ == NULL)
	      alloc();
      if (this != &lnum)
    {
	  ind2 = last_of (lnum.lia_obj_);
          Assert(ind2 < size_);
	  for (i=0; i<=ind2; i++)
	    lia_obj_[i] = lnum.lia_obj_[i];
    }
    }

  return (*this);
}

/*-----------------------------------------------------------------------*/

template <class LISpec>
void LIGen<LISpec>::
alloc (void)
{
  lia_obj_ = new Lia [size_ + 1];
  BZERO(lia_obj_, Lia, size_ + 1);
}

/*-----------------------------------------------------------------------*/

template <class LISpec>
void LIGen<LISpec>::
free (void)
{
  delete[] lia_obj_;
  lia_obj_ = (Lia *)0;
}

/*-----------------------------------------------------------------------*/

template <class LISpec>
int LIGen<LISpec>::size_ = 0;

/*-----------------------------------------------------------------------*/

template<class LISpec>
int LIGen<LISpec>::
getSize (void)
{
  return size_;
}

/*-----------------------------------------------------------------------*/


template <class LISpec>
void LIGen<LISpec>::
setSize (int size)
{
  /*
  if (size_ != 0)
    Assert (size <= size_);
    */
  size_ = size;
  if ((int)LISpec::LI_SIZE == (int)LIBase::LI_LONG)
    lia_length (size_);
}

/*-------------------------------------------------------------------*/

template <class LISpec>
void LIGen<LISpec>::
operator+= (const LIBase &other)
{
  Lia_obj tmp_obj;

  tmp_obj = new Lia [size_ + 1];
  Assert ((lia_obj_ != NULL) && (other.lia_obj_ != NULL));
  lia_add (tmp_obj, lia_obj_, other.lia_obj_);
  delete [] lia_obj_;
  lia_obj_ = tmp_obj;
}

/*-------------------------------------------------------------------*/
template <class LISpec>
void LIGen<LISpec>::
operator-= (const LIBase &other)
{
  Lia_obj tmp_obj;

  tmp_obj = new Lia [size_ + 1];
  Assert ((lia_obj_ != NULL) && (other.lia_obj_ != NULL));
  lia_sub (tmp_obj, lia_obj_, other.lia_obj_);
  delete [] lia_obj_;
  lia_obj_ = tmp_obj;
}

/*-------------------------------------------------------------------*/

template <class LISpec>
void LIGen<LISpec>::
operator*= (const LIBase &other)
{
  Lia_obj tmp_obj;

  tmp_obj = new Lia [size_ + 1];
  Assert ((lia_obj_ != NULL) && (other.lia_obj_ != NULL));
  lia_mul (tmp_obj, lia_obj_, other.lia_obj_);
  delete [] lia_obj_;
  lia_obj_ = tmp_obj;
}

/*-------------------------------------------------------------------*/

inline int LI_Converter::
getFixedW (void) { return fixed_w_; }

inline int LI_Converter::
getFixedA (void) { return fixed_a_; }

inline double LI_Converter::
getScale (void) { return scale_; }

inline void LI_Converter::
trunc (double x[4]) { (this->*truncate_)(x); }

inline void LI_Converter::
truncMul (double x[4]) 
{ vmulw(x, scale_); (this->*truncate_)(x); }

inline void LI_Converter::truncMulDiv (double x[4]) 
{ vmulw(x, scale_); (this->*truncate_)(x); vdivw(x, scale_); }
