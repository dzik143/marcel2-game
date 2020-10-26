; ==============================================================================
; =                                                                            =
; = Author: Sylwester Wysocki <sw143@wp.pl>                                    =
; = Created on: December 2002 to January 2003                                  =
; =                                                                            =
; = This is free and unencumbered software released into the public domain.    =
; =                                                                            =
; = Anyone is free to copy, modify, publish, use, compile, sell, or            =
; = distribute this software, either in source code form or as a compiled      =
; = binary, for any purpose, commercial or non-commercial, and by any          =
; = means.                                                                     =
; =                                                                            =
; = In jurisdictions that recognize copyright laws, the author or authors      =
; = of this software dedicate any and all copyright interest in the            =
; = software to the public domain. We make this dedication for the benefit     =
; = of the public at large and to the detriment of our heirs and               =
; = successors. We intend this dedication to be an overt act of                =
; = relinquishment in perpetuity of all present and future rights to this      =
; = software under copyright law.                                              =
; =                                                                            =
; = THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,            =
; = EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF         =
; = MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     =
; = IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR          =
; = OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,      =
; = ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR      =
; = OTHER DEALINGS IN THE SOFTWARE.                                            =
; =                                                                            =
; = For more information, please refer to <https://unlicense.org>              =
; =                                                                            =
; ==============================================================================

; BX = ile paragrafow wczytac
; CS:[BP] = tu zostanie zwrocony segment
; CS:[DX] = adres buforu z nazwa pliku w kodzie ASCIIZ

LoadFile PROC
                push  ds

                mov   ah, 48h
                int   21h             ;AX = zar. segment
                jc    LowMem
                mov   cs:[bp], ax     ;zapisz numer segmentu
                push  ax              ;SS:[SP] = segment

                push  cs
                pop   ds              ;DS = CS (nazwa ma byc w CS)
                shl   bx, 4           ;paragrafy => bajty
                mov   cx, bx          ;CX = ile bajtow wczytac
                mov   ax, 3d00h       ;Otworz plik
                int   21h             ;AX = uchwyt
                mov   bx, ax          ;BX = uchwyt

                pop   ds              ;SS:[SP] => DS  (zar. segment)
                xor   dx, dx          ;DS:DX = pierwszy zar. bajt
                mov   ah, 3fh
                int   21h             ;File => DS:[DX] rep CX

                mov   ah, 3eh         ;zamknij plik
                int   21h

                pop   ds
                RET
ENDP


       Etap   db  0             ;Aktualny etap
 EtapPlikOfs  dw (Offset EtapPlik1), (Offset EtapPlik2), (Offset EtapPlik3)
   EtapPlik1  db 'etap1',0
   EtapPlik2  db 'etap2',0
   EtapPlik3  db 'etap3',0

   EtapTekst  dw (Offset EtapTekst1), (Offset EtapTekst2), (Offset EtapTekst3)
  EtapTekst1  db 'E T A P  1 - 1$'
  EtapTekst2  db 'E T A P  1 - 2$'
  EtapTekst3  db 'E T A P  1 - 3$'

  EtapPlener  dw (Offset EtapPlener1), (Offset EtapPlener2), (Offset EtapPlener3)
  EtapPlener1 db 'MIASTO.SFG',0
  EtapPlener2 db 'MIASTO.SFG',0
  EtapPlener3 db 'MIASTO.SFG',0

   EtapDeszcz db 1,0,0
    EtapSnieg db 0,0,0
    EtapWoda  dw 16*24+8,16*27+8,0ffffh

LoadEtap PROC
               mov     dx, 3d4h
               mov     ax, 0109h                            ;Wysokosc
               out     dx, ax

               push    cs
               pop     ds                     ;DS = CS
               mov     bl, ds:[Etap]
               xor     bh, bh                 ;BX = nr. etapu
               shl     bl, 1                  ;BX = adres nazwy wzg.[EtapPlik]
               add     bx, offset EtapPlikOfs ;BX = adres wzg. CS

               mov     dx, ds:[bx]            ;DS:[DX] = adres nazwy etapu
               mov     ax, 3d00h
               int     21h
               mov     bx, ax                 ;BX = uchwyt

               lea     dx, IDEtp              ;DS:DX = CS:[IDEtp]
               mov     cx, 4+7                ;SzerEtp i WysEtp + ID(7)
               mov     ah, 3fh
               int     21h

               mov     ax, cs:[SzerEtp]
               mul     word ptr cs:[WysEtp]   ;Teraz AX = rozmiar etapu
               mov     cx, ax                 ;CX = dlugosc etapu

               mov     ds, cs:[EtapSeg]
               xor     dx, dx                 ;DS:DX = Etap:[0]
               mov     ah, 3fh
               int     21h
        ;============= DANE OBIEKTOW ==========
               push    cs
               pop     ds
               lea     dx, IleObkt            ;DS:DX = cs:[IleObkt]
               mov     cx, 2
               mov     ah, 3fh
               int     21h

               mov     ax, ObktLEN            ;AX = rozmiar 1 obiektu
               mul     word ptr cs:[IleObkt]  ;AX = IleObkt*ObktLEN
               mov     cx, ax

               mov     ds, cs:[Obiekty]
               xor     dx, dx                 ;DS:DX = Obiekty:[0]
               mov     ah, 3fh
               int     21h

               mov     ah, 3eh                ;Zamknij dziada
               int     21h
        ;============ USTAW ZMIENNE ETAPU =============
               mov     ax, gs:[y]
               shr     ax, 4 / sub ax, 5
               mov     bx, cs:[WysEtp] / sub bx, 20
               IF     (ax>bx) {mov  ax, bx}
               mov     cs:[KameraY], ax

               mov     ax, gs:[x]
               shr     ax, 4
               sub     ax, 2
               mov     cs:[KameraX], ax

               mov     bl, cs:[Etap]
               xor     bh, bh
               mov     al, cs:[EtapDeszcz+bx] / mov cs:[DeszczFlag], al
               mov     al, cs:[EtapSnieg+bx] / mov cs:[SniegFlag], al
               shl     bl, 1
               mov     ax, cs:[EtapWoda+bx] / mov cs:[PoziomWody], ax
               mov     dx, cs:[EtapPlener+bx] / call LoadPlener
               mov     cs:[Stan], 0

      ;================ NAPIS ==============
               xor     dx, dx / call SetPal
               mov     ax, 0a000h / mov es, ax / call Cls
               call    RysujEtap / call BuforToScreen
               push    0a000h / pop  es
               mov     bl, cs:[Etap]
               xor     bh, bh
               shl     bl, 1
               mov     bp, cs:[EtapTekst+BX]
               mov     di, 320*90+100
               mov     bh, 25
               call    PiszG

WaitSpace:     cmp     cs:[Key+39h],1 / jne WaitSpace
               RET
ENDP

;WE: CS:DX = adres nazwy
LoadPlener PROC
               push  ds

               push  cs
               pop   ds
               mov   ax, 3d00h
               int   21h
               mov   bx, ax

               mov   ds, cs:[PlenerSeg]
               xor   dx, dx
               mov   cx, 64000
               mov   ah, 3fh
               int   21h

               mov    ah, 3eh
               int    21h

               pop    ds
               RET
ENDP

Sleep PROC
            mov       ah, 2ch            ;Pytanie o czas
            int       21h                ;DH = sekundy
            mov       bl, dh
Wait@:      int       21h
            sub       dh, bl
            cmp       dh, 3
            jb        Wait@
            RET
ENDP

 LowMemTekst db 'Brak pamieci!!!$'
LowMem:     mov       ax, 3
            int       10h
            lea       dx, LowMemTekst
            mov       ah, 09h
            int       21h

            MOV       AH, 4CH
            INT       21H
            jmp       Koniec
