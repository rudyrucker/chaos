#include "attract.h"


fpair henon(fpair * u, int *colorptr)
{
	fpair temp;

	temp.x = 1 + u->y - ha * u->x * u->x;
	temp.y = hb * u->x;
	if (abs(temp.x) > 20)
	{
		temp.x = fcenterx + (double) sixteenbitsa() * fdeltax_div_64K;
		*colorptr = 0;
	}
	if (abs(temp.y) > 20)
	{
		temp.y = fcentery + (double) sixteenbitsa() * fdeltay_div_64K;
		*colorptr = 0;
	}
	return temp;
}

fpair genhenon(fpair * u, int *colorptr)
{

	fpair temp;
	double diff;

	diff = u->y - u->x * u->x;
	temp.x = cosa * u->x - sina * diff;
	temp.y = sina * u->x + cosa * diff;
	if (abs(temp.x) > 20)
	{
		temp.x = fcenterx + (double) sixteenbitsa() * fdeltax_div_64K;
		*colorptr = 0;
	}
	if (abs(temp.y) > 20)
	{
		temp.y = fcentery + (double) sixteenbitsa() * fdeltay_div_64K;
		*colorptr = 0;
	}
	return temp;
}


void henon_step(void)
{
	static int color;
	int i, j, k;
	fpair s, t;
	pair p, q, u, v;

	for (i = 0; i < fflock2ptr->n; i++)
	{
		color = (int) (fflock2ptr->color[i]);
		s = fflock2ptr->atom[i];

		/*
		 * Half of the time is spent in one of the following two
		 * routines:
		 */

		if (fancyflag)
			t = genhenon(&s, &color);
		else
			t = henon(&s, &color);


		if (soundflag && i == fflock2ptr->n - 1)
			sound(100 + (unsigned)
			      (abs(1000 * (t.y - floy) - 800)));
		fflock2ptr->atom[i] = t;

		if (1 /* iteration > 100 */ )
		{
			q = convertpair(&t);
			Pix(q.x, q.y, color);
			if (tracetype)
			{	/* ribbon case */
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
	}
	if (tracetype /* && iteration > 100 */ )
	{
		ribbonindex = k;/* inc index after done whole flock */
		if (!k)
			ribbonfull = 1;	/* ribbon full after first wrap */
	}
}

