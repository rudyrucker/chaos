#include "attract.h"
#include <math.h>

fpair yorke(fpair * u)
{

	fpair temp;
	int xwrap = 0, ywrap = 0;

	temp.x = u->x + omega1 + epsbar * (
					 A110 * sin(TWOPI * (u->x + B110)) +
					 A101 * sin(TWOPI * (u->y + B101)) +
				  A111 * sin(TWOPI * (u->x + u->y + B111)) +
				A11m1 * sin(TWOPI * (u->x - u->y + B11m1)));
	if (temp.x > 1)
	{
		xwrap = (int) temp.x;
		temp.x -= xwrap;
	}
	if (temp.x < 0)
	{
		xwrap = 1 + (int) temp.x;
		temp.x += xwrap;
	}
	temp.y = u->y + omega2 + epsbar * (
					 A210 * sin(TWOPI * (u->x + B210)) +
					 A201 * sin(TWOPI * (u->y + B201)) +
				  A211 * sin(TWOPI * (u->x + u->y + B211)) +
				A21m1 * sin(TWOPI * (u->x - u->y + B21m1)));
	if (temp.y > 1)
	{
		ywrap = (int) temp.y;
		temp.y -= (int) temp.y;
	}
	if (temp.y < 0)
	{
		ywrap = 1 + (int) temp.y;
		temp.y += ywrap;
	}
	if (yorketopologyflag)
	{			/* projective plane instead of torus */
		if (ywrap & 1)
			temp.x = 1 - temp.x;
		if (yorketopologyflag == 2 && xwrap & 1)
			temp.y = 1 - temp.y;
	}

	return temp;
}

void yorke_step(void)
{
	static int color;
	int i, j, k;
	fpair s, t;
	pair p, q, u, v;

	for (i = 0; i < fflock2ptr->n; i++)
	{
		color = (int) (fflock2ptr->color[i]);
		s = fflock2ptr->atom[i];
		t = yorke(&s);
		if (soundflag && !i)
			sound(200 + (unsigned) (200 * t.y));
		fflock2ptr->atom[i] = t;
		q = convertpair(&t);
		Pix(q.x, q.y, color);
		if (tracetype)
		{		/* ribbon case */
			p = convertpair(&s);
			Bres(p.x, p.y, q.x, q.y, color);
			if (!ribbonfull & !ribbonindex)
			{	/* first time */
				ribbon[0]->atom[i] = p;
				ribbon[1]->atom[i] = q;
				k = 2;
			}
			else
			{
				k = j = ribbonindex;
				k++;
				if (k == ribbonlength)
					k = 0;
				if (ribbonfull)
				{
					u = ribbon[j]->atom[i];
					v = ribbon[k]->atom[i];
					Bres(u.x, u.y, v.x, v.y, 0);
				}
				ribbon[j]->atom[i] = q;
			}
		}
	}
	if (tracetype)
	{
		ribbonindex = k;/* inc index after done whole flock */
		if (!k)
			ribbonfull = 1;	/* ribbon full after first wrap */
	}
}


