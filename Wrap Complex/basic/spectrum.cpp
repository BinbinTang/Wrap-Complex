#include <math.h>
#include <basic/spectrum.h>

// support for spectral decomposition (eigenvector
// computation) for a 3x3 positive definite matrix


void mmul3x3(double m[9], double p[9])
{	// 3x3 matrix multiply  m=m*p
	int		i, j, k;
	double	row[3];

	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			row[j] = 0.;
			for (k=0; k<3; k++)
				row[j] += m[3*i+k]*p[3*k+j];
			}
		for (j=0; j<3; j++)
			m[3*i+j] = row[j];
		}
}

void mvmul3x3(double m[9], double v[3])
{	// matrix-vector multiply (3x3 by 3x1) v=m*v
	double	col[3];

	col[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2];
	col[1] = m[3]*v[0] + m[4]*v[1] + m[5]*v[2];
	col[2] = m[6]*v[0] + m[7]*v[1] + m[8]*v[2];

	v[0] = col[0];
	v[1] = col[1];
	v[2] = col[2];
}

double nrmlz(double v[3])
{	// normalize 3x1 vector  v = v/||v||
	double	len, d;

	len = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	d = 1./len;

	v[0] *= d;
	v[1] *= d;
	v[2] *= d;

	return len;
}
/*
double vdiff(double u[3], double v[3])
{	// return sup norm of difference = max |u[i]-v[i]|
	double	diff0, diff1, diff2;

	diff0 = fabs(u[0]-v[0]);
	if ((diff1 = fabs(u[1]-v[1])) > diff0) {
		if ((diff2 = fabs(u[2]-v[2])) > diff1)
			return diff2;
		else
			return diff1;
		}
	else {
		if ((diff2 = fabs(u[2]-v[2])) > diff0)
			return diff2;
		else
			return diff0;
		}
}

double bigev(double spd[9], double ev[3])
{// on entry spd is a symmetric positive definite matrix
 // returns the dominant eignevalue of spd and puts the
 // associated eigenvector in ev

	int		n=0;
	double	cp[3], eval;

	ev[2] = ev[1] = ev[0] = 1./sqrt(3.f);

	do {
		mvmul3x3(spd, ev);
		mvmul3x3(spd, ev);
		mvmul3x3(spd, ev);
		eval = nrmlz(ev);
		cp[0]=ev[0];	cp[1]=ev[1];	cp[2]=ev[2];
		mvmul3x3(spd, ev);
		eval = nrmlz(ev);
//		print3(ev);
		n++;
	} while (vdiff(ev,cp) > TVAL);

//	printf("%d iterations\n", n);
	return eval;
}
 */

double vdiff(double u[3], double v[3])
{	// return sup norm of difference = max |u[i]-v[i]|
	double	x, diff0, diff1, diff2;

	diff0 = ((x = u[0]-v[0]) >= 0.f) ? x : -x;
	diff1 = ((x = u[1]-v[1]) >= 0.f) ? x : -x;
	diff2 = ((x = u[2]-v[2]) >= 0.f) ? x : -x;
	if (diff1 > diff0) {
		if (diff2 > diff1)
			return diff2;
		else
			return diff1;
		}
	else {
		if (diff2 > diff0)
			return diff2;
		else
			return diff0;
		}
}

double bigev(double spd[9], double ev[3])
{// on entry spd is a symmetric positive definite matrix. computes
 //	the dominant eignevector of spd by multiply/normalize iteration
 //	and stores it in ev. returns the corresponding eigenvalue.

//	int		n=0;
	double	cp[3], eval;

	ev[2] = ev[1] = ev[0] = 1./sqrt(3.f);
	cp[2] = cp[1] = cp[0] = 0.f;

	while (vdiff(ev,cp) > TVAL) {
		cp[0]=ev[0];  cp[1]=ev[1];  cp[2]=ev[2];
		mvmul3x3(spd, ev);
		mvmul3x3(spd, ev);
		mvmul3x3(spd, ev);
		mvmul3x3(spd, ev);
		eval = nrmlz(ev);
//		n++;
	}

//	printf("%d iterations\n", n);
	return eval;
}

void primerot(double ev[3], double r[9])
{ // puts a rotation in r which takes ev to [1,0,0]
	double	a, b, d, c, s, r0, r1;

	r[7] = r[6] = r[5] = r[2] = 0.f;
	r[8] = 1.;
	a = ev[0];
	b = ev[1];
	d = 1./sqrt(a*a + b*b);
	r[4] = r0 = a*d;
	r1 = b*d;
	r[3] = -r1;

	a = (a*a + b*b)*d;
	b = ev[2];
	d = 1./sqrt(a*a + b*b);
	r[0] = r0*(c = a*d);
	r[1] = r1*c;
	r[2] = s = b*d;
	r[6] = -r0*s;
	r[7] = -r1*s;
	r[8] = c;
}

void QMQt(double m[9], double q[9], double qmqt[9])
{ // returns the product qmq' in qmqt
	int		i, j, k;
	double	sum, row[3];

	for (j=0; j<3; j++) {
		for (i=0; i<3; i++) {
			sum = 0.f;
			for (k=0; k<3; k++)
				sum += q[3*i+k]*m[3*k+j];
			qmqt[3*i+j] = sum;
			}
		}

	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			row[j] = 0.;
			for (k=0; k<3; k++)
				row[j] += qmqt[3*i+k]*q[3*j+k];
			}
		for (j=0; j<3; j++)
			qmqt[3*i+j] = row[j];
		}
}

void diag_1_2(double m[9], double a[9])
{// eliminates the off-diagonal pair m[1,2]|m[2,1] by a rotation R
 // at completion M=RMR' and A=RA (A accumulates the rotations)

	int		k;
	double	ev0, ev1, *row1, *row2, a1[3], a2[3];
    double   d, r, cos2a, sin2a, c, s, c2, s2;

    d = m[8]-m[4];
    r = m[5]+m[7];
	if (r || d)
		cos2a = .5*d/sqrt(d*d + r*r);
	else
		cos2a = .5;
	if (cos2a < 0.) {
		c = sqrt(.5 - cos2a);
		s = -sqrt(.5 + cos2a);
		}
	else {
		c = sqrt(.5 + cos2a);
		s = sqrt(.5 - cos2a);
		}
	if (r < 0.)
		s = -s;

    c2 = c*c;
    sin2a = 2.*s*c*m[5];
    s2 = s*s;

    ev0 = c2*m[4] - sin2a + s2*m[8];
    ev1 = s2*m[4] + sin2a + c2*m[8];

	m[4] = ev0;
	m[8] = ev1;
	m[7] = m[5] = 0.;

	row1 = &a[3];
	row2 = &a[6];
	for (k=0; k<3; k++) {
		a1[k] = c*row1[k] - s*row2[k];
		a2[k] = s*row1[k] + c*row2[k];
		}

	for (k=0; k<3; k++) {
		row1[k] = a1[k];
		row2[k] = a2[k];
		}
}