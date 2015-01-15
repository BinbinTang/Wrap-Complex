#define TVAL .000001

void    mmul3x3(double m[9], double p[9]);
void    mvmul3x3(double m[9], double v[3]);
double  nrmlz(double v[3]);
double  vdiff(double u[3], double v[3]);
double  bigev(double spd[9], double ev[3]);
void    primerot(double ev[3], double r[9]);
void    QMQt(double m[9], double q[9], double qmqt[9]);
void    diag_1_2(double m[9], double a[9]);