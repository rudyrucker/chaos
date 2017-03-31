; Automata routines for Zhabo

        .model  large,c
        .8086
        JUMPS                           ; enable jump aliasing
        LOCALS                          ; enable local labels


        .data
extrn   lookuptable:dword
extrn  eightsumtable:byte
extrn  eatmode:word
extrn  maxeatstate:byte
extrn  eatmods:dword
EXTRN ccacount:word
EXTRN ccaseed:word

        .code
        public  updaterowcao0
updaterowcao0   proc    far
        ;
        ; void updaterowcao0(unsigned char *dest,
        ;                    unsigned char *source,
        ;                    unsigned runwidth,
        ;                    unsigned rowwidth)
        ;
arg     dest:dword,source:dword,runwidth:word,rowwidth:word
        ; runwidth is the number of cells to process
        ; rowwidth is the number of bytes per line,
        ; including the otiose ones on the corners
local   counter:word
local   lut:dword
local   wx2m1:word=AUTO_SIZE
        ; note this is C mode, so stack frame is set up automatically
        push    ds
        push    di
        push    si

        ; Set up some locals
        mov     ax,rowwidth
        add     ax,ax
        dec     ax
        mov     wx2m1,ax

        mov     ax,seg lookuptable
        mov     ds,ax
        les     di,lookuptable
        mov     word ptr lut,di
        mov     word ptr lut+2,es

        ; Get the dest pointer
        lds     si,source
        ; Get the count
        mov     cx,runwidth
        add     cx,7
        ; Divide by 8
        shr     cx,1
        shr     cx,1
        shr     cx,1
        mov     counter,cx
@@OneEighthofRow:
        mov     cx,8

@@18loop:
        xor     bh,bh
        xor     ah,ah
        sub     si,rowwidth             ; NW in dl, N in dh
        dec     si
        mov     dx,[si]
        mov     al,dl                   ; NW
        add     al,dh                   ; N
        adc     ah,bh
        inc     si
        inc     si
        add     al,[si]                 ; right two to NE
        adc     ah,bh
        add     si,wx2m1
        mov     dx,[si]                 ; S and SE
        add     al,dl                   ; S
        adc     ah,bh
        add     al,dh                   ; SE
        adc     ah,bh
        dec     si
        add     al,[si]
        adc     ah,bh                   ; SW
        sub     si,rowwidth             ; up a row
        mov     dx,[si]                 ; W in dl, C in dh, wait with C
        add     al,dl                   ; W
        adc     ah,bh
        inc     si
        inc     si
        add     al,[si]                 ; E; leave SI at E
        adc     ah,bh
        shr     ax,1
        shr     ax,1
        shr     ax,1                    ; 3 shifts for 8average, now in al
        mov     ah,dh                   ; C was waiting

; Look up the new state
        les     di,lut
        add     di,ax
        mov     al,es:di
        les     di,dest
        stosb
        mov     word ptr dest,di

        loop    @@18loop
        dec     counter
        jnz     @@OneEighthofRow

        pop     si
        pop     di
        pop     ds
        ret
updaterowcao0   endp

shiftin macro
        mov     bh,[si]
        shr     bx,1
        endm


        public  updaterowcao1
updaterowcao1   proc    far
        ;
        ;
        ; void updaterowcao1(unsigned char *dest,
        ;                    unsigned char *source,
        ;                    unsigned runwidth,
        ;                    unsigned rowwidth)

arg     dest:dword,source:dword,runwidth:word,rowwidth:word
        ; runwidth is the number of cells to process
        ; rowwidth is the number of bytes per line,
        ; including the otiose ones on the corners
local   counter:word
local   lutseg:word
        push    ds
        push    di
        push    si

        les     di,lookuptable
        mov     lutseg,es       
        lds     si,source
        mov     dx,rowwidth

;We maintain the ninebit neighborhood 369582471 in bx where pattern
;is	1 2 3 A
;	4 5 6 B
;	7 8 9 C
;and	then we use a lookup to get the sum of all but the central
;bit. So at the start of the row you have to get bx started with left 6.
;as 0000 000C | SN WSwNw ---

        xor     bx,bx
        sub     si,dx
        dec     si
        
        shiftin                  ; NW neighbor

        add     si,dx
        add     si,dx           ; SW
        shiftin

        sub     si,dx           ; W
        shiftin

        sub     si,dx
        inc     si
        shiftin                 ; N

        add     si,dx
        add     si,dx
        shiftin                 ; S

        sub     si,dx           ; C
        mov     bh,[si]
        and     bh,1

        mov     cx,runwidth
        add     cx,7
        shr     cx,1            ; /2
        shr     cx,1            ; /4
        shr     cx,1            ; /8
        mov     counter,cx

@@one_eighth:
        mov     cx,8

@@cellcao1:
;FiringEightSumAH:
;We maintain the ninebit neighborhood 693582471 in bx where pattern
;is	1 2 3 A
;	4 5 6 B
;	7 8 9 C
;and	then we use a lookup to get the sum of all but the 5 bit
;We start with one of these bits up in bh, and	the others in bl
;we want to discard three low bits of bl.
        mov     al,[si]         ; get self in al
        shr     bx,1            ; discard old bit 1, bring down old 3

        sub     si,dx
        inc     si        
        shiftin                 ; NE

        add     si,dx
        add     si,dx
        shiftin                 ; SE

        sub     si,dx           ; point to E, leave SI there
        mov     bh,[si]
        and     bh,1

; Now reference through the eightsumtable
        mov     di,seg eightsumtable
        mov     es,di
        mov     ah,es:eightsumtable[bx]
        mov     es,lutseg
        mov     di,ax
        mov     al,es:[di]
        les     di,dest
        stosb
        mov     word ptr dest,di
        loop    @@cellcao1

        dec     counter
        jnz     @@one_eighth
        pop     si
        pop     di
        pop     ds
        ret
updaterowcao1   endp

        public  updaterowcao2
updaterowcao2   proc    far
        ; This one wants to pick random kittens. Perhaps I'll
        ; allow the deterministic version as well. This is the
        ; EATER.
arg     dest:dword,source:dword,runwidth:word,rowwidth:word
        ; runwidth is the number of cells to process
        ; rowwidth is the number of bytes per line,
        ; including the otiose ones on the corners

local   neighbors:word:8
local   emode:word=AUTO_SIZE
        push    ds
        push    di
        push    si

        ; Initialize the neighbor table at the start of each row.
        mov     bx,rowwidth
        neg     bx
        dec     bx
        mov     neighbors,bx
        inc     bx
        mov     neighbors+2,bx
        inc     bx
        mov     neighbors+4,bx
        mov     neighbors+6,-1
        mov     neighbors+8,1
        mov     bx,rowwidth
        dec     bx
        mov     neighbors+10,bx
        inc     bx
        mov     neighbors+12,bx
        inc     bx
        mov     neighbors+14,bx

        mov     ax,seg eatmode
        mov     ds,ax
        mov     ax,eatmode
        mov     emode,ax
        lds     si,source

        mov     cx,runwidth
        mov     dx,rowwidth

@@looper:
        ; Either pick a random neighbor, or check EACH
        ; of the neighbors for the new state. Hm. This fucker
        ; doesn't really need a LUT; rather, I need to determine
        ; (C+1) mod N and then compare each of them. SO I really
        ; only need a ModN table; let's build that and use it.

        cmp     emode,0
        jz      @@randomeater

        lds     si,source
        ; OK, get my value...
        lodsb
        mov     word ptr source,si
        dec     si     ; AHA! But MEANWHILE I need the correct value- RR.
        mov     bl,al
        xor     bh,bh       
        mov     di,seg eatmods
        mov     es,di
       les     di,es:eatmods  
       mov     al,es:[di+bx]   ; replace cell val by value mod maxeat
       inc     bx              ; should be the state to eat source
       mov     dl,es:[di+bx]   ; value to eat cell mod maxeat

        ; OK, now DL has the value we want to compare all
        ; the neighbors with.
        ; for each neighbor I'll get his val; and if I'm past the
        ; second cycle he's already been modded into range maxeat
        ; I don't use [si+1] because this costs 9 clocks vs
        ; inc si, [si] which costs 7 clocks total.

        dec     si
        sub     si,rowwidth             ; NW
        cmp     dl,[si]
        jz      @@gotit
        inc     si
        cmp     dl,[si]                 ; N
        jz      @@gotit
        inc     si
        cmp     dl,[si]                 ; NE
        jz      @@gotit
        add     si,rowwidth
        cmp     dl,[si]                  ; W
        jz      @@gotit
        sub     si,2
        cmp     dl,[si]                  ; E
        jz      @@gotit
        add     si,rowwidth
        cmp     dl,[si]                 ; SE
        jz      @@gotit
        inc     si
        cmp     dl,[si]                  ; S
        jz      @@gotit
        inc     si
        cmp     dl,[si]               ; SE
        jz      @@gotit

;Here's a four neighbor version , diamond style (sides)
;        cmp     dl,[si-1]               ; W
;        jz      @@gotit
;        cmp     dl,[si+1]               ; E
;        jz      @@gotit
;        sub     si,rowwidth
;        cmp     dl,[si]                  ; N
;        jz      @@gotit
;        add     si,rowwidth
;        add     si,rowwidth
;        cmp     dl,[si]                  ; S
;        jz      @@gotit

;Here's a four neighbor version , dice five-spot style (corners)
;        sub     si,rowwidth
;        cmp     dl,[si -1]                  ; NW
;        jz      @@gotit
;        cmp     dl,[si+1]                  ; NE
;        jz      @@gotit
;        add     si,rowwidth
;        add     si,rowwidth
;        cmp     dl,[si-1]                  ; SW
;        jz      @@gotit
;        cmp     dl,[si+1]                  ; SE
;        jz      @@gotit

        ; Didn't match; thing doesn't change
        jmp     @@stashit

@@gotit:
        ; Did match, put in the mod value
        mov     al,dl
@@stashit:
        les     di,dest
        stosb
        mov     word ptr dest,di
        dec cx
        jcxz    @@ca2done
        jmp    @@looper

@@randomeater:
        ; Pick a random neighbor
        mov     ax,seg ccacount
        mov     es,ax
        mov     ax,es:ccaseed
        mov     bx,ax
        mov     dx,ax
        ror     bx,1
        rol     dx,1
        or      ax,bx
        xor     ax,dx
        inc     es:ccacount
        xor     ax,es:ccacount
        mov     es:ccaseed,ax
        mov     bx,ax
        and     bx,7
        shl     bx,1

        ; Now get a neighbor
        mov     di,bx
        mov     bx,neighbors[di]
        mov     ah,[si+bx]
        lodsb        
  
        ; now AH has a neighbor value,
        ; AL has current value

        mov     di,seg eatmods     ;access to mod table
        mov     es,di
        les     di,es:eatmods
        mov     dx,ax
        
        xor     bh,bh
        mov     bl,al
        mov     al,es:[di+bx]     ;get modded value of cell 
        inc     bx
        mov     dl,es:[di+bx]     ; get modded value of cell eater
;        mov     bl,ah            ; don't need to mod neighbor anymore
;        mov     dh,es:[di+bx]    ; after 2nd generation
        cmp     dl,ah
        jnz     @@stashit
        mov     al,ah
        jmp     @@stashit

@@ca2done:
        pop     si
        pop     di
        pop     ds
        ret
updaterowcao2   endp


        end


