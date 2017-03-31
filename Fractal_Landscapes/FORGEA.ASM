DOSSEG
LOCALS
.MODEL LARGE,C

.code
        public  PixelAddr10
        ; This is lifted from Wilton.
        ; caller:       AX=y
        ;               BX=x
        ; returns:      AH=bitmask
        ;               BX=byte offset
        ;               CL=left shift count
        ;               ES=video buffer seg
        ;
PixelAddr10     proc    near
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
        public  HLine
        ; Draw a horizontal line from X1,Y1 to X2,Y2
        ; void HLine(int X1,int Y1,int X2,char color,int rasterop)
HLine   proc    far uses si di ds,X1:word,Y1:word,X2:word,color:word,raster_op:word
        cli
        mov     dx,3ceh
        mov     ah,byte ptr color
        xor     al,al
        out     dx,ax
        mov     ax,0f01h
        out     dx,ax
        mov     ah,byte ptr raster_op
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

        sti
        ret
HLine   endp

public  SetPixel10              ; from Wilton
SetPixel10      proc    far,x:word,y:word,color:byte
        cli
        mov     ax,y
        mov     bx,x
        call    PixelAddr10
        shl     ah,cl
        mov     dx,3ceh
        mov     al,8
        out     dx,ax
        mov     ax,205h
        out     dx,ax
        mov     ah,18h
        mov     al,3
        out     dx,ax

        mov     al,es:[bx]
        mov     al,color
        mov     es:[bx],al

        mov     ax,0ff08h
        out     dx,ax
        mov     ax,5
        out     dx,ax
        mov     ax,3
        out     dx,ax
        sti
        ret

SetPixel10      endp

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

        public  blattbytes
blattbytes      proc    far uses ds di si,buffer:dword,x:word,y:word,len:word
        ; void blattbytes(buffer,row,page)
        ; Blatt the bytes in BUFFER onto row ROW on the EGA.
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
        ;

        call    initegaplanar
        
        

        ; Get the source buffer
        lds     si,buffer
        
        ; calculate the dest addr
        mov     ax,0a000h
        mov     es,ax
        mov     ax,y
        mov     dx,80
        mul     dx

        ; now we want x/8
        mov     di,ax
        mov     ax,x
        shr     ax,1
        shr     ax,1
        shr     ax,1
        add     di,ax

        mov     ax,len
        mov     looper,ax
        ; we'll use ah for our pixel counter
        mov     ah,80h
        mov     dx,03c5h

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

        sti
        ret

blattbytes      endp

public  PixelAddr13
PixelAddr13     proc near
        xchg    ah,al
        add     bx,ax
        shr     ax,1
        shr     ax,1
        add     bx,ax
        mov     ax,0a000h
        mov     es,ax
        ret
PixelAddr13     endp


public  VGALine
VGALine proc far uses si di,x1:word,y1:word,x2:word,y2:word,color:byte
local   incr1:word,incr2:word,routine:word

        mov     si,320          ; initial y-inc
        mov     cx,x2
        sub     cx,x1
        jz      VertLine13

;force x1 < x2
        jns     @@L01
        neg     cx
        mov     bx,x2           ; exchange x1,x2
        xchg    bx,x1
        mov     x2,bx

        mov     bx,y2
        xchg    bx,y1
        mov     y2,bx           ; and y1 and y2

@@L01:  
        ; calculate dy = abs(y2-y1)
        mov     bx,y2
        sub     bx,y1
        jz      HorizLine13

        jns     @@L03           ; jump if slope is positive

        neg     bx
        neg     si              ; negate y-increment

; select appropriate routine for slope of line
@@L03:
        push    si              ; preserve yinc
        mov     routine,offset LoSlopeLine13
        cmp     bx,cx
        jle     @@L04           ; jmp if slope <= 1
        mov     routine,offset HiSlopeLine13
        xchg    bx,cx           ; exchange dy,dx


@@L04:  
        shl     bx,1            ; BX = 2 * dy
        mov     incr1,bx        ; incr1 = 2 * dy
        sub     bx,cx
        mov     si,bx           ; si = d = 2*dy-dx
        sub     bx,cx
        mov     incr2,bx        ; incr2 = 2*(dy-dx)

; calculate first pixel addr
        push    cx
        mov     ax,y1
        mov     bx,x1
        call    PixelAddr13
        mov     di,bx
        pop     cx
        inc     cx
        pop     bx
        jmp     routine

VertLine13:
        mov     ax,y1
        mov     bx,y2
        mov     cx,bx
        sub     cx,ax
        jge     @@L31

        neg     cx
        mov     ax,bx

@@L31:
        inc     cx
        mov     bx,x1
        push    cx
        call    PixelAddr13
        pop     cx
        mov     di,bx
        dec     si
        mov     al,color

@@L32:
        stosb
        add     di,si
        loop    @@L32
        jmp     @@Lexit

HorizLine13:
        push    cx
        mov     ax,y1
        mov     bx,x1
        call    PixelAddr13
        mov     di,bx

        pop     cx
        inc     cx
        mov     al,color
        rep     stosb
        jmp     @@Lexit

LoSlopeLine13:
        mov     al,color
@@L11:
        stosb
        or      si,si
        jns     @@L12
        add     si,incr1
        loop    @@L11
        jmp     @@Lexit

@@L12: 
        add     si,incr2
        add     di,bx
        loop    @@L11
        jmp     @@Lexit

HiSlopeLine13:
        mov     al,color
@@L21:  
        stosb
        add     di,bx
@@L22:
        or      si,si
        jns     @@L23

        add     si,incr1
        dec     di
        loop    @@L21
        jmp     @@Lexit

@@L23:  
        add     si,incr2
        loop    @@L21

@@Lexit:
        ret

VGALine        endp

public  DisplayChar13

DisplayChar13   proc far uses di si ds,argc:word,argx:word,argy:word,fgd:byte,bgd:byte
BytesPerLine    equ     320

        mov     ax,argy
        mov     bx,argx
        call    PixelAddr13
        mov     di,bx
        mov     ax,40h
        mov     ds,ax
        mov     cx,ds:[85h]
        xor     ax,ax
        mov     ds,ax
        mov     ax,argc
        mov     bx,43h*4
        lds     si,ds:[bx]
        mul     cl
        add     si,ax

        mov     bl,fgd
        mov     bh,bgd

@@L10:
        push    cx
        mov     cx,8
        lodsb
        mov     ah,al
@@L11:
        mov     al,bl
        shl     ah,1
        jc      @@L12

        inc     di
        jmp     short   @@L13

@@L12:  stosb
@@L13:  loop    @@L11

        add     di,BytesPerLine - 8
        pop     cx
        loop    @@L10

        ret

DisplayChar13   endp






        



end
