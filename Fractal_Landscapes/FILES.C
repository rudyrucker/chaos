#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <dos.h>
#include <time.h>
#include <io.h>
#include "forge.h"

#define GIF_COOLMAN 0
#define ROUND(x) (((x) > 0) ? ((x) + .5) : ((x) - .5))

static char *file_texts[] = {
   "Save Parameters    [Alt-S]",
   "Load Parameters    [Alt-L]",
   "Save screen as GIF [Alt-G]",
   "View GIF image     [Alt-F]",
   "Animate                [a]",
   "View Animation         [v]",
   "Save as DXF            [x]",
   "F1 for HELP",
   "ESC to Cancel",
   NULL
   };

static rect fileR[10];
static rect *pfileR[10];
static int current_item;

void SaveParameters(char *filename)
{
   FILE *fd;
   time_t timex;
   char tbuf[128];
   int ok = true;
   double d;
   int i;


   fd = fopen(filename,"wt");
   if (!fd)
   {
      FileError(tbuf,NULL);
      return;
   }

   ok &= EOF != fprintf(fd,"CHAOS Forge Data\n");
   timex = time(NULL);
   ok &= EOF != fprintf(fd,"Created: %s",ctime(&timex));

   ok &= EOF != fprintf(fd,"Dimension: %g\n",fracdim);
   ok &= EOF != fprintf(fd,"Height: %g\n",elevfac);
   ok &= EOF != fprintf(fd,"Power: %g\n",log((power)+1));
   ok &= EOF != fprintf(fd,"Azimuth: %g\n",vturn);
   ok &= EOF != fprintf(fd,"Elevation: %g\n",90 - vdown);
   d = siang * 360.0 / (2 * M_PI);
   i = ROUND(d);
   ok &= EOF != fprintf(fd,"Season: %d\n",i);
   d = shang * 24.0/(2*M_PI);
   i = ROUND(d);
   ok &= EOF != fprintf(fd,"Hour: %d\n",i);
   ok &= EOF != fprintf(fd,"Mesh size: %d\n",meshsizes[mesh]);
   ok &= EOF != fprintf(fd,"Terms: %d\n",termsizes[terms]);
   ok &= EOF != fprintf(fd,"Seed: %d\n",rseed);
   ok &= EOF != fflush(fd);

   if (!ok || ferror(fd))
   {
      FileError(filename,fd);
      fclose(fd);
      remove(filename);
   }
   else
   	fclose(fd);
}

void LoadParameters(char *filename)
{
   FILE *fd;
   char tbuf[128];
   double z;
   int t;
   int i;

   fd = fopen(filename,"rt");
   if (!fd)
   {
      FileError(filename,NULL);
      return;
   }

   while(1)
   {
      fgets(tbuf,128,fd);
      if (feof(fd))
         break;
      if (tbuf[0] == '#')
         continue;
      if (strlen(tbuf) == 0)
         continue;

      if (sscanf(tbuf,"Dimension: %lg",&z))
         fracdim = z;
      else if (sscanf(tbuf,"Height: %lg",&z))
         elevfac = z;
      else if (sscanf(tbuf,"Power: %lg",&z))
         power = exp(z)-1;
      else if (sscanf(tbuf,"Azimuth: %lg",&z))
         vturn = z;
      else if (sscanf(tbuf,"Elevation: %lg",&z))
         vdown = 90.0 - z;
      else if (sscanf(tbuf,"Season: %d",&t))
         siang = t * 2 * M_PI / 360.0;
      else if (sscanf(tbuf,"Hour: %d",&t))
         shang = t * 2 * M_PI / 24.0;
      else if (sscanf(tbuf,"Mesh size: %d",&t))
      {
         for(i=0;i<5;i++)
         {
            if (meshsizes[i] == t)
            {
               mesh = i;
               break;
            }
         }
      }
      else if (sscanf(tbuf,"Terms: %d",&t))
      {
         for(i=0;i<6;i++)
         {
            if (termsizes[i] == t)
            {
               terms = i;
               break;
            }
         }
      }
      else if (sscanf(tbuf,"Seed: %d",&t))
         rseed = t;

   }

   fclose(fd);
   initgauss(rseed);
   SetMeshButton();
   SetTermsButton();
   DisplayRseed();
   power_slider_value = log(power+1);
   SetAllSliders();
   uparam(meshsizes[mesh]);
}





void gif_viewer(void)
{
	int found = 0;
   char chosen_file[128];
   event e;
   int mem_err = 0;

	while (!mem_err && select_file("View GIF image", "*.gif", chosen_file, "GIF"))
	{
      union REGS regs;
      if (access(chosen_file,0))
      {
         FileError(chosen_file,NULL);
         continue;
      }
      HideCursor();
      if (!found)
         PushPlanetRects();
		BackColor(BLACK);
		EraseRect(&sR);
		if (GifDisplay(chosen_file) == GIF_COOLMAN)
      {
		   sound(110);
		   delay(10);
		   nosound();
		   while (1)
		   {
			   KeyEvent(true, &e);
			   if ((e.State >> 8) & 7)
				   break;
			   if (e.ASCII || e.ScanCode)
				   break;
		   }
      }
      else
         mem_err = 1;
      found = 1;

      regs.h.ah = 0;
      regs.h.al = (hasVGA) ? 0x12 : 0x10;
      int86(0x10,&regs,&regs);
      LoadPalette(0,15,brightpal);

      ShowCursor();
	}

   if (found)
   {
      /* restore the screen to how we like it */
      union REGS regs;
      HideCursor();
      regs.h.ah = 0;
      regs.h.al = (hasVGA) ? 0x12 : 0x10;
      int86(0x10,&regs,&regs);
      PaintMainWindow();
      PopPlanetRects();
      ShowCursor();
   }
}


static void hilite_item(int item,int inout,int items)
{
   if (item !=-1)
   {
      PushButton(pfileR[item],inout);
      if (item == items-1)
         ExtraHilite(pfileR[item],inout);
   }
}

static int hotkeys[] = {XALTS,XALTL,XALTG,XALTF,'a','v','x'};

void files(void)
{
   int height,width;
   int i;
   rect tR;
   rect R;
   int key;
   int doit = 0;
   char tbuf[128];
   int items;


   height = 9 * (FontHeight+8) + FontHeight + 12;
   width = 0;

   for(i=0;file_texts[i];i++)
      width = max(width,StringWidth(file_texts[i])+12);
   items = i;


   HideCursor();
   PushCursorPosition();

   BasicCenteredBox(&tR,width,height,LIGHTGRAY,"File Menu",BLACK);

   for(i=0;file_texts[i];i++)
   {
      R.Xmin = tR.Xmin + 4;
      R.Xmax = tR.Xmax - 4;
      R.Ymin = tR.Ymin + FontHeight + 4 + i * (FontHeight + 8);
      if (i >= items - 2)
         R.Ymin += 8;
      R.Ymax = R.Ymin + FontHeight + 4;

      fileR[i] = R;
      pfileR[i] = &fileR[i];

      PaintRadioButtonBase(&R,false,false,file_texts[i],
         DARKGRAY,RED,WHITE);
   }
   LimitMouseRect(&tR);
   current_item = items - 1;
   move_to_corner(pfileR[current_item]);

   hilite_item(current_item,true,items);

   ShowCursor();
   while(!doit)
   {
      event e;
      int X,Y;
      
      int last_item = current_item;
      int n;

      n = KeyEvent(false,&e);
      X = e.CursorX;
      Y = e.CursorY;

      ProcessKey(X,Y,&current_item,&key,pfileR,items,n,&e);

      if (key == 0x1b)
         break;

      if (key == 0x0d)
      {
         if (XYInRect(X,Y,pfileR[8]))
            break;

         if (XYInRect(X,Y,pfileR[7]))
            key = XF1;

         for(i=0;i<7;i++)
         {
            if (XYInRect(X,Y,pfileR[i]))
            {
               doit = 1;
               break;
            }
         }
         if (doit)
            break;
      }


      for(i=0;i<7;i++)
      {
         if (key == hotkeys[i])
         {
            doit = true;
            current_item = i;
            break;
         }
      }
        
      navigate(key,NULL,NULL,(int *)-1,(int *)-1,items,pfileR,&current_item);
            
      switch(key)
      {
      case XF1:
         helptext("forgfile.hlp");
         LimitMouseRect(&tR);
         break;
      }



      if (current_item != last_item)
      {
         hilite_item(last_item,false,items);
         hilite_item(current_item,true,items);
      }

   }

   PaintRadioButtonBase(pfileR[current_item],true,true,file_texts[current_item],
      DARKGRAY,RED,WHITE);
   WaitForNothing();
   PaintRadioButtonBase(pfileR[current_item],false,false,file_texts[current_item],
      DARKGRAY,RED,WHITE);
   HideCursor();
   PopRect(&i);
   PopCursorPosition();
   ShowCursor();



   if (doit)
   {
      switch(current_item)
      {
      case 0:
         if (select_file("Save Forgery Parameters","*.FOR",tbuf,"FOR") && Overwrite(tbuf))
            SaveParameters(tbuf);
         break;
      case 1:
         if (select_file("Load Forgery Parameters","*.FOR",tbuf,"FOR"))
            LoadParameters(tbuf);
         break;
      case 2:
         if (select_file("Save Screen as GIF image","*.GIF",tbuf,"GIF"))
         {
            if (Overwrite(tbuf))
               GifOutput(tbuf,true);
         }
         break;
      case 3:
         gif_viewer();
         break;

      case 4:
         if (hasVGA)
            animate();
         else
            ErrorBox("Sorry, you need a VGA to animate.");
         break;
      case 5:
         if (hasVGA)
            view_animation();
         else
            ErrorBox("Sorry, you need a VGA to view animations.");
         break;
      case 6:
         dxfout();
         break;
      }
   }
   LimitMouseRect(&sR);
   

}
