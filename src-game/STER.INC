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

             IF (word ptr gs:[Ener]=0) & (byte ptr cs:[Jasnosc]=1) {
                   call   Splyw
;                   call   PalDown
                   call   LoadEtap
                   jmp    Main
             } ;END if

             xor    si, si           ;GS:SI = adres gracza
;=======================================================================
;============================= C T R L =================================
;=======================================================================

          IF (b cs:[GraczNaWyspie]=1) {mov b gs:[Grunt], 1 / mov b gs:[Air], 0}
          IF (cs:[Key+1dh]=1) & (byte ptr gs:[grunt]!0) {
          ;THEN
               cmp   byte ptr gs:[Air], 1 / je ^
               cmp   byte ptr gs:[Czyn], 4     ;Jesli kucanie to
               je    ^                         ;nie mozna skakac
               IF    (cs:[SB]!Skok1) {Sound Skok1}
               mov   byte ptr gs:[czyn], 2     ;Czynnosc = 2 (skok)
               mov   byte ptr gs:[licz], 23    ;Licznik = 24
               jmp   SterEnd
          } ;koniec CTRL

;=======================================================================
;============================= A L T  ==================================
;=======================================================================

          IF (cs:[Key+38h]=1) {
          ;THEN
                   cmp   cs:[GunReady], 1
                   jne   ^

                   Sound Shot1
                   mov   al, gs:[Kier]      ;AL = Kierunek
                   mov   bx, gs:[x]         ;BX = x
                   IF   (al=1) {add bx,22}\sub bx, 8\
                   mov   cx, gs:[y]         ;CX = y
                   add   cx, 14             ;CX = y+14
                   mov   ax, 20dh           ;AH = typ,   AL = rysynek
                   call  NowyObiekt         ;tworz obkt. w poz. (bx,cx)

                   mov   cs:[GunReady], 0
                   mov   byte ptr gs:[bp+Sila], 1    ;Sila = 1
                   mov   word ptr gs:[bp+Ener], 1    ;Ener = 1
                   mov   byte ptr gs:[bp+RysWyb], 10 ;RysWyb = 10
                   mov   al, gs:[Kier]
                   mov   byte ptr gs:[bp+Kier], al
                   mov   byte ptr gs:[bp+Air], 1     ;w powietrzu

                   mov   di, gs:[ScrOffset]          ;DI = offset gracza
                   add   di, 320*6                   ;O 14 w dol
                   mov   al, gs:[Kier]               ;AL = Kierunek
                   IF   (al=1) {add di,9}\ sub di,15\
                   cbw
                   mov   cs:[_Kier], ax
                   mov   al, 14                      ;AL = 14 (rysunek)
                   call  RysujObiekt
          }\
          ;ELSE
                   mov   cs:[GunReady], 1
           \  ;END ALT

     ;=======================================
     ;=============== D O L  ================
     ;=======================================

             mov  al, gs:[Czyn]
             IF  (cs:[Key+50h]=1) & (al!2) & (b gs:[Air]=0) {
             ;THEN
                 mov   byte ptr gs:[Czyn], 4
                 mov   word ptr gs:[Wys], 26
                 jmp   SterEnd
             }\
             ;ELSE
                 cmp   byte ptr gs:[Czyn], 4
                 jne   ^
                 sub   word ptr gs:[y], 5
                 mov   byte ptr gs:[Czyn], 0
                 mov   word ptr gs:[Wys], 31
              \

     ;=========================================
     ;=============== G O R A  ================
     ;=========================================

             IF (cs:[Key+48h]=2) {
             ;THEN
                  IF   (byte ptr gs:[Czyn]!5) {
                   ;THEN
                       mov   byte ptr gs:[Czyn], 5       ;5 = Wspinanie
                       mov   byte ptr gs:[Licz], 0
             } ;END IF

                  xor   bx, bx
                  mov   cx, -1
                  call  PrzesunObiekt

                  mov   byte ptr gs:[Air], 1
                  inc   byte ptr gs:[Licz]
                  IF    (byte ptr gs:[Licz]=16) {mov byte ptr gs:[Licz],0}


             }  ;END IF


  ;===========================================
  ;=============== P R A W O =================
  ;===========================================

             IF (cs:[Key+4dh]=1) {
             ;THEN
                 cmp    byte ptr gs:[Drzwi], 1
                 je     ^
                 mov    byte ptr gs:[kier], 1        ;Kierunek = 1
                 mov    bx, 2
                 xor    cx, cx
                 call   PrzesunObiekt      ;Przesun o [2,0]

                 mov    ax, gs:[x]         ;AX = X gracza
                 shr    ax, 4              ;AX = x/16
                 sub    ax, cs:[KameraX]   ;AX = X wzg. Kamery

                 cmp    byte ptr gs:[czyn], 2 / je ^
                 mov    byte ptr gs:[Czyn], 1
                 inc    byte ptr gs:[licz]
                 IF    (byte ptr gs:[Licz]=32) {mov byte ptr gs:[Licz], 0}
           } ; END PRAWO
   ;===================================================
   ;=================== L E W O =======================
   ;===================================================

             IF (cs:[Key+4bh]=1) {
             ;THEN
                 cmp    byte ptr gs:[Drzwi], -1
                 je     ^

                 mov    byte ptr gs:[kier], -1        ;Kierunek = -1
                 mov    bx, -2
                 xor    cx, cx
                 call   PrzesunObiekt            ;Przesun o [-2,0]

                 mov    ax, word ptr gs:[x]      ;AX = X
                 shr    ax, 4                    ;AX = x/16
                 sub    ax, cs:[KameraX]         ;AX = X wzg. Kamery

                 IF (byte ptr gs:[czyn]!2) {
                 ;THEN
                     mov    byte ptr gs:[Czyn], 1
                     inc    byte ptr gs:[licz]
                     IF    (byte ptr gs:[Licz]=32) {mov byte ptr gs:[Licz], 0}
                 } ;END iF
           } ;END LEWO


           IF (cs:[Key+1]=1) {call Menu}
SterEnd:
           xor al, al
           mov gs:[Drzwi], al
           mov gs:[Air], al
           mov cs:[GraczNaWyspie], al
