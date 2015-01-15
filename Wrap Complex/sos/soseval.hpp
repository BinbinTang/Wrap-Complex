#define Initialize() \
   // Do nothing...

/*--------------------------------------------------------------------------*/

#define Epsilon_Term(T) \
  result.depth_ = (T); \
  counter_[T] ++; \
  result.two_k_ = 0

#define Epsilon(LAMBDA,KAPPA) \
  result.epsilon_[result.two_k_++] = (LAMBDA); \
  result.epsilon_[result.two_k_++] = (KAPPA)

/* NOTE: These macros can be redefined as static functions with
         #include "primitive.i.c" */

/*--------------------------------------------------------------------------*/

#define Coefficient() \
  Aftermath ()

#define Positive_Coefficient() \
  Aftermath ()

#define Negative_Coefficient() \
  result.value_.negate(); \
  Aftermath ()

#define Aftermath() \
  result.signum_ = result.value_.sign(); \
  if (result.signum_ != 0) return

/* NOTE: These macros can NOT be redifined as functions because of the
         "return" in Aftermath(). */


/*--------------------------------------------------------------------------*/

#define Finish() \
  basic_error ("beyond Finish() in file %s", __FILE__, 1); \
  return;
  /* Note: "1" is just added above to quiet 'gcc -Wall -pedantic' */
