/*
 * sosprim.C -- Method definitions for SOS Primitives
 *
 * author -- Mike Facello
 *
 * date -- 12/6/96
 */

#include "li/lipoints.h"
#include "sosprim.h"

/*-----------------------------------------------------------------------*/
/*--------------------------- CLASS SOS_RESULT --------------------------*/
/*-----------------------------------------------------------------------*/


SOS_PrimEvaluator::
SOS_PrimEvaluator (void)
  : lambda3_("Lambda3"),
    lambda4_("Lambda4"),
    lambda5_("Lambda5")
{ 
  numPos3_ = 0; 
  numAbove3_ = 0;
  numAbove4_ = 0; 
  numInSphere_ = 0; 
  numInSphereP_ = 0; 

  debugFlag_ = FALSE;
}


/*-----------------------------------------------------------------------*/

ostream&
operator<< (ostream& s, SOS_PrimEvaluator& evaluator)
{
  s << evaluator.lambda3_.name_;
  evaluator.lambda3_.outputDepths(s);
  s << evaluator.lambda4_.name_;
  evaluator.lambda4_.outputDepths(s);
  s << evaluator.lambda5_.name_;
  evaluator.lambda5_.outputDepths(s);

  /*
  s << evaluator.lambda3_;
  evaluator.lambda3_.outputDepths(s);
  s << evaluator.lambda4_;
  evaluator.lambda4_.outputDepths(s);
  s << evaluator.lambda5_;
  evaluator.lambda5_.outputDepths(s);
  */

  s.width(12);
  s << evaluator.numPos3_ << " . positive3 evaluations\n";
  s.width(12);
  s << evaluator.numAbove3_ << " . above3 evaluations\n";
  s.width(12);
  s << evaluator.numAbove4_ << " . above4 evaluations\n";
  s.width(12);
  s << evaluator.numInSphere_ << " . inSphere evaluations\n";
  s.width(12);
  s << evaluator.numInSphereP_ << " . inSphereP evaluations\n";

  return s;
}

/*-----------------------------------------------------------------------*/

Bool SOS_PrimEvaluator::
positive3 (const LI_PArray &pArray, 
	   VIndex i, VIndex j, VIndex k, VIndex l)
{
 // numPos3_ ++;
  int s = basic_isort4 ((int *)(&i), (int *)(&j), (int *)(&k), (int *)(&l));
  lambda4_.eval(pArray[i], pArray[j], pArray[k], pArray[l], 
		i, j, k, l, result_);
  /*
	double val = 0.0;
	LI_PArray * pArr = (LI_PArray * ) &pArray;
	LI_Converter * converter = pArr->getConverter();
	val = converter->toReal(result_.value());
  */
  int d = result_.sign ();
  int answer = If (Odd (s), (d == -1), (d == 1));

  return answer;
}


/*-----------------------------------------------------------------------*/

Bool SOS_PrimEvaluator::
above3 (const LI_PArray &pArray, 
	VIndex i, VIndex j, VIndex k, VIndex l)
{
 // numAbove3_ ++;
  (void) basic_isort3 ((int *)(&i), (int *)(&j), (int *)(&k));
  lambda3_.eval (pArray[i], pArray[j], pArray[k], 
		 i, j, k, result_);
  int d1 = result_.sign ();
  int s = basic_isort4p ((int *)(&i), (int *)(&j), (int *)(&k), (int *)(&l));
  lambda4_.eval (pArray[i], pArray[j], pArray[k], pArray[l], 
		 i, j, k, l, result_);
  int d2 = result_.sign ();
  int answer = If (Odd (s), (d1 == d2), (d1 != d2));
  return (answer);
}


/*-----------------------------------------------------------------------*/

Bool SOS_PrimEvaluator::
above4 (const LI_PArray &pArray, 
	VIndex i, VIndex j, VIndex k, VIndex l, VIndex m)
{
 // numAbove4_ ++;
  (void) basic_isort4 ((int *)(&i), (int *)(&j), (int *)(&k), (int *)(&l));
  lambda4_.eval (pArray[i], pArray[j], pArray[k], pArray[l], 
		 i, j, k, l, result_);
  int d1 = result_.sign ();
  int s = basic_isort5p ((int *)(&i), (int *)(&j), (int *)(&k), (int *)(&l), (int *)(&m));
  lambda5_.eval (pArray[i], pArray[j], pArray[k], pArray[l], pArray[m], 
		 i, j, k, l, m, result_);
  int d2 = result_.sign ();
  int answer = If (Odd (s), (d1 == d2), (d1 != d2));
  return (answer);
}


/*-----------------------------------------------------------------------*/

Bool SOS_PrimEvaluator::
inSphere (const LI_PArray &pArray, 
	  VIndex o, VIndex i, VIndex j, VIndex k, VIndex p)
{
 // numInSphere_ ++;
  (void) basic_isort4 ((int *)(&o), (int *)(&i), (int *)(&j), (int *)(&k));
  lambda4_.eval (pArray[o], pArray[i], pArray[j], pArray[k], 
		 o, i, j, k, result_);
  int d1 = result_.sign ();
  int s = basic_isort5p ((int *)(&o), (int *)(&i), (int *)(&j), (int *)(&k), (int *)(&p));
  lambda5_.eval (pArray[o], pArray[i], pArray[j], pArray[k], pArray[p], 
		 o, i, j, k, p, result_);
  int d2 = result_.sign ();
  int answer = If (Odd (s), (d1 != d2), (d1 == d2));
  /* Note: This is wrong in \cite{edels:sos}. */
  return (answer);
}


/*-----------------------------------------------------------------------*/

Bool SOS_PrimEvaluator::
inSphereP (const LI_PArray &pArray, 
	   VIndex o, VIndex i, VIndex j, VIndex k, VIndex p)
{
 // numInSphereP_ ++;
  int s = basic_isort4 ((int *)(&o), (int *)(&i), (int *)(&j), (int *)(&k));

  s += basic_isort5p ((int *)(&o), (int *)(&i), (int *)(&j), (int *)(&k), (int *)(&p));
  lambda5_.eval (pArray[o], pArray[i], pArray[j], pArray[k], pArray[p], 
		 o, i, j, k, p, result_);
  int d2 = result_.sign ();
	/*
	double val = 0.0;
	double xo[3], xi[3], xj[3], xk[3], xp[3], tmp;
	LI_PArray * pArr = (LI_PArray * ) &pArray;
	LI_Converter * converter = pArr->getConverter();

	pArr->getPoint(o, xo[0], xo[1], xo[2], tmp);
  pArr->getPoint(i, xi[0], xi[1], xi[2], tmp);
	pArr->getPoint(j, xj[0], xj[1], xj[2], tmp);
	pArr->getPoint(k, xk[0], xk[1], xk[2], tmp);
	pArr->getPoint(p, xp[0], xp[1], xp[2], tmp);

	val = converter->toReal(result_.value());
	*/
  int answer = If (Odd (s), (1 == d2), (1 != d2));

  return answer;
}


/*-----------------------------------------------------------------------*/

void SOS_PrimEvaluator::
debugOn (void)
{
  debugFlag_ = TRUE;
}

/*-----------------------------------------------------------------------*/

void SOS_PrimEvaluator::
debugOff (void)
{
  debugFlag_ = FALSE;
}

