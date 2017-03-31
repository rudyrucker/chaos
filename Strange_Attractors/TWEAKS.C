#include <math.h>
#include <stdlib.h>
#include <dos.h>
#include <stdio.h>

#include "attract.h"

/* This one is the most complicated of the main menus, because a different
tweak controller comes up depending upon which mode we are in.

This particular pile of source is well desiring of being shrunk (lotsa
repetitive stuff here. */

char *standard_button_texts[] = {
	"F1 for HELP",
	"ESC to Cancel",
	"ACCEPT"
};


extern void logistic_tweaker(void), henon_tweaker(void),
 lorenz_tweaker(void), yorke_tweaker(void);

pf tweaks_items[] = {
	NULL,
	logistic_tweaker,
	henon_tweaker,
	lorenz_tweaker,
	yorke_tweaker
};


void do_tweaks_menu(void)
{
	erasecursor();
	(*tweaks_items[dimension]) ();
}
