;attraca7.asm, to be linked with attract7.c
DOSSEG
        .MODEL LARGE,C
.DATA
EXTRN mode,maxx,maxy,lox,hiy,deltax,minx,deltay:WORD
EXTRN cursoron:WORD,cursoroffset:WORD
EXTRN cursorpatch:BYTE:64
EXTRN grayflag:WORD
EXTRN curx:WORD,cury:WORD, palettenumber:WORD
EXTRN cursorshape:WORD
EXTRN cursorcolor:WORD
EXTRN stamprxmin:WORD, stamprxmax:WORD, stamprymin:WORD, stamprymax:WORD

public  shiftregister
public  sixteen_count

shiftregister dw 17
sixteen_count dw 0

public egacolortable

egacolortable db 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0
;This is at palettenumber 0 which we randomize.  17 bytes long.

;presetegacolortable1
 db 0,4,2,3,4,5,6,7,14,9,10,11,12,13,14,15,0

;presetegacolortable2
 db 0,4,4,2,2,3,3,5,5,6,6,7,7,8,8,4,0

;presetegacolortable3
 db 0,2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,0

activedactable db 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16

public  vgacolortable

vgacolortable db 0,0,0
db 0,0,42
db 63,00,30
db 63,00,40
db 42,0,0
db 50,00,63
db 40,00,63
db 42,42,42
db 21,21,21
db 63,45,00
db 63,52,00
db 63,55,00
db 63,63,00
db 39,60,06
db 30,58,52
db 63,63,63

;This is palettenumber 0 which we randomize all the time.
;The following are various presetvgacolortablen, each of which
;is found at offset vgacolortable + n*51.

;presetvgacolortable1
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

;presetvgacolortable2
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

;presetvgacolortable3
db   0,   0,   0   ;   0
db 63,   0,   0   ;   1
db 63,   0,  0   ;   2
db 63,   0,  0   ;   3
db  0,  63,  0   ;   4
db  0,  63,  0   ;   5
db  0,  63,  0   ;   6
db  0,   0,  63   ;   7
db  0,   0,  63   ;   8
db  0,   0,  63   ;   9
db  63,  0,  63   ;  10
db  63,  0,  63   ;  11
db  63,  0,  63   ;  12
db  63, 63,  0   ;  13
db  63, 63,  0   ;  14
db  63, 63,  0   ;  15
db   0,   0,   0   ;  16 overscan

;presetvgacolortable4
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

masktable db 32 dup (80h,40h,20h,10h,08h,04h,02h,01h)

.CODE

PUBLIC sixteenbitsa
PUBLIC egavgapixel
PUBLIC segavgapixel
PUBLIC xegavgapixel
PUBLIC grayscale
PUBLIC setmode
PUBLIC getmode
PUBLIC mouse
;PUBLIC drawcursor
;PUBLIC erasecursor
PUBLIC changepalette

;---------------------------------------
sixteenbitsa proc far
	mov	ax,shiftregister
	mov	bx,ax
	ror	bx,1		; at each slot bx holds L bit
	rol	dx,1		; at each slot dx holds R bit
	or	ax,dx	; NewC= L XOR (C OR R)
	xor	ax,bx
	xor	ax,sixteen_count
	mov	shiftregister,ax
	inc	word ptr sixteen_count
	ret
sixteenbitsa ENDP

;-----------------------------------------------
changepalette proc far
	push	di
	mov	di,1		;starting at 1,
	mov	cx,15	;we will change 15 colors in EGA
	cmp mode,10h
	je egapalette
	mov	di,3		;starting at byte 3 (red of DAC 1),
	mov	cx,45	;we will change 45 bytes in VGA
	vgapalette:
		call	sixteenbitsa
		and	ax,63
		mov	vgacolortable[di],al
		inc	di
		loop	vgapalette
		pop	di
		ret
	egapalette:
		call sixteenbitsa
		and	al,15
		mov egacolortable[di],al
		inc	di
		loop	egapalette
		pop 	di
		ret
changepalette endp
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


;setmode(int m) attempts set to graphmode n, returns nothing.
setmode proc far
arg     _mode:word
	mov	ax,_mode
	xor	ah,ah
	int	10h
	ret
setmode endp
;-----------------------------------------------------------
;getmode() gets the current display mode and returns it (to ax)
getmode proc far
	mov	ax,0f00h
	int 10h
	xor	ah,ah		;this would have # of char columns
	ret
getmode endp
;------------------------------------
;this reads 4 passsed variables into ax,bx,cx,ds and calls a microsoft ;mouse function. return values are in _AX,_BX,_CX,_DX.
mouse proc far
arg     _ax:word,_bx:word,_cx:word,_dx:word
	mov ax,_ax
	mov bx,_bx
	mov cx,_cx
	mov dx,_dx
	int 33h
	ret
mouse endp

;---------------------------------------------------
egavgapixel proc far uses di,_i:word,_j:word,_col:byte
        cli
	cmp	_i,639		; should I clip?
	ja	doneegavgapixel	; unsigned compare views negative nums as big
        mov     ax,_i
        cmp     ax,minx
        jle     doneegavgapixel
        mov     ax,_j
        cmp     ax,maxy
        ja      doneegavgapixel
        cmp     ax,0
        jb      doneegavgapixel

	mov	ax,0A000h
	mov	es,ax	;set Vseg
	mov	dx,03CEh		;point to address register
	mov	ax,0F01h		;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The F in ah says reset all 4 bit planes
	mov	ax,_j		;get j in ax
	shl	ax,1			;80*j same as (64+16)* j
	shl	ax,1
	shl	ax,1
	shl	ax,1
	mov	di,ax		;16*j
	shl	ax,1
	shl	ax,1
	add	di,ax		;16*j + 64*j
	mov	cx,_i			; get i in cx
	mov	al,cl
	mov	bx, OFFSET masktable
	xlat					;get bitmask in al
	mov	ah,8				;8 selects bitmask register
	xchg	al,ah			;put them in right order
	out	dx,ax			;dx still 03CEh.  load the bitmask
	shr	cx,1
	shr	cx,1
	shr	cx,1			;get [i/8]
	add	di,cx		;target byte 80*j + [i/8]
	xor	al,al		;0 in al selects set/reset register
	mov	ah,_col 	;get byte of color for color
	out	dx,ax		; dx is still 03CEh.  
	or	es:[di],al	;latch and load
	mov	ax,0001h		;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The 0 in ah is default value
	mov	ax,0000h		;0 in al is set/reset register, 0 in ah is
	out	dx,ax		;default color to hold.  Restore.
	mov	ax,0FF08h		;al 8 selects bitmask.  FF in ah is default.
	out	dx,ax		;restore bitmask to default.
doneegavgapixel:
        sti
	ret
egavgapixel ENDP
;---------------------------------------------------
segavgapixel proc far
arg     _i:word,_j:word,_col:byte
        cli
	mov ax,_i
	cmp	ax,stamprxmin		; should I clip?
	jle	donesegavgapixel	
	cmp	ax,stamprxmax
	jge	donesegavgapixel
	mov	ax,_j
	cmp	ax,stamprymin
	jle	donesegavgapixel
	cmp	ax,stamprymax
	jge	donesegavgapixel
	push	di				;ok, let's do the pixel
	mov	ax,0A000h
	mov	es,ax	;set Vseg
	mov	dx,03CEh		;point to address register
	mov	ax,0F01h		;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The F in ah says reset all 4 bit planes
	mov	ax,_j		;get j in ax
	shl	ax,1			;80*j same as (64+16)* j
	shl	ax,1
	shl	ax,1
	shl	ax,1
	mov	di,ax		;16*j
	shl	ax,1
	shl	ax,1
	add	di,ax		;16*j + 64*j
	mov	cx,_i			; get i in cx
	mov	al,cl
	mov	bx, OFFSET masktable
	xlat					;get bitmask in al
	mov	ah,8				;8 selects bitmask register
	xchg	al,ah			;put them in right order
	out	dx,ax			;dx still 03CEh.  load the bitmask
	shr	cx,1
	shr	cx,1
	shr	cx,1			;get [i/8]
	add	di,cx		;target byte 80*j + [i/8]
	xor	al,al		;0 in al selects set/reset register
	mov	ah,_col 	;get byte of color for color
	out	dx,ax		; dx is still 03CEh.  
	or	es:[di],al	;latch and load
	mov	ax,0001h		;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The 0 in ah is default value
	mov	ax,0000h		;0 in al is set/reset register, 0 in ah is
	out	dx,ax		;default color to hold.  Restore.
	mov	ax,0FF08h		;al 8 selects bitmask.  FF in ah is default.
	out	dx,ax		;restore bitmask to default.
	pop	di
donesegavgapixel:
        sti
	ret
segavgapixel ENDP
;----------------------------------------------
xegavgapixel proc far
arg     _i:word,_j:word,_col:byte
        cli
	cmp	word ptr _i,639		; should I clip?
	ja	donexegavgapixel	; unsigned compare views negative nums as big
        mov     ax,_i
        cmp     ax,minx
        jle     donexegavgapixel
	mov	ax,maxy			; can be 349 or 479
	cmp	word ptr _j,ax
	ja	donexegavgapixel
	push	di				;ok, let's do the pixel
	mov	ax,0A000h
	mov	es,ax	;set Vseg
	mov	dx,03CEh		;point to address register
	mov	ax,0F01h		;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The F in ah says reset all 4 bit planes
	mov	ax,1803h		;3 in al selects DATA ROTATE/FUNCTION SELECT reg
	out	dx,ax		;The 18 in ah says xor color with the latches
	mov	ax,_j		;get j in ax
	shl	ax,1			;80*j same as (64+16)* j
	shl	ax,1
	shl	ax,1
	shl	ax,1
	mov	di,ax		;16*j
	shl	ax,1
	shl	ax,1
	add	di,ax		;16*j + 64*j
	mov	cx,_i			; get i in cx
	mov	al,cl
	mov	bx, OFFSET masktable
	xlat					;get bitmask in al
	mov	ah,8				;8 selects bitmask register
	xchg	al,ah			;put them in right order
	out	dx,ax			;dx still 03CEh.  load the bitmask
	shr	cx,1
	shr	cx,1
	shr	cx,1			;get [i/8]
	add	di,cx		;target byte 80*j + [i/8]
	xor	al,al		;0 in al selects set/reset register
	mov	ah,_col		;get byte of color for color
	out	dx,ax		; dx is still 03CEh.  
	or	es:[di],al	;latch and load
	mov	ax,0001h		;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The 0 in ah is default value
	mov	ax,0000h		;0 in al is set/reset register, 0 in ah is
	out	dx,ax		;default color to hold.  Restore.
	mov	ax,0FF08h		;al 8 selects bitmask.  FF in ah is default.
	out	dx,ax		;restore bitmask to default.
	mov	ax,0003h		;3 in al selects DATA ROTATE/FUNCTION SELECT reg
	out	dx,ax		;The 00 in ah restores normal color write
	pop	di
donexegavgapixel:
        sti
	ret
xegavgapixel ENDP
;--------------------------------------------------
COMMENT !
erasecursor proc far
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
;	mov	dx,03CEh		;My bitmask is already at default
;	mov	ax,0FF08h
;	out	dx,ax
;        mov     ax,5	;I didn't change writemode
;        out     dx,ax	
;        mov     ax,3	;I didn't change datarotate function select
;        out     dx,ax
	pop	si
	pop	di
doneerasecursor:
	ret
erasecursor ENDP

;----------------------------------------------
drawcursor proc far
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
	mov	ax,[cursorcolor]	;ready to load (00) color 01
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
	jmp	closedrawcursor
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
	closedrawcursor:
	mov	ax,0001h		;01 for set/reset enable, 00 default no planes
	out	dx,ax
	mov	ax,0000h		;00 for set/reset register default color 00
	out	dx,ax
;	mov	ax,5			; turn off set/reset enable so its 
;	out	dx,ax		; restored to default.
;        mov     ax,3
;        out     dx,ax
	pop	si		
	pop	di
	ret
drawcursor ENDP
;--------------------------------------------------
!
END
