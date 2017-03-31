MASM51
QUIRKS
;manda39.asm
;      (C) Copyright 1990 by Autodesk, Inc.
;
;******************************************************************************
;*									     *
;* The information contained herein is confidential, proprietary to Autodesk, *
;* Inc., and considered a trade secret as defined in section 499C of the      *
;* penal code of the State of California.  Use of this information by anyone  *
;* other than authorized employees of Autodesk, Inc. is granted only under a  *
;* written non-disclosure agreement, expressly prescribing the scope and      *
;* manner of such use.							     *
;*									     *
;******************************************************************************/


;-----------------------------Equates-----
shift EQU 28
cubicshift EQU 25
CUBICCUT EQU 0F8000000h	; 0200 0000h is 1, cutoff is 4
CUBICCUT6 EQU 0F800h	; 0200h is 1
;-----------------------------Macros------
shift12 MACRO
	REPT 12
		shr dx,1
		rcr ax,1
	ENDM
ENDM
;------------------------------------------------------
shift11 MACRO
	REPT 11
		shr dx,1
		rcr ax,1
	ENDM
ENDM
;------------------------------------------------------
shift9 MACRO
	REPT 9
		sar dx,1
		rcr ax,1
	ENDM
ENDM
;-----------------------------Program Header---------
LOCALS
DOSSEG
.MODEL LARGE,C
	extrn	iplot_orbit:far		
	extrn	scrub_orbit:far		

.DATA
EXTRN chip:WORD,mode:WORD
EXTRN maxiteration:WORD,x:DWORD,y:DWORD
EXTRN curx:WORD,cury:WORD,cursoron:WORD,cursoroffset:WORD
EXTRN u:DWORD,v:DWORD
EXTRN a:DWORD, b:DWORD, a3:DWORD, b3:DWORD, nega:DWORD, negb:DWORD
EXTRN mousebutton:WORD,mousex:WORD,mousey:WORD,coarsemode:WORD
EXTRN grayflag:WORD, palettenumber:WORD, insideflag:WORD
EXTRN cursorshape:WORD
EXTRN vga_lut:WORD
EXTRN ega_lut:WORD
EXTRN cpu:WORD
	extrn	orbit_ptr:word		; "orbit pointer" flag
	extrn	inside:word		; "inside" color, normally 1 (blue)
	extrn	show_orbit:word		; "show-orbit" flag
	extrn	lx0:dword, ly0:dword	; arrays of (dword) increment values
        extrn pixelx:word,pixely:word
        extrn creal:dword,cimag:dword
	extrn	delx:dword, dely:dword	; actual increment values
        extrn   lm:dword
        extrn   bitshift:word


;PUBLIC egacolortable
;PUBLIC vgacolortable

extrn minx:WORD
cursorpatch     db 64 dup(0);
cubictake       dw 0
poscolor	dw 0
stackpointer    dw ?
smallestval     dd ?
smallestindex   dw ?
shiftregister   dw 17h
count	        dw 0
xa		dd 0		;need these in cubicmandelbrot
yb		dd 0
temp	        dd 0



xx              dd      0
yy              dd      0
IXX             dw      0
IYY             dw      0
k               dw      0
aa              dd      0
bb              dd      0
oldcolor        dw     0
savedand	dw	0		; AND value for periodicity checks
savedmask	dd	0		; saved values mask
savedx		dd	0		; saved values of X and Y iterations
savedy		dd	0		;  (for periodicity checks)
period		db	0		; periodicity, if in the lake
savedincr	dw	0		; flag for incrementing AND value
savedmax	dd	0		; saved max(delx, dely)

;egacolortable db 17 dup(0)
;This is at palettenumber 0 which we randomize.  17 bytes long.

;default_egacolortable
 db 0,1,2,3,4,5,14h,7,38h,39h,3ah,3bh,3ch,3dh,3eh,3fh,0

;preset_egacolortable1
 db 0,4,2,3,4,5,6,7,14,9,10,11,12,13,14,15,0

;preset_egacolortable2
 db 0,4,4,2,2,3,3,5,5,6,6,7,7,8,8,4,0

;preset_egacolortable3
 db 0,2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,0

public activedactable
activedactable db 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16



;vgacolortable db 51 dup(0)
;This is palettenumber 0 which we randomize all the time.
;The following are various preset_vgacolortablen, each of which
;is found at offset vgacolortable + n*51.

;preset_vgacolortable1
db   0,   0,   0   ;   0
db  63,   0,   0   ;   1
db  63,   0,  10   ;   2
db  63,   0,  20   ;   3
db  63,   0,  30   ;   4
db  63,   0,  40   ;   5
db  63,   0,  50   ;   6
db  50,   0,  63   ;   7
db  40,   0,  63   ;   8
db  30,   0,  63   ;   9
db  20,   0,  63   ;  10
db  10,   0,  63   ;  11
db  63,  45,   0   ;  12
db  63,  52,   0   ;  13
db  63,  55,   0   ;  14
db  63,  63,   0   ;  15
db   0,   0,   0   ;  16 overscan

;preset_vgacolortable2
db   0,   0,   0   ;   0
db  63,   0,   0   ;   1
db  63,   0,  10   ;   2
db  63,   0,  20   ;   3
db  63,   0,  30   ;   4
db  63,   0,  40   ;   5
db  0,   63,   0   ;   1
db  0,  63,  10   ;   2
db  0,  63,  20   ;   3
db  0,  63,  30   ;   4
db  0,  63,  40   ;   5
db  0,  63,   0   ;   1
db 63,  63,  10   ;   2
db 63,  63,  20   ;   3
db 63,  63,  30   ;   4
db 63,  63,  40   ;   5
db   0,   0,   0   ;  16 overscan

;preset_vgacolortable3
db  0,   0,   0    ;   0
db  0,  0 , 42   ;   1
db  0, 21 , 42  ;   2
db  0, 32 , 42  ;   3
db 21, 42 , 42  ;   4
db 30, 42 , 42  ;   5
db 37, 43 , 34  ;   6
db 42, 44 , 26   ;   7
db 63, 63 , 36   ;   8
db 63, 57 , 20   ;   9
db 63, 46 , 20   ;  10
db 63, 40 , 18   ;  11
db 62, 31 , 12   ;  12
db 63, 21 ,  3  ;  13
db 63, 13 ,  0  ;  14
db 63,  0 ,  0  ;  15
db  0,  0,  0  ; 16 overscan

;preset_vgacolortable4
db    0,   0,   0    ;   0
db    0,   0,  42  ;   1
db    0,  21,  42 ;   2
db    0,  42,  42 ;   3
db   21,  42,  42 ;   4
db   42,  42,  42 ;   5
db   42,  42,  63 ;   6
db   42,  63,  63  ;   7
db   63,  63,  63  ;   8
db   63,  63,  42  ;   9
db   63,  42,  42  ;  10
db   63,  42,  21  ;  11
db   63,  21,  21  ;  12
db   42,  21,  21 ;  13
db   21,  21,  21 ;  14
db   21,   0,   0 ;  15
db  0,  0,  0  ; 16 overscan

;preset_vgacolortable5
db   0,   0,   0    ;   0
db   4,   4,   4  ;   1
db   8,   8,   8 ;   2
db  12,  12,  12 ;   3
db  16,  16,  16 ;   4
db  20,  20,  20 ;   5
db  24,  24,  24 ;   6
db  28,  28,  28  ;   7
db  32,  32,  32  ;   8
db  36,  36,  36  ;   9
db  41,  41,  41  ;  10
db  45,  45,  45  ;  11
db  50,  50,  50  ;  12
db  54,  54,  54 ;  13
db  59,  59,  59 ;  14
db  63,  63,  63 ;  15
db  0,  0,  0  ; 16 overscan

;preset_vgacolortable6
db   0,   0,   0   ;   0
db 63,   0,   0   ;   1
db 63,   0,  9   ;   2
db 63,   0,  18   ;   3
db 63,   0,  27   ;   4
db 63,   0,  36  ;   5
db 63,   0,  45  ;   6
db 63,   0,  54   ;   7
db 63,   0,  63   ;   8
db 63,   0,  54   ;   9
db  63,  0,  45   ;  10
db  63,  0,  36   ;  11
db  63,  0,  27   ;  12
db  63,  0,  18  ;  13
db  63,  0,  9   ;  14
db  63,  0,  2   ;  15
db   0,   0,   0   ;  16 overscan


masktable db 32 dup (80h,40h,20h,10h,8,4,2,1)

.CODE

PUBLIC egavgapixel
PUBLIC drawcursor
PUBLIC erasecursor
PUBLIC cpu_type
PUBLIC mouse
PUBLIC setmode
PUBLIC getmode
;PUBLIC oldchangepalette
;PUBLIC oldusepalette
;PUBLIC oldspinpalette
PUBLIC sixteenbitsa
PUBLIC grayscale
PUBLIC mandel32
PUBLIC in1man32
PUBLIC in2man32
PUBLIC in1man16
PUBLIC julia32
PUBLIC cubicmandel32
PUBLIC in1cubicmandel32
PUBLIC in2cubicmandel32
PUBLIC in1cubmand16
PUBLIC cubicjulia32
PUBLIC mandel16
PUBLIC julia16
PUBLIC cubicmandel16
PUBLIC cubicjulia16
;PUBLIC oldrevspinpalette
;PUBLIC fimand32
;--------------------------------Procedures--------------

erasecursor PROC far
	mov	ax,0A000h
	mov	es,ax	;otiosely set es because i'm getting paranoid
	cmp	word ptr cursoron,1	;don't erase if its not there
	je	egaerasecursor
	jmp	doneerasecursor
egaerasecursor:
	push	di
	push	si
	mov	word ptr cursoron,0 ;in case you try and erase it twice
	mov	di,cursoroffset	;this has location of OLD cursor.
	xor	si,si
	mov	dx,03CEh
	mov	al,8
	mov	ah,0FFh
	out	dx,ax		;egamask for all eight bits
	mov	ax,0001h		;don't want to set/reset any colors
	out	dx,ax
	mov	dx,03C4h		;sequencer port
	mov	al,2
	out	dx,al		;colorplane write enable register
	inc	dx
	REPT 8
	mov	al,1			;choose to write plane 0
	out	dx,al
	mov	al,cursorpatch[si]
	mov	es:[di],al
	inc	si
	mov	al,2
	out	dx,al
	mov	al,cursorpatch[si]
	mov	es:[di],al
	inc	si
	mov	al,4
	out	dx,al
	mov	al,cursorpatch[si]
	mov	es:[di],al
	inc	si
	mov	al,8
	out	dx,al
	mov	al,cursorpatch[si]
	mov	es:[di],al
	inc	si
	add	di,80
	ENDM
	mov	al,0Fh
	out	dx,al		;put plane select back to default
	pop	si
	pop	di
	doneerasecursor:
	ret
erasecursor ENDP

egavgapixel PROC far,i:word,j:word,color:byte
        cli
	push	di			;ok, let's do the pixel
	mov	ax,0A000h
	mov	es,ax	;set Vseg
	mov	dx,03CEh		;point to address register
	mov	ax,0F01h		;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The F in ah says reset all 4 bit planes
	mov	ax,j		;get j in ax
	shl	ax,1			;80*j same as (64+16)* j
	shl	ax,1
	shl	ax,1
	shl	ax,1
	mov	di,ax		;16*j
	shl	ax,1
	shl	ax,1
	add	di,ax		;16*j + 64*j
	mov	cx,i			; get i in cx
	mov	al,cl
	mov	bx, OFFSET masktable
	xlat					;get bitmask in al
	shr	cx,1				
	shr	cx,1
	shr	cx,1			;get [i/8]
	add	di,cx		        ;target byte 80*j + [i/8]
;	cmp	di,cursoroffset	;8 pixel wide byte-aligned top ofcursor
;	jne	cursoroutofway
;	push	ax			;save the bitmask which is in al
;	call    erasecursor		;nuke the cursor!
;	mov	dx,03CEh		;point to address register
;	mov	ax,0F01h		;1 in al selects SET/RESET ENABLE register
;	out	dx,ax		;The F in ah says reset all 4 bit planes
;	pop	ax			;retrieve the bitmask
cursoroutofway:
	mov	ah,8			;8 selects bitmask register
	xchg	al,ah		;put them in right order
	out	dx,ax		;dx still 03CEh.  load the bitmask
	xor	al,al		;0 in al selects set/reset register
	mov	ah,color	;get byte of color for color
	out	dx,ax		; dx is still 03CEh.  
	or	es:[di],al	;latch and load
	mov	ax,0001h	;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The 0 in ah is default value
	mov	ax,0000h	;0 in al is set/reset register, 0 in ah is
	out	dx,ax		;default color to hold.  Restore.
	mov	ax,0FF08h	;al 8 selects bitmask.  FF in ah is default.
	out	dx,ax		;restore bitmask to default.
	pop	di
doneegavgapixel:
        sti
	ret
egavgapixel ENDP





        public  PixelAddr10
        ; This is lifted from Wilton.
        ; caller:       AX=y
        ;               BX=x
        ; returns:      AH=bitmask
        ;               BX=byte offset
        ;               CL=left shift count
        ;               ES=video buffer seg
        ;
PixelAddr10     proc    far
        mov     cl,bl
        push    dx
        mov     dx,80   ; bytes per line
        mul     dx
        pop     dx

        shr     bx,1
        shr     bx,1
        shr     bx,1
        add     bx,ax
        
        mov     ax,0a000h
        mov     es,ax

        and     cl,7
        xor     cl,7
        mov     ah,1
        ret
PixelAddr10     endp

public  xorcolor

xorcolor PROC far uses di,i:word,j:word,color:byte
        cli
	mov	ax,0A000h
	mov	es,ax	;set Vseg
	mov	dx,03CEh		;point to address register
	mov	ax,0F01h		;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The F in ah says reset all 4 bit planes
	mov	ax,1803h		;3 in al selects DATA ROTATE/FUNCTION SELECT reg
	out	dx,ax		;The 18 in ah says xor color with the latches
	mov	ax,j		;get j in ax
	shl	ax,1			;80*j same as (64+16)* j
	shl	ax,1
	shl	ax,1
	shl	ax,1
	mov	di,ax		;16*j
	shl	ax,1
	shl	ax,1
	add	di,ax		;16*j + 64*j
	mov	bx,i
	mov	cx,bx
	and	cl,7			;get the pixel number in cl
	mov	ah,80h		;start with a mask at leftmost pixel #0
	shr	ah,cl		;shift the mask to pixel position
	mov	al,8			;8 selects the bitmask register
	out	dx,ax		;load the bitmask
	shr	bx,1
	shr	bx,1
	shr	bx,1			;get [i/8]
	add	di,bx		;target byte 80*j + [i/8]
	xor	al,al		;0 in al selects set/reset register
	mov	ah,color		;get byte of color for color
	out	dx,ax
	or	es:[di],al	;latch and load
	mov	ax,0003h		;3 in al selects DATA ROTATE/FUNCTION SELECT reg
	out	dx,ax		;The 00 in ah restores normal color write
	ret
xorcolor ENDP


public  egavgagetpixel
egavgagetpixel proc far uses si,px:word,py:word
        mov     ax,py
        mov     bx,px
        call    PixelAddr10
        mov     ch,ah
        shl     ch,cl
        mov     si,bx
        xor     bl,bl
        mov     dx,3ceh
        mov     ax,304h
@@loop:
        out     dx,ax
        mov     bh,es:[si]
        and     bh,ch
        neg     bh
        rol     bx,1
        dec     ah
        jge     @@loop

        mov     al,bl
        xor     ah,ah
        ret
egavgagetpixel  endp


;----------------------------------------------
sixteenbitsa PROC far
	mov	ax,shiftregister
	mov	bx,ax
	ror	bx,1		; at each slot bx holds L bit
	rol	dx,1		; at each slot dx holds R bit
	or	ax,dx	; NewC= L XOR (C OR R)
	xor	ax,bx
	xor	ax,count
	mov	shiftregister,ax
	inc	word ptr count
	ret
sixteenbitsa ENDP
;------------------------------------------------------
;-----------------------------------------
grayscale proc far
	push	cx
	push di
	push dx
	mov	ax,grayflag
	xor	al,1
	mov	ah,12h
	mov	bl,33h
	int	10h
	pop	dx
	pop	di
	pop	cx
	ret
grayscale endp
;------------------------------------------------
;setmode(int n) attempts set to graphmode n, returns nothing.
setmode proc far,mymode:word
	mov	ax,mymode
	xor	ah,ah
	int	10h
	ret
setmode endp
;-----------------------------------------------------------
;getmode() gets the current display mode and returns it to ax
getmode proc far
	mov	ax,0f00h
	int 10h
	xor	ah,ah		;this would have # of char columns
	ret
getmode endp
;------------------------------------
;this reads 4 passsed variables into ax,bx,cx,ds and calls a microsoft ;mouse function. return values are in _AX,_BX,_CX,_DX.
mouse proc far,_ax:word,_bx:word,_cx:word,_dx:word
	mov ax,_ax
	mov bx,_bx
	mov cx,_cx
	mov dx,_dx
	int 33h
	ret
mouse endp
;----------------------------------------------
;***************************************************
;  CPU_TYPE - return type of processor: 8086/8088, 80286, or 80386
;       This routine relies on Intel-approved code that takes advantage
;       of the documented behavior of the high nibble of the flag word
;       in the REAL MODE of the various processors.  The MSB (bit 15)
;       is always a one on the 8086 and 8088 and a zero on the 286 and
;       386.  Bit 14 (NT flag) and bits 13/12 (IOPL bit field) are
;       always zero on the 286, but can be set on the 386.
;
;       CALLABLE FROM REAL MODE ONLY
;
;   ENTRY:      (none)
;   EXIT:       AX = processor type:    0086h if 8086 or 8088
;                                       0286h if 80286
;                                       0386h if 80386
;   USED:       AX
;   STACK:      6 bytes
;------------------------------------------------------------
cpu_type        proc far
	pushf
	xor	ax,ax			; 0000 into AX
	push	ax
	popf					; try to put that in the flags
	pushf
	pop	ax				; look at what really went into flags
	and	ax,0F000h			; mask off bit 15
	cmp	ax,0F000h			; Q: was high bit on?
	je	is_86			; Y: It's an 8086 (or 8088)
	mov	ax,0F000h			; N: try to set bit 14
	push	ax
	popf
	pushf
	pop	ax				; look at actual flags
	and	ax,0F000h			; Is bit 14 set?
	jz	is_286			; N: It's an 80286
	is_386:
		mov	ax,386		; Y: 80386
		jmp	short ct_exit
	is_286:
		mov	ax,286		; 80286
		jmp	short ct_exit
	is_86:
		mov	ax,86h		; 8086 or 8088
	ct_exit:
		mov	chip,1		;assume a shitty chip
		cmp	ax,386		;DO I have a 386?
		jne	donecpu		;no, exit
		mov	chip,0		;yes, put a 0 in chip for 386
	donecpu:
	popf
	ret
cpu_type endp
;----------------------------------------------
drawcursor PROC far
	mov	ax,0A000h		;set Vseg just in case
	mov	es,ax
	call    erasecursor	;this erases old cursor if there is one
	push	di
	push	si
	mov	word ptr cursoron,1
	mov	ax,cury		;calculate new cursoroffset
	mov	bx,80
	mul	bx
	mov	di,curx
	shr	di,1
	shr	di,1
	shr	di,1
	add	di,ax
	mov	cursoroffset,di	;new cursoroffset 80*cury + curx/8.
	mov	dx,03CEh
	xor	si,si
	mov	al,4			;for readplane select  register
	out	dx,al
	inc	dx			;bump dx for changing contents of readplane select
	REPT 8
		mov	al,0			;select plane 0
		out	dx,al
		mov	al,es:[di]	;get eight bits
		mov	BYTE PTR cursorpatch[si],al
		inc	si
		mov	al,1			;select plane 1
		out	dx,al
		mov	al,es:[di]
		mov	BYTE PTR cursorpatch[si],al
		inc	si
		mov	al,2			;select plane 2
		out	dx,al
		mov	al,es:[di]
		mov	BYTE PTR cursorpatch[si],al
		inc	si
		mov	al,3			;plane 3
		out	dx,al
		mov	al,es:[di]
		mov	BYTE PTR cursorpatch[si],al
		add	di,80		; 640/8 is bytes per line
		inc	si
	ENDM
	mov	al,0
	out	dx,al		;restore readplane select to 0 default
	sub	di,8*80
	dec	dx			;dx back to 03CEh
	mov	ax,00F01h		;01 for set/reset enable, F for all planes
	out	dx,ax
	mov	ax,0000h		;ready to load cursor background color 0
	out	dx,ax		;put color in set/reset register
	REPT 8
		or	es:[di],al	;black horizontal line
		add	di,80
	ENDM				;cursor is now a black square
	sub	di,8*80		;back to row one
	mov	ax,0100h		;ready to load (00) color 01
	out	dx,ax		;in set/reset
	cmp	cursorshape,0	; is cursorshape zero?
	jz	cursor0
	jmp	cursor1
	cursor0:			;shape of a hollow square
	or	es:[di],al
	add	di,80
	or	es:[di],al
	add	di,80
	mov	ax,0C308h		;08 for bitmask, C3h = 1100 0011b
	out	dx,ax		;means set 2 pixels at either end
	or	es:[di],al
	add	di,80
	or	es:[di],al
	add	di,80
	or	es:[di],al
	add	di,80
	or	es:[di],al
	add	di,80
	mov	ax,0FF08h
	out	dx,ax		;restore bitmask to default
	or	es:[di],al
	add	di,80
	or	es:[di],al
	jmp	closeerasecursor
	cursor1:			; shape of half a diagonally cut square 
	or	es:[di],al
	mov	ax,0FF08h		;08 for bitmask
	REPT 7
		add	di,80
		shl	ah,1		;keep moving one bit to left
		out	dx,ax
		or	es:[di],al
	ENDM
	mov	ah,0FFh
	out	dx,ax		;restore bitmask to default
	closeerasecursor:
	mov	ax,0001h		;01 for set/reset enable, 00 default no planes
	out	dx,ax
	mov	ax,0000h		;00 for set/reset register default color 00
	out	dx,ax
	pop	si		
	pop	di
	ret
drawcursor ENDP
;--------------------------------------------------

;--------------------------------------------------
;mandel32 uses 32 bit arithmetic to calculate the iteration count at
;global screen address pixelx,pixely.  We turn these into 32 bit integers
;using our deltax and deltay integers per pixel step value.
; The answer is returned to ax, ;with the understanding that if an
; iteration goes to maximum we return 0

; I've changed this to use DWORD PTR xx and DWORD PTR yy rather than dick with ebp
; and esp; screwing with them messed up debugging pretty badly,
; and the savings isn't really worth it.


        .code

mandel32 PROC far uses di si bp
	.386
	mov	edi,DWORD PTR x	;accumulate tx in edi
	mov	esi,DWORD PTR y	;accumulate ty in esi
        mov     DWORD PTR xx,edi
        mov     DWORD PTR yy,esi
	mov	cx,WORD PTR maxiteration
mstep:
	mov	eax,esi		;calculate y2=product32(ty,ty);
	imul	esi
	shrd	eax,edx,shift
	shr	edx,shift-1
	cmp	edx,0		;were bits 32 or 33 set?
	jne	mbig	

	mov	ebx,eax		;save y2 in ebx
	mov	eax,edi		;calculate x2=product32(tx,tx)
	imul	edi
	shrd	eax,edx,shift
	shr	edx,shift-1
	cmp	edx,0		;bits 32 or 33 set?
	jne	mbig

	mov	edx,eax		;save x2 in edx
	add	eax,ebx		;eax=x2+y2
	js	mbig			;too big if flow into bit 32
	jo	mbig			;too big if flow into bit 33

	mov	eax,edi		;save old tx in eax
	sub	edx,ebx		;subtract y2 from x2
        add     edx,DWORD PTR xx
	mov	edi,edx		;tx=x2-y2+x
	imul	esi			;old tx still in eax, esi is old ty
	shrd	eax,edx,shift-1
        add     eax,DWORD PTR yy
	mov	esi,eax		;ty= 2*product32(tx,ty)+y

        ; this is where the loop checking goes

        loop    mstep



        xor	ax,ax
        jmp	minside
mbig:
	mov	ax,WORD PTR maxiteration
	sub	ax,cx
	inc	ax
minside:
        .8086
        ret
mandel32 ENDP
;--------------------------------------------------

;in1man32 uses Peitgen's method of shading the interior

in1man32 PROC far uses di si bp
	.386
	mov	edi,DWORD PTR x	;accumulate tx in edi
	mov	esi,DWORD PTR y	;accumulate ty in esi
	mov	DWORD PTR xx,edi			;keep x in DWORD PTR xx
	mov	DWORD PTR yy,esi			;keep y in DWORD PTR yy
	mov	cx,WORD PTR maxiteration
	mov	DWORD PTR smallestval, 70000000h		;large value
	mov	smallestindex, 0
inmstep:
	mov	eax,esi		;calculate y2=product32(ty,ty);
	imul	esi
	shrd	eax,edx,shift
	shr	edx,shift-1
	cmp	edx,0		;were bits 32 or 33 set?
	jne	inmbig	
	mov	ebx,eax		;save y2 in ebx
	mov	eax,edi		;calculate x2=product32(tx,tx)
	imul	edi
	shrd	eax,edx,shift
	shr	edx,shift-1
	cmp	edx,0		;bits 32 or 33 set?
	jne	inmbig
	mov	edx,eax		;save x2 in edx
	add	eax,ebx		;eax=x2+y2
	js	inmbig			;too big if flow into bit 32
	jo	inmbig			;too big if flow into bit 33
	cmp	eax, smallestval
	ja	notsmallest
	mov	DWORD PTR smallestval, eax
	mov	smallestindex, cx
notsmallest:
        mov	eax,edi		;save old tx in eax
	sub	edx,ebx		;subtract y2 from x2
	add	edx,DWORD PTR xx
	mov	edi,edx		;tx=x2-y2+x
	imul	esi			;old tx still in eax, esi is old ty
	shrd	eax,edx,shift-1
	add	eax,DWORD PTR yy
	mov	esi,eax		;ty= 2*product32(tx,ty)+y
	loop inmstep
	jmp	inminside
inmbig:
	mov	ax,WORD PTR maxiteration
	sub	ax,cx
	inc	ax
.8086
        jmp     in1man32done
inminside:
	mov ax, WORD PTR maxiteration
	sub ax, smallestindex
in1man32done:
        ret
in1man32 ENDP
;--------------------------------------------------
;in2man32 uses pickover's method of shading interior
; again not using sp or bp anymore

in2man32 PROC far uses di si bp
	.386
	mov	edi,DWORD PTR x	;accumulate tx in edi
	mov	esi,DWORD PTR y	;accumulate ty in esi
	mov	DWORD PTR xx,edi			;keep x in DWORD PTR xx
	mov	DWORD PTR yy,esi			;keep y in DWORD PTR yy
	mov	cx,WORD PTR maxiteration
	mov	DWORD PTR smallestval,70000000h  ; large value
	mov	smallestindex, 0
	in2mstep:
        mov	eax,esi		;calculate y2=product32(ty,ty);
        imul	esi
        shrd	eax,edx,shift
        shr	edx,shift-1
        cmp	edx,0		;were bits 32 or 33 set?
        jne	in2mbig	
        mov	ebx,eax		;save y2 in ebx
        mov	eax,edi		;calculate x2=product32(tx,tx)
        imul	edi
        shrd	eax,edx,shift
        shr	edx,shift-1
        cmp	edx,0		;bits 32 or 33 set?
        jne	in2mbig
        mov	edx,eax		;save x2 in edx
        add	eax,ebx		;eax=x2+y2
        js	in2mbig			;too big if flow into bit 32
        jo	in2mbig			;too big if flow into bit 33
        cmp	eax, smallestval
        ja	notsmallest2	
        mov	DWORD PTR smallestval, eax
        mov	smallestindex, cx
notsmallest2:
        mov	eax,edi		;save old tx in eax
        sub	edx,ebx		;subtract y2 from x2
        add	edx,DWORD PTR xx
        mov	edi,edx		;tx=x2-y2+x
        imul	esi			;old tx still in eax, esi is old ty
        shrd	eax,edx,shift-1
        add	eax,DWORD PTR yy
        mov	esi,eax		;ty= 2*product32(tx,ty)+y
        loop in2mstep
        jmp	in2minside
in2mbig:
	mov	ax,WORD PTR maxiteration
	sub	ax,cx
	inc	ax
        jmp     short in2man32done
        .386
in2minside:
	mov eax, smallestval
	shr eax, 16
donein2minside:
in2man32done:
.8086
        ret
in2man32 ENDP
;--------------------------------------------------
;in3man32 uses pickover's method of shading interior

in3man32 PROC far uses di si bp
	.386
	mov	edi,DWORD PTR x	;accumulate tx in edi
	mov	esi,DWORD PTR y	;accumulate ty in esi
	mov	DWORD PTR xx,edi			;keep x in DWORD PTR xx
	mov	DWORD PTR yy,esi			;keep y in DWORD PTR yy
	mov	cx,WORD PTR maxiteration
	mov	DWORD PTR smallestval,70000000h  ; large value
	mov	smallestindex, 0
in3mstep:
	mov	eax,esi		;calculate y2=product32(ty,ty);
	imul	esi
	shrd	eax,edx,shift
	shr	edx,shift-1
	cmp	edx,0		;were bits 32 or 33 set?
	jne	in3mbig	
	mov	ebx,eax		;save y2 in ebx
	mov	eax,edi		;calculate x2=product32(tx,tx)
	imul	edi
	shrd	eax,edx,shift
	shr	edx,shift-1
	cmp	edx,0		;bits 32 or 33 set?
	jne	in3mbig
	mov	edx,eax		;save x2 in edx
	add	eax,ebx		;eax=x2+y2
	js	in3mbig			;too big if flow into bit 32
	jo	in3mbig			;too big if flow into bit 33
	cmp	eax, smallestval
	ja	notsmallest3	
	mov	DWORD PTR smallestval, eax
	mov	smallestindex, cx
	notsmallest3:
	mov	eax,edi		;save old tx in eax
	sub	edx,ebx		;subtract y2 from x2
	add	edx,DWORD PTR xx
	mov	edi,edx		;tx=x2-y2+x
	imul	esi			;old tx still in eax, esi is old ty
	shrd	eax,edx,shift-1
	add	eax,DWORD PTR yy
	mov	esi,eax		;ty= 2*product32(tx,ty)+y
	loop in3mstep
	jmp	in3minside
in3mbig:
	mov	ax,WORD PTR maxiteration
	sub	ax,cx
	inc	ax
        jmp     doni3
in3minside:
	mov ebx, smallestval   ; has form (real)k * 2**28
	shr ebx,12		   ; make bx be 2**16 * k.
	mov	ax,1				; this makes weird moire
	test bx, 32
	jz donein3minside
	xor ax,ax
donein3minside:
doni3:
.8086
        ret
in3man32 ENDP
;--------------------------------------------------
;julia32 computes the julia set associated with the point
;where the cursor is, fed as (bcurx,bcury).

julia32 PROC far uses di si bp
.386
	mov	edi,DWORD PTR x	;accumulate tx in edi
	mov	esi,DWORD PTR y	;accumulate ty in esi
	mov	eax,DWORD PTR u	;keep bcurx in DWORD PTR xx
        mov     DWORD PTR xx,eax
	mov	eax,DWORD PTR v	;keep bcury in DWORD PTR yy
        mov     DWORD PTR yy,eax
	mov	cx,WORD PTR maxiteration
jstep:
	mov	eax,esi		;calculate y2=product32(ty,ty);
	imul	esi
	shrd	eax,edx,shift
	shr	edx,shift-1
	cmp	edx,0		;were bits 32 or 33 set?
	jne	jbig	
	mov	ebx,eax		;save y2 in ebx
	mov	eax,edi		;calculate x2=product32(tx,tx)
	imul	edi
	shrd	eax,edx,shift
	shr	edx,shift-1
	cmp	edx,0		;bits 32 or 33 set?
	jne	jbig
	mov	edx,eax		;save x2 in edx
	add	eax,ebx		;eax=x2+y2
	js	jbig			;too big if flow into bit 32
	jo	jbig			;too big if flow into bit 33
	mov	eax,edi		;save old tx in eax
	sub	edx,ebx		;subtract y2 from x2
	add	edx,DWORD PTR xx
	mov	edi,edx		;tx=x2-y2+x
	imul	esi			;old tx still in eax, esi is old ty
	shrd	eax,edx,shift-1
	add	eax,DWORD PTR yy
	mov	esi,eax		;ty= 2*product32(tx,ty)+y
	loop jstep
	xor	ax,ax
	jmp	jinside
jbig:
	mov	ax,WORD PTR maxiteration
	inc ax
	sub	ax,cx
jinside:
.8086
        ret
julia32 ENDP
;--------------------------------------------------
;The square map is set c = (x,y) start at (0,0), and watch z -> z^2 + c.
;	Is called the Mandelbrot set, but is more generally known as the
;	quadratic connectedness map in analogy to the degree three
;	CUBIC CONNECTEDNESS MAP, which is calculated as follows:
;	Start at plus and minus A = (a,b) and watch
;	z -> z^3 -3*A^2*z + c.  If I write (tx,ty) for z, and set
;	a3 = -3 * (a*a - b*b) and b3 = -3 * (2*a*b), this is
;	(tx,ty) -> ( (tx,ty)*(tx,ty) + (a3,b3) ) * (tx,ty) + (x,y), or,
;	if x2 = tx*tx and y2 = ty*ty, is same as
;	(tx,ty) ->  ( x2 - y2 + a3, 2*x*y + b3) * (tx,ty) + (x,y), or, if
;	xa = x2 - y2 + a3, and  yb = 2*x*y - b3, is same as
;	(tx,ty) -> ( xa * tx - xb * ty, xa * ty + xb * tx ) + (x,y), or
;	tx = xa * tx - xb * ty + x; ty = xa * ty + xb * tx + y
;	In code, we
;	initialize:
;		tx = a;
;		ty = b;
;		a3 = -3 * ( a * a - b * b );
;		b3 = -3 * ( 2 * a * b );
;	and then iterate:
;		x2 = tx * tx;
;		y2 = ty * ty;
;		xa = x2 - y2 + a3;
;		yb = 2*x*y + b3;
;		temp = xa * tx - yb * ty + x;
;		ty = xa * ty + yb * tx + y;
;		tx = temp;
;	and then reinitialize
;		tx = -a;
;		ty = -b;
;	and iterate again,
;		... ;
;	with each iteration returning the iteration number at which 
;	x2 + y2 > 7, or returning 0 if x2 + y2 never escapes.  The 
;	value assigned to x,y is 0 if both iterations yield 0, it is
;	nonzero iteration count if exactly one iteration yeilds 0, and
;	it is the smaller of the two iteration counts if both are nonzero.
;	While the escape value for mandelbrot must be greater than 2, the
;	escape in the cubic connectedness map must only be greater than
;	square root of two.  For the mandelbrot I take my escape value to
;	be square root of seven (i compare square of distance to seven,
;	rather than four, as naive programmers incorrectly do), but here
;	I CAN take escape value two, and compare square to four.

cubicmandel32 PROC far uses di si bp		;cubic black mandelbrot with terrace coloring
.386
	mov	edi,DWORD PTR a	;accumulate tx in edi
	mov	esi,DWORD PTR b	;accumulate ty in esi
starttake:
	mov	cx,WORD PTR maxiteration
;x2=product32(tx,tx)
	mov	eax,edi
	imul	edi
	shrd	eax,edx,cubicshift	;here tx = a < 4, so eax is correct tx*tx
	mov	DWORD PTR xx,eax		;keep x2 in DWORD PTR xx
;y2=product32(ty,ty)
	mov	eax,esi
	imul	esi
	shrd	eax,edx,cubicshift	;eax is correct ty*ty if ty < 4.
	mov	DWORD PTR yy,eax		;keep y2 in DWORD PTR yy
	add	eax,DWORD PTR xx		;check x2 + y2
	jc	toobig		;if carry its over 16
	test	eax,CUBICCUT	;and also too big if hits left mask
	jnz	toobig
	jmp	cstep
toobig:
	jmp	cbig
cstep:
	;xa = x2 - y2 + a3;
	;yb = 2*x*y + b3;
	;temp = xa * tx - yb * ty + x;
	mov	eax,DWORD PTR xx			;eax now holds x2
	sub	eax,DWORD PTR yy			;eax now holds x2 - y2
	jo	cbig
	add	eax,DWORD PTR a3	;eax now holds xa
	jo	cbig
	mov	DWORD PTR xa, eax
	imul	edi
	shrd	eax,edx,cubicshift		;eax now hold xa * tx
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or
	jnz	cbig				;(if neg) all 1s, and dh = dl.
	mov	ebx,eax			;use ebx as temp register
	mov	eax,edi
	imul	esi
	shrd	eax,edx,cubicshift
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or
	jnz	cbig				;(if neg) all 1s, and dh = dl.
	add	eax,eax
	jo	cbig
	add	eax,DWORD PTR b3	;eax now holds yb
	jo	cbig
	mov	DWORD PTR yb, eax
	imul	esi
	shrd	eax,edx,cubicshift		;eax now holds yb * ty
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or 
	jnz	cbig				;(if neg) all 1s, and dh = dl.
	sub	ebx,eax			;ebx now holds xa*tx - yb*ty
	jo	cbig
	add	ebx,DWORD PTR x	;ebx is now new tx value
	jo	cbig
	mov	DWORD PTR temp, ebx ;save it in temp
;ty = ty * xa + tx * yb + y
;tx = temp
	mov	eax,esi
	imul	DWORD PTR xa
	shrd	eax,edx,cubicshift		;eax holds ty * xa
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or 
	jnz	cbig				;(if neg) all 1s, and dh = dl.
	mov	ebx,eax			;save in ebx
	mov	eax,edi
	imul	DWORD PTR yb
	shrd	eax,edx,cubicshift		;eax now holds tx * yb
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, dx should be all 0s or 
	jnz	cbig				;(if neg) all 1s, and dh = dl.
	add	eax,ebx			;eax now holds ty*xa + tx*yb
	jo	cbig
	add	eax,DWORD PTR y	;eax now holds new ty
	jo	cbig
	mov	esi,eax			;save ty in esi
	mov	edi,DWORD PTR temp	;save tx in edi
;y2=product32(ty,ty)
	imul	esi			;eax already holds esi, or ty
	shrd	eax,edx,cubicshift
	shr	edx,cubicshift		;edx:eax is 64 bit product
	or	edx,edx		;too big if over 16
	jnz	cbig
	mov	DWORD PTR yy,eax		;store y2 in DWORD PTR yy
;x2=product32(tx,tx)
	mov	eax,edi
	imul	edi
	shrd	eax,edx,cubicshift
	shr	edx,cubicshift		;edx:eax is 64 bit product
	or	edx,edx		;too big if over 16
	jnz	cbig
	mov	DWORD PTR xx,eax		;store x2 in DWORD PTR xx
;check size of x2 + y2
	add	eax,DWORD PTR yy		;eax is x2 + y2
	jc	cbig			;if carry, is over 16
	test	eax,CUBICCUT	;too big if hits bits 8 or 4
	jnz	cbig
	dec	cx
	jnz	cstep
cbig:
	xor	cubictake,1			;this is 1 after first run, 0 after 2nd
	jz	donecubictakes
	mov	poscolor,cx			;save cx from positive a,b run
	mov	edi,DWORD PTR nega		;get ready for negative run
	mov	esi,DWORD PTR negb
	jmp	starttake
donecubictakes:			;done both runs now
	cmp	cx,poscolor			;get the worst (highest) cx score
	ja	fixcolor				;where the guy exploded fastest
	mov	cx,poscolor			; and only set ax to 0 if 
	xor	ax,ax				; if cx runs all the way to 0
	test	cx,cx
	jz	colorready
fixcolor:
	mov	ax,maxiteration		;otherwise color is bigger than 0
	inc	ax
	sub	ax,cx
colorready:
.8086
        ret
cubicmandel32 ENDP
;-------------------------------------------------------
in1cubicmandel32 PROC far uses di si bp
.386
	mov	edi,DWORD PTR a	;accumulate tx in edi
	mov	esi,DWORD PTR b	;accumulate ty in esi
	mov	DWORD PTR smallestval, 70000000h		;large value
	mov	smallestindex, 0
starttakein1:
	mov	cx,WORD PTR maxiteration
;x2=product32(tx,tx)
	mov	eax,edi
	imul	edi
	shrd	eax,edx,cubicshift	;here tx = a < 4, so eax is correct tx*tx
	mov	DWORD PTR xx,eax		;keep x2 in DWORD PTR xx
;y2=product32(ty,ty)
	mov	eax,esi
	imul	esi
	shrd	eax,edx,cubicshift	;eax is correct ty*ty if ty < 4.
	mov	DWORD PTR yy,eax		;keep y2 in DWORD PTR yy
	add	eax,DWORD PTR xx		;check x2 + y2
	jc	toobigin1		;if carry its over 16
	test	eax,CUBICCUT	;and also too big if hits left mask
	jnz	toobigin1
	jmp	cstepin1
toobigin1:
        jmp	cbigin1
cstepin1:
	;xa = x2 - y2 + a3;
	;yb = 2*x*y + b3;
	;temp = xa * tx - yb * ty + x;
	mov	eax,DWORD PTR xx			;eax now holds x2
	sub	eax,DWORD PTR yy			;eax now holds x2 - y2
	jo	cbigin1
	add	eax,DWORD PTR a3	;eax now holds xa
	jo	cbigin1
	mov	DWORD PTR xa, eax
	imul	edi
	shrd	eax,edx,cubicshift		;eax now hold xa * tx
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or
	jnz	cbigin1				;(if neg) all 1s, and dh = dl.
	mov	ebx,eax			;use ebx as temp register
	mov	eax,edi
	imul	esi
	shrd	eax,edx,cubicshift
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or
	jnz	cbigin1				;(if neg) all 1s, and dh = dl.
	add	eax,eax
	jo	cbigin1
	add	eax,DWORD PTR b3	;eax now holds yb
	jo	cbigin1
	mov	DWORD PTR yb, eax
	imul	esi
	shrd	eax,edx,cubicshift		;eax now holds yb * ty
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or 
	jnz	cbigin1				;(if neg) all 1s, and dh = dl.
	sub	ebx,eax			;ebx now holds xa*tx - yb*ty
	jo	cbigin1
	add	ebx,DWORD PTR x	;ebx is now new tx value
	jo	cbigin1
	mov	DWORD PTR temp, ebx ;save it in temp
;ty = ty * xa + tx * yb + y
;tx = temp
	mov	eax,esi
	imul	DWORD PTR xa
	shrd	eax,edx,cubicshift		;eax holds ty * xa
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or 
	jnz	cbigin1				;(if neg) all 1s, and dh = dl.
	mov	ebx,eax			;save in ebx
	mov	eax,edi
	imul	DWORD PTR yb
	shrd	eax,edx,cubicshift		;eax now holds tx * yb
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, dx should be all 0s or 
	jnz	cbigin1				;(if neg) all 1s, and dh = dl.
	add	eax,ebx			;eax now holds ty*xa + tx*yb
	jo	cbigin1
	add	eax,DWORD PTR y	;eax now holds new ty
	jo	cbigin1
	mov	esi,eax			;save ty in esi
	mov	edi,DWORD PTR temp	;save tx in edi
;y2=product32(ty,ty)
	imul	esi			;eax already holds esi, or ty
	shrd	eax,edx,cubicshift
	shr	edx,cubicshift		;edx:eax is 64 bit product
	or	edx,edx		;too big if over 16
	jnz	cbigin1
	mov	DWORD PTR yy,eax		;store y2 in DWORD PTR yy
;x2=product32(tx,tx)
	mov	eax,edi
	imul	edi
	shrd	eax,edx,cubicshift
	shr	edx,cubicshift		;edx:eax is 64 bit product
	or	edx,edx		;too big if over 16
	jnz	cbigin1
	mov	DWORD PTR xx,eax		;store x2 in DWORD PTR xx
;check size of x2 + y2
	add	eax,DWORD PTR yy		;eax is x2 + y2
	jc	cbigin1			;if carry, is over 16
	test	eax,CUBICCUT	;too big if hits bits 8 or 4
	jnz	cbigin1
	cmp	eax, DWORD PTR smallestval
	ja	notsmallestin1
	mov	DWORD PTR smallestval, eax
	mov	smallestindex, cx
	notsmallestin1:
	dec	cx
	jnz	cstepin1
cbigin1:
	xor	cubictake,1			;this is 1 after first run, 0 after 2nd
	jz	donecubictakesin1
	mov	poscolor,cx			;save cx from positive a,b run
	mov	edi,DWORD PTR nega		;get ready for negative run
	mov	esi,DWORD PTR negb
	jmp	starttakein1
donecubictakesin1:			;done both runs now
	cmp	cx,poscolor			;get the worst (highest) cx score
	ja	fixcolorin1				;where the guy exploded fastest
	mov	cx,poscolor			; do special color if inside
	test	cx,cx
	jz	insidein1
fixcolorin1:
	mov	ax,maxiteration		;otherwise color is bigger than 0
	inc	ax
	sub	ax,cx
	jmp	colorreadyin1
insidein1:
	mov	ax, maxiteration
	sub	ax, smallestindex

colorreadyin1:
.8086
        ret
in1cubicmandel32 ENDP
;-----------------------------------------------------
in2cubicmandel32 PROC far uses di si bp
.386
	mov	edi,DWORD PTR a	;accumulate tx in edi
	mov	esi,DWORD PTR b	;accumulate ty in esi
	mov	DWORD PTR smallestval, 70000000h		;large value
	mov	smallestindex, 0
starttakein2:
	mov	cx,WORD PTR maxiteration
;x2=product32(tx,tx)
	mov	eax,edi
	imul	edi
	shrd	eax,edx,cubicshift	;here tx = a < 4, so eax is correct tx*tx
	mov	DWORD PTR xx,eax		;keep x2 in DWORD PTR xx
;y2=product32(ty,ty)
	mov	eax,esi
	imul	esi
	shrd	eax,edx,cubicshift	;eax is correct ty*ty if ty < 4.
	mov	DWORD PTR yy,eax		;keep y2 in DWORD PTR yy
	add	eax,DWORD PTR xx		;check x2 + y2
	jc	toobigin2		;if carry its over 16
	test	eax,CUBICCUT	;and also too big if hits left mask
	jnz	toobigin2
	jmp	cstepin2
toobigin2:
	jmp	cbigin2
cstepin2:
	;xa = x2 - y2 + a3;
	;yb = 2*x*y + b3;
	;temp = xa * tx - yb * ty + x;
	mov	eax,DWORD PTR xx			;eax now holds x2
	sub	eax,DWORD PTR yy			;eax now holds x2 - y2
	jo	cbigin2
	add	eax,DWORD PTR a3	;eax now holds xa
	jo	cbigin2
	mov	DWORD PTR xa, eax
	imul	edi
	shrd	eax,edx,cubicshift		;eax now hold xa * tx
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or
	jnz	cbigin2				;(if neg) all 1s, and dh = dl.
	mov	ebx,eax			;use ebx as temp register
	mov	eax,edi
	imul	esi
	shrd	eax,edx,cubicshift
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or
	jnz	cbigin2				;(if neg) all 1s, and dh = dl.
	add	eax,eax
	jo	cbigin2
	add	eax,DWORD PTR b3	;eax now holds yb
	jo	cbigin2
	mov	DWORD PTR yb, eax
	imul	esi
	shrd	eax,edx,cubicshift		;eax now holds yb * ty
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or 
	jnz	cbigin2				;(if neg) all 1s, and dh = dl.
	sub	ebx,eax			;ebx now holds xa*tx - yb*ty
	jo	cbigin2
	add	ebx,DWORD PTR x	;ebx is now new tx value
	jo	cbigin2
	mov	DWORD PTR temp, ebx ;save it in temp
;ty = ty * xa + tx * yb + y
;tx = temp
	mov	eax,esi
	imul	DWORD PTR xa
	shrd	eax,edx,cubicshift		;eax holds ty * xa
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or 
	jnz	cbigin2				;(if neg) all 1s, and dh = dl.
	mov	ebx,eax			;save in ebx
	mov	eax,edi
	imul	DWORD PTR yb
	shrd	eax,edx,cubicshift		;eax now holds tx * yb
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, dx should be all 0s or 
	jnz	cbigin2				;(if neg) all 1s, and dh = dl.
	add	eax,ebx			;eax now holds ty*xa + tx*yb
	jo	cbigin2
	add	eax,DWORD PTR y	;eax now holds new ty
	jo	cbigin2
	mov	esi,eax			;save ty in esi
	mov	edi,DWORD PTR temp	;save tx in edi
;y2=product32(ty,ty)
	imul	esi			;eax already holds esi, or ty
	shrd	eax,edx,cubicshift
	shr	edx,cubicshift		;edx:eax is 64 bit product
	or	edx,edx		;too big if over 16
	jnz	cbigin2
	mov	DWORD PTR yy,eax		;store y2 in DWORD PTR yy
;x2=product32(tx,tx)
	mov	eax,edi
	imul	edi
	shrd	eax,edx,cubicshift
	shr	edx,cubicshift		;edx:eax is 64 bit product
	or	edx,edx		;too big if over 16
	jnz	cbigin2
	mov	DWORD PTR xx,eax		;store x2 in DWORD PTR xx
;check size of x2 + y2
	add	eax,DWORD PTR yy		;eax is x2 + y2
	jc	cbigin2			;if carry, is over 16
	test	eax,CUBICCUT	;too big if hits bits 8 or 4
	jnz	cbigin2
	cmp	eax, DWORD PTR smallestval
	ja	notsmallestin2
	mov	DWORD PTR smallestval, eax
	mov	smallestindex, cx
	notsmallestin2:
	dec	cx
	jnz	cstepin2
cbigin2:
	xor	cubictake,1			;this is 1 after first run, 0 after 2nd
	jz	donecubictakesin2
	mov	poscolor,cx			;save cx from positive a,b run
	mov	edi,DWORD PTR nega		;get ready for negative run
	mov	esi,DWORD PTR negb
	jmp	starttakein2
donecubictakesin2:			;done both runs now
	cmp	cx,poscolor			;get the worst (highest) cx score
	ja	fixcolorin2				;where the guy exploded fastest
	mov	cx,poscolor			; do special color if inside
	test	cx,cx
	jz	insidein2
fixcolorin2:
	mov	ax,maxiteration		;otherwise color is bigger than 0
	inc	ax
	sub	ax,cx
	jmp	colorreadyin2
insidein2:
	mov eax, smallestval
	shr eax,16
colorreadyin2:
.8086
        ret
in2cubicmandel32 ENDP
;-----------------------------------------------------
cubicjulia32 PROC far uses di si bp		;cubic julia
.386
	mov	edi,x	;accumulate tx in edi,accumulate ty in esi
	mov	esi,y	;start out at x,y
starttaker:
	mov	cx,WORD PTR maxiteration
;x2=product32(tx,tx)
	mov	eax,edi
	imul	edi
	shrd	eax,edx,cubicshift	;here tx = a < 4, so eax is correct tx*tx
	mov	DWORD PTR xx,eax		;keep x2 in DWORD PTR xx
;y2=product32(ty,ty)
	mov	eax,esi
	imul	esi
	shrd	eax,edx,cubicshift	;eax is correct ty*ty if ty < 4.
	mov	DWORD PTR yy,eax		;keep y2 in DWORD PTR yy
	add	eax,DWORD PTR xx		;check x2 + y2
	jc	toobigr	;if carry its over 16
	test	eax,CUBICCUT	;and also too big if hits left mask
	jnz	toobigr
	jmp	cstepr
	toobigr:
	jmp	cbigr
	cstepr:
	;xa = x2 - y2 + a3;
	;yb = 2*x*y + b3;
	;temp = xa * tx - yb * ty + x;
	mov	eax,DWORD PTR xx			;eax now holds x2
	sub	eax,DWORD PTR yy			;eax now holds x2 - y2
	jo	cbigr
	add	eax,DWORD PTR a3	;eax now holds xa
	jo	cbigr
	mov	DWORD PTR xa, eax
	imul	edi
	shrd	eax,edx,cubicshift		;eax now hold xa * tx
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or
	jnz	cbigr				;(if neg) all 1s, and dh = dl.
	mov	ebx,eax			;use ebx as temp register
	mov	eax,edi
	imul	esi
	shrd	eax,edx,cubicshift
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or
	jnz	cbigr				;(if neg) all 1s, and dh = dl.
	add	eax,eax
	jo	cbigr
	add	eax,DWORD PTR b3	;eax now holds yb
	jo	cbigr
	mov	DWORD PTR yb, eax
	imul	esi
	shrd	eax,edx,cubicshift		;eax now holds yb * ty
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or 
	jnz	cbigr				;(if neg) all 1s, and dh = dl.
	sub	ebx,eax			;ebx now holds xa*tx - yb*ty
	jo	cbigr
	add	ebx,DWORD PTR u	;ebx is now new tx value
	jo	cbigr
	mov	DWORD PTR temp, ebx ;save it in temp
;ty = ty * xa + tx * yb + y
;tx = temp
	mov	eax,esi
	imul	DWORD PTR xa
	shrd	eax,edx,cubicshift		;eax holds ty * xa
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, edx should be all 0s or 
	jnz	cbigr				;(if neg) all 1s, and dh = dl.
	mov	ebx,eax			;save in ebx
	mov	eax,edi
	imul	DWORD PTR yb
	shrd	eax,edx,cubicshift		;eax now holds tx * yb
	sar	edx,cubicshift		;edx:eax is the true product.  So its
	xor	dh,dl			;under 33 bits, dx should be all 0s or 
	jnz	cbigr				;(if neg) all 1s, and dh = dl.
	add	eax,ebx			;eax now holds ty*xa + tx*yb
	jo	cbigr
	add	eax,DWORD PTR v	;eax now holds new ty
	jo	cbigr
	mov	esi,eax			;save ty in esi
	mov	edi,DWORD PTR temp	;save tx in edi
;y2=product32(ty,ty)
	imul	esi			;eax already holds esi, or ty
	shrd	eax,edx,cubicshift
	shr	edx,cubicshift		;edx:eax is 64 bit product
	or	edx,edx		;too big if over 16
	jnz	cbigr
	mov	DWORD PTR yy,eax		;store y2 in DWORD PTR yy
;x2=product32(tx,tx)
	mov	eax,edi
	imul	edi
	shrd	eax,edx,cubicshift
	shr	edx,cubicshift		;edx:eax is 64 bit product
	or	edx,edx		;too big if over 16
	jnz	cbigr
	mov	DWORD PTR xx,eax		;store x2 in DWORD PTR xx
;check size of x2 + y2
	add	eax,DWORD PTR yy		;eax is x2 + y2
	jc	cbigr			;if carry, is over 16
	test	eax,CUBICCUT	;too big if hits bits 8 or 4
	jnz	cbigr
	dec	cx
	jnz	cstepr
cbigr:
;	xor	cubictake,1			;this is 1 after first run, 0 after 2nd
;	jz	donecubictakesr
;	mov	poscolor,cx			;save cx from positive a,b run
;	mov	edi,DWORD PTR nega		;get ready for negative run
;	mov	esi,DWORD PTR negb
;	jmp	starttaker
;	donecubictakesr:			;done both runs now
;	cmp	cx,poscolor			;get the worst (highest) cx score
;	ja	fixcolorr				;where the guy exploded fastest
;	mov	cx,poscolor			; and only set ax to 0 if 
	xor	ax,ax				; if cx runs all the way to 0
	test	cx,cx
	jz	colorreadyr
fixcolorr:
	mov	ax,maxiteration		;otherwise color is bigger than 0
	inc	ax
	sub	ax,cx
colorreadyr:
.8086
        ret
cubicjulia32 ENDP
;-------------------------------------------------------
;mandel16 uses 16 bit arithmetic to calculate the iteration count at
;global long integer addresses x and y.  The answer is returned to ax, 
;with the understanding that if an iteration goes to maximum we return 0

mandel16 PROC far uses di si bp
	mov	di,WORD PTR x		;accumulate tx in di
	mov	si,WORD PTR y		;accumulate ty in si
	mov	IXX,di			;keep x in IXX
	mov	IYY,si			;keep y in IYY
	mov	cx,WORD PTR maxiteration
imstep:
	mov	ax,si		;calculate y2=product16(ty,ty);
	imul	si
	shift12
	or	dx,dx
	jz	iok1
	jmp	imbig
iok1:
	or	ax,ax
	jns	iok2
	jmp	imbig
iok2:
	mov	bx,ax		;save y2 in ebx
	mov	ax,di		;calculate x2=product16(tx,tx)
	imul	di
	shift12
	or	dx,dx
	jnz	imbig
	or	ax,ax
	js	imbig
	mov	dx,ax		;save x2 in dx
	add	ax,bx		;ax=x2+y2
	js	imbig		;too big if flow into bit 16
	jc	imbig		;too big if flow into bit 17
	mov	ax,di		;save old tx in ax
	sub	dx,bx		;subtract y2 from x2
	add	dx,IXX
	mov	di,dx		;tx=x2-y2+x
	imul	si			;old tx still in ax, si is old ty
	shift11
	add	ax,IYY
	mov	si,ax		;ty= 2*product32(tx,ty)+y
	dec	cx
	jcxz iminside
	jmp imstep
imbig:
	mov	ax,WORD PTR maxiteration
	sub	ax,cx		;gives number between 0 and maxiter-1
	inc	ax			;only the interior gets 0 value
	jmp	imdone
iminside:
	xor ax,ax
imdone:
        ret
mandel16 ENDP

;-------------------------------------------------------
;mandel16 uses 16 bit arithmetic to calculate the iteration count at
;global long integer addresses x and y.  The answer is returned to bx,
;with the understanding that if an iteration goes to maximum we return 0

in1man16 PROC far uses di si bp
	mov	di,WORD PTR x		;accumulate tx in di
	mov	si,WORD PTR y		;accumulate ty in si
	mov	IXX,di			;keep x in IXX
	mov	IYY,si			;keep y in IYY
	mov	cx,WORD PTR maxiteration
	mov	WORD PTR smallestval, 7000h
	mov	smallestindex,0
imstepin1:
	mov	ax,si		;calculate y2=product16(ty,ty);
	imul	si
	shift12
	or	dx,dx
	jz	iok1in1
	jmp	imbigin1
	iok1in1:
	or	ax,ax
	jns	iok2in1
	jmp	imbigin1
	iok2in1:
	mov	bx,ax		;save y2 in ebx
	mov	ax,di		;calculate x2=product16(tx,tx)
	imul	di
	shift12
	or	dx,dx
	jnz	imbigin1
	or	ax,ax
	js	imbigin1
	mov	dx,ax		;save x2 in dx
	add	ax,bx		;ax=x2+y2
	js	imbigin1		;too big if flow into bit 16
	jc	imbigin1		;too big if flow into bit 17
	cmp	ax,WORD PTR smallestval
	ja	notsmallestm6in1
	mov	WORD PTR smallestval,ax
	mov	smallestindex,cx
	notsmallestm6in1:
	mov	ax,di		;save old tx in ax
	sub	dx,bx		;subtract y2 from x2
	add	dx,IXX
	mov	di,dx		;tx=x2-y2+x
	imul	si			;old tx still in ax, si is old ty
	shift11
	add	ax,IYY
	mov	si,ax		;ty= 2*product32(tx,ty)+y
	dec	cx
	jcxz iminsidein1
	jmp imstepin1
imbigin1:
	mov	ax,WORD PTR maxiteration
	sub	ax,cx		;gives number between 0 and maxiter-1
	inc	ax			;only the interior gets 0 value
	jmp	imdonein1
iminsidein1:
	mov	ax, WORD PTR smallestval
	cmp	insideflag,1
	je	imdonein1
	mov	ax,maxiteration
	sub	ax,smallestindex
imdonein1:
        ret
in1man16 ENDP
;--------------------------------------------------
;julia16 computes the julia set associated with the point
;where the cursor is, fed as (bcurx,bcury).

julia16 PROC far uses di si bp
	mov	di,WORD PTR x		;accumulate tx in di
	mov	si,WORD PTR y	;accumulate ty in si
	mov	ax,WORD PTR u		;keep bcurx in IXX
        mov     IXX,ax
	mov	ax,WORD PTR v		;keep bcury in eIYY
        mov     IYY,ax
	mov	cx,WORD PTR maxiteration
j6step:
	mov	ax,si		;calculate y2=product16(ty,ty);
	imul	si
	shift12
	or	dx,dx
	jz	j6ok1
	jmp	j6big
j6ok1:
	or	ax,ax
	jns	j6ok2
	jmp	j6big
j6ok2:
	mov	bx,ax
	mov	ax,di		;calculate x2=product16(tx,tx)
	imul	di
	shift12
	or	dx,dx
	jnz	j6big
	or	ax,ax
	js	j6big
	mov	dx,ax
	add	ax,bx
	js	j6big			;too big if flow into bit 16
	jc	j6big			;too big if flow into bit 17
	mov	ax,di		;save old tx in eax
	sub	dx,bx		;subtract y2 from x2
	add	dx,IXX
	mov	di,dx		;tx=x2-y2+x
	imul	si			;old tx still in eax, esi is old ty
	shift11
	add	ax,IYY
	mov	si,ax		;ty= 2*product16(tx,ty)+y
	dec	cx
	jcxz	j6donestep
	jmp	j6step
j6donestep:
        xor	ax,ax
        jmp	j6inside
j6big:
	mov	ax,WORD PTR maxiteration
	inc ax
	sub	ax,cx
j6inside:
        ret
julia16 ENDP
;--------------------------------------------------
cubicmandel16 PROC far uses di si bp		;cubic black mandelbrot with terrace coloring
	mov	di,WORD PTR a	;accumulate tx in di
	mov	si,WORD PTR b	;accumulate ty in si
starttake6:
	mov	cx,WORD PTR maxiteration
;x2=product32(tx,tx)
	mov	ax,di
	imul	di
	shift9			;here tx = a < 4, so eax is correct tx*tx
	mov	IXX,ax		;keep x2 in IXX
;y2=product32(ty,ty)
	mov	ax,si
	imul	si
	shift9			;ax is correct ty*ty if ty < 4.
	mov	IYY,ax		;keep y2 in IYY
	add	ax,IXX		;check x2 + y2
	jc	toobig6		;if carry its over 16
	test	ax,CUBICCUT6	;and also too big if hits left mask
	jnz	toobig6
	jmp	cstep6
toobig6:
	jmp	cbig6
cstep6:
	;xa = x2 - y2 + a3;
	;yb = 2*x*y + b3;
	;temp = xa * tx - yb * ty + x;
	mov	ax,IXX			;ax now holds x2
	sub	ax,IYY			;ax now holds x2 - y2
	jo	prebailx
	add	ax,WORD PTR a3	;ax now holds xa
	jno	continuex6
	prebailx:
	jmp	cbig6
	continuex6:
	mov	WORD PTR xa, ax
	imul	di
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailx			;(if neg) all 1s, and dh = dl.
	mov	bx,ax			;use bx as temp register
	mov	ax,di
	imul	si
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailx			;(if neg) all 1s, and dh = dl.
	add	ax,ax
	jo	bailx
	add	ax,WORD PTR b3	;ax now holds yb
	jo	bailx
	mov	WORD PTR yb, ax
	imul	si
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailx			;(if neg) all 1s, and dh = dl.
	sub	bx,ax			;bx now holds xa*tx - yb*ty
	jo	bailx
	add	bx,WORD PTR x	;bx is now new tx value
	jo	bailx
	mov	WORD PTR temp, bx ;save it in temp
	jmp	starty
	bailx:
	jmp	cbig6
;ty = ty * xa + tx * yb + y
;tx = temp
starty:
	mov	ax,si
	imul	WORD PTR xa
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	baily			;(if neg) all 1s, and dh = dl.
	mov	bx,ax			;save in bx
	mov	ax,di
	imul	WORD PTR yb
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	baily			;(if neg) all 1s, and dh = dl.
	add	ax,bx			;ax now holds ty*xa + tx*yb
	jo	baily
	add	ax,WORD PTR y	;ax now holds new ty
	jo	baily
	mov	si,ax			;save ty in si
	mov	di,WORD PTR temp	;save tx in di
	jmp	dosquares
	baily:
	jmp	cbig6
dosquares:
;y2=product32(ty,ty)
	imul	si			;ax already holds si, or ty
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	cbig6			;(if neg) all 1s, and dh = dl.
	mov	IYY,ax		;store y2 in IYY
;x2=product32(tx,tx)
	mov	ax,di
	imul	di
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	cbig6			;(if neg) all 1s, and dh = dl.
	mov	IXX,ax		;store x2 in IXX
;check size of x2 + y2
	add	ax,IYY		;ax is x2 + y2
	jc	cbig6			;if carry, is over 16
	test	ax,CUBICCUT6	;too big if hits bits 8 or 4
	jnz	cbig6
	dec	cx
	jz	cbig6
	jmp	cstep6
cbig6:
	xor	cubictake,1			;this is 1 after first run, 0 after 2nd
	jz	donecubictakes6
	mov	poscolor,cx			;save cx from positive a,b run
	mov	di,WORD PTR nega		;get ready for negative run
	mov	si,WORD PTR negb
	jmp	starttake6
donecubictakes6:			;done both runs now
	cmp	cx,poscolor			;get the worst (highest) cx score
	ja	fixcolor6				;where the guy exploded fastest
	mov	cx,poscolor			; and only set ax to 0 if 
	xor	ax,ax				; if cx runs all the way to 0
	test	cx,cx
	jz	colorready6
fixcolor6:
	mov	ax,maxiteration		;otherwise color is bigger than 0
	inc	ax
	sub	ax,cx
colorready6:
        ret
.8086
cubicmandel16 ENDP
;-------------------------------------------------------
in1cubmand16 PROC far uses di si bp
	mov	di,WORD PTR a	;accumulate tx in di
	mov	si,WORD PTR b	;accumulate ty in si
	mov	cx,WORD PTR maxiteration
	mov	 WORD PTR smallestval, 7FFFh ; = 8, as bloat is 200
	mov	smallestindex, 0
starttake6in1:
	mov	cx,WORD PTR maxiteration
;x2=product32(tx,tx)
	mov	ax,di
	imul	di
	shift9			;here tx = a < 4, so eax is correct tx*tx
	mov	IXX,ax		;keep x2 in IXX
;y2=product32(ty,ty)
	mov	ax,si
	imul	si
	shift9			;ax is correct ty*ty if ty < 4.
	mov	IYY,ax		;keep y2 in IYY
	add	ax,IXX		;check x2 + y2
	jc	toobig6in1		;if carry its over 16
	test	ax,CUBICCUT6	;and also too big if hits left mask
	jnz	toobig6in1
	jmp	cstep6in1
	toobig6in1:
	jmp	cbig6in1
	cstep6in1:
	;xa = x2 - y2 + a3;
	;yb = 2*x*y + b3;
	;temp = xa * tx - yb * ty + x;
	mov	ax,IXX			;ax now holds x2
	sub	ax,IYY			;ax now holds x2 - y2
	jo	prebailxin1
	add	ax,WORD PTR a3	;ax now holds xa
	jno	continuex6in1
prebailxin1:
	jmp	cbig6in1
continuex6in1:
	mov	WORD PTR xa, ax
	imul	di
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailxin1			;(if neg) all 1s, and dh = dl.
	mov	bx,ax			;use bx as temp register
	mov	ax,di
	imul	si
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailxin1			;(if neg) all 1s, and dh = dl.
	add	ax,ax
	jo	bailxin1
	add	ax,WORD PTR b3	;ax now holds yb
	jo	bailxin1
	mov	WORD PTR yb, ax
	imul	si
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailxin1			;(if neg) all 1s, and dh = dl.
	sub	bx,ax			;bx now holds xa*tx - yb*ty
	jo	bailxin1
	add	bx,WORD PTR x	;bx is now new tx value
	jo	bailxin1
	mov	WORD PTR temp, bx ;save it in temp
	jmp	startyin1
bailxin1:
	jmp	cbig6
;ty = ty * xa + tx * yb + y
;tx = temp
startyin1:
	mov	ax,si
	imul	WORD PTR xa
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailyin1			;(if neg) all 1s, and dh = dl.
	mov	bx,ax			;save in bx
	mov	ax,di
	imul	WORD PTR yb
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailyin1			;(if neg) all 1s, and dh = dl.
	add	ax,bx			;ax now holds ty*xa + tx*yb
	jo	bailyin1
	add	ax,WORD PTR y	;ax now holds new ty
	jo	bailyin1
	mov	si,ax			;save ty in si
	mov	di,WORD PTR temp	;save tx in di
	jmp	dosquaresin1
	bailyin1:
	jmp	cbig6in1
dosquaresin1:
;y2=product32(ty,ty)
	imul	si			;ax already holds si, or ty
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	cbig6in1			;(if neg) all 1s, and dh = dl.
	mov	IYY,ax		;store y2 in IYY
;x2=product32(tx,tx)
	mov	ax,di
	imul	di
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	cbig6in1			;(if neg) all 1s, and dh = dl.
	mov	IXX,ax		;store x2 in IXX
;check size of x2 + y2
	add	ax,IYY		;ax is x2 + y2
	jc	cbig6in1			;if carry, is over 16
	test	ax,CUBICCUT6	;too big if hits bits 8 or 4
	jnz	cbig6in1
	cmp	ax, WORD PTR smallestval
	ja	notsmallest6in1
	mov	WORD PTR smallestval, ax
	mov	smallestindex, cx
	notsmallest6in1:
	dec	cx
	jz	cbig6in1
	jmp	cstep6in1
cbig6in1:
	xor	cubictake,1			;this is 1 after first run, 0 after 2nd
	jz	donecubictakes6in1
	mov	poscolor,cx			;save cx from positive a,b run
	mov	di,WORD PTR nega		;get ready for negative run
	mov	si,WORD PTR negb
	jmp	starttake6in1
donecubictakes6in1:			;done both runs now
	cmp	cx,poscolor			;get the worst (highest) cx score
	ja	fixcolor6in1			;where the guy exploded fastest
	mov	cx,poscolor			; do IYYecial if inside
	test	cx,cx
	jz	inside6in1	
fixcolor6in1:
	mov	ax,maxiteration		;otherwise color is bigger than 0
	inc	ax
	sub	ax,cx
	jmp	colorset6in1
inside6in1:
	mov	ax, WORD PTR smallestval
	cmp	insideflag,1
	je	colorset6in1
	mov	ax, maxiteration
	sub	ax, smallestindex
colorset6in1:
        ret
in1cubmand16 ENDP
;-------------------------------------------------------
cubicjulia16 PROC far uses di si IXX		;cubic black mandelbrot with terrace coloring
	mov	di,WORD PTR x	;accumulate tx in di
	mov	si,WORD PTR y	;accumulate ty in si
starttake6j:
	mov	cx,WORD PTR maxiteration
;x2=product32(tx,tx)
	mov	ax,di
	imul	di
	shift9			;here tx = a < 4, so eax is correct tx*tx
	mov	IXX,ax		;keep x2 in IXX
;y2=product32(ty,ty)
	mov	ax,si
	imul	si
	shift9			;ax is correct ty*ty if ty < 4.
	mov	IYY,ax		;keep y2 in IYY
	add	ax,IXX		;check x2 + y2
	jc	toobig6j		;if carry its over 16
	test	ax,CUBICCUT6	;and also too big if hits left mask
	jnz	toobig6j
	jmp	cstep6j
toobig6j:
	jmp	cbig6j
cstep6j:
	;xa = x2 - y2 + a3;
	;yb = 2*x*y + b3;
	;temp = xa * tx - yb * ty + x;
	mov	ax,IXX			;ax now holds x2
	sub	ax,IYY			;ax now holds x2 - y2
	jo	prebailxj
	add	ax,WORD PTR a3	;ax now holds xa
	jno	continuex6j
	prebailxj:
	jmp	cbig6j
	continuex6j:
	mov	WORD PTR xa, ax
	imul	di
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailxj			;(if neg) all 1s, and dh = dl.
	mov	bx,ax			;use bx as temp register
	mov	ax,di
	imul	si
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailxj			;(if neg) all 1s, and dh = dl.
	add	ax,ax
	jo	bailxj
	add	ax,WORD PTR b3	;ax now holds yb
	jo	bailxj
	mov	WORD PTR yb, ax
	imul	si
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailxj			;(if neg) all 1s, and dh = dl.
	sub	bx,ax			;bx now holds xa*tx - yb*ty
	jo	bailxj
	add	bx,WORD PTR u	;bx is now new tx value
	jo	bailxj
	mov	WORD PTR temp, bx ;save it in temp
	jmp	startyj
	bailxj:
	jmp	cbig6j
;ty = ty * xa + tx * yb + y
;tx = temp
startyj:
	mov	ax,si
	imul	WORD PTR xa
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailyj			;(if neg) all 1s, and dh = dl.
	mov	bx,ax			;save in bx
	mov	ax,di
	imul	WORD PTR yb
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	bailyj			;(if neg) all 1s, and dh = dl.
	add	ax,bx			;ax now holds ty*xa + tx*yb
	jo	bailyj
	add	ax,WORD PTR v	;ax now holds new ty
	jo	bailyj
	mov	si,ax			;save ty in si
	mov	di,WORD PTR temp	;save tx in di
	jmp	dosquaresj
bailyj:
	jmp	cbig6j
dosquaresj:
;y2=product32(ty,ty)
	imul	si			;ax already holds si, or ty
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	cbig6j			;(if neg) all 1s, and dh = dl.
	mov	IYY,ax		;store y2 in IYY
;x2=product32(tx,tx)
	mov	ax,di
	imul	di
	shift9				;dx:ax is the true product.  So its
	xor	dh,dl			;under 17 bits, edx should be all 0s or
	jnz	cbig6j			;(if neg) all 1s, and dh = dl.
	mov	IXX,ax		;store x2 in IXX
;check size of x2 + y2
	add	ax,IYY		;ax is x2 + y2
	jc	cbig6j			;if carry, is over 16
	test	ax,CUBICCUT6	;too big if hits bits 8 or 4
	jnz	cbig6j
	dec	cx
	jz	cbig6j
	jmp	cstep6j
cbig6j:
;	xor	cubictake,1			;this is 1 after first run, 0 after 2nd
;	jz	donecubictakes6j
;	mov	poscolor,cx			;save cx from positive a,b run
;	mov	di,WORD PTR nega		;get ready for negative run
;	mov	si,WORD PTR negb
;	jmp	starttake6j
;	donecubictakes6j:			;done both runs now
;	cmp	cx,poscolor			;get the worst (highest) cx score
;	ja	fixcolor6j				;where the guy exploded fastest
;	mov	cx,poscolor			; and only set ax to 0 if 
	xor	ax,ax				; if cx runs all the way to 0
	test	cx,cx
	jz	colorready6j
fixcolor6j:
	mov	ax,maxiteration		;otherwise color is bigger than 0
	inc	ax
	sub	ax,cx
colorready6j:
        ret
cubicjulia16 ENDP

;-------------------------------------------------------
END

                                               
