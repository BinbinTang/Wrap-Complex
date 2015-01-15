/*
 * soseval.h -- Class definitions for SOS Evaluators
 *
 * author -- Mike Facello
 *
 * date -- 10/23/96
 */

#ifndef __RGI_SOSEVAL_H__
#define __RGI_SOSEVAL_H__

#include <iostream>
#include <fstream>
#include <strstream>

#include <cstdlib>
#include <cstdio>

#include <basic/dumpable.h>
#include <li/li.h>
#include <sos/soseval.hpp>

using namespace std;

class LI_Point;
class LI_VertMinorEvaluator;

/************************************************************************/

/*
 * Class SOS_Result
 *
 * This object summarizes the result of an SOS determinant evaluation.  It
 * is used in the incremental evaluation of the determinant.  Currently,
 * the only operation of interest to the user of SOS is the sign of this
 * result, which can be accessed through the sign() method.
 *
 */

class SOS_Result
{
public:
  SOS_Result (void);
  ~SOS_Result (void);
  
  int sign (void) const;
	LI& value (void) { return value_; }

  friend class SOS_Evaluator;
  friend class Lambda3_Eval;
  friend class Lambda4_Eval;
  friend class Lambda5_Eval;

  //friend class ostream& operator<< (ostream& s, SOS_Result& result);

private:
  int signum_;
  LI value_;
  int depth_;
  int two_k_;
  RArray<int> epsilon_;
};

/************************************************************************/

/*
 * Class SOS_Evaluator
 *
 * This class is an abstract base class for SOS evaluators.  It
 * defines the necessary variables for evaluating an SOS determinant.
 * Currently, it is set up to act just like a function, which is
 * what it is representing.
 *
 * To create a new evaluator, create a subclass that redefines the
 * operator() and the constants DIM, MAX_T, and MAX_K.
 *
 * Questions still to be resolved:
 * - this uses an LI_Vert_Minor_Evaluator...  Should there be one per
 *   evaluator, or should there be a shared one?
 */

class SOS_Evaluator
{
public:
  SOS_Evaluator (char *name = "Abstract Base Class");
  virtual ~SOS_Evaluator (void);

  void outputDepths (ostream& s);
  //friend class ostream& operator<< (ostream& s, SOS_Evaluator& evaluator);

protected:
  virtual int getMax (void);

  LI_VertMinorEvaluator *minorEval_;

  // Constants that should be defined for each subclass of this type
  enum { DIM = 0 };
  enum { MAX_T = 0 };
  enum { MAX_K = 0 };

  //  Removed:
  //  SOS_Result result_;
  //  RArray<int> epsilonProduct_;              // The values of epsilon used

public:
  char *name_;                              // The name of the primitive
  //  int max_;                                 // Maximum number of terms
  RArray<int> counter_;                     // Depth counters
  //  int dimension_;                           // Dimension of the minor
  int num_calls_;                           // Number of calls to this
                                            //     primitive

  // Internal "macro" functions -- moved from primitive.h
  //  INLINE void init (void);
  //  INLINE void initStar (void);
  //  INLINE void epsilonTerm (int index);
  //  INLINE void epsilon (int lambda, int kappa);
  //  INLINE void coefficient (LI *li);
  //  INLINE void posCoefficient (LI *li);
  //  INLINE void negCoefficient (LI *li);
  //  INLINE void aftermath (void);    
  // NOTE:  this may not work because aftermath() returns a value, and is
  //        used to escape out of an evaluation function.
};


/************************************************************************/

/*
 * Class Lambda3_Eval
 *
 * This object evaluates a lambda3 determinant using SOS, and returns
 * a *reference* to an SOS_Result.
 *
 */

class Lambda3_Eval : public SOS_Evaluator
{
public:
  Lambda3_Eval (char *name = "Lambda3");
  virtual ~Lambda3_Eval () {}

  void eval (const LI_Point & p1, const LI_Point & p2, const LI_Point & p3,
	     VIndex i, VIndex j, VIndex k,
	     SOS_Result & result);

private:
  virtual int getMax (void);

  // Constants that should be defined for each subclass of this type
  enum { DIM = 3 };
  enum { MAX_T = 14 };
  enum { MAX_K = 2 };
};


/************************************************************************/

/*
 * Class Lambda4_Eval
 *
 * This object evaluates a lambda4 determinant using SOS, and returns
 * a *reference* to an SOS_Result.
 *
 */

class Lambda4_Eval : public SOS_Evaluator
{
public:
  Lambda4_Eval (char *name = "Lambda4");
  virtual ~Lambda4_Eval () {}

  void eval (const LI_Point & p1, const LI_Point & p2, const LI_Point & p3, 
	     const LI_Point & p4,
	     VIndex i, VIndex j, VIndex k, VIndex l,
	     SOS_Result & result);

private:
  virtual int getMax (void);

  // Constants that should be defined for each subclass of this type
  enum { DIM = 4 };
  enum { MAX_T = 14 };
  enum { MAX_K = 3 };
};


/************************************************************************/

/*
 * Class Lambda5_Eval
 *
 * This object evaluates a lambda5 determinant using SOS, and returns
 * a *reference* to an SOS_Result.
 *
 */

class Lambda5_Eval : public SOS_Evaluator
{
public:
  Lambda5_Eval (char *name = "Lambda5");
  virtual ~Lambda5_Eval () {}
  
  void eval (const LI_Point & p1, const LI_Point & p2, const LI_Point & p3, 
	     const LI_Point & p4, const LI_Point & p5,
	     VIndex i, VIndex j, VIndex k, VIndex l, VIndex m,
	     SOS_Result & result);

private:
  virtual int getMax (void);

  // Constants that should be defined for each subclass of this type
  enum { DIM = 5 };
  enum { MAX_T = 49 };
  enum { MAX_K = 4 };
};



#endif // __SOSEVAL_H__
