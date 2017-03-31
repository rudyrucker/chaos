        .model  large,c
        LOCALS
        .code
        public rowblast

initegaplanar   proc    far
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


public blast_in_row

blast_in_row proc far uses di si ds,buffer:dword,y:word
        ; void blast_in_row(char far *buffer,int y)
        ; Read the whole row i into the buffer, a plane at
        ; a time. Should be faster than picking repeatedly.

        les     di,buffer

        ; calculate the source address
        mov     ax,0a000h


        mov     ds,ax

        mov     ax,y
        mov     dx,80
        mul     dx
        mov     si,ax

        mov     dx,3ceh
        mov     ax,0005h
        out     dx,ax           ; read mode 0
        mov     ax,0004h        
        mov     cx,4

@@looper:
        push    cx              ; save count
        out     dx,ax           ; output row to read
        push    si              ; push source address
        mov     cx,40
        rep     movsw
        pop     si
        pop     cx
        inc     ah
        loop    @@looper

        mov     dx,03c4h
        mov     ax,0f02h
        out     dx,ax

        ret
blast_in_row    endp




        

        

rowblast proc far
        ; void rowblast(unsigned char far *buffer, int x,int y,int count)
arg     buffer:dword,x:word,y:word,count:word
        ; Strategy: In memory, where it is fast, we strip the bits
        ; off of the data. Then we blast a plane of memory at a time.

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
        mov     ax,y
        mov     dx,80
        mul     dx
        mov     bx,x
        shr     bx,1
        shr     bx,1
        shr     bx,1
        add     ax,bx

        mov     di,ax

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
        dec     count
        jnz     @@looper

        mov     dx,03c4h
        mov     ax,0f02h
        out     dx,ax

        pop     di
        pop     si
        pop     ds

        
        ret
rowblast        endp

        end
