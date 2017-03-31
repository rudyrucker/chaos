#ifndef _BUTTONSH_
#define _BUTTONSH_
typedef void (*pf)(void);
typedef pf *ppf;
#define VERTICAL 0
#define HORIZONTAL 1
extern rect mainbuttonR[];
typedef struct {
   char *text;
   rect boxR;
   rect tR;
} radio_button;

typedef struct {
   char *title;
   int orientation;     /* horizontal or vertical */
   int count;           /* how many buttons in this thing */
   int value;
   radio_button *b[8];  /* up to eight buttons allowed */
   rect R;              /* the entire rect for all of these */
} radio_button_set;

typedef struct {
   rect nR;
   char text[30];
} text_button;

typedef struct {
   char *title;
   double value;
   double old_value;
   double min,max;
   rect tR;
   rect sR;    /* rect of the slider "box" */
   rect bR;    /* rect of the marker-bar */
//   rect nR;    /* rect where the printed number goes */
   rect zR[6];
   int bubblewidth;
   text_button TB;
} slider;



void reposition_slider_v(slider *s,int selected);
void reposition_slider_X(slider *s,int X,int selected);
void create_slider(slider *s,rect *mR,int x,int y);
int radio_button_set_height(radio_button_set *rbs);
int slider_height(slider *s);
void create_slider_base(slider * s, rect * mR, int x, int y, int floater,
      int tinies,int frame, int numbers);
void create_slider_base_width(slider * s, rect * mR, int x, int y, int floater,
      int tinies,int frame, int numbers,int width);
void reposition_slider_v_base(slider * s, int selected, int floater, int numbers);
void reposition_slider_v(slider * s, int selected);
void reposition_slider_X_base(slider *s,int X,int selected,int floater,int numbers);
void CreateRadioPanel(rect *tR,char *msgs[],rect bR[],int n,int current);
void CreateRadioPanelLabel(rect *tR,char *msgs[],rect bR[],int n,int current,char *label,int fg,int bg);
void PaintRadioButton(rect *bR,int inout,int hilite,char *txt);
void PaintRadioButtonBase(rect *bR,int inout,int hilite,char *txt,int c1,int c2,int c3);
int CheckRadioButtons(int X,int Y,rect *bR,int n,int *w,char *msgs[]);
void ExtraHilite(rect *R,int n);
void CreateButtonPanel(rect *tR,char *msgs[],rect bR[],int n,int *vals[]);
int CheckNormalButtons(int X,int Y,rect *bR,int n,int *vals[],char *msgs[]);
void PaintNumberBox(rect *tR,text_button *nR,double value,char *prompt,int type);
int GetNumber(text_button *nR,double *result,int type,double lo,double hi);
int CheckSingleButton(int X,int Y,rect *bR,char *msg,int *n);
void DoublePress(rect *dR,int inout,int color);
void InvertInsides(text_button *R);
void navigate(int key,int *l,int *r,int *u,int *d,int items,rect *bR[],int *ci);
void PaintNumberBoxBase(rect *tR,text_button *nR,double value,char *prompt,
   int type,int c1,int c2,int c3,int c4);
void PaintTextBoxEntry(text_button *tB,int c3,int c4,char *text);
void PaintTextBoxBase(rect *tR,text_button *tbR,char *prompt,char *text,
   int c1,int c2,int c3,int c4);
int GetText(text_button *tbR,char *result,char *start,int backcolor,int textcolor);
void PaintNumberBoxEntry(text_button *nR,double value,int type);
void PushButton(rect *R,int inout);
void JString(char *,int,int,int,int,int,int);
void BasicCenteredBox(rect *dR,int width,int height,int bcolor,char *title,int titlecolor);
void PlusMinusButtons(rect *R,rect *dR);
void PushOrDoublePress(rect *R,int inout,int selected);
void PaintNumberBoxBasePrecision(rect *tR,text_button *nR,double value,char *prompt,
   int type,int c1,int c2,int c3,int c4,int width,int precision);
void PaintNumberBoxEntryPrecision(text_button *nR,double value,int type,int width,int precision);
int GetNumberBase(text_button *tbR,double *result,int type,double lo,
   double hi,int width,int precision);
void WaitForNothing(void);
void PaintTextBox(rect *tR,text_button *nR,char *prompt,char *text);

typedef enum {
   GS_ANYTHING,
   GS_INTEGER,
   GS_UNSIGNED,
   GS_FLOAT
} GS_TYPE;

typedef struct {
   rect tR;
   text_button TB;
   rect mR;
   rect pR;
   GS_TYPE type;
   double value;
   int p1,p2;        /* precision */
} numbertool;

void CreateNumberTool(numbertool *t);
int ProcessSlider(slider *s,int current_item);
int _jGetString(char *dest, char *start, int maxlen, GS_TYPE type);
int jGetString(char *dest, char *start, int maxlen);
void RangeError(char *);
#endif
