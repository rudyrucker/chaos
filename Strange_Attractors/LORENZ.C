#include "attract.h"
#include <math.h>

ftriple lorenzderivs(ftriple * w)
{

	ftriple temp;

	temp.x = ls * (w->y - w->x);
	temp.y = (lr - w->z) * w->x - w->y;
	temp.z = lb * w->z + w->x * w->y;
	return temp;
}


ftriple rk4lorenzimage(ftriple * w, int *colorptr)
{
/* see Numerical Recipes in C p. 572 */

	ftriple temp, temp1, temp2, temp3, derivs1, derivs2, derivs3, derivs4;

	derivs1 = lorenzderivs(w);
	temp1.x = w->x + derivs1.x * dt2;
	temp1.y = w->y + derivs1.y * dt2;
	temp1.z = w->z + derivs1.z * dt2;
	derivs2 = lorenzderivs(&temp1);
	temp2.x = w->x + derivs2.x * dt2;
	temp2.y = w->y + derivs2.y * dt2;
	temp2.z = w->z + derivs2.z * dt2;
	derivs3 = lorenzderivs(&temp2);
	temp3.x = w->x + derivs3.x * dt;
	temp3.y = w->y + derivs3.y * dt;
	temp3.z = w->z + derivs3.z * dt;
	derivs4 = lorenzderivs(&temp3);
	newdr.x = dt6 *
		(derivs1.x + 2.0 * (derivs2.x + derivs3.x) + derivs4.x);
	newdr.y = dt6 *
		(derivs1.y + 2.0 * (derivs2.y + derivs3.y) + derivs4.y);
	newdr.z = dt6 *
		(derivs1.z + 2.0 * (derivs2.z + derivs3.z) + derivs4.z);
	temp.x = w->x + newdr.x;
	temp.y = w->y + newdr.y;
	temp.z = w->z + newdr.z;
	if (fabs(temp.x) > 1000)
	{
		temp.x = fcenterx + fdeltax * (double) sixteenbitsa() / 0x10000L;
		*colorptr = 0;
	}
	if (fabs(temp.y) > 1000)
	{
		temp.y = fcentery + fdeltay * (double) sixteenbitsa() / 0x10000L;
		*colorptr = 0;
	}
	if (fabs(temp.z) > 1000)
	{
		temp.z = fcentery + fdeltay * (double) sixteenbitsa() / 0x10000L;
		*colorptr = 0;
	}

	return temp;
}

pair projectpixel(ftriple * w, char axis)
{
	ftriple local, templocal;
	pair temp;
	double pfact;

	if (!lorenzflyflag)
   {
		switch (axis)
		{
		case 'x':
			pfact = pcentx / (pcentx - w->x);
			temp.x = xyscale * (pfact * w->y - floy) + minx;
			temp.y = zscale * (fhiz - pfact * w->z);
			break;
		case 'y':
			pfact = pcenty / (pcenty - w->y);
			temp.x = xscale * (pfact * w->x - flox) + minx;
			temp.y = zscale * (fhiz - pfact * w->z);
			break;
		case 'z':
			pfact = (pcentz - pcanvasz) / (pcentz - w->z);
			temp.x = xscale * (pfact * w->x - flox) + minx;
			temp.y = yscale * (fhiy - pfact * w->y);
			break;
		}
   }
	else
	{
		templocal.x = w->x - viewspot.x;
		templocal.y = w->y - viewspot.y;
		templocal.z = w->z - viewspot.z;
		local.x = templocal.x * viewnormal.x +
			templocal.y * viewnormal.y + templocal.z * viewnormal.z;
		local.y = templocal.x * viewbinormal.x + templocal.y *
			viewbinormal.y + templocal.z * viewbinormal.z;
		local.z = templocal.x * viewtangent.x + templocal.y *
			viewtangent.y + templocal.z * viewtangent.z;
		pfact = pcentfly / (pcentfly - local.z);
		temp.x = xscale * (pfact * local.x - flox) + minx;
		temp.y = yscale * (fhiy - pfact * local.y);
	}
	return temp;
}

void lorenz_step(void)
{
	static int color;
	int i, j, k, l, m;
	double norm;
	pair p, q, u, v;
	ftriple fw, fz;
	int skipflag;
	ftriple tangent, normal, binormal;

	for (i = 0; i < fflock3ptr->n; i++)
	{
		fw = fflock3ptr->atom[i];
		color = (int) (fflock3ptr->color[i]);
		fz = rk4lorenzimage(&fw, &color);
		fflock3ptr->atom[i] = fz;
		p = projectpixel(&fw, axis);
		q = projectpixel(&fz, axis);
		if (i < fflock3ptr->n - trihedroncount || !trihedrons_exist)
		{
			Bres(p.x, p.y, q.x, q.y, (int) color);
			if (tracetype)
			{	/* manipulate ribbon */
				if (!ribbonfull && !ribbonindex)
				{
               /* 1st tme */
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
			}	/* end of ribbon manipulation */
			if (soundflag && i == fflock3ptr->n - 1)
			{
				norm = sqrt(newdr.x * newdr.x +
				     newdr.y * newdr.y + newdr.z * newdr.z);
				sound((unsigned)
				      (norm * 3000 / (1 + fabs(fz.z))));
			}
		}		/* end of if not trihedron */
		else
		{		/* trihedron case */
			skipflag = 0;
			norm = sqrt(newdr.x * newdr.x +
				    newdr.y * newdr.y + newdr.z * newdr.z);
			if (soundflag && i == fflock3ptr->n - 1)
				sound((unsigned)
				      (norm * 3000 / (1 + fabs(fz.z))));
			tangent.x = newdr.x / norm;
			tangent.y = newdr.y / norm;
			tangent.z = newdr.z / norm;
			normal.x = tangent.x - oldtangent[i].x;
			normal.y = tangent.y - oldtangent[i].y;
			normal.z = tangent.z - oldtangent[i].z;
			oldtangent[i].x = tangent.x;
			oldtangent[i].y = tangent.y;
			oldtangent[i].z = tangent.z;
			norm = sqrt(normal.x * normal.x +
				 normal.y * normal.y + normal.z * normal.z);
			if (!norm)
				skipflag = 1;
			else
			{
				normal.x = normal.x / norm;
				normal.y = normal.y / norm;
				normal.z = normal.z / norm;
			}
			binormal.x = tangent.y * normal.z -
				tangent.z * normal.y;
			binormal.y = tangent.z * normal.x -
				tangent.x * normal.z;
			binormal.z = tangent.x * normal.y -
				tangent.y * normal.x;
			norm = sqrt(binormal.x * binormal.x + binormal.y *
				    binormal.y + binormal.z * binormal.z);
			if (!norm)
				skipflag = 1;
			else
			{
				binormal.x = binormal.x / norm;
				binormal.y = binormal.y / norm;
				binormal.z = binormal.z / norm;
			}
			if (!skipflag)
			{
				if (trihedronon)
				{
					wBres(antintip[i].x, antintip[i].y,
					    ntip[i].x, ntip[i].y, 0, WIDTH);
					wBres(antibntip[i].x, antibntip[i].y,
					  bntip[i].x, bntip[i].y, 0, WIDTH);
					wBres(antittip[i].x, antittip[i].y,
					    ttip[i].x, ttip[i].y, 0, WIDTH);
				}
				if (i == fflock3ptr->n - 1)
				{
					flyspot = fz;
					flytangent = tangent;
					flynormal = normal;
					flybinormal = binormal;
					trihedronon = 1;
				}
				tangent.x = STAKE * tangent.x + fz.x;
				tangent.y = STAKE * tangent.y + fz.y;
				tangent.z = STAKE * tangent.z + fz.z;
				ttip[i] = projectpixel(&tangent, axis);
				antittip[i].x = q.x + q.x - ttip[i].x;
				antittip[i].y = q.y + q.y - ttip[i].y;
				normal.x = STAKE * normal.x + fz.x;
				normal.y = STAKE * normal.y + fz.y;
				normal.z = STAKE * normal.z + fz.z;
				ntip[i] = projectpixel(&normal, axis);
				antintip[i].x = q.x + q.x - ntip[i].x;
				antintip[i].y = q.y + q.y - ntip[i].y;
				binormal.x = STAKE * binormal.x + fz.x;
				binormal.y = STAKE * binormal.y + fz.y;
				binormal.z = STAKE * binormal.z + fz.z;
				bntip[i] = projectpixel(&binormal, axis);
				antibntip[i].x = q.x + q.x - bntip[i].x;
				antibntip[i].y = q.y + q.y - bntip[i].y;
				color = fflock3ptr->color[i];
				wBres(antittip[i].x, antittip[i].y,
				      ttip[i].x, ttip[i].y,
				      color, WIDTH);
				wBres(antintip[i].x, antintip[i].y,
				      ntip[i].x, ntip[i].y,
				      colorize(color + 5), WIDTH);
				wBres(antibntip[i].x, antibntip[i].y,
				      bntip[i].x, bntip[i].y,
				      colorize(color + 10), WIDTH);
			}	/* end of don't skip */
			if (tritracetype == 1)
			{	/* manipulate ribbon */
				if (!ribbonfull & !ribbonindex)
				{	/* 1st tme */
					ribbon[0]->atom[i] = p;
					ribbon[1]->atom[i] = q;
					k = 2;
				}
				else
				{
					k = j = ribbonindex;
					k++;
					if (k == ribbonlength)
					{
						k = 0;
						if (!ribbonfull)
							for (l = 0;
							     l < ribbonlength - 2; l++)
							{
								u = ribbon[l]->atom[i];
								v = ribbon[l + 1]->atom[i];
								Bres(u.x, u.y, v.x, v.y, color);
							}
					}
					if (ribbonfull)
					{
						u = ribbon[j]->atom[i];
						v = ribbon[k]->atom[i];
						Bres(u.x, u.y, v.x, v.y, 0);
						l = (j + ribbonlength - 8) %
							ribbonlength;
						m = (l + 1) % ribbonlength;
						u = ribbon[l]->atom[i];
						v = ribbon[m]->atom[i];
						Bres(u.x, u.y, v.x, v.y, color);
					}
					ribbon[j]->atom[i] = q;
				}
			}	/* end of ribbon manipulation */
		}
	}
   if (tracetype)
   {
   	ribbonindex = k;	/* inc index after whole flock is done */
   	if (!k)
	   	ribbonfull = 1;	/* ribbon is full after first wrap */
   }
}
