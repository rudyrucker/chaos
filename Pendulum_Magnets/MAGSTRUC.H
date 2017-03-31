#define MAXMAGS 32
#define MAXCONFIGS 32

/*-------------Types------------------*/
typedef struct
{
	int n;			/* number of magnets */
	int autopattern;	/* 0 is use values position values here, 1 is
				 * load symmetrically by formulas in the
				 * setuphole procedure, 2 means randomize
				 * positions, and 3 is random positions in
				 * upper part of screen, and 4 is a grid
				 * centered in the screen */


	int maxx, maxy; 	/* What were the maxx and maxy when this was
				 * recorded? */
	struct
	{
		int x, y;	/* pixel coordinates of magnet center */
		long charge;	/* + attracts, - repels.  Sizes about 1-60. */
	} magnet[MAXMAGS];
} magnetstructure;



typedef struct
{
	magnetstructure M;
	long centerpull;	/* pendulum force = centerpull * distance
				 * from center.  This force keeps things on
				 * screen. */
	long chargeunit;	/* charge to use if all magnets are given the
				 * same charge, as when you press '6' and
				 * load a hexagonal pattern of equal charges. */
	long freq;		/* dt = 1/freq .  High freq is slower &
				 * smoother */
	double radius;		/* pixel distance between magnets & center of
				 * screen in symmetric setups. */
	long magnetradius;	/* Outside the magnet radius, the forcefield
				 * is charge/distance^2 or charge/distance
				 * (depending on forcetype).  To prevent
				 * infinite forces we make the forces fall
				 * off to 0 at magnet center.  Change over
				 * between two ways of computing at
				 * magnetradius. radius at which magnet force
				 * changes */
	double friction;	/* 1 is decent, but you can go to like 5 */
	int reversibleflag;	/* If this is on, then no roundoffs are kept
				 * track of.  This has a positive result of
				 * making the trajectories perfectly
				 * reversible.	But if reversible flag is on,
				 * motion is less lively and,worst of all, if
				 * the frequency is very high and the
				 * reversible flag is on then almost all
				 * motion is lost in roundoff and the bob
				 * does little pieces of straight line. */
	int xsection;		/* radius of sticky spot at magnet center. If
				 * bob gets this close to cetner it stops. */
	int basinflag;		/* 0 is normal mode, 1 is draw basins */
	int forcetype;		/* 0 is inverse square force, 1 is inverse
				 * linear force.  That is, if bob is distance
				 * d from a charge and force type is 0, then
				 * force is charge/d^2.  If force type is 1,
				 * then force is charge/d. */
	int tracetype;		/* 0 is no trace, 1 is lines, 2 is lines &
				 * balls 3 is a snake of oldballs.n line
				 * segs. */
	int frictiontype;	/* 0 is no friction, 1 is friction force
				 * gotten by friction*speed, and 2 is
				 * friction force gotten by
				 * friction*speed^2/100.  The 100 keeps the
				 * value from getting so large out of control
				 * that spurious results arise (such as
				 * friction changing the sign of the speed). */
} configstructure;

/*--------------Global Variables-----------*/

extern magnetstructure M;	/* this global holds the current
				 * magnetstructure */
