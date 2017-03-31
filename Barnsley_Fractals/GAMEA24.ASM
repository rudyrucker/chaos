DOSSEG
LOCALS
.MODEL LARGE,C
SHIFTNUMBER EQU 10 ; # of 0s in BLOAT. We started with 11, tried 9.
.DATA
EXTRN mode,minx,maxx,maxy,lox,hiy,deltax,deltay,miny:WORD
EXTRN cursoron:WORD,cursoroffset:WORD
EXTRN cursorpatch:BYTE:64
EXTRN grayflag:WORD
EXTRN BLOAT: WORD
EXTRN curx:WORD,cury:WORD
EXTRN cursorcolor:WORD
;PUBLIC egacolortable
;PUBLIC vgacolortable
extrn   raster_op:byte

shiftregister dw 17
count dw 0
masktable db 32 dup (80h,40h,20h,10h,8,4,2,1)

;egacolortable db 0,4,2,3,4,5,6,7,14,9,10,11,12,13,14,63,0
	; byte 16 is overscan color, leave 0

defaultegacolortable db 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0
	; byte 16 is overscan color, leave 0

;vgacolortable db  0,   0,   0   ;   0
;db  63,   0,   0   ;   1
;db  63,  32,  32   ;   2
;db  32,  63,  32   ;   3
;db  32,  32,  63   ;   4
;db  63,  32,   0   ;   5
;;db   0,  32,  63   ;   6
;db  32,  63,   0   ;   7
;db   0,  63,   0   ;   8
;db   0,   0,  63   ;   9
;db  63,   0,  32   ;  10
;db  32,   0,  63   ;  11
;db   0,  63,  32   ;  12
;db  60,  40,  20   ;  13
;db  20,  60,  40   ;  14
;db  63,  63,  63   ;  15 this should act as maxcolor of VGA
;db   0,   0,   0   ;  16 perhaps acts as overscan color

activedactable db 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0


.CODE

PUBLIC sixteenbitsa
PUBLIC egavgapixel
PUBLIC barnimage
PUBLIC pixel
;PUBLIC usepalette
;PUBLIC changepalette
;PUBLIC spinpalette
;PUBLIC revspinpalette
PUBLIC grayscale
PUBLIC setmode
PUBLIC getmode
PUBLIC mouse


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
;---------------------------------------
; the barnimage procedure computes a new pair w = h[z].
;	w.x = intmul(h.a, z.x) + intmul(h.b, z.y) + h.e;
;	w.y = intmul(h.c, z.x) + intmul(h.d, z.y) + h.f;
;
; I carry x and y in di, si, and I 
; I accumulate w.x in bx, w.y in cx, and load them into ax and dx
; to return a four byte structure (see TASM User manual p. 329)
; barnimage takes two structures as parameters, a barnmap and a pair.
; these structures are treated as lists of integers and are pushed
; on stack from right to left.
;h.a		bp+4
;h.b		bp+6
;h.c		bp+8
;h.d		bp+10
;h.e		bp+12
;h.f		bp+14
;z.x		bp+16
;z.y		bp+18

; Rewritten to intelligently use POINTERS instead of passing the
; whole goddamed structure. Now it is
; pair barnimage(barnmap *h,fpair *z)
_barnmap struc
        h_a     dw ?
        h_b     dw ?
        h_c     dw ?
        h_d     dw ?
        h_e     dw ?
        h_f     dw ?
_barnmap ends

_fpair struc
        z_x     dw ?
        z_y     dw ?
_fpair  ends



barnimage PROC far uses di si ds,hp:ptr,zp:ptr
local h:_barnmap,z:_fpair
        lea     di,h
        mov     ax,ss
        mov     es,ax
        
        lds     si,hp
        mov     cx,6
        rep     movsw

        lea     di,z
        lds     si,zp
        movsw
        movsw 


	mov	di,z.z_x
	mov	si,z.z_y
	mov	ax,h.h_a
	imul	di
	REPT SHIFTNUMBER
		shr dx,1
		rcr ax,1
	ENDM
	mov	bx,ax
	mov	ax,h.h_b
	imul	si
	REPT SHIFTNUMBER
		shr dx,1
		rcr ax,1
	ENDM
	add	bx,ax
	add	bx,h.h_e
	mov	ax,h.h_c
	imul	di
	REPT SHIFTNUMBER
		shr dx,1
		rcr ax,1
	ENDM
	mov	cx,ax
	mov	ax,h.h_d
	imul	si
	REPT SHIFTNUMBER
		shr dx,1
		rcr ax,1
	ENDM
	add	cx,ax
	add	cx,h.h_f
	mov	ax,bx
	mov	dx,cx
	ret
barnimage ENDP
;-----------------------------------
;the pixel procedure is called as pixel(z).  z is a pair
;structure and its two integers are pushed on stack. pixel returns
;two integers in ax and dx, pixel(z).x and pixel(z).y
;pixel(z).x=(z.x-lox)*maxx/deltax + minx and 
;pixel(z).y=(hix-z.y)*maxy/deltay
;I allow for the possibility of offscreen points by using imul and idiv.

; I've changed it to pass a POINTER to the pair, rather than the
; pair itself. I've got my reasons. Called as
; void pixel(pair *)
; Regardless of how many codes it _seems_ to add, the order of the
; lea and stuff like that is much smaller than either the idiv or
; imul, so we'll leave it be at that.
;

_pair   struc
        pz_x     dw ?
        pz_y     dw ?
_pair   ends

pixel PROC far uses di si,__pair:ptr
local pair:_pair
        push    ds
        lea     di,pair
        mov     ax,ss
        mov     es,ax
        lds     si,__pair
        movsw
        movsw
        pop     ds

	mov	ax,hiy
	sub	ax,pair.pz_y
	imul	maxy
	idiv	deltay
	mov	bx,ax		;save pixel.y value for puttin in dx below
	mov	ax,pair.pz_x
	sub	ax,lox
	imul	maxx
	idiv	deltax		;pixel(z).x in ax now.
	add	ax,minx		;this used to be in the egavga(x+minx,y) call.
	mov	dx,bx		;pixel(z).y value
        add     dx,miny
	ret
pixel ENDP
;--------------------------------------------------
COMMENT %
usepalette proc far
	push	cx
	push di
	push es
	mov	ax,ds
	mov	es,ax
	cmp mode,10h
	je egapaletteu
vgapaletteu:
	mov	dx,offset activedactable ;standard color indices
	mov	ax,1002h		;programmers guide to egavga p. 158
	int	10h
	mov	dx,offset vgacolortable
	mov	ax,1012h	;programmers egavga guide p. 161
	xor	bx,bx	;first DAC register to reset is 0
	mov	cx,16	;number of DAC regs to reset (all but overscan)
	int	10h
	pop	es
	pop	di
	pop	cx
	ret
egapaletteu:
	mov	dx,offset egacolortable
	mov	ax,1002h		;programmers guide to egavga p. 158
	int	10h
	pop	es
	pop	di
	pop	cx
	ret
usepalette endp
;---------------------------------------------------
changepalette proc far
	push	cx
	push	di
	mov	di,1		;we don't change EGA color 0
	mov	cx,13	;we don't change EGA 15 or 16
	cmp mode,10h
	je egapalette
	mov	di,3		;we don't change DAC 0
	mov	cx,3*14	;we don't change DAC 15 (white) or 16 (overscan)
vgapalette:
	call	sixteenbitsa
	and	ax,63
	mov	vgacolortable[di],al
	inc	di
	loop	vgapalette
	pop	di
	pop	cx
	ret
egapalette:
	call sixteenbitsa
	and	al,63
	mov egacolortable[di],al
	inc	di
	loop	egapalette
	pop 	di
	pop	cx
	ret
changepalette endp
;-----------------------------------------
spinpalette proc far
	push	di
	mov	di,1
	mov	cx,15
	mov	al,egacolortable[15]
	cmp	mode,10h
	je	egaspin
	mov	al,activedactable[15]
	dacspin:
		mov	ah,activedactable[di]
		mov	activedactable[di],al
		inc	di
		mov	al,ah
	loop dacspin
	jmp	donespin
	egaspin:
		mov	ah,egacolortable[di]
		mov	egacolortable[di],al
		inc	di
		mov	al,ah
	loop egaspin
	donespin:
	pop di
	call usepalette
	ret
spinpalette endp
;-----------------------------------------
revspinpalette proc far
	push	di
	mov	di,15
	mov	cx,15
	mov	al,egacolortable[1]
	cmp	mode,10h
	je	revegaspin
	mov	al,activedactable[1]
	revdacspin:
		mov	ah,activedactable[di]
		mov	activedactable[di],al
		dec	di
		mov	al,ah
	loop revdacspin
	jmp	revdonespin
	revegaspin:
		mov	ah,egacolortable[di]
		mov	egacolortable[di],al
		dec	di
		mov	al,ah
	loop revegaspin
	revdonespin:
	pop di
	call usepalette
	ret
revspinpalette endp
;-----------------------------------------
%
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
setmode proc far,m:word
	mov	ax,m
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
mouse proc far,_ax:word,_bx:word,_cx:word,_dx:word
	mov ax,_ax
	mov bx,_bx
	mov cx,_cx
	mov dx,_dx
	int 33h
	ret
mouse endp

;---------------------------------------------------

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



public  egavgagetpixel
egavgagetpixel proc far uses si,x:word,y:word
        mov     ax,y
        mov     bx,x
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



egavgapixel PROC far uses di,x,y:word,color:byte
	mov	ax,x
        mov     bx,maxx
        add     bx,minx
	cmp	ax,bx
	jge	@@done
	cmp	ax,minx
	jle	@@done
	mov	ax,y
        mov     bx,maxy
        add     bx,miny
	cmp	ax,bx		;unsigned compare treats negatives as big.
	jae	@@done
        cmp     ax,miny
        jbe     @@done

        cli                             ; no ints here please
	mov	ax,0A000h
	mov	es,ax	;set Vseg
	mov	dx,03CEh		;point to address register
	mov	ax,0F01h		;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The F in ah says reset all 4 bit planes
	mov	ax,y		;get j in ax
	shl	ax,1			;80*j same as (64+16)* j
	shl	ax,1
	shl	ax,1
	shl	ax,1
	mov	di,ax		;16*j
	shl	ax,1
	shl	ax,1
	add	di,ax		;16*j + 64*j
	mov	cx,x			; get i in cx
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
	mov	ah,color		;get byte of color for color
	out	dx,ax		; dx is still 03CEh.  
	or	es:[di],al	;latch and load

;	mov	ax,0001h		;1 in al selects SET/RESET ENABLE register
;	out	dx,ax		;The 0 in ah is default value
;	mov	ax,0000h		;0 in al is set/reset register, 0 in ah is
;	out	dx,ax		;default color to hold.  Restore.
;	mov	ax,0FF08h		;al 8 selects bitmask.  FF in ah is default.
;	out	dx,ax		;restore bitmask to default.

        mov     ax,0ff08h
        out     dx,ax
        mov     ax,5
        out     dx,ax
        mov     ax,3
        out     dx,ax
        mov     ax,1
        out     dx,ax
        sti

@@done:
	ret
egavgapixel ENDP


        public  HLine
        ; Draw a horizontal line from X1,Y1 to X2,Y2
        ; void HLine(int X1,int Y1,int X2,char color)
HLine   proc    far uses si di ds,X1:word,Y1:word,X2:word,color:byte

        mov     dx,3ceh
        mov     ah,color
        xor     al,al
        out     dx,ax
        mov     ax,0f01h
        out     dx,ax
        mov     ah,raster_op
        mov     al,3
        out     dx,ax

        mov     ax,Y1
        mov     bx,X1

        call    PixelAddr10

        mov     di,bx

        mov     dh,ah
        not     dh
        shl     dh,cl
        not     dh

        mov     cx,X2
        and     cl,7
        xor     cl,7
        mov     dl,0ffh
        shl     dl,cl

; determine offset of first and last pel on line
        mov     ax,X2
        mov     bx,X1
        mov     cl,3
        shr     ax,cl
        shr     bx,cl
        mov     cx,ax
        sub     cx,bx           ; cx now = (# bytes) - 1

        mov     bx,dx
        mov     dx,3ceh
        mov     al,8

        push    es
        pop     ds
        mov     si,di

        or      bh,bh
        js      @@L43

        or      cx,cx
        jnz     @@L42

        and     bl,bh
        jmp     short @@L44

@@L42:  
        mov     ah,bh
        out     dx,ax
        movsb
        dec     cx

@@L43:  
        mov     ah,11111111b
        out     dx,ax
        rep     movsb

; rightmost shit
@@L44:  
        mov     ah,bl
        out     dx,ax
        movsb


        xor     ax,ax
        out     dx,ax
        inc     ax
        out     dx,ax
        mov     al,3
        out     dx,ax
        mov     ax,0ff08h
        out     dx,ax

        ret
HLine   endp

;--------------------------------------------------
END
