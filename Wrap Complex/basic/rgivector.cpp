
#include <math.h>
#include <string.h>

#include "binio.h"
#include "rgivector.h"


void RgiVector::
shift(double dx, double dy, double dz)
{

	v_[0] += dx;
	v_[1] += dy;
	v_[2] += dz;
}

ostream& operator<<(ostream& s, const RgiVector& u) {
	s << "(" << u[0] << ","
	         << u[1] << ","
		 << u[2] << ")";
	return s;
}

istream& operator>>(istream& s, RgiVector& u) {
    char ch;

    s >> ch;
    if(ch!='(') { s.clear(ios::badbit); return s; }

    s >> u[0]; 
    if(!s.good()) { return s; }

    s >> ch;
    if(ch!=',') { s.clear(ios::badbit); return s; }

    s >> u[1]; 
    if(!s.good()) { return s; }

    s >> ch;
    if(ch!=',') { s.clear(ios::badbit); return s; }
    
    s >> u[2]; 
    if(!s.good()) { return s; }

    s >> ch;
    if(ch!=')') { s.clear(ios::badbit); return s; }

    return s;
}

void RgiVector::
binDump (BinOutput &bo)
{
	bo.writeDoubles (v_, 3);
}

int RgiVector::
binInit (BinInput &bi)
{
	return bi.readDoubles (v_, 3);
}

