#include "mand.h"



typedef enum {BLANK,BULLSEYES,FEATHERS} FILLTYPE;
typedef enum {JULIA,CUBICJULIA,MANDEL,CUBICMANDEL,RUDY} FRACTYPE;
typedef enum {UV,AB,AU,BV,BU,AV} SLICETYPE;


typedef struct {
   char *name;
   FRACTYPE type;
   FILLTYPE filltype;
   SLICETYPE slicetype;
   double x,y,width;
   int iterations;
   double u,v,a,b;
} preset;


preset CJFLYER = {
   "Cubic Julia FLYER",
   CUBICJULIA,
   BLANK,
   UV,
   0.06,0.1260775862,4,
   20,
   0,0.965448,0,0.463103
   };

preset CJPIECES = {
   "Cubic Julia PIECES",
   CUBICJULIA,
   BLANK,
   UV,
	0.004285714286,
	0.09375,
	3.121428571,
	20,
	0.2737089286,
	0.7181278864,
	-0.2914285714,
	-0.09489655172,
   };

preset CJSHMOO = {
   "Cubic Julia SHMOO",
   CUBICJULIA,
   BLANK,
   BV,
	-0.06,
	0.02909482759,
	4,
	20,
	0.08017241379,
	0.7079741379,
	-0.495,
	-0.08,
   };

preset CMBOZO = {
   "Cubic Mandelbrot BOZO",
   CUBICMANDEL,
   BULLSEYES,
   UV,
	-0.01714285714,
	0.9989224138,
	1.185714286,
	20,
	0,
	0,
	0,
	0,
   };

preset CMHEART = {
   "Cubic Mandelbrot HEART",
   CUBICMANDEL,
   FEATHERS,
   UV,
	0.4037956971,
	0.7305117002,
	0.2983855422,
	31,
	-0.1942857143,
	0.7887,
	0.531,
	0.1862,
   };

preset CMLAUGH = {
   "Cubic Mandelbrot LAUGH",
   CUBICMANDEL,
   BLANK,
   UV,
	0.1208187721,
	0.874052614,
	0.6749725693,
	25,
	-0.1942857143,
	0.7887,
	-0.4055278757,
	-0.113517179,
   };

preset CMPAIR = {
   "Cubic Mandelbrot PAIR",
   CUBICMANDEL,
   BLANK,
   UV,
	0.02028571429,
	-0.03844673645,
	2.331535714,
	20,
	0,
	0,
	-0.5142857143,
	-0.2735623892,
   };

preset CMSPIN = {
   "Cubic Mandelbrot SPIN",
   CUBICMANDEL,
   FEATHERS,
   UV,
	0.02,
	-0.06788793103,
	3.09,
	20,
	0,
	0.965448,
	0.42,
	-0.2948275862,
   };

preset CMSPIRAL = {
   "Cubic Mandelbrot SPIRAL",
   CUBICMANDEL,
   BLANK,UV,
	-0.07891447921,
	0.7825726195,
	0.0005268164062,
	300,
	0,
	0,
	-0.112,
	-0.112,
   };

preset CMGALAXY = {
   "Cubic Mandelbrot GALAXY",
   CUBICMANDEL,BLANK,AB,
	0.15,
	0.02909482759,
	4,
	20,
	0.5957142857,
	0.45,
	0,
	-0.2271428571,
   };

preset CMWORLD = {
   "Cubic Mandelbrot WORLD",
   CUBICMANDEL,BLANK,AV,
	0.1,
	0.05818965517,
	3.04,
	20,
	0.5431034483,
	0,
	0,
	-0.2271428571,
   };

preset JESSES = {
   "Julia ESSES",
   JULIA,BLANK,UV,
	0,
	-0.1163793103,
	3.55,
	50,
	-0.8550714286,
	-0.1889778325,
	-0.112,
	-0.112,
   };

preset JWHIRL = {
   "Julia WHIRL",
   JULIA,BLANK,UV,
	0.04,
	-0.05818965517,
	3.34,
	300,
	0.004,
	0.64,
	0,
	0,
   };

preset MANT22 = {
   "Mandelbrot ANT 22",
   MANDEL,BLANK,UV,
	-0.7239834171,
	0.2867201688,
	3.651115382e-05,
	2000,
	0.3,
	0.2,
	0,
	0,
   };

preset MBOZO = {
   "Mandelbrot BOZO",
   MANDEL,BULLSEYES,UV,
	-0.123890051,
	0.7368188391,
	0.69,
	50,
	0,
	0,
	-0.112,
	-0.112,
   };

preset MFEATHER = {
   "Mandelbrot FEATHER",
   MANDEL,FEATHERS,UV,
	-0.56,
	-0.01939655172,
	3.06,
	50,
	0,
	0,
	-0.112,
	-0.112,
   };

preset MNOSE = {
   "Mandelbrot NOSE",
   MANDEL,BLANK,UV,
	-1.3850407,
	-0.002054337991,
	0.07738215743,
	133,
	-0.0469189522,
	1.131710191,
	-0.2308571429,
	-0.2604137931,
   };

preset MSEAHORSE = {
   "Mandelbrot SEAHORSE VALLEY",
   MANDEL,BLANK,UV,
	-0.7458771984,
	0.1049027605,
	0.009096351568,
	244,
	-0.0469189522,
	1.131710191,
	-0.2308571429,
	-0.2604137931,
   };

preset MWARP = {
   "Mandelbrot WARP",
   MANDEL,BLANK,UV,
	0.4432412484,
	0.3740342317,
	0.009988810708,
	240,
	0,
	0,
	-0.112,
	-0.112,
   };

preset CMROACH = {
   "Cubic Mandelbrot ROACH",
   CUBICMANDEL,BLANK,BV,
   0.3030407083,
   0.5436345198,
   0.4100381751,
   23,
   -0.4722758621,
   0.5586206615,
   -0.1507123099,
   -0.05614517229,
   };

preset RLOTUS = {
   "Rudy LOTUS",
   RUDY,BLANK,UV,
   0,
   0.6200092733,
   0.1576458216,
   48,
   0,
   0.9654479921,
   -0.0289927721,
   0.6527712643,
   };
                   
   


preset *preset_list[] = {
	&CJFLYER,
	&CJPIECES,
	&CJSHMOO,
	&CMBOZO,
	&CMHEART,
	&CMLAUGH,
	&CMPAIR,
	&CMSPIN,
	&CMSPIRAL,
	&CMGALAXY,
	&CMWORLD,
	&CMROACH,
	&JESSES,
	&JWHIRL,
	&MANT22,
	&MBOZO,
	&MFEATHER,
	&MNOSE,
	&MSEAHORSE,
	&MWARP,
   &RLOTUS,
   NULL
   };

int current_preset = 0;

void next_preset(void)
{
   current_preset++;
   if (preset_list[current_preset] == NULL)
      current_preset = 0;
}

void previous_preset(void)
{
   current_preset--;
   if (current_preset < 0)
   {
      int i;
      for(i=0;preset_list[i];i++);
      current_preset = i - 1;
   }
}

void load_preset(void)
{
   preset *p = preset_list[current_preset];
	pushstamp(&stampviews[saveptr]);

   MyTypeFlag = p->type;
   MyTypetoFlags();
   insideflag = p->filltype;
   maxiteration = p->iterations;
   slicetype = p->slicetype;
   fu = p->u;
   fv = p->v;
   fa = p->a;
   fb = p->b;

   flox = p->x - p->width/2.0;
   fhix = p->x + p->width/2.0;
   floy = p->y - (4.5/5.6) * p->width/2.0;
   fhiy = p->y + (4.5/5.6) * p->width/2.0;

	switch (MyTypeFlag)
	{
	case 0:
		pushview(&startview);
		break;
	case 1:
		pushview(&startview);
		break;
	case 2:
		pushview(&mandelview);
		break;
	case 3:
		pushview(&cubicview);
		break;
	case 4:
		pushview(&ruckerview);
		break;
	}

	resetcursor();
   ParameterDisplayMode = 0;
   SaveMe = 1;
   float_to_integer();
	useview();
   menutext(p->name);
}







