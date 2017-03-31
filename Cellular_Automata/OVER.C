/* Stuff from various modules that really should be in an overlay. */

#include "toy.h"
#include <sys\stat.h>
#include <dir.h>
#include <stdlib.h>
extern int init_mem_err;

extern rect doubledragrect,coarsedragrect,coarsecorners,doublecorners;
static int hodge_lefters[] = {
   8,6,7,
   11,9,10
   };
static int hodge_righters[] = {
   7,8,6,
   10,11,9,
   };
static int hodge_uppers[] = {
   5,5,5,
   6,7,8,
   9,10,11
   };
static int hodge_downers[] = {
   7,
   9,10,11,
   12,13,14,
   };

static int tube_lefters[] = {
   8,6,7,
   11,9,10,
   14,12,13,
   16,15,
   };
static int tube_righters[] = {
   7,8,6,
   10,11,9,
   13,14,12,
   16,15,
   };
static int tube_uppers[] = {
   5,5,5,
   6,7,8,
   9,10,11,
   13,14,
   15,15,16
   };
static int tube_downers[] = {
   7,
   9,10,11,
   12,13,14,
   15,15,16,
   17,19
   };

static int eat_lefters[] = {
   7,6,
   10,8,9
   };
static int eat_righters[] = {
   7,6,
   9,10,8
   };
static int eat_uppers[] = {
   5,5,
   6,6,7
   };
static int eat_downers[] = {
   6,
   8,10,
   11,12,13
   };
/* we do nluky by formula because its so regular */


extern char *RSS[];
extern rect SSR;

unsigned char *eatmods = NULL;



int set_boxes(void)
{
	rect R;
   int n;


	/* Only bother with this if not hires and not coarse */

	if (display_mode == HI || display_mode == COARSE)
		return false;


	R = (display_mode == DOUBLE) ? coarsedragrect : doubledragrect;
	n = DragRect(&R, R.Xmin, R.Ymin);
   if (n<=0)
      return n;

	/*
	 * Hokay. Now, translate the new coordinates into the proper space.
	 * All we really need to calculate is Xmin and Ymin proportional to
	 * the screen size.
	 */

	if (display_mode == DOUBLE)
	{
		/* calculate a new coarsecorners */
		coarsecorners.Xmin = doublecorners.Xmin +
			(R.Xmin - 160) / 4;
		coarsecorners.Ymin = doublecorners.Ymin + R.Ymin / 4;
		coarsecorners.Xmax = coarsecorners.Xmin + LOXCOUNT - 1;
		coarsecorners.Ymax = coarsecorners.Ymin + LOYCOUNT - 1;

		/* Make sure we haven't gone too far. */
		while (coarsecorners.Xmax > sR.Xmax)
			OffsetRect(&coarsecorners, -1, 0);
		while (coarsecorners.Ymax > sR.Ymax)
			OffsetRect(&coarsecorners, 0, -1);
		/* and fix the other way */
		while (coarsecorners.Xmin < 1)
			OffsetRect(&coarsecorners, 1, 0);
		while (coarsecorners.Ymin < 1)
			OffsetRect(&coarsecorners, 0, 1);
		coarsedragrect = R;

	}
	else
	{
		doublecorners.Xmin = (R.Xmin - 160) / 2 + 1;
		doublecorners.Ymin = R.Ymin / 2 + 1;
		doublecorners.Xmax = doublecorners.Xmin + MIDDLEXCOUNT - 1;
		doublecorners.Ymax = doublecorners.Ymin + MIDDLEYCOUNT - 1;

		/* Make sure we haven't gone too far. */
		while (doublecorners.Xmax > sR.Xmax)
			OffsetRect(&doublecorners, -1, 0);
		while (doublecorners.Ymax > sR.Ymax)
			OffsetRect(&doublecorners, 0, -1);
		/* and fix the other way */
		while (doublecorners.Xmin < 1)
			OffsetRect(&doublecorners, 1, 0);
		while (doublecorners.Ymin < 1)
			OffsetRect(&doublecorners, 0, 1);

		doubledragrect = R;
	}
   return 1;
}


void jloadlookuptable(unsigned char *, int, int, int);


void loadlookuptable(int increment, int maxstate)
{
	int i;
   extern void load_nluky(void);

	if (caotype == CA_NLUKY)
		load_nluky();
/*   else if (caotype == CA_TUBE)
      load_tube(); */
	else if (caotype == CA_EAT)	/* eater */
	{
		/* This is straightforward modding */
		if (!eatmods)
			eatmods = farcalloc(256L, 1L);
		for (i = 0; i < 256; i++)
			eatmods[i] = (unsigned char) (i % maxeatstate);
	}

	else
		jloadlookuptable(lookuptable, increment, maxstate, caotype);
}

void allocatebuffers(void)
{
	int i;
	unsigned char *p;

	void build8sumtable(void);

	build8sumtable();	/* this is for cao1 */

	/* For each of the buffers, allocate enough space for this shit. */
	if (!memok(0x10000L + 64)) {
	    init_mem_err = 1;
	    return;
	}

	fakelookuptable = farcalloc(0x10000L + 64, 1L);

	/*
	 * Now kludge the real lookuptable address so it lies on an even page
	 * boundry
	 */
	/*
	 * We can just zero out the bottom part and bump the top by one; that
	 * will suffice.
	 */

	lookuptable = MK_FP(FP_SEG(fakelookuptable) + 1, 0);


	/*
	 * This is all fucking ridiculous. No reason at all for lots of
	 * buffers. We allocate ONE humongous one, a HUGE one I insist! and
	 * step through.
	 */
	if (memok(((long) HIYCOUNT + 2) * ((long) HIXCOUNT + 2))) {
	    egabuf = farcalloc((long) HIYCOUNT + 2, (long) HIXCOUNT + 2);
	    oldbuf = egabuf;
	}
	else
	{

		if (!memok(((long) MEDYCOUNT + 2) * ((long) MEDXCOUNT + 2))) {
			    init_mem_err = 1;
			    return;
		}

		oldbuf = farcalloc((long) MEDYCOUNT + 2, (long) MEDXCOUNT + 2);
		allocatefailflag = 1;
	}

	if (!allocatefailflag)
	{
                /* Anyway, let's load egarowptrs... */
		for (p = (unsigned char *) egabuf, i = 0; i < HIYCOUNT + 2; i++)
		{
			egarowptrs[i] = p = normalize(p, HIXCOUNT * 3, HIXCOUNT * 3);
			p += HIXCOUNT + 2;
		}
	}
	for (p = (unsigned char *) oldbuf, i = 0; i < MEDYCOUNT + 2; i++)
	{
		oldbufrowptrs[i] = p = normalize(p, MEDXCOUNT * 3, MEDXCOUNT * 3);
		p += MEDXCOUNT + 2;
	}


	if (allocatefailflag) {
		if (!memok(((long) MEDXCOUNT + 2) * ((long) MEDYCOUNT + 2))) {
			init_mem_err = 1;
			allocatefailflag = 0;
			return;
		}
		newbuf = farcalloc((long) MEDXCOUNT + 2, (long) MEDYCOUNT + 2);
	}
	else
#pragma warn -sig
		newbuf = egarowptrs[HIYCOUNT + 1] + HIYCOUNT + 2 - (MEDXCOUNT + 2L) * (MEDYCOUNT + 2L);
#pragma warn .sig
	for (p = (unsigned char *) newbuf, i = 0; i < MEDYCOUNT + 2; i++)
	{
		newbufrowptrs[i] = p = normalize(p, MEDXCOUNT * 3, MEDXCOUNT * 3);
		p += MEDXCOUNT + 2;
	}

	/* Check to see if there is enough memory to later go from
	    med to high resolution. */

	if (!allocatefailflag)
	    if (!memok((MEDXCOUNT+2L)*(MEDYCOUNT+2L)))
		 allocatefailflag = 1;
}


extern rect typenameR;
extern rect dummyR[10];
extern rect runstopstepR[3];

#define _setup(name,rank,x) x=name##_##rank##ers; x##n = sizeof name##_##rank##ers
#define setup(name) _setup(name,left,l);_setup(name,right,r);_setup(name,upp,u);_setup(name,down,d);
void set_downers(void)
{
   int *l,*r,*u,*d;
   int ln,rn,un,dn;
   int i;
   switch(caotype)
   {
   case CA_HODGE:
      setup(hodge);
      break;
   case CA_EAT:
      setup(eat);
      break;
   case CA_TUBE:
      setup(tube);
      break;
   }

   if(caotype != CA_NLUKY)
   {
      memcpy(main_lefters+6,l,ln);
      memcpy(main_righters+6,r,rn);
      memcpy(main_uppers+6,u,un);
      memcpy(main_downers+5,d,dn);
   }
   else
   {
      main_downers[5] = 7;
      for(i=0;i<6;i++)
      {
	 int j = i*3;
	 int k;
	 main_lefters[6+j] = main_righters[7+j] = j + 2 + 6;
	 main_lefters[7+j] = main_righters[8+j] = j + 6;
	 main_lefters[8+j] = main_righters[6+j] = j + 1 + 6;

	 for(k=0;k<3;k++)
	 {
	    if (i == 0)
	       main_uppers[6+j+k] = 5;
	    else
	       main_uppers[6+j+k] = j + k - 3 + 6;
	    main_downers[6+j+k] = j + k + 3 + 6;
	 }

      }
   }
   /* now all have some things in common */
   main_uppers[0] = main_items - 2;
   for(i=0;i<3;i++)
      main_downers[main_items - (i+1)] = 0;

   main_lefters[main_items - 1] = main_righters[main_items - 3] = main_items - 2;
   main_lefters[main_items - 2] = main_righters[main_items - 1] = main_items - 3;
   main_lefters[main_items - 3] = main_righters[main_items - 2] = main_items - 1;
}




void initialize_numbers(void)
{
   rect R;
   int i;
   extern void setTypename(void);

   RasterOp(zREPz);

   PenColor(BLUE);
   PaintRect(&typenameR);
   for(i=0;i<5;i++)
   {
      R = dummyR[i];
      R.Xmin = 1;
      PaintRect(&R);
   }

   main_items = 6;

   switch (caotype)
	{
	case 0:
		setNumberOfStates(0);
		setIncrementDecrement(0);
		break;
	case 1:
		setNLUKY_N(0);
		setNLUKY_L(0);
		setNLUKY_U(0);
		setNLUKY_K(0);
		setNLUKY_Y(0);
		break;
	case 2:
		setEatD(0);
		setEATN(0);
		break;
	case 3:
		setTubeHiding(0);
		setTubeEating(0);
		setTubeFreak(0);
		setTubeFuzz(0);
		break;
	}
   current_main_item = -1;


   for(i=0;i<3;i++)
      mainR[main_items++] = &runstopstepR[i];

   setTypename();

   /* set up the left buttons */
   set_downers();

}




void initialize_buttons(void)
{
   rect R;
   int i;

   RasterOp(zREPz);
   main_items = 0;
   R.Xmin =0;
   R.Xmax = 158;
   R.Ymin = 0;
   R.Ymax = HIYCOUNT-1;
   PenColor(BLUE);
   PaintRect(&R);
   PenColor(WHITE);
   FrameRect(&R);


   R.Xmin = 8;
   R.Xmax = 160 - 8;
   R.Ymin = 8;
   R.Ymax = R.Ymin + FontHeight + 4;

   for(i=0;maintexts[i];i++)
   {
      mainbuttonR[i] = R;
      PaintRadioButton(&R,false,false,maintexts[i]);
      OffsetRect(&R,0,3*FontHeight/2);
      mainR[main_items++] = &mainbuttonR[i];
   }

   R.Ymin += 4;
   R.Ymax = R.Ymin + 2 * FontHeight + 8;
   mainbuttonR[i] = R;
   PaintQuitButton(false);
   mainR[main_items++] = &mainbuttonR[i];

   OffsetRect(&R,0,R.Ymax-R.Ymin + 8);
   R.Ymax = R.Ymin + FontHeight + 4;

   typenameR = R;


   OffsetRect(&R,0,2*FontHeight);
   for(i=0;i<6;i++)
   {
      dummyR[i] = R;
      OffsetRect(&R,0,3*FontHeight/2+4);
   }

   R.Ymax = HIYCOUNT-4;
   R.Ymin = R.Ymax - FontHeight - 4;
   CreateRadioPanel(&R,RSS,runstopstepR,3,stopped);
   SSR = R;
   initialize_numbers();
}

int ShadowAndSave(rect * tR)
{

	rect shR = *tR;
	rect uR;
	rect r1, r2, r3;
	int err;
	int i;
#ifdef ZOOMING
   int cx,cy;
   int width,height;
   double aspect;



   /* Let's try zooming into it. It sounds cute to me. */

   Centers(tR,&cx,&cy);
   width = tR->Xmax - tR->Xmin + 1;
   height = tR->Ymax - tR->Ymin + 1;

   aspect = width / (double)height;

   for(i=1;i<width/2;i += 4)
   {
      rect R;
      R.Xmin = cx - i;
      R.Xmax = cx + i;
      R.Ymin = cy - i / aspect;
      R.Ymax = cy + i / aspect;
      PenColor(WHITE);
      RasterOp(zXORz);
      FrameRect(&R);
      FrameRect(&R);
   }

   RasterOp(zREPz);
#endif



	OffsetRect(&shR, 6, 6);
	r3 = shR;
	ShiftRect(&r3, -6, -6, &r1, &r2);


	UnionRect(&shR, tR, &uR);

	PushRect(&uR, &err);
	if (err)
		return 0;

	RasterOp(zXORz);
	PenColor(DARKGRAY);
	PaintRect(&r1);
	PaintRect(&r2);
	/* and get the corners hee hee? */
	for (i = 1; i < 6; i++)
	{
		MoveTo(tR->Xmin + i, tR->Ymax);
		LineTo(tR->Xmin + i, tR->Ymax + i);
		MoveTo(tR->Xmax, tR->Ymin + i);
		LineTo(tR->Xmax + i, tR->Ymin + i);
	}
	MoveTo(tR->Xmax, tR->Ymax);
	LineTo(uR.Xmax, uR.Ymax);
	RasterOp(zREPz);
	return 1;

}



void RangeError(char *msg)
{
	rect R;
	int cx, cy;
	int width = 0;
	int i;
	int height = 3 * FontHeight + 4;

	char tbuf[3][128];

	Centers(&sR, &cx, &cy);


	strcpy(tbuf[0], "Range Error! Acceptable values are");
	strcpy(tbuf[1], msg);
	strcpy(tbuf[2], "Press any key or click to continue");

	for (i = 0; i < 3; i++)
		width = max(width, StringWidth(tbuf[i]) + 4);

	R.Xmin = cx - width / 2;
	R.Xmax = R.Xmin + width;
	R.Ymin = cy - height / 2;
	R.Ymax = R.Ymin + height;

	HideCursor();

	if (!ShadowAndSave(&R))
	{
		ShowCursor();
		return;
	}

	PenColor(RED);
	PaintRect(&R);
	PenColor(WHITE);
	FrameRect(&R);

	PenColor(MENUTEXT);
	BackColor(RED);
	TextAlign(alignCenter, alignTop);

	for (i = 0; i < 3; i++)
	{
		MoveTo(cx, R.Ymin + 2 + FontHeight * i);
		DrawString(tbuf[i]);
	}
	ShowCursor();
	while (1)
	{
		event e;

		KeyEvent(true, &e);

		if ((e.State & 0x700) || e.ASCII || e.ScanCode)
			break;
	}

	HideCursor();
	PopRect(&i);
	ShowCursor();
}

int cancel_ok_msg(char *msg)
{
	int current_item = 1;
	int centerx = sR.Xmax / 2;
	int centery = sR.Ymax / 2;
	rect okRect, cancelRect, R;
	int err;
	int retval = 0;

	int height = FontHeight + FontHeight + 14;
	int width = StringWidth(msg) + 12;

	R.Xmin = centerx - width / 2;
	R.Xmax = R.Xmin + width;
	R.Ymin = centery - height / 2;
	R.Ymax = R.Ymin + height;

	RasterOp(zREPz);

   PushMouseRectLimit(&R);
	HideCursor();
	PushCursorPosition();
	WaitForNothing();

	if (!ShadowAndSave(&R))
	{
		ShowCursor();
		LimitMouseRect(&sR);
		return 1;
	}
	PenColor(MENUBACK);
	PaintRect(&R);
	PenColor(BUTTONFRAME);
	FrameRect(&R);
	PenColor(MENUTEXT);
	BackColor(MENUBACK);
	TextAlign(alignCenter, alignTop);
	MoveTo(centerx, R.Ymin + 1);
	DrawString(msg);

	okRect.Xmin = R.Xmin + 4;
	okRect.Xmax = centerx - 2;
	okRect.Ymax = R.Ymax - 4;
	okRect.Ymin = okRect.Ymax - FontHeight - 4;
	PaintRadioButton(&okRect, false, false, "Yes");
	ExtraHilite(&okRect, false);

	cancelRect.Xmax = R.Xmax - 4;
	cancelRect.Xmin = centerx + 2;
	cancelRect.Ymax = R.Ymax - 4;
	cancelRect.Ymin = cancelRect.Ymax - FontHeight - 4;
	PaintRadioButton(&cancelRect, false, false, "No");
	move_to_corner(&cancelRect);
	PushButton(&cancelRect, true);
	current_item = 1;
	//ArrowCursor();
	ProtectOff();
	ShowCursor();

	while (1)
	{
		event e;
		int key = 0;
		int button;
		int X, Y;
		int n;
		int last_item = current_item;



		n = KeyEvent(false, &e);
		X = e.CursorX;
		Y = e.CursorY;

		if (n)
		{

			if (e.ASCII && e.ASCII != 0xe0)
				key = e.ASCII;
			else
				key = e.ScanCode << 8;
			button = (e.State >> 8) & 0x7;
			if (button == swRight)
				break;
			if (button == swLeft)
				key = 0x0d;

			if (key == 0x0d)
			{
				if (XYInRect(X, Y, &okRect))
					retval = 1;
				else if (XYInRect(X, Y, &cancelRect))
					retval = 0;
				break;
			}

			if (key == 'y' || key == 'Y')
			{
				retval = 1;
				break;
			}

			if (key == 0x1b || key == 'n' || key == 'N')
				break;

			if (key == XRARROW || key == XLARROW || key == ' ')
			{
				current_item ^= 1;
				move_to_corner(current_item ? &cancelRect : &okRect);
			}
		}
		else
		{
			if (XYInRect(X, Y, &okRect))
				current_item = 0;
			else if (XYInRect(X, Y, &cancelRect))
				current_item = 1;
			else
				current_item = -1;
		}

		if (current_item != last_item)
		{
			switch (last_item)
			{
			case 0:
				PushButton(&okRect, false);
				ExtraHilite(&okRect, false);
				break;
			case 1:
				PushButton(&cancelRect, false);
				break;
			}
			switch (current_item)
			{
			case 0:
				PushButton(&okRect, true);
				ExtraHilite(&okRect, true);
				break;
			case 1:
				PushButton(&cancelRect, true);
				break;
			}
		}



	}

	/* Depress the appropriate button */
	if (retval == 1)
	{
		PaintRadioButton(&okRect, true, true, "Yes");
		ExtraHilite(&okRect, true);
	}
	else
		PaintRadioButton(&cancelRect, true, true, "No");


	/* Wait for the key to be lifted */
	WaitForNothing();

	if (retval == 1)
	{
		PaintRadioButton(&okRect, false, false, "Yes");
		ExtraHilite(&okRect, false);
	}
	else
		PaintRadioButton(&cancelRect, false, false, "No");

	HideCursor();
	PopRect(&err);
	PopCursorPosition();
   PopMouseRect();
	ShowCursor();
	return retval;
}

void ErrorBox(char *s)
{
	int width = StringWidth(s) + 8;
	int height = 2 * FontHeight + 4;
	rect R;
	int cx, cy;
	char *msg = "Press any key or click to continue.";
	int n;

	width = max(width, StringWidth(msg) + 8);
	Centers(&sR, &cx, &cy);

	R.Xmin = cx - width / 2;
	R.Xmax = R.Xmin + width - 1;
	R.Ymin = cy - height / 2;
	R.Ymax = R.Ymin + height - 1;
	HideCursor();
	if (!ShadowAndSave(&R))
	{
		ShowCursor();
		return;
	}


	PenColor(RED);
	PaintRect(&R);
	PenColor(0);
	FrameRect(&R);
	PenColor(MENUTEXT);
	BackColor(RED);
	TextAlign(alignCenter, alignTop);
	MoveTo(cx, R.Ymin + 2);
	DrawString(s);
	MoveTo(cx, R.Ymin + FontHeight + 2);
	DrawString(msg);
	ShowCursor();
	while (1)
	{
		event e;

		KeyEvent(true, &e);

		if ((e.State & 0x700) || e.ASCII || e.ScanCode)
			break;
	}
	HideCursor();
	PopRect(&n);
	ShowCursor();
}

int Overwrite(char *f)
{
	struct stat statbuf;
	int i;
	char tbuf[128];
	char name[30];
	char ext[20];

	i = stat(f, &statbuf);

	if (i)
		return 1;       /* ok to write, file doesn't exist */

   /* It might be read only, in which case we stop it here */
   if (statbuf.st_mode & S_IFDIR)
   {
      sprintf(tbuf,"Error: %s is a directory.",f);
      ErrorBox(tbuf);
      return 0;
   }

   if (!(statbuf.st_mode & S_IWRITE))
   {
      sprintf(tbuf,"Error: %s is read-only.",f);
      ErrorBox(tbuf);
      return 0;
   }



	fnsplit(f, NULL, NULL, name, ext);
	sprintf(tbuf, "Overwrite %s%s?", name, ext);
	return cancel_ok_msg(tbuf);
}
