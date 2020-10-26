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

               mov    ax, ObktLEN
               mov    cx, cs:[IleObkt]   ;CX = Ilosc obiektow
               mul    cx
               mov    si, ax

               mov    es, cs:[BuforVGA]  ;ES = BuforVGA

ObiektyLoop:   push   cx
               sub    si, ObktLEN

;======================================================================
;=============== K O L I Z J A   Z   G R A C Z E M ====================
;======================================================================

               or     si, si / je NieStyka
               xor    bp, bp

               call   Kolizja      ;sprawdz czy obkt. GS:[SI] ma punkty
               jz     NieStyka     ;wpsolne z obkt. GS:[BP]

        ;=================================================
        ;========== T Y P 05  (M o n e t a) ==============
        ;=================================================

               IF    (byte ptr gs:[si+Typ]=5) {       ;TYP 5 = "moneta"
               ;THEN
                   IF    (cs:[SB]!Get1) {Sound Get1}
                   mov    ax, gs:[si+Ener]            ;AX = ile ener. dodac
                   add    gs:[Ener], ax
                   IF    (word ptr gs:[Ener]>25) {mov word ptr gs:[Ener], 25}

                   mov    ax, gs:[si+Punkty]
                   add    cs:[PunktyGracz], ax
                   mov    al, gs:[si+Maska]
                   or     cs:[Stan], al

                   call   UsunObiekt
                   jmp    NextObiekt
                }

        ;=================================================
        ;========== T Y P 06  (W y j s c i e) ============
        ;=================================================

               IF    (byte ptr gs:[si+Typ]=6) {     ;TYP 6 = "wyjscie"
               ;THEN
                   call   PalDown

                   inc    byte ptr cs:[Etap]
                   mov    bl, cs:[Etap]
                   call   LoadEtap
                   jmp    Main
               } ;END IF

 ;=====================================================================
 ;=================== T Y P  08  (Z a m e k) ==========================
 ;=====================================================================

               IF (byte ptr gs:[si+Typ]=8) & (cs:[Key+48h]=1) {
               ;THEN
                    mov   al, cs:[Stan]           ;AL = Stan
                    mov   ah, gs:[si+Maska]       ;AH = Maska drzwi
                    or    ah, ah / je Open        ;Maska=0 <=> brak wymagan
                    and   al, ah / jne Open       ;Czy jest odp. klucz?
                    mov   cs:[TekstOfs], (Offset ClosedTekst)
                    mov   cs:[TekstLicz], 0ffh
                    jmp   NieStyka

Open:
                    not   al / and cs:[Stan], al / and gs:[si+Maska], al

                    cmp  byte ptr gs:[si+Active+ObktLEN], 0 / jne ^
                    IF   (b gs:[si+ObktLEN+ActiveLicz]=0) {
                    ;THEN
                       mov   cs:[TekstOfs], (offset OpenedTekst)
                       mov   cs:[TekstLicz], 255
                       Sound Open1
                       inc   byte ptr gs:[si+Rys]                ;zaznacz ,ze juz
                       mov   byte ptr gs:[si+Active+ObktLEN], 1  ;dzrzwi sie otw.
                       jmp   NieStyka
                    } ;END IF

                    IF   (b gs:[si+ObktLEN+ActiveLicz]=30) {
                    ;THEN
                       Sound Open1
                       dec   byte ptr gs:[si+Rys]                ;zaznacz ,ze juz
                       mov   byte ptr gs:[si+Active+ObktLEN], -1 ;dzrzwi sie zam.
                       jmp   NieStyka
                    } ;END IF
               } ;END IF
          ;=========================================================
          ;=============== P O Z O S T A L E =======================
          ;=========================================================
                   IF    (byte ptr cs:[MigaLicz]=0) {
                   ;THEN
                      mov    al, gs:[si + Sila]         ;Sila=0 =>
                      or     al, al / je NieStyka       ;NieOdejmuj i OneColor=0
                      mov    byte ptr cs:[MigaLicz], 40
                      mov    byte ptr gs:[OneColor], 1
                      cbw
                      sub    gs:[Ener], ax / jns NieStyka
                      mov    word ptr gs:[Ener], 0
                   } ;END IF
NieStyka:

 ;==================================================================
 ;============= S P R A W D Z   C Z Y   T R U P ====================
 ;==================================================================

       IF (word ptr gs:[si+Ener]=0) & (byte ptr gs:[si+Trup]=0) {
       ;THEN
             mov   byte ptr gs:[si+Typ], 3       ;TYP=3 (Wybuch)
             mov   byte ptr gs:[si+Licz], 0      ;Licz=0
             mov   al, gs:[si+RysWyb]            ;AL = RysWyb
             mov   gs:[si+Rys], al               ;Rys = RysWyb
             mov   byte ptr gs:[si+Trup], 1

             mov   ax, gs:[si+Punkty]
             add   cs:[PunktyGracz], ax

             IF  (byte ptr gs:[si+CzyWybucha]=1) {call Wybuch/mov cs:[Jasnosc], 8*4/ Sound Wybuch1}
       } ;END IF

 ;================================================================
 ;==================== R Y S U J =================================
 ;================================================================

               mov    ax, gs:[si + y]    ;AX = y
               inc    ax                 ;mala poprawka....
               mov    bx, cs:[KameraY]   ;BX = KameraY
               shl    bx, 4              ;BX = KameraY*16
               sub    ax, bx             ;AX = y - KameraY*16
               cmp    ax, -20
               jl     PozaEkranem
               add    ax, cs:[Py]        ;AX = y - KameraY*16 + Py
               cmp    ax, 192
               ja     PozaEkranem

               xchg   ah, al             ;AX = y*256
               mov    di, ax             ;DI = y*256
               shr    ax, 2              ;AX = y*64
               add    di, ax             ;DI = y*320

               mov    ax, gs:[si + x]    ;AX = x
               mov    bx, cs:[KameraX]   ;BX = KameraX
               shl    bx, 4              ;BX = KameraX*16
               sub    ax, bx             ;AX = x - KameraX*16
               add    ax, cs:[Px]        ;AX = x - KameraX*16 + Px
               cmp    ax, -60
               jl     PozaEkranem
               cmp    ax, 380
               jg     PozaEkranem

               xor     dx, dx                 ;DX = IleWychodzi = 0
               cmp     ax, 16
               jnl     NieMniejszy
               mov     dx, 15
               sub     dx, ax
               add     ax, dx
NieMniejszy:
               mov     bx, ax                 ;BX = x
               add     bx, gs:[si+Szer]       ;BX = x+Szer
               cmp     bx, 320
               jng     NieWiekszy
               mov     dx, bx                 ;DX = x+Szer
               sub     dx, 321                ;DX = x+Szer-320
NieWiekszy:
               mov     cs:[IleWychodzi], dx

               add    di, ax                 ;DI = (y-KameraY*320)
               mov    gs:[si+ScrOffset], di  ;   + (x - KameraX)
               mov    cs:[_x], ax


      ;=============== ONE COLOR =================================
               IF (si=0) {cmp b cs:[MigaLicz], 0
                          jp ^
                          mov b gs:[OneColor],1
                         } ;END IF

               mov    al, gs:[si + kier]
               cbw
               mov    cs:[_Kier], ax

               mov    al, gs:[si + OneColor]
               mov    cs:[_OneColor], al

    ;======================================================
    ;============== N U M E R   R Y S U N K U =============
    ;======================================================

               mov    al, gs:[si + rys]                    ;AL = rysunek

               IF (byte ptr gs:[si+Czyn]=1) {              ;1 = Chodzenie
               ;THEN
                    mov    bl, gs:[si + Licz]
                    shr    bl, 3
                    xor    bh, bh
                    add    al, cs:[ChodAnm + bx]
                } ;END IF

               IF (b gs:[si+Czyn]=2) & (b gs:[si+Typ]!11) {add  al, 2}   ;2 = Skok
               IF (byte ptr gs:[si+Czyn]=3) {              ;3 = Animacja
               ;THEN
                    mov  ah, gs:[si+Licz]
                    shr  ah, 2
                    IF  (byte ptr gs:[si+Typ]=9) {shr ah, 1}
                    add  al, ah
               } ;END IF

               IF (byte ptr gs:[si+Czyn]=4) {add  al, 3}   ;4 = Kleczenie
               IF (byte ptr gs:[si+Czyn]=5) {              ;5 = Wspinanie
                ;THEN
                     add  al, 4
                     mov  bl, gs:[si+Licz]
                     shr  bl, 3
                     add  al, bl
               } ;END IF

               IF (al>RysMAX) {xor al, al}

               IF (byte ptr gs:[si+Typ]=7) {         ;DRZWI
                 ;THEN
                    mov  ah, gs:[si+ActiveLicz]
                    mov cs:[_Wys], ah
               }\;ELSE
                    mov cs:[_Wys], 0\

               call   RysujObiekt
               mov    byte ptr gs:[si + OneColor], 0
RuszajSie:
               INCLUDE CPU.TMS               ;Ruchy komputera

   ;============================================================
   ;=================== G R A W I T A C J A ====================
   ;============================================================

             IF (b gs:[si+Air]!1) {
             ;THEN
                mov    gs:[si+Grunt], bl      ;Wyzeruj
                xor    bx, bx
                mov    cx, 4
                call   PrzesunObiekt          ;Przesun o wektor [0,4]
                jz     ^
                xor    bx, bx
                mov    cx, 1
                call   PrzesunObiekt
                mov    byte ptr gs:[si+Grunt], 1   ;Zapisz ze stoi na ziemi
             } ;END IF

   ;===========================================================
   ;===================  S K O K  =============================
   ;===========================================================

          IF (byte ptr gs:[si+czyn]=2) {
               ;THEN
                   mov  bl, gs:[si+licz]    ;BL = licznik
                   xor  bh, bh              ;BX = licznik
                   shr  bx, 2               ;BX = licznik/4
                   mov  al, cs:[Skok + bx]  ;AL = AddSkok       ;!-grawitacja!
                   cbw                      ;AL => AX

                   mov  cx, ax
                   xor  bx, bx
                   call PrzesunObiekt       ;Przesun o wektor [bx, cx]
                   jz   Shifted
                   xor  bx, bx
                   mov  cx, -4
                   call PrzesunObiekt

Shifted:           dec  byte ptr gs:[si + Licz]
                   jne  ^
                   mov  byte ptr gs:[si + czyn], 0   ;Czynnosc = 0
               } ;Koniec skoku
               jmp   NextObiekt

;===========================================================================
PozaEkranem:   IF  (byte ptr gs:[si+Typ]=2) {call UsunObiekt}      ;Kula
               cmp  byte ptr gs:[si+Typ], 14 / je RuszajSie       ;Wyspa
;===========================================================================

NextObiekt:    pop   cx
               dec   cx
               jne   ObiektyLoop


            IF (byte ptr cs:[WybuchLicz]>0) {
            ;THEN
                   dec     byte ptr cs:[WybuchLicz]
                   call    RysujWybuch
            }  ;END WYBUCH

            IF (cs:[Jasnosc]>1) {dec cs:[Jasnosc]}
            IF (cs:[MigaLicz]>0) {dec cs:[MigaLicz]}
