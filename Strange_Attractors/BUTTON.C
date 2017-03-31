#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <dos.h>
#include "attract.h"

void strip_blanks(char *tbuf)
{
	int i;

	for (i = strlen(tbuf) - 1; i >= 0; i--)
	{
		if (tbuf[i] != ' ')
		{
			tbuf[i + 1] = 0;
			break;
		}
	}
}

void PaintTextBoxEntry(text_button * tB, int c3, int c4, char *text)
{
	HideCursor();
	PenColor(c3);
	PaintRect(&tB->nR);
	PushButton(&tB->nR, true);
	TextAlign(alignLeft, alignTop);
	MoveTo(tB->nR.Xmin + 2, tB->nR.Ymin + 2);
	BackColor(c3);
	PenColor(c4);
	DrawString(text);
	strcpy(tB->text, text);
	ShowCursor();
}


void PaintTextBoxBase(rect * tR, text_button * tbR, char *prompt, char *text,
		 int labelcolor, int labelback, int textcolor, int textback)
{
	rect nR;

	HideCursor();


	nR = *tR;

	nR.Xmin += StringWidth(prompt) + 4;
	tbR->nR = nR;

	PenColor(labelcolor);
	TextAlign(alignRight, alignTop);
	MoveTo(nR.Xmin - 2, nR.Ymin + 2);
	BackColor(labelback);
	DrawString(prompt);

	PaintTextBoxEntry(tbR, textcolor, textback, text);

	ShowCursor();
}



void PaintTextBox(rect * tR, text_button * nR, char *prompt, char *text)
{
	PaintTextBoxBase(tR, nR, prompt, text, WHITE, LIGHTGRAY, DARKGRAY, WHITE);
}

void PaintNumberBoxEntryPrecision(text_button * nR, double value, int type, int width, int precision)
{
	char tbuf[128];

	switch (type)
	{
	case GS_UNSIGNED:
		sprintf(tbuf, "% -u", (int) value);
		break;
	case GS_INTEGER:
		sprintf(tbuf, "% -d", (int) value);
		break;
	default:
		sprintf(tbuf, "% -*.*f", width, precision, value);
		strip_blanks(tbuf);
		break;
	}
	PaintTextBoxEntry(nR, DARKGRAY, WHITE, tbuf);
}

void PaintNumberBoxEntry(text_button * nR, double value, int type)
{
	PaintNumberBoxEntryPrecision(nR, value, type, 4, 3);
}

void PaintNumberBoxBasePrecision(rect * tR, text_button * nR, double value, char *prompt,
	 int type, int c1, int c2, int c3, int c4, int width, int precision)
{


	char tbuf[20];

	switch (type)
	{
	case GS_UNSIGNED:
		sprintf(tbuf, "% -u", (int) value);
		break;
	case GS_INTEGER:
		sprintf(tbuf, "% -d", (int) value);
		break;
	default:
		sprintf(tbuf, "% -*.*f", width, precision, value);
		strip_blanks(tbuf);
		break;
	}
	PaintTextBoxBase(tR, nR, prompt, tbuf, c1, c2, c3, c4);
}

void PaintNumberBoxBase(rect * tR, text_button * nR, double value, char *prompt,
			 int type, int c1, int c2, int c3, int c4)
{
	PaintNumberBoxBasePrecision(tR, nR, value, prompt, type, c1, c2, c3, c4, 4, 3);
}


void PaintNumberBox(rect * tR, text_button * nR, double value, char *prompt, int type)
{

	PaintNumberBoxBase(tR, nR, value, prompt, type,
			   WHITE, LIGHTGRAY, DARKGRAY, WHITE);

}

int GetText(text_button * tbR, char *result, char *start, int backcolor, int textcolor)
{

	char tbuf[128];
	char sbuf[128];

	rect R = tbR->nR;
	int retval;
	int width;
	char *p;

	HideCursor();
	InsetRect(&R, 1, 1);
	PenColor(~backcolor);
	PaintRect(&R);
	TextAlign(alignLeft, alignTop);
	MoveTo(R.Xmin + 2, R.Ymin + 2);
	BackColor(~backcolor);
	PenColor(~textcolor);
	strcpy(tbuf, start);
	MoveTo(R.Xmin + 2, R.Ymin + FontHeight + 1);
	width = (R.Xmax - R.Xmin + StringWidthX - 1) / StringWidthX - 1;

	if (_jGetString(sbuf, tbuf, width, GS_ANYTHING))
	{
		p = strpbrk(sbuf, " ");
		if (p)
			*p = 0;
		strcpy(result, sbuf);
		retval = 1;
	}
	else
		retval = 0;

	/* Strip the fucking trailing spaces */


	PenColor(backcolor);
	PaintRect(&R);
	TextAlign(alignLeft, alignTop);
	MoveTo(R.Xmin + 2, R.Ymin + 2);
	BackColor(backcolor);
	PenColor(textcolor);
	if (retval)
	{
		DrawString(result);
		strcpy(tbR->text, result);
	}
	else
	{
		DrawString(start);
		strcpy(tbR->text, start);
	}

	ShowCursor();
	return retval;
}

int GetNumberBase(text_button * tbR, double *result, int type, double lo,
		   double hi, int width, int precision)
{

	char tbuf[128];
	char sbuf[128];

	rect R = tbR->nR;
	int retval;
	int wwidth;


	HideCursor();
	InsetRect(&R, 1, 1);
	PenColor(LIGHTGRAY);
	PaintRect(&R);
	TextAlign(alignLeft, alignTop);
	MoveTo(R.Xmin + 2, R.Ymin + 2);
	BackColor(LIGHTGRAY);
	PenColor(BLACK);


	switch (type)
	{
	case GS_UNSIGNED:
		sprintf(tbuf, "% -u", (int) *result);
		break;
	case GS_INTEGER:
		sprintf(tbuf, "% -d", (int) *result);
		break;
	default:
		sprintf(tbuf, "% -*.*f", width, precision, *result);
		strip_blanks(tbuf);
		break;
	}

	MoveTo(R.Xmin + 2, R.Ymin + FontHeight + 1);

	wwidth = (R.Xmax - R.Xmin + StringWidthX - 1) / StringWidthX - 2;

	if (_jGetString(sbuf, tbuf, wwidth, type))
	{
		double d = atof(sbuf);

		if (d < lo || d > hi)
		{
			sprintf(sbuf, "%g to %g", lo, hi);
			RangeError(sbuf);
			retval = 0;
		}
		else
		{
			*result = d;
			retval = 1;
		}
	}
	else
		retval = 0;

	PenColor(BUTTONBACK);
	PaintRect(&R);
	//PushButton(&R, true);
	TextAlign(alignLeft, alignTop);
	MoveTo(R.Xmin + 2, R.Ymin + 2);
	BackColor(BUTTONBACK);
	PenColor(MENUTEXT);
	switch (type)
	{
	case GS_UNSIGNED:
		sprintf(tbuf, "% -u", (int) *result);
		break;
	case GS_INTEGER:
		sprintf(tbuf, "% -d", (int) *result);
		break;
	default:
		sprintf(tbuf, "% -*.*f", width, precision, *result);
		strip_blanks(tbuf);
		break;
	}
	DrawString(tbuf);
	strcpy(tbR->text, tbuf);

	ShowCursor();
	return retval;
}

int GetNumber(text_button * tbR, double *result, int type, double lo, double hi)
{
	/* First repaint the whole box. Don't repaint the guts. */

	return GetNumberBase(tbR, result, type, lo, hi, 4, 3);
}

void PaintRadioButtonBase(rect * bR, int inout, int hilite, char *txt, int c1, int c2, int c3)
{
	int cx, cy;

	HideCursor();

	PenColor(hilite ? c2 : c1);
	PaintRect(bR);
	PushButton(bR, inout);
	PenColor(c3);
	BackColor(hilite ? c2 : c1);
	Centers(bR, &cx, &cy);
	MoveTo(cx, cy);
	TextAlign(alignCenter, alignMiddle);
	DrawString(txt);
	ShowCursor();
}


void PaintRadioButton(rect * bR, int inout, int hilite, char *txt)
{
	PaintRadioButtonBase(bR, inout, hilite, txt, DARKGRAY, RED, WHITE);
}

void PushButton(rect * R, int inout)
{
	HideCursor();
	PenColor(inout ? 0 : WHITE);
	MoveTo(R->Xmax, R->Ymin);
	LineTo(R->Xmin, R->Ymin);
	LineTo(R->Xmin, R->Ymax);
	PenColor(inout ? WHITE : 0);
	LineTo(R->Xmax, R->Ymax);
	LineTo(R->Xmax, R->Ymin);
	ShowCursor();
}

void CreateButtonPanel(rect * tR, char *msgs[], rect bR[], int n, int *vals[])
{
	rect R;
	int i;
	int t1, t2;

	t1 = (tR->Xmax - tR->Xmin + n - 1) / n;
	t2 = t1 - 8;

	for (i = 0; i < n; i++)
	{
		R.Xmin = tR->Xmin + 4 + t1 * i;
		R.Xmax = R.Xmin + t2;
		R.Ymax = tR->Ymax - 2;
		R.Ymin = R.Ymax - FontHeight - 4;
		bR[i] = R;
		PaintRadioButton(&R, *vals[i], *vals[i], msgs[i]);
	}
}



void CreateRadioPanel(rect * tR, char *msgs[], rect bR[], int n, int current)
{
	rect R;
	int i;
	int t1, t2;

	t1 = (tR->Xmax - tR->Xmin + n - 1) / n;
	t2 = t1 - 8;

	for (i = 0; i < n; i++)
	{
		R.Xmin = tR->Xmin + 4 + t1 * i;
		R.Xmax = R.Xmin + t2;
		R.Ymax = tR->Ymax - 2;
		R.Ymin = R.Ymax - FontHeight - 4;
		bR[i] = R;
		PaintRadioButton(&R, current == i, current == i, msgs[i]);
	}
}




int CheckSingleButton(int X, int Y, rect * bR, char *msg, int *n)
{
	if (XYInRect(X, Y, bR))
	{
		*n ^= 1;
		PaintRadioButton(bR, *n, *n, msg);
		return 1;
	}
	else
		return 0;
}

int CheckNormalButtons(int X, int Y, rect * bR, int n, int *vals[], char *msgs[])
{
	int i;

	for (i = 0; i < n; i++)
		if (CheckSingleButton(X, Y, &bR[i], msgs[i], vals[i]))
			return 1;

	return 0;
}





int CheckRadioButtons(int X, int Y, rect * bR, int n, int *w, char *msgs[])
{
	int i;

	for (i = 0; i < n; i++)
	{
		if (XYInRect(X, Y, &bR[i]))
		{
			if (i != *w)
			{
				if (*w != -1)
					PaintRadioButton(&bR[*w], false, false, msgs[*w]);
				PaintRadioButton(&bR[*w], true, true, msgs[*w = i]);
				DoublePress(&bR[*w], true, RED);
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

void ExtraHilite(rect * R, int inout)
{

	HideCursor();
	PenColor(inout ? 0 : WHITE);
	MoveTo(R->Xmax + 1, R->Ymin - 1);
	LineTo(R->Xmin - 1, R->Ymin - 1);
	LineTo(R->Xmin - 1, R->Ymax + 1);
	PenColor(inout ? WHITE : 0);
	LineTo(R->Xmax + 1, R->Ymax + 1);
	LineTo(R->Xmax + 1, R->Ymin - 1);
	ShowCursor();
}

void InvertInsides(text_button * R)
{
	rect R1 = R->nR;

	HideCursor();
	InsetRect(&R1, 1, 1);
	R1.Xmax = R1.Xmin + StringWidth(R->text);
	InvertRect(&R1);
	ShowCursor();
}

void DoublePress(rect * dR, int inout, int color)
{
	rect R = *dR;

	InsetRect(&R, 1, 1);

	HideCursor();
	if (!inout)
	{
		PenColor(color);
		FrameRect(&R);
	}
	else
		PushButton(&R, true);
	ShowCursor();
}



void navigate(int key, int *l, int *r, int *u, int *d, int items, rect * bR[], int *current_item)
{
	int ci = *current_item;

	if (key != XLARROW && key != XRARROW && key != XUARROW
	    && key != XDARROW && key != XPGUP && key != XPGDN &&
	    key != XEND && key != XHOME)
		return;

	if (*current_item == -1)
		return;


	switch (key)
	{
	case XLARROW:
		if (l == (int *) -1)
		{
			ci--;
			if (ci < 0)
				ci = items - 1;
		}
		else if (l != NULL)
			ci = l[ci];
		break;
	case XRARROW:
		if (r == (int *) -1)
		{
			ci++;
			if (ci >= items)
				ci = 0;
		}
		else if (r != NULL)
			ci = r[ci];
		break;
	case XUARROW:
		if (u == (int *) -1)
		{
			ci--;
			if (ci < 0)
				ci = items - 1;
		}
		else if (u != NULL)
			ci = u[ci];
		break;
	case XDARROW:
		if (d == (int *) -1)
		{
			ci++;
			if (ci >= items)
				ci = 0;
		}
		else if (d != NULL)
			ci = d[ci];
		break;
	case XEND:
		ci = items - 1;
		break;
	case XHOME:
		ci = 0;
		break;
	default:
		return;
	}
	if (bR)
		move_to_corner(bR[ci]);
	*current_item = ci;
}

void PlusMinusButtons(rect * R, rect * dR)
{
	int cx, cy;

	HideCursor();
	PenColor(DARKGRAY);
	PaintRect(R);
	Centers(R, &cx, &cy);
	PenColor(BLACK);
	MoveTo(cx, R->Ymin + 1);
	LineTo(cx, R->Ymax - 1);
	dR[0] = *R;
	dR[1] = *R;
	dR[0].Xmax = cx - 1;
	dR[1].Xmin = cx;
	TextAlign(alignCenter, alignMiddle);
	Centers(&dR[0], &cx, &cy);
	MoveTo(cx, cy);
	PenColor(WHITE);
	BackColor(DARKGRAY);
	DrawChar('-');
	Centers(&dR[1], &cx, &cy);
	MoveTo(cx, cy);
	DrawChar('+');
	PushButton(&dR[0], false);
	PushButton(&dR[1], false);
	ShowCursor();
}


void BasicCenteredBox(rect * dR, int width, int height, int bcolor, char *title, int titlecolor)
{
	int cx, cy;
	rect R;

	HideCursor();
	Centers(&sR, &cx, &cy);

	R.Xmin = cx - width / 2;
	R.Xmax = R.Xmin + width - 1;
	R.Ymin = cy - height / 2;
	R.Ymax = R.Ymin + height - 1;

	ShadowAndSave(&R);
	*dR = R;
	PenColor(bcolor);
	PaintRect(&R);
	PenColor(BLACK);
	FrameRect(&R);
	if (title)
	{

		TextAlign(alignCenter, alignTop);
		MoveTo(cx, R.Ymin + 2);
		PenColor(titlecolor);
		BackColor(bcolor);
		DrawString(title);
	}
	ShowCursor();
}

void PushOrDoublePress(rect * R, int inout, int selected)
{
	if (selected)
		DoublePress(R, inout, RED);
	else
		PushButton(R, inout);
}

/* Another nice button thing. This consists a prompt, a text box, and
a plus-minus box. */

void CreateNumberTool(numbertool * t)
{
	/* You have to have set tR here, kids. */

	rect R;
	int cx, cy;
	int width;

	R.Xmax = t->tR.Xmax;
	R.Xmin = R.Xmax - (StringWidthX + 5);
	R.Ymin = t->tR.Ymin;
	R.Ymax = t->tR.Ymax;


	t->pR = R;
	PaintRadioButton(&R, false, false, "");
	width = R.Xmax - R.Xmin - 7;
	Centers(&R, &cx, &cy);

	PenColor(WHITE);
	MoveTo(R.Xmin + 4, cy);
	LineTo(R.Xmax - 3, cy);
	MoveTo(cx + 1, cy - width / 2);
	LineRel(0, width);

	OffsetRect(&R, -(StringWidthX + 5 + 2), 0);
	t->mR = R;
	PaintRadioButton(&R, false, false, "");
	Centers(&R, &cx, &cy);
	PenColor(WHITE);
	MoveTo(R.Xmin + 4, cy);
	LineTo(R.Xmax - 3, cy);


	R.Xmax = R.Xmin - 4;
	R.Xmin = t->tR.Xmin;
	t->TB.nR = R;

	PaintNumberBoxEntryPrecision(&t->TB, t->value, t->type, t->p1, t->p2);

}

void FileError(char *t,FILE *fd)
{
	char tbuf[128];

   /* It might really be a disk full. How can we find out? */

   if (fd)
   {
      /* OK, we've got an open file we can't write to. Probably
         disk full. Fuck, CERTAINLY disk full. */
      DiskFullError(t);
      return;
   }

  	sprintf(tbuf, "%s: %s", t, strerror(errno));
  	tbuf[strlen(tbuf) - 1] = 0;
	ErrorBox(tbuf);
}

void DiskFullError(char *t)
{
	char tbuf[128];

	sprintf(tbuf, "%s: Disk full", t);
	ErrorBox(tbuf);
}
