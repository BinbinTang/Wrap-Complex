#include <basic/basic.h>
#include <math.h>
#include <limits.h>
#include <basic/miscmath.h>

/*--------------------------------------------------------------------------*/
// compute middle point Z equally far from a,b,c,d using solve3()
// return 0 on failure

int mid2 (double a[3], double b[3], double z[3])
{
  vadd (a, b, z);
  vdiv (z, 2.0);
  return 1;
}

/*--------------------------------------------------------------------------*/

int mid3 (double a[3], double b[3], double c[3], double z[3])
{
  // translate a to 0  i.e.  u = b-a, v = c-a

  double u[3], v[3];  vsub (b,a,u);   
                      vsub (c,a,v);

  double     uxv[3];  vcross (u,v,uxv);                 // u cross v
                      vdiv   (uxv, vlength (u));        // make it smaller

  // Solve for s the mid point of {0,u,v} from :- 
  //
  //    [ 2u      ] s = u.u
  //    [ 2v      ] s = v.v
  //    [ uxv/|u| ] s =  0
  //     -----      ---
  //     l m n       o
  
  double l[3], m[3], n[3], o[3], s[3];

  l[0]= 2.0* u[0];   m[0]= 2.0* u[1];   n[0]= 2.0* u[2];   o[0]= - vdot (u,u);
  l[1]= 2.0* v[0];   m[1]= 2.0* v[1];   n[1]= 2.0* v[2];   o[1]= - vdot (v,v);
  l[2]=    uxv[0];   m[2]=    uxv[1];   n[2]=    uxv[2];   o[2]= - 0.0;
   
  int result = solve3 (l,m,n,o, s);

  // translate origin back to a

  vadd (s,a, z);

  return result;
}

/*--------------------------------------------------------------------------*/

int mid4 (double a[3], double b[3], double c[3], double d[3], double z[3])
{
  // translate a to 0  i.e.  u = b-a, v = c-a, w = d-a

  double u[3], v[3], w[3];  vsub (b,a,u);   
                            vsub (c,a,v);  
                            vsub (d,a,w);

  // Solve for s the mid point of {0,u,v,w} from :-
  //
  //   [ 2u ] s = u.u
  //   [ 2v ] s = v.v
  //   [ 2w ] s = w.w
  //    ----      ---
  //    l m n      o

  double l[3], m[3], n[3], o[3], s[3];

  l[0]= 2.0* u[0];   m[0]= 2.0* u[1];   n[0]= 2.0* u[2];   o[0]= - vdot (u,u);
  l[1]= 2.0* v[0];   m[1]= 2.0* v[1];   n[1]= 2.0* v[2];   o[1]= - vdot (v,v);
  l[2]= 2.0* w[0];   m[2]= 2.0* w[1];   n[2]= 2.0* w[2];   o[2]= - vdot (w,w);
  
  int result = solve3 (l,m,n,o, s);

  // translate origin back to a

  vadd (s,a, z);

  return result;
}

/*--------------------------------------------------------------------------*/

/*
  Solves the linear system equation
  a[0]*x[0] + b[0]*x[1] + c[0]*x[2] + d[0] = 0;
  a[1]*x[0] + b[1]*x[1] + c[1]*x[2] + d[1] = 0;
  a[2]*x[0] + b[2]*x[1] + c[2]*x[2] + d[2] = 0;

  returns 1 if unique solution exists.
  returns 0 if not.

*/

int solve3 (double a[3], double b[3], double c[3], double d[3], 
            double x[3])
{
#define MIN_FRACTION 0.005
  double denom = det3 (a, b, c);
  
  if (denom == 0)
	  return 0;
  // maxVol is of the order of volume
  double maxVol = maxAbsCoord(a)*maxAbsCoord(b)*maxAbsCoord(c);
  if (fabs(denom) <= MIN_FRACTION*maxVol)
    return 0;

  denom = -1.0 / denom;

  x[0] = det3 (d, b, c) * denom;
  x[1] = det3 (a, d, c) * denom;
  x[2] = det3 (a, b, d) * denom;

  return 1;
#undef MIN_FRACTION
}

/*--------------------------------------------------------------------------*/

double
tetraOrVolume (double a[3], double b[3], double c[3], double d[3])
{
  double x[3], y[3], z[3];
  return det3 (vsub(a,d,x), vsub(b,d,y), vsub(c,d,z)) / 6.0;
}


/*--------------------------------------------------------------------------*/

double 
tetraVolume (double a[3], double b[3], double c[3], double d[3])
{
  return fabs(tetraOrVolume(a, b, c, d));
}

/*--------------------------------------------------------------------------*/

void
computeNormal (double a[3], double b[3], double c[3], double normal[3])

{
#define ABS_MAX(a,b) (fabs (a) > fabs (b) ? fabs (a) : fabs (b))

  register double dx1, dy1, dz1, dx2, dy2, dz2, cMax;

  dx1 = a[0] - b[0]; dy1 = a[1] - b[1]; dz1 = a[2] - b[2];
  dx2 = b[0] - c[0]; dy2 = b[1] - c[1]; dz2 = b[2] - c[2];

  cMax = ABS_MAX (dx1, ABS_MAX (dx2, ABS_MAX (dy1, ABS_MAX (dy2, 
	 ABS_MAX (dz1, dz2)))));

  dx1 /= cMax; dy1 /= cMax; dz1 /= cMax; 
  dx2 /= cMax; dy2 /= cMax; dz2 /= cMax; 

  normal[0] = (dy1 * dz2) - (dz1 * dy2);
  normal[1] = (dz1 * dx2) - (dx1 * dz2);
  normal[2] = (dx1 * dy2) - (dy1 * dx2);

#undef ABS_MAX
}

/*--------------------------------------------------------------------------*/

void
normalNormalize (double normal[3])

{
  vnorm (normal);
}

/*--------------------------------------------------------------------------*/

void
computeNormalizedNormal (double a[3], double b[3], double c[3], 
			 double normal[3])
{
  computeNormal (a, b, c, normal);
  normalNormalize (normal);
}

/*--------------------------------------------------------------------------*/

double 
computeDotProduct (double a[3], double b[3])
{
  return vdot(a, b);
}

/*--------------------------------------------------------------------------*/

static unsigned _SEED_;

void setSeed (unsigned seed)
{
  _SEED_ = seed;
  srand (_SEED_);
}

/*--------------------------------------------------------------------------*/

unsigned getSeed (int seed)
{
  return _SEED_;
}

/*--------------------------------------------------------------------------*/
// static ofstream osf("loglog.txt");
// ofstream *os = &osf;

int
getRandom (void)
{
    int a = rand ();
// Used to be #if (RAND_MAX < GET_RANDOM_MAX)
    if (RAND_MAX < GET_RANDOM_MAX)
    {
      a *= (RAND_MAX + 1);
      a |= rand ();
    }
//#endif
    return a & (GET_RANDOM_MAX);
}

/*--------------------------------------------------------------------------*/

double * project (double x[3], double y[3], double z[3], double w[3], double r[3])
{
 // first find the plane normal
  double a[3], b[3], d[3], norm[3];

  vcross(vsub(x, z, a), vsub(y, z, b), norm);
  double tVol = det3(a, b, vsub(w, z, d));

  if (tVol == 0)
    return vcopy(w, r);

  return vsub(w, vmul(norm, tVol/vlength2(norm)), r);
}

/*--------------------------------------------------------------------------*/

 void movePoint (
	 double x, double y, double z, 
	 double n1, double n2, double n3, 
	 double length,
	 double &newX, double &newY, double &newZ)
 {
	double offsetC[3];

	double zyAngle = asin (n1/ 1);
  if (zyAngle > 1.0) zyAngle = 1.0;
  else if (zyAngle < -1.0) zyAngle = -1.0;
	offsetC[0] = sin (zyAngle) * length;

	double xzAngle = asin (n2 / 1);
  if (xzAngle > 1.0) xzAngle = 1.0;
  else if (xzAngle < -1.0) xzAngle = -1.0;
	offsetC[1] = sin (xzAngle) * length;

	double xyAngle = asin (n3 / 1);
  if (xyAngle > 1.0) xyAngle = 1.0;
  else if (xyAngle < -1.0) xyAngle = -1.0;
	offsetC[2] = sin (xyAngle) * length;

	newX = x+offsetC[0];
	newY = y+offsetC[1];
	newZ = z+offsetC[2];
 }

 //-----------------------------------------------------------------------
double * projectToLine (double vx[3], double ax[3], 
						double bx[3], double wx[3])
{
  double v[3], b[3];
  vsub(vx, ax, v);
  vsub(bx, ax, b);
  double lb = vlength2(b);
  if (!lb) 
	return NULL;
  double k = vdot(v, b)/lb;
  vmul(b, k, wx);
  vadd(wx, ax);
  return wx;
}

double pointToLineDist2 (double vx[3], double ax[3], double bx[3])
{
  double v[3], b[3], w[3];
  vsub(vx, ax, v);
  vsub(bx, ax, b);
  double lb = vlength2(b);
  if (!lb) 
	return 0;
  double k = vdot(v, b)/lb;
  vmul(b, k, w);
  return vdist2(v, w);
}

//----------------------------------------------------------------------------

Bool findDigits (const char *s, int &left, int &right, int &zero)
{
	// COMMENT ("string: " << s);

	if ((s[0] == '+') || (s[0] == '-')) {
		s++;
	}
	// COMMENT ("  skipped sign: " << s);

	Bool empty = 1;
	while (s[0] == '0') {
		empty  = 0;
		s++;
	}
	if (s[0] == '\0') {
		if (empty) {
			return 0;
		}
		left = -INT_MAX;
		right = -INT_MAX;
		zero = 1;
		return 1;
	}
	// COMMENT ("  skipped zeros: " << s);

	if (!isdigit(s[0]) && (s[0] != '.')) {
		// COMMENT ("  not a number");
		return 0;
	}
	left = -1;
	right = 0;

	const char *mark = 0;
	while (isdigit(s[0])) {
		if (s[0] != '0') {
			mark = s;
		}
		left++;
		s++;
	}
	if (mark) {
		right = s - mark - 1;
	}
	// COMMENT ("  left of point: " << left);

	if (s[0] == '.') {
		// COMMENT ("  found the point");
		s++;

		const char *mark = s;
		Bool leftFinal = (left >= 0);
		right = 0;
		while (isdigit(s[0])) {
			if (s[0] != '0') {
				right = mark - s - 1;
				if (!leftFinal) {
					left = right;
					leftFinal = 1;
				}
			}
			s++;
		}
		// COMMENT ("  new left of point: " << left);
		// COMMENT ("  right of point: " << right);
	}

	if ((left < 0) && (right == 0)) {
		// COMMENT ("  it's zero");
		left = -INT_MAX;
		right = -INT_MAX;
		zero = 1;
		return 1;
	}
	zero = 0;

	if (s[0] == '\0') {
		return 1;
	}

	if ((s[0] != 'e') && (s[0] != 'E')) {
		// COMMENT ("  not a number");
		return 0;
	}
	s++;

	int power = 0;
	int sign = 1;
	if (s[0] == '-') {
		sign = -1;
		s++;
	} else if (s[0] == '+') {
		s++;
	} else if (!isdigit (s[0])) {
		// COMMENT ("  not a number");
		return 0;
	}
	while (isdigit (s[0])) {
		power = 10*power + s[0] - '0';
		s++;
	}
	// COMMENT ("  exponent: " << power);
	left += sign * power;
	right += sign * power;
	// COMMENT ("  final left of point: " << left);
	// COMMENT ("  final right of point: " << right);

	return 1;
}

