/* clib/liminor.C */

/* The functions sos_minor*() calculate given minors using the Lia Library.
   
   Normally, all given indices are assumed to be > 0.  HOWEVER...

     o if the FIRST row index is 0, the unperturbed, this indicates
       the *-row sos_lia_0(), and
     o if the LAST column index is 0, this really means the
       unperturbed 1-column {1,1,...,1}.

   The returned address points to a Lia object representing the
   value of the corresponding determinant.  This result is stored
   only TEMPORARILY and will be overwritten by upcoming calls
   of sos_minor*().  It's the callers responsibility to save it
   via lia_copy() or lia_assign() where needed. */

/*--------------------------------------------------------------------------*/

#include "liminor.h"
#include "lidet.h"
#include "lipoints.h"

/*--------------------------------------------------------------------------*/

LI_VertMinorEvaluator::
LI_VertMinorEvaluator (void)
{
  for (register int i = 0; i < 6; i++)
    num_calls_[i] = 0;
  dEval_ = new LI_DetEvaluator;
}

/*--------------------------------------------------------------------------*/

LI_VertMinorEvaluator::
~LI_VertMinorEvaluator (void)
{
  delete dEval_;
}

/*--------------------------------------------------------------------------*/

void LI_VertMinorEvaluator::
minor1 (const LI_Point & p1,
	int a,
	LI & result)
     /* Added for consistency only! */
{
  num_calls_[1] ++;
  if (a)
    result = p1[a];
  else
    result = LI(1);
}

/*--------------------------------------------------------------------------*/

void LI_VertMinorEvaluator::
minor2 (const LI_Point & p1, const LI_Point & p2,
	int a, int b,
	LI & result)
{
  num_calls_[2] ++;
  if (b)
    { /* | Pi[i,a] Pi[i,b] |
	 | Pi[j,a] Pi[j,b] |
	 */
      dEval_->det2 (p1[a], p1[b],
		   p2[a], p2[b], result);
    }
  else
    { /* | Pi[i,a] 1 |
	 | Pi[j,a] 1 |
	 */
      result.sub (p1[a], p2[a]);
    }
}

/*--------------------------------------------------------------------------*/

void LI_VertMinorEvaluator::
minor3 (const LI_Point & p1, const LI_Point & p2, const LI_Point & p3,
	int a, int b, int c,
	LI & result)
{
  num_calls_[3] ++;
  if (c)
    { /* | Pi[i,a] Pi[i,b] Pi[i,c] |
	 | Pi[j,a] Pi[j,b] Pi[j,c] |
	 | Pi[k,a] Pi[k,b] Pi[k,c] |
	 */
      dEval_->det3 (p1[a], p1[b], p1[c],
		   p2[a], p2[b], p2[c],
		   p3[a], p3[b], p3[c], result);
    }
  else
    { /* | Pi[i,a] Pi[i,b] 1 |
	 | Pi[j,a] Pi[j,b] 1 |
	 | Pi[k,a] Pi[k,b] 1 |
	 */
      //      LI x(p1[a]), y(p1[b]);
      /* 2nd row - 1st */
      wspace_[1].sub (p2[a], p1[a]);
      wspace_[2].sub (p2[b], p1[b]);
      /* 3rd row - 1st */
      wspace_[3].sub (p3[a], p1[a]);
      wspace_[4].sub (p3[b], p1[b]);
      /* 2-by-2 det */
      dEval_->det2 (wspace_[1], wspace_[2],
		   wspace_[3], wspace_[4], result);
    }
}

/*--------------------------------------------------------------------------*/

void LI_VertMinorEvaluator::
minor4 (const LI_Point & p1, const LI_Point & p2, const LI_Point & p3,
	const LI_Point & p4,
	int a, int b, int c, int d,
	LI & result)
{
  num_calls_[4] ++;
  if (d)
    { /* | Pi[i,a] Pi[i,b] Pi[i,c] Pi[i,d] |
	 | Pi[j,a] Pi[j,b] Pi[j,c] Pi[j,d] |
	 | Pi[k,a] Pi[k,b] Pi[k,c] Pi[k,d] |
	 | Pi[l,a] Pi[l,b] Pi[l,c] Pi[l,d] |
	 */
      dEval_->det4 (p1[a], p1[b], p1[c], p1[d],
		   p2[a], p2[b], p2[c], p2[d],
		   p3[a], p3[b], p3[c], p3[d],
		   p4[a], p4[b], p4[c], p4[d], result);
    }
  else
    { /* | Pi[i,a] Pi[i,b] Pi[i,c] 1 |
	 | Pi[j,a] Pi[j,b] Pi[j,c] 1 |
	 | Pi[k,a] Pi[k,b] Pi[k,c] 1 |
	 | Pi[l,a] Pi[l,b] Pi[l,c] 1 |
	 */
      //      LI x(p1[a]), y(p1[b]), z(p1[c]);
      /* 2nd row - 1st */
      wspace_[1].sub (p2[a], p1[a]);
      wspace_[2].sub (p2[b], p1[b]);
      wspace_[3].sub (p2[c], p1[c]);
      /* 3rd row - 1st */
      wspace_[4].sub (p3[a], p1[a]);  
      wspace_[5].sub (p3[b], p1[b]);
      wspace_[6].sub (p3[c], p1[c]);
      /* 4th row - 1st */
      wspace_[7].sub (p4[a], p1[a]);  
      wspace_[8].sub (p4[b], p1[b]);
      wspace_[9].sub (p4[c], p1[c]);

      /* (-1) * 3-by-3 det */
      dEval_->det3 (wspace_[1], wspace_[2], wspace_[3],
		   wspace_[4], wspace_[5], wspace_[6],
		   wspace_[7], wspace_[8], wspace_[9], result);
      result.negate();
    }
}



/*--------------------------------------------------------------------------*/

void LI_VertMinorEvaluator::
minor5 (const LI_Point & p1, const LI_Point & p2, const LI_Point & p3,
	const LI_Point & p4, const LI_Point & p5,
	int a, int b, int c, int d, int e,
	LI & result)
{
  num_calls_[5] ++;
  if (e)
    { /* | Pi[i,a] Pi[i,b] Pi[i,c] Pi[i,d] Pi[i,e] |
	 | Pi[j,a] Pi[j,b] Pi[j,c] Pi[j,d] Pi[j,e] |
+	 | Pi[k,a] Pi[k,b] Pi[k,c] Pi[k,d] Pi[k,e] |
	 | Pi[l,a] Pi[l,b] Pi[l,c] Pi[l,d] Pi[l,e] |
	 | Pi[m,a] Pi[m,b] Pi[m,c] Pi[m,d] Pi[m,e] |
	 */
      basic_error ("sos_minor5: full 5-by-5 determinant not implemented");
    }
  else
    { /* | Pi[i,a] Pi[i,b] Pi[i,c] Pi[i,d] 1 |
	 | Pi[j,a] Pi[j,b] Pi[j,c] Pi[j,d] 1 |
	 | Pi[k,a] Pi[k,b] Pi[k,c] Pi[k,d] 1 |
	 | Pi[l,a] Pi[l,b] Pi[l,c] Pi[l,d] 1 |
	 | Pi[m,a] Pi[m,b] Pi[m,c] Pi[m,d] 1 |
	 */
      //      LI x(p1[a]), y(p1[b]), z(p1[c]), u(p1[d]);
      /* 2nd row - 1st */
      wspace_[1].sub (p2[a], p1[a]);
      wspace_[2].sub (p2[b], p1[b]);
      wspace_[3].sub (p2[c], p1[c]);
      wspace_[4].sub (p2[d], p1[d]);
      /* 3rd row - 1st */
      wspace_[5].sub (p3[a], p1[a]); 
      wspace_[6].sub (p3[b], p1[b]);
      wspace_[7].sub (p3[c], p1[c]);
      wspace_[8].sub (p3[d], p1[d]);
      /* 4th row - 1st */
      wspace_[9].sub (p4[a], p1[a]); 
      wspace_[10].sub (p4[b], p1[b]);
      wspace_[11].sub (p4[c], p1[c]);
      wspace_[12].sub (p4[d], p1[d]);
      /* 5th row - 1st */
      wspace_[13].sub (p5[a], p1[a]); 
      wspace_[14].sub (p5[b], p1[b]);
      wspace_[15].sub (p5[c], p1[c]);
      wspace_[16].sub (p5[d], p1[d]);
      /* 4-by-4 det */
      dEval_->det4 (wspace_[1],  wspace_[2],  wspace_[3],  wspace_[4],
		   wspace_[5],  wspace_[6],  wspace_[7],  wspace_[8],
		   wspace_[9],  wspace_[10], wspace_[11], wspace_[12],
		   wspace_[13], wspace_[14], wspace_[15], wspace_[16], result);
    }
}

/*--------------------------------------------------------------------------*/

ostream& 
operator<< (ostream& s, LI_VertMinorEvaluator& evaluator)
{
  s << endl << *evaluator.dEval_ << endl << endl;

  s << "LI Minor evaluations\n\n";
  s.width(12);
  s << evaluator.num_calls_[1] << " . 1 x 1 minors\n";
  s.width(12);
  s << evaluator.num_calls_[2] << " . 2 x 2 minors\n";
  s.width(12);
  s << evaluator.num_calls_[3] << " . 3 x 3 minors\n";
  s.width(12);
  s << evaluator.num_calls_[4] << " . 4 x 4 minors\n";
  s.width(12);
  s << evaluator.num_calls_[5] << " . 5 x 5 minors\n\n";

  return s;
}

/*--------------------------------------------------------------------------*/



