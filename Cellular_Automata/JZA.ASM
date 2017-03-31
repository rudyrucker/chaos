; Josh's assembly code to interface with jz.c 
        .model  large,c
        .8086
        JUMPS
        LOCALS

        ; MEDXCOUNT is now the number of DISPLAYED pixels, for
        ; Josh's sanity now. Argh. Sorry, kids.

FIRSTXPIXEL	equ	160
FIRSTXBYTE	equ	FIRSTXPIXEL/8

HIXCOUNT        equ     640-FIRSTXPIXEL
HIXWIDTH        equ     HIXCOUNT+2

MEDXCOUNT       equ     HIXCOUNT/2
MEDXWIDTH       equ     MEDXCOUNT+2

LOXCOUNT        equ     HIXCOUNT/8
LOXWIDTH        equ     LOXCOUNT+2

HIYCOUNT        equ     (350/8)*8
MEDYCOUNT       equ     HIYCOUNT/2
LOYCOUNT        equ     HIYCOUNT/8

COARSECORNERX	equ	(MEDXCOUNT-LOYCOUNT)/2
COARSECORNERY	equ	(MEDYCOUNT-LOYCOUNT)/2

SEQUENCER       equ     03c5h   ; sequencer data register




        .DATA
EXTRN nlukyn:BYTE, nlukyl:BYTE, nlukyu:BYTE, nlukyk:BYTE, nlukyy:BYTE;
EXTRN keybreakflag:BYTE, donescreenflag:BYTE
EXTRN maxeatstate:BYTE
extrn egarowptrs:dword
extrn newbufrowptrs:dword
extrn oldbufrowptrs:dword

EXTRN tubefuzz:BYTE, tubealarm:BYTE, tubelive:BYTE, tuberest:BYTE

extrn oddeven:dword
extrn evenoddflag:word

hinabetable	dw	-HIXWIDTH-1,-HIXWIDTH
                dw      1-HIXWIDTH,-1
                dw      1,HIXWIDTH-1
                dw      HIXWIDTH,HIXWIDTH+1

lonabetable	dw	-MEDXWIDTH-1,-MEDXWIDTH
                dw      1-MEDXWIDTH,-1
                dw      1,MEDXWIDTH-1
                dw      MEDXWIDTH,MEDXWIDTH+1

                public  eightsumtable
eightsumtable	db	512 dup(?)
                public ccaseed,ccacount
ccaseed	        dw	11h
ccacount	dw	0
                
                
                .code

; Normalizing functions want to make sure that ds:si or es:di
; are greater than HIXCOUNT but less than 0x10000-HIXCOUNT.
; Right now, not so precise, though.

hixblock        equ     HIXWIDTH and not 0fh
hixsegs         equ     hixblock / 16

normalize       proc    near
        push    cx
        push    ax
        mov     ax,8*HIXWIDTH
        mov     cx,ds
n1:     cmp     ax,si
        jnb     n2
        sub     si,hixblock
        add     cx,hixsegs
        jmp     n1
n2:     ; Now it is low enough.Is it high enough?
        mov     ax,2*HIXWIDTH
n3:     cmp     ax,si
        jb      n4
        add     si,hixblock
        sub     cx,hixsegs
        jmp     n3
n4:
        mov     ds,cx
        pop     ax
        pop     cx
        ret
normalize       endp

dinormalize     proc    near
        push    ax
        push    cx
        mov     ax,8*HIXWIDTH
        mov     cx,es
dn1:    cmp     ax,di
        jnb     dn2
        sub     di,hixblock
        add     cx,hixsegs
        jmp     dn1
dn2:    mov     ax,2*HIXWIDTH
dn3:    cmp     ax,di
        jb      dn4
        add     di,hixblock
        sub     cx,hixsegs
        jmp     dn3
dn4:
        mov     es,cx
        pop     cx
        pop     ax
        ret
dinormalize     endp

public onerandom
onerandom       proc    far uses ds
        ; Generate a single random number, return in AX
        mov     ax,seg ccacount
        mov     ds,ax
        mov     ax,ccaseed
        mov     bx,ax
        mov     dx,ax
        ror     bx,1
        rol     dx,1
        or      ax,bx
        xor     ax,dx
        inc     ccacount
        xor     ax,ccacount
        mov     ccaseed,ax
        ret
onerandom  endp

        public  JSetOrigin
JSetOrigin       proc    far
        ; void SetOrigin(page)
arg     _page:word

        xor     bx,bx
        cmp     _page,0
        je      @so1
        mov     bx,32*1024
@so1:
        mov     ax,40h
        mov     es,ax
        mov     dx,word ptr es:63h      ; get the 6845 address
        add     dl,6                    ; video status port

@l20:   in      al,dx
        test    al,8
        jz      @l20

@l21:   in      al,dx
        test    al,8
        jnz     @l21                    ; wait for retrace

        cli                             ; disable interrupts
        sub     dl,6                    ; back to 6845 base
        mov     ah,bh
        mov     al,0ch
        out     dx,ax                   ; Start address high

        mov     ah,bl
        inc     al
        out     dx,ax                   ; and low
        sti                             ; reenable ints
        ret
JSetOrigin       endp

        public  blattbytes
blattbytes      proc    far
        ; void blattbytes(buffer,row,page)
        ; Blatt the bytes in BUFFER onto row ROW on the EGA.
arg     buffer:dword,row:word,_page:word
local   looper:word
        ; Register usage here is tricky, since I'm trying
        ; to make this loop real fast.
        ; al -- byte to read
        ; ah -- pixel counter
        ; bl -- plane 0
        ; bh -- plane 1
        ; cl -- plane 2
        ; ch -- plane 3
        ;

        ;
        cli
        push    ds
        push    si
        push    di
        ;

        call    initegaplanar
        
        

        ; Get the source buffer
        lds     si,buffer
        
        ; calculate the dest addr
        mov     ax,0a000h
        mov     es,ax
        mov     ax,row
        mov     dx,80
        mul     dx
        add     ax,FIRSTXBYTE

        cmp     _page,1
        jnz     @@notpage1
        add     ax,32*1024                ; bytes per page
@@notpage1:
        mov     di,ax

        ; we'll use ah for our pixel counter
        mov     ah,80h
        mov     dx,03c5h
        mov     looper,HIXCOUNT

@@looper:
        lodsb

        irp     reg,<bl,bh,cl,ch>                ; shift bits into place...
        shr     al,1
        rcl     reg,1
        endm

        ror     ah,1
        jnc     @@samebyte

        mov     al,1

        irp     reg,<bl,bh,cl,ch>
        out     dx,al
        mov     es:[di],reg

        if      reg ne ch
        add     al,al
        endif

        endm

        inc     di

@@samebyte:
        dec     looper
        jnz     @@looper

        mov     dx,03c4h
        mov     ax,0f02h
        out     dx,ax

        pop     di
        pop     si
        pop     ds
        sti
        ret

blattbytes      endp

        public  doubleblattbuffer
doubleblattbuffer      proc    far
        ; void blattbytes(source,screen,page,skipper)
        ; Blatt the bytes in BUFFER onto row ROW on the EGA.
        ; Put each puppy in 4 times.


arg     source:dword,row:word,_page:word,skipper:word
        ; skipper determines whether to put a black line or not
local   sk1:byte,sk2:byte,looper:word=AUTO_SIZE
        ; Register usage here is tricky, since I'm trying
        ; to make this loop real fast.
        ; al -- byte to read
        ; ah -- pixel counter
        ; bl -- plane 0
        ; bh -- plane 1
        ; cl -- plane 2
        ; ch -- plane 3
        ;

        ;
        cli
        push    ds
        push    si
        push    di
        ;
        ;
        ; set up sk1 and sk2 with 10h and 1h
        ;
        mov     sk1,0
        mov     sk2,0
        cmp     skipper,0
        jz      @@skipper
        mov     sk1,10h
        mov     sk2,1
@@skipper:
        call    initegaplanar
        
        mov     ax,row
        dec     ax
        add     ax,ax           ; now (row-1)*2
        add     ax,ax           ; now (row-1) * 4
        mov     dx,80           ; bytes per row
        mul     dx              ; now have offset!
        add     ax,FIRSTXBYTE
        cmp     _page,0
        je      @@db1
        add     ax,32*1024
@@db1:
        ; and we do it in the middle
        mov     di,ax
        mov     ax,0a000h
        mov     es,ax

        ; now load the row
        lds     si,source

        ; we'll use ah for our pixel counter
        mov     ah,2h
        mov     dx,03c5h
        mov     looper,HIXCOUNT/8

@@looper:
        lodsb

        irp     reg,<bl,bh,cl,ch>                ; shift bits into place...
        xor     reg,reg
        shr     al,1
        jnc     @@no1&reg
        mov     reg,0e0h
        or      reg,sk1
@@no1&reg:
        endm

        lodsb                                   ; second cell

        irp     reg,<bl,bh,cl,ch>                ; shift bits into place...
        shr     al,1
        jnc     @@no2&reg
        or      reg,0eh
        or      reg,sk2
@@no2&reg:
        endm

        mov     al,1

        irp     reg,<bl,bh,cl,ch>
        out     dx,al

        irp     val,<0,1,2>
        mov     es:[di+val*80],reg
        endm

        cmp     skipper,0
        jz      @@noskip&reg

        mov     es:[di+3*80],reg     
        jmp     @@skip&reg
@@noskip&reg:
        mov     byte ptr es:[di+3*80],0
@@skip&reg:
        if      reg ne ch
        add     al,al
        endif

        endm

        inc     di

@@samebyte:
        dec     looper
        jnz     @@looper

        mov     dx,03c4h
        mov     ax,0f02h
        out     dx,ax

        pop     di
        pop     si
        pop     ds
        sti
        ret

doubleblattbuffer      endp



        ; lores regular output

        ; void loregular(char far *buffer)
        ;

        public  loregular
loregular       proc    far
arg     buffer:dword,screen:dword
        cli
        push    si
        push    di
        push    ds

        lds     si,buffer
        les     di,screen
        call    normalize

        cli
        mov     dx,03ceh

	mov	al,5		;select MODE register
	out	dx,al
	inc	dx
	mov	al,2		;select write mode 2 PACKED
	out	dx,al
	dec	dx		;dx holds graphics controller register again.

	mov	al,8		;BITMASK register
	out	dx,al
	inc	dx		;ready now to keep outputting bitmasks.
	mov	al,0C0h	;Keep bitmask in al, start with 1100 0000.

	mov	cx,MEDXCOUNT
loegapixel:
	out	dx,al		;set the bitmask
	mov	ah,[si]		;get state from newbufseg
	mov	es:[di],ah	;write new colors
	inc	si			;go to next color
	ror	al,1
	ror	al,1
	jc	newloegabyte
	mov	ah,es:[di]	;latch byte values.
	dec	di			;correction of automatic up step
newloegabyte:
	inc	di
        loop    loegapixel
nextloegarow:
	add	di,FIRSTXBYTE	;skip over menu bytes
	dec	dx		;go back to controller register
	mov	al,5		;select MODE register
	out	dx,al
	inc	dx
	mov	al,1		;go to write mode 1 for	fast copy
	out	dx,al
	dec	dx		;point	back to controller

        ; reload si and di 
        lds     si,screen
        lds     di,screen
        add     di,80

	mov	cx,LOXCOUNT 	        ;8 bit bytes
	REP	MOVSB			;copy the bytes just written 

        mov     dx,03ceh
        mov     al,5
        out     dx,al
        inc     dx
        mov     al,0
        out     dx,al

        sti
        pop     ds
        pop     di
        pop     si
        sti

        ret

loregular  endp



        public  locoarse
locoarse        proc    far
        ; void locoarse(char far *buffer,char far *screen,int skipper)
arg     buffer:dword,screen:dword,skipper:word
        cli
        push    si
        push    di
        push    ds

        mov     bx,0
        cmp     skipper,0
        jz      @@lc1
        inc     bx
@@lc1:

        lds     si,buffer
        les     di,screen

        cli
        mov	dx,03CEh
	mov	al,5		;select MODE register
	out	dx,al
	inc	dx
	mov	al,2		;select write mode 2 PACKED
	out	dx,al
	dec	dx

	mov	al,8		;select BIT MASK register
	out	dx,ax
	inc	dx
	mov	al,0FFh	;get ready to set 8 bits
	out	dx,al
	xor	al,al
	mov	es:[di],al	;turn all pixels to zero
	mov	al,es:[di]	;read the zeroes into the latches

	mov	al,0FEh	;from now on, always set 7 bits
        add     al,bl
	out	dx,al
	dec	dx
        
	mov	cx,LOXCOUNT 
locoarseegapixel:
        lodsb

        irpc    where,<0123456>
        mov     es:[di+where*80],al
        endm

        or      bl,bl
        jz      @@lc3
        mov     es:[di+7*80],al
        jmp     short @@lc4
@@lc3:
        mov     byte ptr es:[di+7*80],0
@@lc4:
        inc	di
        loop    locoarseegapixel
	                           ;now go down to next row 
                                   ;and move left to edge of patch
donecoarseegamove:
	mov	dx,03CEh
	mov	al,5		;select MODE register
	out	dx,al
	inc	dx
	mov	al,0		;restore	write mode 0
	out	dx,al

        pop     ds
        pop     di
        pop     si
        sti
        ret
locoarse        endp

        public  build8sumtable
        ; void build8sumtable(void)

build8sumtable  proc    far

        push    di
        push    ds

        mov     ax,seg eightsumtable
        mov     ds,ax
;fixeightsumtable:
	xor	di,di		;index into the table
@@fix:
	mov	bx,di	;copy the index so as to strip and add its bits
	xor	ax,ax	;use ah as a standard 0, accumulate in al
	shr	bx,1		;pattern is 693582471, we want all but 5.
	adc	al,ah	;1
	shr	bx,1
	adc	al,ah	;7
	shr	bx,1
	adc	al,ah	;4
	shr	bx,1
	adc	al,ah	;2
	shr	bx,1
	adc	al,ah	;8
	shr	bx,1		;5 gets skipped
	shr	bx,1
	adc	al,ah	;3
	shr	bx,1
	adc	al,ah	;9
	shr	bx,1
	adc	al,ah	;6
	mov	eightsumtable[di],al	;put the sum in its place
	inc	di		;next spot
	cmp	di,512
	jne     @@fix

        pop     ds
        pop     di
        ret
build8sumtable  endp

        public  jloadlookuptable

jloadlookuptable        proc    far
        ; jloadlookuptable(char far *,int,int,int)
arg     lookuptable:dword,_increment:word,_maxstate:word,_catype:word

        push    di
        push    si
        push    ds

        les     di,lookuptable
        mov     ax,_catype
        cmp     ax,0
        jnz     not0
        jmp     catype0

not0:
not1:   cmp     ax,2
        jnz     not2
        jmp     catype2
not2:   

        ; Hokay, so it's gotta be catype3...

catype3:
;tube rule. see z.pas in rudy's calab\ca directory.
	xor	dx,dx
	xor	di,di
	mov	cx,0FFFh	; Twelve bits for firing eightsum high, eightbit low.
        mov     ax,seg tubealarm
        mov     ds,ax
loadentryca3:
	;I view dx as holding firingeightsum and eightbit  in dh, dl
	; respectively, and I put the rule value in bl.

	push	cx				;because i need it as a parameter

	mov	bh,1			; use bh to signify CoolCondition
	cmp	dh,tubealarm
	jb	donealarm		; cool if under alarm
	mov	bh,0			;assume uncool
	cmp	tubefuzz,1
	jne	donealarm
	mov	cl,dh		;use cl as temporary holder of eighsum
	dec	cl			;is eightsum-1= alarm, same as eightsum=alarm+1
	cmp	cl,tubealarm
	jne	donealarm
	mov	bh,1
donealarm:		;use bh to keep alarmflag

	mov	cl,tubelive	;use cl to keep highodd
	shl	cl,1
	dec	cl

	mov	ch,tuberest	;use ch to keep higheven
	shl	ch,1
	sub	ch,2

	;crop by setting bl = dl MOD min(highodd+1,higheven)
	mov	bl,ch
	cmp	ch,cl
	ja	blset
	mov	bl,cl
	inc	bl
blset:
	mov	ah,0
	mov	al,dl
	div	bl		;division remainder goes in ah
	mov	bl,ah	;put remainder in bl

	test	dl,1
	jnz	oddruleca3
	jmp	evenruleca3

oddruleca3:
	cmp	dl,cl
	ja	doneruleca3
	cmp	dl,cl
	jne	nothighodd
	mov	bl,0
	jmp	doneruleca3

nothighodd:
	cmp	dl,1
	jne	notone3
	mov	bl,1
	cmp	bh,1			;cool?
	je	doneruleca3	;mellow!
	mov	bl,3			;karma time
	jmp	doneruleca3

notone3:
	mov	bl,dl
	add	bl,2
	jmp	doneruleca3

evenruleca3:
	cmp	dl,ch
	ja	doneruleca3	;mod the big
	mov	bl,dl
	add	bl,2
	cmp	dl,ch		;not max?
	jb	doneruleca3	;bail
	mov	bl,1			;assume cool
	cmp	bh,1			;cool?
	je	doneruleca3	;mellow
	mov	bl,3			;karma
doneruleca3:
	mov	es:[di],bl
	inc	dx
	inc	di
	pop	cx
	dec	cx
	jcxz endcatype3
	jmp	loadentryca3

endcatype3:
        jmp     doneinitlookup

catype2:
        mov     ax,seg maxeatstate
        mov     ds,ax
        xor     dx,dx
        xor     di,di
        mov     cx,0ffffh
loadentryca2:
        xor     ah,ah
        mov     al,dl
        mov     bl,maxeatstate
        inc     bl
        div     bl
        mov     bl,ah   ;now bl is (self) mode (maxeatstate+1)
        xor     ah,ah
        mov     al,dh
        mov     bh,maxeatstate
        inc     bh
        div     bh
        mov     bh,ah           ; bh is name mode (maxeat+1)
selfinrange:
        mov     al,bl
        inc     al
        cmp     al,maxeatstate
        jbe     eaterset
        xor     al,al
eaterset:
        cmp     al,bh
        jne     ruledoneca2
        mov     bl,al
ruledoneca2:
        mov     es:[di],bl
        inc     dx
        inc     di
        loop    loadentryca2
        jmp     doneinitlookup


catype0:
	xor	dx,dx
	mov	cx,0FFFFh ; Sixteen bits for	eightbit high, eightaverage low.
LoadEntryca0:
	;I view dx as holding eightbit and eightaverage in dh, dl, respectively,
	;and I put the rule value in bl.
	cmp	dh,0
	jnz	NonZeroRuleca0
;ZeroRule:
	xor	bl,bl
	or	dl,dl
	jz	RuleDone		;if there's any action
	mov	bl,byte ptr _increment	;get turned on to (increment/2)+1
	shr	bl,1
	inc	bl
	cmp	dl,byte ptr _increment	;unless I'm bigger than increment
	jb	RuleDone
	mov	bh,byte ptr _increment	;and less than 2 times the increment
	shl	bh,1
	cmp	dl,bh
	ja	RuleDone
	mov	bl,byte ptr _increment	;in which case I turn on to increment.
	jmp	RuleDone
NonZeroRuleca0:
	cmp	dh,byte ptr _maxstate
	jb	NotMaxRule
	xor	bl,bl
	jmp	RuleDone
NotMaxRule:
	mov	bl,dl
	add	bl,byte ptr _increment
	cmp	bl,byte ptr _maxstate
	jb	RuleDone
	mov	bl,byte ptr _maxstate
RuleDone:
	mov	es:[di],bl
	inc	dx
	inc	di
	loop	LoadEntryca0
endcatype0:
doneinitlookup:
        pop     ds
        pop     si
        pop     di
        ret
jloadlookuptable        endp

        
initegaplanar   proc    near
	mov	dx,03CEh	;graphics contoller register
	mov	al,8		;BITMASK register
	out	dx,al
	inc	dx
	mov	al,0FFh	        ;will always reset all eight pixels
	out	dx,al	        ;I am going to use planar write.
	mov	dx,03C4h	;sequencer register
	mov	al,2		;write plane register
	out	dx,al		;set up for	addressing
        ret
initegaplanar   endp

public  normalize_memcpy
normalize_memcpy        proc far
        ; void normalize_memcpy(char *s,char *d,int count)
arg     source:dword,dest:dword,count:word

        ; Normalize the pointers and copy memory. We might want
        ; to speed this up with intelligence about word and
        ; byte moves, but its not critical enough to spend the
        ; energy on it.
        push    ds
        push    si
        push    di

        lds     si,source
        call    normalize
        les     di,dest
        call    dinormalize
        mov     cx,count
        rep     movsb

        pop     di
        pop     si
        pop     ds
        ret
normalize_memcpy        endp

        public  normalize_memset
normalize_memset proc far
        ; void normalize_memset(char huge *d,char c,int count);
arg     buffer:dword,char:byte,xx:byte,count:word

        push    di
        les     di,buffer
        call    dinormalize
        mov     cx,count
        mov     al,char
        rep     stosb
        pop     di
        ret
normalize_memset        endp


public  ca_randomize
ca_randomize    proc    far uses di,buffer:dword,xcount:word
        mov cx, xcount
        les di, dword ptr buffer
        mov ax, ccaseed
carloop:
        mov bx, ax
        mov dx, bx
        ror bx, 1
        rol dx, 1
        or ax, bx
        xor ax, dx

        inc word ptr ccacount
        xor ax, ccacount
        stosw
        loop carloop
        mov ccaseed, ax
        ret
ca_randomize    endp

        end
        

 
