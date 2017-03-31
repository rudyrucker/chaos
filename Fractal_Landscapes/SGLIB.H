#ifndef _SGLIBH_
#define _SGLIBH_
/*

	Definitions for the simple graphics library

*/

#include "zaster.h"

#ifndef abs
#define abs(x) (((x) < 0) ? (-(x)) : (x))
#endif
#define torad(x) ((x) * (M_PI / 180.0))

#define X  0			   /* Coordinate indices */
#define Y  1
#define Z  2
#define T  3

#define TRUE  1
#define FALSE 0

typedef double point3d[3];	   /* Three dimensional point */
typedef double vector[4];	   /* Homogeneous coordinate vector */
typedef double matrix[4][4];	   /* Transformation matrix */

extern double vecdot(); 	   /* Dot product */
extern double vecmag(); 	   /* Magnitude of a vector */

extern matrix ct;		   /* Current transformation matrix */

extern void vecget(vector v, double x, double y, double z);
extern void vecxmat(vector vo, vector v, matrix m);
extern void vecput(double *x, double *y, double *z, vector v);
extern void vecsub(point3d o, point3d a, point3d b);
extern void vecscal(point3d o, point3d a, double s);
extern double vecmag(point3d a);
extern void vecnorm(point3d o, point3d a);
extern void veccross(point3d o, point3d a, point3d b);
extern void matident(matrix a);
extern void matrot(matrix m, double theta, int j);
extern void matpers(matrix m, double alpha, double zn, double zf);
extern void matorie(matrix m, double a, double b, double c, double d,
   double e, double f, double p, double q, double r);
extern void matshad(matrix m, double x, double y, double z, int w);
extern void tran(double tx, double ty, double tz);
extern void scal(double sx, double sy, double sz);
extern void rot(double theta, int j);
extern void pers(double alpha, double zn, double zf);
extern void orie(double a, double b, double c, double d, double e, double f, double p, double q, double r);
extern void shad(double x, double y, double z, int w);
extern void mattran(matrix m, double tx, double ty, double tz);
extern void matscal(matrix m, double sx, double sy, double sz);
extern void matmul(matrix o, matrix a, matrix b);
extern void matcopy(matrix o, matrix a);
extern void pointget(point3d p, double x, double y, double z);
extern void pointcopy(point3d po, point3d p);
extern double vecdot(point3d a, point3d b);

#endif
