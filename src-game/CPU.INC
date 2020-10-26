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

              mov    al, gs:[si + Typ]

 ;========================================================================
 ;=================== T Y P  01  (P a j a k) =============================
 ;========================================================================

     IF (AL=1) {
     ;THEN
             IF   (byte ptr gs:[si+Drzwi]!0) {neg byte ptr gs:[si+Kier]}

             mov   bx, cs:[_Kier]          ;BX = Kierunek
             mov   cx, 1                   ;CX = 1
             call  PrzesunObiekt           ;Sproboj przesunoac o [Kier,1]
             jnz   NieSpada                ;Jesli sie nie da to grunt
             neg   byte ptr gs:[si+Kier]   ;Jesli mozna to neguj Kierunek
             mov   ax, cs:[_Kier]
             sub   gs:[si+x], ax           ;Przywroc stan poprzedni
             dec   word ptr gs:[si+y]

NieSpada:    inc   byte ptr gs:[si+Licz]
             IF   (byte ptr gs:[si+Licz]=32) {mov byte ptr gs:[si+Licz], 0}

             mov   al, gs:[si+Kier]       ;AL = Kierunek
             cbw                          ;AL => AX
             mov   bx, ax                 ;BX = Kierunek
             xor   cx, cx
             call  PrzesunObiekt          ;Przesun o [Kier,0]
             jz    ^                      ;Jesli O.K to wyskok

             neg   byte ptr gs:[si+Kier]
     } ;END PAJAK

 ;========================================================================
 ;=================== T Y P  02  (P o c i s k) ===========================
 ;========================================================================

     IF (AL=2) {
     ;THEN
              IF   (byte ptr gs:[si+Drzwi]!0) {mov  word ptr gs:[si+Ener],0}

              xor   bp, bp                     ;BP = 1. obkt.
              mov   cx, cs:[IleObkt]           ;CX = IleObkt

SprKolizja:   IF   (bp!si) & (byte ptr gs:[bp+Typ]!2) & (byte ptr gs:[bp+Trup]!1) {
              ;THEN
                   call  Kolizja
                   jz    ^

                   mov   byte ptr gs:[bp+OneColor], 1  ;Ustaw OneColor
                   mov   al, gs:[si+Sila]              ;AL = sila pocisku
                   cbw                                 ;AL => AX
                   sub   gs:[bp+Ener], ax              ;Ener traf. -= Sila
                   jns   NieUjemne
                   xor   ax, ax
                   mov   gs:[bp+Ener], ax

NieUjemne:         mov   al, gs:[bp+Sila]             ;AL = sila trafionego
                   inc   al                           ;AL = sila+1
                   cbw                                ;AL => AX
                   sub   gs:[si+Ener], ax             ;Ener pocisku -= Sila
                   jns   PoKolizji
                   xor   ax, ax
                   mov   gs:[si+Ener], ax
                   jmp   PoKolizji
              } ;END IF

              add   bp, ObktLEN              ;Sprawdz
              dec   cx                       ;nastepny
              jne   SprKolizja               ;obiekt
PoKolizji:

              mov   al, gs:[si+Kier]          ;AL = Kier
              cbw                             ;AL => AX
              mov   bx, ax                    ;BX = AX
              add   bx, bx                    ;BX = BX*2
              add   bx, bx                    ;BX = BX*4
              xor   cx, cx                    ;CX = 0
              call  PrzesunObiekt             ;Przesun o [Kier, 0]
              jz    ^                         ;Jesli wszystko O.K

              mov   word ptr gs:[si+Ener], 0  ;Jesli sciana
              jmp   EndCPU
       } ;Koniec Pocisku

 ;=====================================================================
 ;=================== T Y P  03  (W y b u c h) ========================
 ;=====================================================================

   IF (byte ptr gs:[si+Typ]=3) {
   ;THEN
         mov   byte ptr gs:[si+Sila], 0        ;Animacja
         mov   byte ptr gs:[si+Czyn], 3        ;Animacja
         mov   byte ptr gs:[si+Trup], 1        ;Niesmiertelny
         inc   byte ptr gs:[si+Licz]           ;Animacja
         IF   (byte ptr gs:[si+Licz]=3*4) {call UsunObiekt}
         jmp   EndCPU
    } ;END IF

 ;=====================================================================
 ;=================== T Y P  04  (R o b o c o p) ======================
 ;=====================================================================

  IF (byte ptr gs:[si+Typ]=4) {
  ;THEN
       IF (byte ptr gs:[si+Active]=0) {
       ;THEN
             mov   bx, cs:[_Kier]          ;BX = Kierunek
             mov   cx, 1                   ;CX = 1
             call  PrzesunObiekt           ;Sproboj przesunoac o [Kier,1]
             jnz   NieSpada2               ;Jesli sie nie da to grunt
             neg   byte ptr gs:[si+Kier]   ;Jesli mozna to neguj Kierunek
             mov   ax, cs:[_Kier]
             sub   gs:[si+x], ax           ;Przywroc stan poprzedni
             dec   word ptr gs:[si+y]

NieSpada2:
             inc   byte ptr gs:[si+Licz]
             IF   (byte ptr gs:[si+Licz]=32) {mov byte ptr gs:[si+Licz], 0}

             mov   al, gs:[si+Kier]       ;AL = Kierunek
             cbw                          ;AL => AX
             mov   bx, ax                 ;BX = Kierunek
             xor   cx, cx
             call  PrzesunObiekt          ;Przesun o [Kier,0]
             jz    ^                      ;Jesli O.K to wyskok

             neg   byte ptr gs:[si+Kier]
       } ;END CHODZENIE

       call   Random                               ;AX = rnd
       IF    (ax>65000) {    ;65000
       ;THEN
             IF   (cs:[SB]!Shot1) {Sound Shot1}
             xor   byte ptr gs:[si+Active], 0ffh
             mov   ax,  gs:[x]                     ;AX = x gracza

             IF   (ax<gs:[si+x]) {                 ;Jesli gracz przed
             ;THEN                                 ;obiektem to idzie i
                   mov byte ptr gs:[si+Kier],-1}\  ;patrzy w lewo
             ;ELSE
                   mov byte ptr gs:[si+Kier], 1\   ;jesli nie to w prawo
       ;=========== S T R Z A L ==============
            mov    al, gs:[si+Kier]                ;AL = Kierunek
            mov    bx, gs:[si+x]                   ;BX = x
            IF    (al=1) {add bx,22}\sub bx, 8\
            mov    cx, gs:[si+y]                   ;CX = y
            add    cx, 14                          ;CX = y+14
            mov    ax, 20dh                        ;AH = typ,   AL = rysynek
            call   NowyObiekt                      ;tworz obkt. w poz. (bx,cx)

            mov    byte ptr gs:[bp+Sila], 2        ;Sila = 5
            mov    word ptr gs:[bp+Ener], 1        ;Ener = 1
            mov    byte ptr gs:[bp+RysWyb], 10      ;RysWyb = 6

            mov    al, gs:[si+Kier]
            mov    gs:[bp+Kier], al
            mov    byte ptr gs:[bp+Air], 1         ;w powietrzu

         ;============ OGIEN Z KARABINU ===========
            mov   di, gs:[si+ScrOffset]       ;DI = offset gracza
            add   di, 320*16                  ;O 14 w dol
            mov   al, gs:[si+Kier]            ;AL = Kierunek
            IF   (al=1) {add di,16}\ sub di, 4\
            cbw
            mov   cs:[_Kier], ax
            mov   al, 14                      ;AL = 14 (rysunek)
            call  RysujObiekt
       }  ;END IF
      jmp  EndCPU
  } ;END ROBOCOP

 ;=====================================================================
 ;=================== T Y P  05  (A p t e c z k a) ====================
 ;=====================================================================

 ;=====================================================================
 ;=================== T Y P  06  (W y j s c i e ) =====================
 ;=====================================================================

 ;=====================================================================
 ;=================== T Y P  07  (D r z w i) ==========================
 ;=====================================================================
  IF (si!0) {mov byte ptr gs:[si+Drzwi], 0}
  IF (byte ptr gs:[si+Typ]=7) {
  ;THEN
              mov  al, gs:[si+Active]
              add  gs:[si+ActiveLicz], al

              IF  (b gs:[si+ActiveLicz]=30) {mov b gs:[si+Active],0 / jmp EndCPU}
              IF  (b gs:[si+ActiveLicz]=0) {mov b gs:[si+Active],0}
     ;===============================================================
     ;========== SPRAWDZ CZY STYKA SIE Z JAKIMS OBIEKTEM ============
     ;===============================================================
              xor   bp, bp                      ;BP = 1. obkt.
              REPEAT (cx, cs:[IleObkt]) {
     ;===================================================
                IF   (bp!si) {
                ;THEN
                     xchg  si, bp
                     call  Kolizja
                     xchg  si, bp
                     jz    ^

                     mov   ax, gs:[si+x]              ;AX = x dzrzwi
                     IF   (ax>gs:[bp+x]) {mov al, 1}\mov al, -1\
                     jmp   ^
                } ;END IF

                mov   gs:[bp+Drzwi], al
                add   bp, ObktLEN              ;Sprawdz
              } ;LOOP CX
     ;====================================================
              jmp  EndCPU
  } ;END DRZWI

 ;=====================================================================
 ;=================== T Y P  08  (Z a m e k) ==========================
 ;=====================================================================

 ;=====================================================================
 ;=================== T Y P  09  (A n i m a c j a) ====================
 ;=====================================================================
  IF (byte ptr gs:[si+Typ]=9) {
  ;THEN
      inc   byte ptr gs:[si+Licz]
      IF   (byte ptr gs:[si+Licz]=8*3) {mov  byte ptr gs:[si+Licz], 0}
      mov   byte ptr gs:[si+Czyn], 3
      jmp   EndCPU
  } ;END ANIMACJA

 ;=====================================================================
 ;=================== T Y P  10  (A c m e) ============================
 ;=====================================================================

  IF (byte ptr gs:[si+Typ]=10) {
  ;THEN
      IF (byte ptr gs:[si+Grunt]=1) {mov word ptr gs:[si+Ener], 0}

      mov  ax, gs:[si+x] / sub ax, 20
      cmp  gs:[x], ax / jb ^
      add  ax, gs:[si+Szer] / add ax, 40
      cmp  gs:[x], ax / ja ^

      mov  byte ptr gs:[si+Air], 0
      ;IF (ax>gs:[si+x]) & (ax<bx) {mov byte ptr gs:[si+Air], 0}
      jmp  EndCPU
  } ;END ACME

 ;=====================================================================
 ;=================== T Y P  11  (M i n a) ============================
 ;=====================================================================

  IF (byte ptr gs:[si+Typ]=11) {
  ;THEN
      IF (b gs:[si+Grunt]=1) {
      ;THEN
         IF (b gs:[si+Licz]=8*5) {mov b gs:[si+Czyn], 2 / mov b gs:[si+Licz],8*4-1}\
         ;ELSE
          inc byte ptr gs:[si+Licz]\
      } ;END IF
  } ;END MINA

 ;=====================================================================
 ;=================== T Y P  12  (W i n d a) ==========================
 ;=====================================================================

 IF (byte ptr gs:[si+Typ]=12) {
  ;THEN
          mov   byte ptr gs:[Air], 0
          mov   di, gs:[si+ScrOffset] / sub di, 320*16*3
          mov   al, gs:[si+Rys]
          call  RysujObiekt

      ;============= JEDZIE W GORE ==================
         IF (byte ptr gs:[si+Active]=-1) {
          ;THEN
              dec    word ptr gs:[si+y]           ;Widna w gore
              dec    word ptr gs:[y]              ;Gracz w gore
              inc    byte ptr gs:[si+ActiveLicz]
              mov    al, gs:[si+Ener]
              IF    (b gs:[si+ActiveLicz]=al) {mov b gs:[si+Active], 0}
          } ;END IF

       ;============== JEDZIE W DOL ==================
          IF (byte ptr gs:[si+Active]=1) {
          ;THEN
              inc    word ptr gs:[si+y]           ;Widna w gore
              inc    word ptr gs:[y]              ;Gracz w gore
              dec    byte ptr gs:[si+ActiveLicz]
              jne    ^
              mov    byte ptr gs:[si+Active], 0
          } ;END IF
;================== SPRAWDZ CZY GRACZ W WINDZIE ================
          mov    ax, gs:[si+x]                    ;AX = x1 windy
          cmp    gs:[x], ax / jb ^                ;Gracz przed winda
          add    ax, gs:[si+Szer]                 ;AX = x2 windy
;          sub    ax, gs:[Szer]
          cmp    gs:[x], ax / ja ^                ;Gracz za winda

          mov    ax, gs:[y] / add ax, gs:[Wys]    ;AX = y2 Gracza
          mov    bx, gs:[si+y]                    ;BX = y1 Windy
          cmp    ax, bx / jb ^                    ;Gracz przed winda
          add    bx, 16                           ;BX = y2 Windy
          cmp    ax, bx / ja ^                    ;Gracz za winda
          mov    byte ptr gs:[Air], 1

          IF (cs:[Key+48h]=1) & (b gs:[si+Active]=0) {
          ;THEN
             Sound Winda1
             IF (b gs:[si+ActiveLicz]=0) {mov b gs:[si+Active], -1}\mov b gs:[si+Active],1\
          } ;END IF
      } ;END WINDA

 ;=====================================================================
 ;=================== T Y P  13  (Z o o m b i) ========================
 ;=====================================================================

 IF (byte ptr gs:[si+Typ]=13) & (byte ptr gs:[si+Drzwi]=0) {
  ;THEN
             inc   byte ptr gs:[si+Licz]
             IF   (byte ptr gs:[si+Licz]=32) {mov byte ptr gs:[si+Licz], 0}

             mov   ax, gs:[x]
             cmp   ax, gs:[si+x] / je ^
             IF   (ax<gs:[si+x]) {mov al, -1}\ mov al, 1\

             mov   gs:[si+Kier], al
             cbw                          ;AL => AX
             mov   bx, ax                 ;BX = Kierunek
             xor   cx, cx
             call  PrzesunObiekt          ;Przesun o [Kier,0]
             jmp   EndCPU
 } ;END ZOOMBI

 ;=====================================================================
 ;=================== T Y P  14  (W y s p a) ==========================
 ;=====================================================================

 IF (byte ptr gs:[si+Typ]=14) {
  ;THEN
          mov    al, gs:[si+Kier]        ;AL = Kierunek
          cbw                            ;AL => AX
          mov    bx, ax                  ;BX = Kierunek
          xor    cx, cx
          call   PrzesunObiekt           ;Przesun o [Kier,0]
          jz     NoInvert                ;Jesli O.K to wyskok
          neg    byte ptr gs:[si+Kier]

NoInvert:
          ;xor    bp, bp
          ;xchg   bp, si
          ;call   Kolizja
          ;xchg   bp, si
          ;jz     ^

          mov     ax, gs:[si+x]        ;AX = x1 wyspy
          mov     bx, gs:[x]           ;BX = x1 gracza
          mov     cx, bx               ;CX = BX = x1 gracza
          add     bx, gs:[Szer]        ;BX = x2 gracza
          cmp     bx, ax / jb ^
          add     ax, gs:[si+Szer]     ;AX = x2 wyspy
          cmp     cx, ax / ja ^

          mov     ax, gs:[y]           ;AX = y gracza
          add     ax, gs:[Wys]         ;AX = y2 gracza
          cmp     ax, gs:[si+y] / jb ^
          sub     ax, 4
          cmp     ax, gs:[si+y] / ja ^


          mov    byte ptr gs:[Air], 1
          mov    byte ptr cs:[GraczNaWyspie], 1

          mov    al, gs:[si+Kier] / cbw
          add    gs:[x], ax
  } ;END WYSPA

EndCPU:
