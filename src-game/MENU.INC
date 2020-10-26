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

Tekst1 db 'BY SZEFO & SZEFO CORP. 2003$'

MenuTekst db 'NOWA GRA$'
          db 'WYJaCIE$'
          db 'POMOC$'
          db 'POWR',60h,'T DO GRY$'

    MenuPoz db 4      ;Pozycja w menu (od dolu)
     OldKey dw 0      ;Adres ostatniego klawisza (wzg. [Key])
 GameActive db 0      ;cs:[GameActive]=1  <=>  Gra w toku

Menu PROC
            mov       cs:[_kier], 1
            mov       dx, 768 / call SetPal          ;Paleta na ogien
            mov       dx, 3d4h
            mov       ax, 0309h
            out       dx, ax

            mov       es, cs:[BuforVGA]
            call      Cls
            mov       cs:[MenuPoz], 4

PetlaMenu:
            mov       es, cs:[BuforVGA]
            include   OGIEN.INC

   ;===================== L O G O ======================

            mov       di, 320*15+100 / mov al, 37
            call      RysujObiekt
            mov       di, 320*30+60 / mov bh, 100
            lea       bp, Tekst1
            call      PiszG

   ;================== T R E S C   M E N U ===================

            lea       bp, MenuTekst
            mov       di, 320*40+120
            REPEAT (cl, 4) {
              ;---------------------;
                 IF (cl=cs:[MenuPoz]) {mov bh, 200}\mov bh, 80\
                 call  PiszG
                 add   di, 320*10

              ;---------------------;
            } ;LOOP CL

     ;================ K L A W I A T U R A +====================

            call      BuforToScreen
            call      WaitSync


            mov bx, cs:[OldKey]
            IF (cs:[Key+bx]!1) {
              mov cs:[OldKey], 0
              ;THEN
              IF (cs:[Key+50h]=1) & (cs:[MenuPoz]>1) {dec cs:[MenuPoz] / mov cs:[OldKey], 50h}
              IF (cs:[Key+48h]=1) & (cs:[MenuPoz]<4) {inc cs:[MenuPoz] / mov cs:[OldKey], 48h}
              IF (cs:[Key+1ch]=1) {
               ;THEN ============= E N T E R ==============
                     mov  al, cs:[MenuPoz]
                     IF (al=4) {mov cs:[Etap], 0 / mov cs:[GameActive],1 / call Splyw / call LoadEtap / mov cs:[PunktyGracz], 0 / RET}
                     IF (al=3) {call Splyw / call PalDown / mov ax, 3 / int 10h / jmp Koniec}
                     IF (al=2) {}
                     IF (al=1) & (cs:[GameActive]=1) {xor dx,dx / call SetPal / mov  dx, 3d4h / mov ax, 109h / out dx, ax / RET}
              } ;END ENTER
            } ;END IF
            jmp   PetlaMenu


ENDP
