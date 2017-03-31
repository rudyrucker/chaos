;rugraphz.asm
;This has palette spins and graphmode set and get.
LOCALS
DOSSEG
.MODEL MEDIUM,C

.DATA
EXTRN mode:WORD
EXTRN vgaflag:WORD
EXTRN grayflag:WORD, palettenumber:WORD

count dw 0
shiftregister dw 7

PUBLIC egacolortable
PUBLIC vgacolortable

egacolortable db 17 dup(0)
;This is at palettenumber 0 which we randomize or sping.  17 bytes.

public HODGE_ct
HODGE_ct equ this byte
;preset_egacolortable1 spectrum for hodge
 db 0,61,60,4,60,60,4,20,62,58,2,3,59,57,1,5,0

;preset_egacolortable2
 db 0,4,2,3,4,5,6,7,14,9,10,11,12,13,14,15,0

;preset_egacolortable3
 db 0,1,2,3,4,5,14h,7,38h,39h,3ah,3bh,3ch,3dh,3eh,3fh,0

;preset_egacolortable4 slow grays
 db 0,56,56,56,7,7,7,63,63,63,56,56,56,7,7,0,0

;preset_egacolortable5 grays
 db 0,56,63,7,56,63,7,56,0,7,56,63,7,56,63,7,0

public NLUKY_ct
NLUKY_ct equ this byte
;preset_egacolortable6 for nluky (high odds not used)
 db 0,63,4,60,62,20,58,2,3,59,57,1,2,61,60,4,0

public EAT_ct
EAT_ct equ this byte
;preset_egacolortable7 all color jumble for eat
 db 0,57,58,59,60,61,62,1,2,3,4,5,7,20,56,63,0

public TUBE_ct
TUBE_ct equ this byte
;preset 8 odd/even spectrum, for tube
 db 0,60,57,4,1,20,5,62,61,58,60,2,4,3,63,59,0

public activedactable
activedactable db 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16

vgacolortable db 51 dup(0)
;This is palettenumber 0 which we randomize all the time.
;The following are various preset_vgacolortablen, each of which
;is found at offset vgacolortable + n*51.

public HODGE_colortable
HODGE_colortable equ this byte
;preset_vgacolortable1
db  00,00,00;   0
db  21,21,21;   1
db  63,00,00;   2
db  42,21,00;   3
db  42,00,00;   4
db  00,00,63;   5
db  21,42,21;   6
db  42,42,42;   7
db  00,00,42;   8
db  00,21,63;   9
db  00,21,00;  10
db  42,42,00;  11
db  00,63,63;  12
db  63,21,00;  13
db  63,63,00;  14
db  63,63,63;  15
db  0,  0,  0 ;  16 overscan

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
db    0,   0,   0  ;   0
db    0,   0,  42  ;   1
db    0,  21,  42  ;   2
db    0,  42,  42  ;   3
db   21,  42,  42  ;   4
db   42,  42,  42  ;   5
db   42,  42,  63  ;   6
db   42,  63,  63  ;   7
db   63,  63,  63  ;   8
db   63,  63,  42  ;   9
db   63,  42,  42  ;  10
db   63,  42,  21  ;  11
db   63,  21,  21  ;  12
db   42,  21,  21  ;  13
db   21,  21,  21  ;  14
db   21,   0,   0  ;  15
db    0,   0,   0  ; 16 overscan

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

public NLUKY_colortable
NLUKY_colortable equ this byte
;preset_vgacolortable6 used for NLUKY
db 00,00,00	
db 55,00,36	
db 63,63,00	
db 63,33,00	
db 63,00,00	
db 00,63,00	
db 00,00,63	
db 63,00,63	
db 19,19,19	
db 63,00,20	
db 63,00,30	
db 63,00,40	
db 63,00,50	
db 63,00,63	
db 50,00,63	
db 63,63,63	
db   0,   0,   0   ;  16 overscan

public EAT_colortable
EAT_colortable equ this byte
COMMENT !
;preset_vgacolortable7  Used for Eat
db   00,00,00  ;   0
db   33,40,10  ;   1
db   10,60,40  ;   2
db   40,10,60  ;   3
db   63,16,00  ;   4
db   49,49,49  ;   5
db   00,63,00  ;   6
db   44,44,16  ;   7
db   15,14,41  ;   8
db   63,00,63  ;   9
db   63,63,00  ;  10
db   30,30,63  ;  11
db   30,63,30  ;  12
db   63,30,30  ;  13
db   30,30,30  ;  14
db   63,63,63  ;  14
db    0,   0,   0  ;  16 overscan
!
db	00,00,00
db	62,01,01
db	61,38,01
db	61,61,01
db	01,61,02
db	02,61,54
db	00,23,61
db	39,01,61
db	48,01,35
db	42,00,02
db	31,14,00
db	30,30,01
db	01,36,02
db	01,28,19
db	02,14,31
db	63,63,63
db      0,0,0

public TUBE_colortable
TUBE_colortable equ this byte
;preset_vgacolortable8   Used for Tube
db   00,00,00  ;   0
db   60,00,30  ;   1
db   63,63,00  ;   2
db   63,00,00  ;   3
db   56,11,00  ;   4
db   63,30,00  ;   5
db   00,63,00  ;   6
db   54,38,17  ;   7
db   00,39,10  ;   8
db   30,63,00  ;   9
db   00,30,60  ;  10
db   00,63,00  ;  11
db   00,00,60  ;  12
db   00,60,30  ;  13
db   30,00,60  ;  14
db   63,63,63  ;  15
db   0,0,0  ;  16 overscan


masktable db 32 dup (80h,40h,20h,10h,8,4,2,1)

.CODE

PUBLIC egavgapixel
PUBLIC sixteenbitsa
PUBLIC grayscale

;--------------------------------Procedures--------------
egavgapixel PROC far
;	cmp	word ptr [bp+4],639	; should I clip? don't need to here.
;	ja	doneegavgapixel	; unsigned compare views negative nums as big
;	mov	ax,maxy		; can be 349 or 479
;	cmp	word ptr [bp+6],ax
;	ja	doneegavgapixel
	push	di			;ok, let's do the pixel
	mov	ax,0A000h
	mov	es,ax	;set Vseg
	mov	dx,03CEh		;point to address register
	mov	ax,0F01h		;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The F in ah says reset all 4 bit planes
	mov	ax,[bp+8]		;get j in ax
	shl	ax,1			;80*j same as (64+16)* j
	shl	ax,1
	shl	ax,1
	shl	ax,1
	mov	di,ax		;16*j
	shl	ax,1
	shl	ax,1
	add	di,ax		;16*j + 64*j
	mov	cx,[bp+6]			; get i in cx
	mov	al,cl
	mov	bx, OFFSET masktable
	xlat					;get bitmask in al
	shr	cx,1				
	shr	cx,1
	shr	cx,1			;get [i/8]
	add	di,cx		        ;target byte 80*j + [i/8]
	mov	ah,8			;8 selects bitmask register
	xchg	al,ah		;put them in right order
	out	dx,ax		;dx still 03CEh.  load the bitmask
	xor	al,al		;0 in al selects set/reset register
	mov	ah,[bp+10]	;get byte of color for color
	out	dx,ax		; dx is still 03CEh.  
	or	es:[di],al	;latch and load
	mov	ax,0001h	;1 in al selects SET/RESET ENABLE register
	out	dx,ax		;The 0 in ah is default value
	mov	ax,0000h	;0 in al is set/reset register, 0 in ah is
	out	dx,ax		;default color to hold.  Restore.
	mov	ax,0FF08h	;al 8 selects bitmask.  FF in ah is default.
	out	dx,ax		;restore bitmask to default.
;        mov     ax,5
;        out     dx,ax
;        mov     ax,3
;        out     dx,ax
	pop	di
doneegavgapixel:
	ret
egavgapixel ENDP
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
END
