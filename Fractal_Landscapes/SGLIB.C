/*
 * The Simple Graphics Library
 * 
 * Implemented by John Walker in March of 1988.
 * 
 * This  is  a  special, stripped down, version of SGLIB prepared expressly for
 * inclusion in the fractal landscape generator for Chaos.
 * 
 * This is a simple three dimensional transformation and modeling library
 * based   on   Jim  Blinn's  modeling  primitives,  as documented in Jim
 * Blinn's Corner, IEEE Computer  Graphics  and Applications, October 1987,
 * and subsequent columns.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

#include "sglib.h"

/* Coordinate system transforms  */

matrix ct = {			/* Current transformation matrix */
	{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 0.0, 0.0, 1.0}
};

/* General vector routines  */

/* VECGET  --	Set vector from X, Y, and Z coordinates  */

void vecget(v, x, y, z)
	vector v;
	double x, y, z;
{
	v[X] = x;
	v[Y] = y;
	v[Z] = z;
	v[T] = 1.0;
}

/* VECPUT  --	Store vector into X, Y, and Z coordinates  */

void vecput(x, y, z, v)
	double *x, *y, *z;
	vector v;
{
	double w;

	w = v[T];
	*x = v[X] / w;
	*y = v[Y] / w;
	*z = v[Z] / w;
}

#ifdef NEEDED

/* VECCOPY  --  Copy vector to another  */

void veccopy(vo, v)
	vector vo, v;
{
	register int i;

	for (i = X; i <= T; i++)
		vo[i] = v[i];
}

#endif

/* VECXMAT  --  Multiply a vector by a matrix	 */

void vecxmat(vo, v, m)
	vector vo, v;
	matrix m;
{
	register int i, j;
	register double sum;

	for (i = 0; i < 4; i++)
	{
		sum = 0;
		for (j = 0; j < 4; j++)
		{
			sum += v[j] * m[j][i];
		}
		vo[i] = sum;
	}
}

/* Vector algebra routines which operate on points  */

/* POINTGET  --  Set point from X, Y, and Z coordinates  */

void pointget(p, x, y, z)
	point3d p;
	double x, y, z;
{
	p[X] = x;
	p[Y] = y;
	p[Z] = z;
}

/* POINTCOPY  --  Copy point to another  */

void pointcopy(po, p)
	point3d po, p;
{
	po[X] = p[X];
	po[Y] = p[Y];
	po[Z] = p[Z];
}

/*
 * VECDOT  --	Computes the dot (inner) product of two vectors and returns
 * the result as a double.  Since this will frequently be used on points as
 * well as vectors, only the first three terms are computed.
 */

double vecdot(a, b)
	point3d a, b;
{
	int i;
	double product;

	product = 0.0;
	for (i = 0; i < 3; i++)
	{
		product += a[i] * b[i];
	}

	return product;
}

/*
 * VECCROSS  --  Computes the cross product of two vectors and stores the
 * result in a third.  This actually works on points; if a vector is passed,
 * the fourth item is ignored.
 */

void veccross(o, a, b)
	point3d o, a, b;
{
	point3d r;

	r[X] = a[Y] * b[Z] - a[Z] * b[Y];
	r[Y] = a[Z] * b[X] - a[X] * b[Z];
	r[Z] = a[X] * b[Y] - a[Y] * b[X];

	pointcopy(o, r);
}

#ifdef NEEDED

/*
 * VECADD  --	Add two vectors and store the sum in a third. Operates on
 * points.
 */

void vecadd(o, a, b)
	point3d o, a, b;
{
	o[X] = a[X] + b[X];
	o[Y] = a[Y] + b[Y];
	o[Z] = a[Z] + b[Z];
}

#endif

/*
 * VECSUB  --	Subtracts vector b from vector a and stores the result in
 * vector o.  Expects points as arguments.
 */

void vecsub(o, a, b)
	point3d o, a, b;
{
	o[X] = a[X] - b[X];
	o[Y] = a[Y] - b[Y];
	o[Z] = a[Z] - b[Z];
}

/*
 * VECSCAL  --  Multiply vector by a scalar and store the result in a second
 * vector.  Expects points.
 */

void vecscal(o, a, s)
	point3d o, a;
	double s;
{
	o[X] = a[X] * s;
	o[Y] = a[Y] * s;
	o[Z] = a[Z] * s;
}

/*
 * VECMAG  --	Returns magnitude of a vector.	This expects a point and uses
 * only the first three terms.
 */

double vecmag(a)
	point3d a;
{
	return sqrt(a[X] * a[X] + a[Y] * a[Y] + a[Z] * a[Z]);
}

/*
 * VECNORM  --  Normalise vector and store normalised result in a second
 * vector.  Works on points.
 */

void vecnorm(o, a)
	point3d o, a;
{
	vecscal(o, a, 1.0 / vecmag(a));
}

#ifdef NEEDED

/* VECPRINT  --  Print a vector  */

void vecprint(v)
	vector v;
{
	int j;

	fprintf(stderr, "+-----------------------------------------+\n");
	fprintf(stderr, "|");
	for (j = 0; j < 4; j++)
	{
		fprintf(stderr, " %9.4f", v[j]);
	}
	fprintf(stderr, " |\n");
	fprintf(stderr, "+-----------------------------------------+\n");
}

#endif

/* General matrix routines  */

/* MATMUL  --	Multiply two 4 X 4 matrices, storing copy in a third.  */

void matmul(o, a, b)
	matrix o, a, b;
{
	register int i, j, k;
	register double sum;

	for (i = 0; i < 4; i++)
	{
		for (k = 0; k < 4; k++)
		{
			sum = 0.0;
			for (j = 0; j < 4; j++)
			{
				sum += a[i][j] * b[j][k];
			}
			o[i][k] = sum;
		}
	}
}

/* MATIDENT  --  Set a matrix to the identity matrix  */

void matident(a)
	matrix a;
{
	register int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			a[i][j] = (i == j) ? 1.0 : 0.0;
		}
	}
}

/* MATCOPY  --  Copy a matrix to another  */

void matcopy(o, a)
	matrix o, a;
{
	register int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			o[i][j] = a[i][j];
		}
	}
}

#ifdef NEEDED

/* MATPRINT  --  Print a matrix  */

void matprint(a)
	matrix a;
{
	int i, j;

	fprintf(stderr, "+-----------------------------------------+\n");
	for (i = 0; i < 4; i++)
	{
		fprintf(stderr, "|");
		for (j = 0; j < 4; j++)
		{
			fprintf(stderr, " %9.4f", a[i][j]);
		}
		fprintf(stderr, " |\n");
	}
	fprintf(stderr, "+-----------------------------------------+\n");
}

#endif

/* Transformation matrix construction routines  */

/* MATTRAN  --  Build translation matrix  */

void mattran(m, tx, ty, tz)
	matrix m;
	double tx, ty, tz;
{
	matident(m);
	m[T][X] = tx;
	m[T][Y] = ty;
	m[T][Z] = tz;
}

/* MATSCAL  --  Build scaling matrix  */

void matscal(m, sx, sy, sz)
	matrix m;
	double sx, sy, sz;
{
	matident(m);
	m[X][X] = sx;
	m[Y][Y] = sy;
	m[Z][Z] = sz;
}

/*
 * MATROT  --	Build rotation matrix.	THETA is the rotation angle, in
 * radians, and J is the axis about which the rotation is to be performed,
 * expressed as one of the manifest constants X, Y, or Z.
 */

void matrot(m, theta, j)
	matrix m;
	double theta;
	int j;
{
	double s, c;

	s = sin(theta);
	c = cos(theta);

	matident(m);
	switch (j)
	{

	case X:
		m[1][1] = m[2][2] = c;
		m[1][2] = -s;
		m[2][1] = s;
		break;

	case Y:
		m[0][0] = m[2][2] = c;
		m[0][2] = s;
		m[2][0] = -s;
		break;

	case Z:
		m[0][0] = m[1][1] = c;
		m[0][1] = -s;
		m[1][0] = s;
		break;

	default:
		fprintf(stderr, "\nInvalid axis (J) argument %d to matrot.\n",
			j);
		abort();
	}
}

#ifdef NEEDED

/*
 * MATPERS  --  Build perspective transformation matrix.  ALPHA is the field
 * of view, ZN is the near clipping plane, and ZF is the far clipping plane.
 */

void matpers(m, alpha, zn, zf)
	matrix m;
	double alpha, zn, zf;
{
	double s, c, q;

	s = sin(alpha / 2.0);
	c = cos(alpha / 2.0);
	q = s / (1.0 - zn / zf);
	matident(m);
	m[X][X] = m[Y][Y] = c;
	m[Z][Z] = q;
	m[T][Z] = -q * zn;
	m[Z][T] = s;
	m[T][T] = 0.0;
}

#endif

#ifdef NEEDED

/* MATORIE  --  Specify explicit orientation  */

void matorie(m, a, b, c, d, e, f, p, q, r)
	matrix m;
	double a, b, c, d, e, f, p, q, r;
{
	matident(m);
	m[0][0] = a;
	m[1][0] = b;
	m[2][0] = c;
	m[0][1] = d;
	m[1][1] = e;
	m[2][1] = f;
	m[0][2] = p;
	m[1][2] = q;
	m[2][2] = r;
}

#endif

#ifdef NEEDED

/*
 * MATSHAD  --  Specify matrix for fake shadow generation.  The light source
 * is at X, Y, and Z, and W is FALSE for a light source at infinity and TRUE
 * for a local light source.
 */

void matshad(m, x, y, z, w)
	matrix m;
	double x, y, z;
	int w;
{
	matident(m);
	m[0][0] = z;
	m[1][1] = z;
	m[2][0] = -x;
	m[2][1] = -y;
	m[2][2] = 0.0;
	m[2][3] = w ? -1.0 : 0.0;
	m[3][3] = z;
}

#endif

/* Current coordinate system transformation composition routines  */

/* TRAN  --  Compose translation matrix  */

void tran(tx, ty, tz)
	double tx, ty, tz;
{
	matrix m, m1;

	mattran(m, tx, ty, tz);
	matmul(m1, m, ct);
	matcopy(ct, m1);
}

/* SCAL  --  Build scaling matrix  */

void scal(sx, sy, sz)
	double sx, sy, sz;
{
	matrix m, m1;

	matscal(m, sx, sy, sz);
	matmul(m1, m, ct);
	matcopy(ct, m1);
}

/*
 * ROT  --  Build rotation matrix.  THETA is the rotation angle, in radians,
 * and J is the axis about which the rotation is to be performed, expressed
 * as one of the manifest constants X, Y, or Z.
 */

void rot(theta, j)
	double theta;
	int j;
{
	matrix m, m1;

	matrot(m, theta, j);
	matmul(m1, m, ct);
	matcopy(ct, m1);
}

#ifdef NEEDED

/*
 * PERS  --  Build perspective transformation matrix.	ALPHA is the field of
 * view, ZN is the near clipping plane, and ZF is the far clipping plane.
 */

void pers(alpha, zn, zf)
	double alpha, zn, zf;
{
	matrix m, m1;

	matpers(m, alpha, zn, zf);
	matmul(m1, m, ct);
	matcopy(ct, m1);
}

/* ORIE  --  Specify explicit orientation  */

void orie(a, b, c, d, e, f, p, q, r)
	double a, b, c, d, e, f, p, q, r;
{
	matrix m, m1;

	matorie(m, a, b, c, d, e, f, p, q, r);
	matmul(m1, m, ct);
	matcopy(ct, m1);
}

/*
 * SHAD  --  Compose matrix for fake shadow generation.  The light source is
 * at X, Y, and Z, and W is FALSE for a light source at infinity and TRUE for
 * a local light source.
 */

void shad(x, y, z, w)
	double x, y, z;
	int w;
{
	matrix m, m1;

	matshad(m, x, y, z, w);
	matmul(m1, m, ct);
	matcopy(ct, m1);
}

#endif
