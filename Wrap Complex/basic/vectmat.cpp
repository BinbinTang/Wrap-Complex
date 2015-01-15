#include "vectmat.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M_PI 3.141592654
 

Vector3 Vector3::OnLine(Vector3 p0,Vector3 p1)
{
  Vector3 ans,n;

  n = p1-p0;
  n.normalize();

  ans = p0 + n*dot_prod(n,*this-p0);
  return ans;
}

double Vector3::DistWithLine(Vector3 p0,Vector3 p1)
{
  return ((*this)-OnLine(p0,p1)).length();
}

bool Vector3::operator == (Vector3 v)
{
	return (x[0]==v.x[0]) && (x[1]==v.x[1]) && (x[2]==v.x[2]);
}

Vector3 Vector3::operator = (Vector3 v)
{
  x[0] = v.x[0];
  x[1] = v.x[1];
  x[2] = v.x[2];
  return v;
}

Vector3 Vector3::operator - (Vector3 x2)
{
  Vector3 diff;

  diff.x[0] = x[0] - x2.x[0];
  diff.x[1] = x[1] - x2.x[1];
  diff.x[2] = x[2] - x2.x[2];

  return diff;
}

Vector3 Vector3::operator - ()
{
  Vector3 neg;
  neg.x[0] = -x[0];
  neg.x[1] = -x[1];
  neg.x[2] = -x[2];
  return neg;
}

Vector3 Vector3::operator + (Vector3 x2)
{
  Vector3 sum;

  sum.x[0] = x[0] + x2.x[0];
  sum.x[1] = x[1] + x2.x[1];
  sum.x[2] = x[2] + x2.x[2];

  return sum;
}

Vector3 Vector3::operator * (double d)
{
  Vector3 prod;

  prod.x[0] = x[0] * d;
  prod.x[1] = x[1] * d;
  prod.x[2] = x[2] * d;

  return prod;
}

Vector3 Vector3::operator * (Matrix33 m)
{
  Vector3 prod;
  int i,j;

  for(i=0;i<3;i++)
  {
    prod.x[i] = 0;
    for(j=0;j<3;j++)
      prod.x[i] += x[j] * m(j,i);
  }
  
  return prod;
}

Vector3 Vector3::operator / (double d)
{
  Vector3 qu;

  Assert(d!=0);

  qu.x[0] = x[0] / d;
  qu.x[1] = x[1] / d;
  qu.x[2] = x[2] / d;

  return qu;
}

/*
Vector3 Vector3::operator = (float* y)
{
  Vector3 dummy;

  dummy.x[0] = x[0] = y[0];
  dummy.x[1] = x[1] = y[1];
  dummy.x[2] = x[2] = y[2];

  return dummy;
}
*/

Vector3 Vector3::operator = (double* y)
{
  Vector3 dummy;

  dummy.x[0] = x[0] = y[0];
  dummy.x[1] = x[1] = y[1];
  dummy.x[2] = x[2] = y[2];

  return dummy;
}

void Vector3::show()
{
  printf("(%16.12f %16.12f %16.12f) \n",x[0],x[1],x[2]);
}

float* Vector3::fp()
{
  static float f[3];
  f[0] = (float) x[0];
  f[1] = (float) x[1];
  f[2] = (float) x[2];
  return f;
}


void Vector3::normalize()
{
  double len;

  len = sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);

  if(len!=0)
  {
    x[0]/=len;
    x[1]/=len;
    x[2]/=len;
  }
}


double dot_prod(Vector3 v1,Vector3 v2)
{
  return (v1.x[0]*v2.x[0]) + (v1.x[1]*v2.x[1]) + (v1.x[2]*v2.x[2]);
}

Vector3 cross_prod(Vector3 v1,Vector3 v2)
{
  Vector3 prod;

  prod.x[0] =   v1.x[1] * v2.x[2] - v1.x[2] * v2.x[1];
  prod.x[1] = -(v1.x[0] * v2.x[2] - v1.x[2] * v2.x[0]);
  prod.x[2] =   v1.x[0] * v2.x[1] - v1.x[1] * v2.x[0];

  return prod;
}


void intersect(Vector3 p0,Vector3 p1,Vector3 n0,Vector3 n1,
               Vector3 n,double d,Vector3& out,Vector3& outn)
{
  Vector3 p0p1,tp0p1;
  double t;
  Vector3 tn0,oneminustn1;

  n0.normalize();
  n1.normalize();
  p0p1 = p1 - p0;
  if(dot_prod(p0p1,n)==0)
  {
    out = p1;
    outn = n1;
    return;
/*
    p0p1.show();
    n.show();
    puts("LALALA");
*/
  }
  t = (d - dot_prod(p0,n)) / dot_prod(p0p1,n);

/*
  if(VERYSMALL(p0.x[0])&&VERYSMALL(p0.x[1])&&VERYSMALL(p0.x[2]))
  {
    printf("warning 1\n");
  }
  if(VERYSMALL(p1.x[0])&&VERYSMALL(p1.x[1])&&VERYSMALL(p1.x[2]))
  {
    printf("warning 2\n");
  }
*/
/*
  if(VERYSMALL(t))
  {
    puts("t");
    p0.show();
    p1.show();
    printf("%6.2f",d);
    n.show();
  }
  if(VERYSMALL(1-t))
  {
    puts("1-t");
    p0.show();
    p1.show();
    printf("%6.2f",d);
    n.show();
  }
*/
  tp0p1 = p0p1 * t;

/*
  tn0   = n0   * t;
  oneminustn1 = n1 * (1-t);
*/
  tn0   = n1 * t ;
  oneminustn1 = n0 * (1-t);

  out = p0 + tp0p1;
  outn = tn0 + oneminustn1;
  outn.normalize();
};

Matrix33::Matrix33(double x00,double x01,double x02,
		      double x10,double x11,double x12,
			  double x20,double x21,double x22)
{
	m[0][0] = x00;
	m[0][1] = x01;
	m[0][2] = x02;
	m[1][0] = x10;
	m[1][1] = x11;
	m[1][2] = x12;
	m[2][0] = x20;
	m[2][1] = x21;
	m[2][2] = x22;
}

void Matrix33::SetToIdentical()
{
  int i,j;
  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
      m[i][j] = i==j ? 1 : 0;
}

Matrix33 Matrix33::operator=(Matrix33 input)
{
  int i,j;
  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
      m[i][j] = input.m[i][j];
  return input;
}

void Matrix33::show()
{
  int i,j;
  for(i=0;i<3;i++)
  {
    printf(" | ");  
    for(j=0;j<3;j++)
      printf("%20.10f ",m[i][j]);
    puts("|");
  }
  printf("\n");
}

Matrix33 Matrix33::operator*(Matrix33 input)
{
  Matrix33 prod;
  int i,j,k;
  double sum;

  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
    {
      sum = 0;
      for(k=0;k<3;k++)
        sum+= m[i][k] * input.m[k][j];
      prod.m[i][j] = sum;
    }

  return prod;
}

Matrix33 Matrix33::Transpose()
{
	Matrix33 t;
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			t.m[i][j] = m[j][i];
	return t;
}

Matrix33 Matrix33::Inverse(bool* sing)
{
  Matrix33 inv;
  int i,j,k;

  if(sing)
	  *sing = false;
  double im[3][6];
  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
    {
      im[i][j] = m[i][j];
      im[i][j+3] = i==j ? 1 : 0;
    }

  for(i=0;i<3;i++)
  {
    if( im[i][i] == 0.0)
    {
      for(k=i+1;k<3&&im[k][i]==0.0;k++);
	  Assert(k!=3);
	  if(k==3)
		  if(sing)
			  *sing = true;
      for(j=i;j<6;j++)
        im[i][j] += im[k][j];
    }

    for(k=i;k<3;k++)
      if(im[k][i] != 0.0)
        for(j=5;j>=i;j--)
          im[k][j] /= im[k][i];
    for(k=i+1;k<3;k++)
      if(im[k][i] != 0.0)
        for(j=i;j<6;j++)
          im[k][j] -= im[i][j];
  }

  for(i=0;i<2;i++)
    for(k=i+1;k<3;k++)
      for(j=5;j>=k;j--)
        im[i][j] -= im[k][j] * im[i][k];

  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
      inv.m[i][j] = im[i][j+3];


  return inv;
}


double length(Vector3 v)
{
  return sqrt( v.x[0]*v.x[0] + v.x[1]*v.x[1] + v.x[2]*v.x[2]);
}

double lengthsqr(Vector3 v)
{
  return ( v.x[0]*v.x[0] + v.x[1]*v.x[1] + v.x[2]*v.x[2]);
}

Matrix33 Rotation(double theta,int axis)
{
  Matrix33 M;

  switch(axis)
  {
    case X_AXIS:
      M.m[1][1] = cos(theta);
      M.m[1][2] = sin(theta);
      M.m[2][2] = cos(theta);
      M.m[2][1] = -sin(theta);
      break;

    case Y_AXIS:
      M.m[0][0] = cos(theta);
      M.m[0][2] = -sin(theta);
      M.m[2][2] = cos(theta);
      M.m[2][0] = sin(theta);
      break;


    case Z_AXIS:
      M.m[0][0] = cos(theta);
      M.m[0][1] = sin(theta);
      M.m[1][1] = cos(theta);
      M.m[1][0] = -sin(theta);
      break;

    default:
      break;
  };

  return M;
}

Matrix33 AlignWithYAxis(Vector3 v1,Vector3 v2)
{
  Matrix33 R1 = AlignWithYAxis(v1);
  Vector3 v=v2*R1;
  double theta;

  if(v.x[0]==0)
  {
    if(v.x[2] > 0)
      theta = M_PI/2;
    else
      theta = M_PI*3/2;
  }
  else
  {
    theta = atan(v.x[2]/v.x[0]);
    if(v.x[0] < 0)
      theta += M_PI;
  }

  return R1*Rotation(theta,Y_AXIS);
}


Matrix33 AlignWithYAxis(Vector3 v)
{
  Matrix33 R;
  double theta,phi;
  Vector3 temp;

  if((v.x[0]==0)&&(v.x[2]==0))
  {
	  if(v.x[1]>0)
		R = Matrix33(1,0,0,0,1,0,0,0,1);
	  else
		R = Matrix33(-1,0,0,0,-1,0,0,0,1);
  } else if((v.x[0]==0)&&(v.x[1]==0))
  {
	  if(v.x[2]>0)
		R = Matrix33(1,0,0,0,0,-1,0,1,0);
	  else
		R = Matrix33(1,0,0,0,0,1,0,-1,0);
  } else if((v.x[2]==0)&&(v.x[1]==0))
  {
	  if(v.x[0]>0)
		R = Matrix33(0,1,0,-1,0,0,0,0,1);
	  else
		R = Matrix33(0,-1,0,1,0,0,0,0,1);
  } else {
	  if(v.x[0]==0)
	  {
		if(v.x[2] > 0)
		  theta = M_PI/2;
		else
		  theta = M_PI*3/2;
	  }
	  else
	  {
		theta = atan(v.x[2]/v.x[0]);
		if(v.x[0] < 0)
		  theta += M_PI;
	  }
	  temp = v * Rotation(theta,Y_AXIS);
	  if(temp.x[1]==0)
	  {
		if(temp.x[0] > 0)
		  phi = M_PI/2;
		else
		  phi = M_PI*3/2;
	  }
	  else
	  {
		phi = atan(temp.x[0]/temp.x[1]);
		if(temp.x[1]<0)
		  phi += M_PI;
	  }
	  R = (Rotation(theta,Y_AXIS)*Rotation(phi,Z_AXIS));
  }
  Assert( (v*R).x[1]>0);
  return R;
}

int side(int v1,int v2)
{
  if(v1==0&&v2==1)
    return SIDE01;

  if(v1==1&&v2==2)
    return SIDE12;

  if(v1==2&&v2==0)
    return SIDE20;

  if(v1==1&&v2==0)
    return SIDE10;

  if(v1==2&&v2==1)
    return SIDE21;

  if(v1==0&&v2==2)
    return SIDE02;

  puts("WIRED!!!!!");
    return 0;
}

int PtQuiteNear(Vector3 v1,Vector3 v2)
{
/*
  if(QUITESMALL ( length(v1-v2) ) )
  {
    puts("PQN");
    v1.show();
    v2.show();
    printf("%f\n",length(v1-v2));
    puts("PQNE");
  }
*/
//  return QUITESMALL( length(v1-v2) );
//  return length(v1-v2) < 0.00001;
//  return VERYSMALL(v1.x[0]-v2.x[0])&&VERYSMALL(v1.x[1]-v2.x[1])&&VERYSMALL(v1.x[2]-v2.x[2]);
  return MEDIUMSMALL(v1.x[0]-v2.x[0])&&MEDIUMSMALL(v1.x[1]-v2.x[1])&&MEDIUMSMALL(v1.x[2]-v2.x[2]);
}

/*
double min(double a,double b)
{
  return a>b?b:a;
}

double max(double a,double b)
{
  return a>b?a:b;
}

*/

double atanh(double x)
{
	return 0.5*log((1+x)/(1-x));
}

Vector3 tetraCenter(Vector3 a,Vector3 b,Vector3 c,Vector3 d)
{
	Vector3 abm = (a+b)/2;
	Vector3 acm = (a+c)/2;
	Vector3 adm = (a+d)/2;

	Vector3 ab = b-a;
	Vector3 ac = c-a;
	Vector3 ad = d-a;
/*
	Matrix33 m(ab.x[0],ac.x[0],ad.x[0],
		       ab.x[1],ac.x[1],ad.x[1],
			   ab.x[2],ac.x[2],ad.x[2]);

	Vector3 p(dot_prod(abm,ab),
		      dot_prod(acm,ac),
		      dot_prod(adm,ad));


	return p*m.Inverse();
*/
	return intersect3Planes(ab,dot_prod(abm,ab),ac,dot_prod(acm,ac),ad,dot_prod(adm,ad));
}

Vector3 circumCenter(Vector3 a,Vector3 b,Vector3 c,bool* sing)
{
	Vector3 abm = (a+b)/2;
	Vector3 acm = (a+c)/2;

	Vector3 ab = b-a;
	Vector3 ac = c-a;
	Vector3 n  = cross_prod(ab,ac);

/*
	Matrix33 m(ab.x[0],ac.x[0],n.x[0],
		       ab.x[1],ac.x[1],n.x[1],
			   ab.x[2],ac.x[2],n.x[2]);

	Vector3 p(dot_prod(abm,ab),
		      dot_prod(acm,ac),
		      dot_prod(c,n));
	return p*m.Inverse();
*/
	return intersect3Planes(ab,dot_prod(abm,ab),ac,dot_prod(acm,ac),n,dot_prod(c,n),sing);

}

Vector3 intersect3Planes(Vector3 n1,double d1,Vector3 n2,double d2,Vector3 n3,double d3,bool* sing)
{

  Matrix33 m(n1.x[0],n2.x[0],n3.x[0],
	         n1.x[1],n2.x[1],n3.x[1],
			 n1.x[2],n2.x[2],n3.x[2]);

  Vector3  p(d1,d2,d3);

  return p*m.Inverse(sing);
}

Bool lineIntersectTri(Vector3 x,Vector3 n,Vector3 a,Vector3 b,Vector3 c)
{
	Vector3 y = x+n;
	double v1,v2,v3;
	
	if(x==a)
		return true;
	if(x==b)
		return true;
	if(x==c)
		return true;

	v1 = Orient4(x,y,a,b);
	v2 = Orient4(x,y,b,c);
	int sign1 = (v1>0.0) ? 1 : ((v1<0.0)?-1:0);
	int sign2 = (v2>0.0) ? 1 : ((v2<0.0)?-1:0);
	int sign3;
	if(sign1!=sign2)
		return FALSE;
	v3 = Orient4(x,y,c,a);
	sign3 = (v3>0.0) ? 1 : ((v3<0.0)?-1:0);

	return sign3==sign2;
}

double Orient4(Vector3 a1,Vector3 a2,Vector3 a3,Vector3 a4)
{
	Vector3 b1,b2,b3;
	b1 = a2-a1;
	b2 = a3-a1;
	b3 = a4-a1;

	return Orient3(b1,b2,b3);
}

double Orient3(Vector3 a1,Vector3 a2,Vector3 a3)
{
	return a1.x[0] * (a2.x[1]*a3.x[2]-a2.x[2]*a3.x[1]) -
		   a1.x[1] * (a2.x[0]*a3.x[2]-a2.x[2]*a3.x[0]) +
		   a1.x[2] * (a2.x[0]*a3.x[1]-a2.x[1]*a3.x[0]) ;
}

Vector3 lineIntersectPlane(Vector3 lv,Vector3 ln,Vector3 pn,double d)
{

	double nominator = dot_prod(pn,ln);
	Assert(nominator!=0.0);
	Vector3 result = lv+ln*( (d-dot_prod(lv,pn)) / nominator);
	return result;
}

double segmentIntersectPlane(Vector3 p1,Vector3 p2,Vector3 pn,double d)
{

	double nominator = dot_prod(pn,p2-p1);
	Assert(nominator!=0.0);
	return  (d-dot_prod(p1,pn)) / nominator;
}

double angle(Vector3 a,Vector3 b,Vector3 c)
{
	Vector3 ba = a-b;
	Vector3 bc = c-b;
	ba.normalize();
	bc.normalize();
	return acos(dot_prod(ba,bc));
}

double plane2PtDist2(Vector3 a,Vector3 b,Vector3 c,Vector3 v)
{
    Vector3 n = cross_prod(c-b,a-b);
    double tmp = dot_prod(n,v-b);
    return tmp*tmp/n.lengthsqr();
}

double line2PtDist2(Vector3 a,Vector3 b,Vector3 v)
{
    Vector3 av = v-a;
    Vector3 ab = b-a;
    return (av-(ab*(dot_prod(av,ab)/ab.lengthsqr()))).lengthsqr();
}

double tri2PtDist2(Vector3 a,Vector3 b,Vector3 c,Vector3 v)
{

    Vector3 n = cross_prod(c-b,a-b);
	Vector3 ba = a-b;
	Vector3 cb = b-c;
	Vector3 ac = c-a;
	Vector3 bv = v-b;
	Vector3 cv = v-c;
	Vector3 av = v-a;
//    bool outab,outbc,outca;

	double dummy[11];


	if(((dummy[0]=dot_prod(av,ac))<=0)&&((dummy[1]=dot_prod(av,-ba))<=0))
		return av.lengthsqr();
	if(((dummy[2]=dot_prod(bv,ba))<=0)&&((dummy[3]=dot_prod(bv,-cb))<=0))
		return bv.lengthsqr();
	if(((dummy[4]=dot_prod(cv,cb))<=0)&&((dummy[5]=dot_prod(cv,-ac))<=0))
		return cv.lengthsqr();

	if(/*outab = */((dummy[6]=dot_prod(cross_prod(ba,n),bv))<0)
		&&(dot_prod(av,-ba)>=0)
		&&(dot_prod(bv,ba)>=0))
		return line2PtDist2(a,b,v);
    if(/*outbc = */(dot_prod(cross_prod(cb,n),cv)<0)
		&&(dot_prod(cv,cb)>=0)
		&&(dot_prod(bv,-cb)>=0))
		return line2PtDist2(b,c,v);
    if(/*outca = */(dot_prod(cross_prod(ac,n),av)<0)
		&&(dot_prod(cv,-ac)>=0)
		&&(dot_prod(av,ac)>=0))
		return line2PtDist2(c,a,v);

	/*
    if(outab)
    {
        if(outbc)
        {
            Assert(!outca);
            return (v-b).lengthsqr();
        } else if(outca)
        {
            return (v-a).lengthsqr();
        } else
            return line2PtDist2(a,b,v);
    } else if(outbc) {
        if(outca)
            return (v-c).lengthsqr();
        else
            return line2PtDist2(b,c,v);
    } else if(outca)
        return line2PtDist2(c,a,v);
*/
	assert(lineIntersectTri(v,n,a,b,c));
    return plane2PtDist2(a,b,c,v);
}


double cosDihedralAngle(Vector3 a,Vector3 b,Vector3 c,Vector3 d)
{
	Vector3 abcn = cross_prod(b-a,c-a);
	abcn.normalize();
	Vector3 badn = cross_prod(a-b,d-b);
	badn.normalize();

	return dot_prod(abcn,badn);


}