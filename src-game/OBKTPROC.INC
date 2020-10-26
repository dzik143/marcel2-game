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

 ;GS:SI  =  adres obiektu
 _XEnd dw 0
 _YEnd dw 0
PrzesunObiekt PROC
                add    bx, gs:[si + x]        ;BX = nowy X
                add    cx, gs:[si + y]        ;CX = nowy Y
                push   cx                     ;y
                push   bx                     ;x

                mov    ax, bx                 ;x => AX
                add    ax, gs:[si+Szer]       ;AX = x + Szer
                mov    cs:[_xEnd], ax         ;_xEnd = x + Szer
                mov    ax, cx                 ;y => AX
                add    ax, gs:[si+Wys]        ;AX = y + Wys
                mov    cs:[_yEnd], ax         ;_yEnd = y + Wys

Spr:            call   Sprawdz

                add    bx, 8
                cmp    bx, cs:[_xEnd]
                jna    Spr

                pop    bx
                push   bx
                add    cx, 10h
                cmp    cx, cs:[_yEnd]
                jna    Spr


                mov    cx, cs:[_yEnd]
                call   Sprawdz
                mov    bx, cs:[_xEnd]
                call   Sprawdz

                pop    bx
                push   bx
                add    bx, 8
                call   Sprawdz


OK:             pop    word ptr gs:[si + x]
                pop    word ptr gs:[si + y]

                xor    al, al                   ;CF = 0
                RET

JestMur:        or     al, 1                    ;CF = 1
                add    sp, 4
                RET

;WE: BX,CX = wsp. obiektu
;WY: BP = adres obkt. na planszy
Sprawdz:
                mov    ax, bx
                shr    ax, 4
                mov    bp, ax
                mov    ax, cx
                shr    ax, 4
                mul    cs:[SzerEtp]
                add    bp, ax

                cmp    byte ptr fs:[bp], 0ffh
                je     DrogaWolna
                cmp    byte ptr fs:[bp], sciana
                jna    DrogaWolna
                add    sp, 2
                jmp    JestMur
DrogaWolna:     RET

ENDP


 ;GS:[SI] = adres 1 obiektu
 ;GS:[BP] = adres 2 obiektu
Kolizja PROC
                 push   cx
      ;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
      ;³Wynik negatywny <=> neg (ProbaXA v ProbaXB)             ³
      ;³  [x1>x2 ^ x1<(x2+s2)] v [(x1+s1)>x2 ^ (x1+s1)<(x2+s2)] ³
      ;³         ProbaXA       v           ProbaXB              ³
      ;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
                  mov   ax, gs:[si + x]          ;AX = x1
                  mov   bx, gs:[bp + x]          ;BX = x2
                  mov   cx, bx
                  add   cx, gs:[bp + Szer]       ;CX = x2+Szer


ProbaXA:          cmp   ax, bx                   ;Jesli X1<X2 to
                  jb    ProbaXB                  ;to druga proba dla X

                  cmp   ax, cx                   ;Oba testy XA pozytwynie
                  jb    ProbaY                   ;skacz do proby w pionie

ProbaXB:          add   ax, gs:[si + Szer]       ;AX = x1+Szer
                  cmp   ax, bx                   ;Jesli (x1+Szer < x2)
                  jb    KolizjaEnd               ;to wynik negatywny

                  cmp   ax, cx                   ;Jesli (x1+Szer > x2+Szer)
                  ja    KolizjaEnd               ;to wynik negatywny
      ;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
      ;³Wynik negatywny <=> neg (ProbaYA v ProbaYB)             ³
      ;³  [y1>y2 ^ y1<(y2+w2)] v [(y1+w1)>y2 ^ (y1+y1)<(y2+w2)] ³
      ;³         ProbaYA       v           ProbaYB              ³
      ;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
ProbaY:           mov   ax, gs:[si + y]          ;AX = y1
                  mov   bx, gs:[bp + y]          ;BX = y2
                  mov   cx, bx
                  add   cx, gs:[bp + Wys]        ;CX = y2+Wys

ProbaYA:          cmp   ax, bx                   ;Jesli y1<y2 to
                  jb    ProbaYB                  ;to druga proba dla Y

                  cmp   ax, cx                   ;Oba testy YA pozytwynie
                  jb    WynikPozytywny           ;to wynik O.K

ProbaYB:          add   ax, gs:[si + Wys]        ;AX = y1+Wys
                  cmp   ax, bx                   ;Jesli (y1+Wys < y2)
                  jb    KolizjaEnd               ;to wynik negatywny

                  cmp   ax, cx                   ;Jesli (y1+Wys > y2+Wys)
                  ja    KolizjaEnd               ;to wynik negatywny


WynikPozytywny:   pop   cx
                  or    al, 1
                  RET

KolizjaEnd:       pop   cx
                  xor   al, al
                  RET
ENDP


 ;GS:SI = adres obkt. do usuniecia
UsunObiekt PROC
            or    si, si
            je    NieUsuwaj
            push  es
            push  si

            mov   ax, gs
            mov   ds, ax
            mov   es, ax

            mov   di, si
            add   si, ObktLEN
            mov   cx, ObktLEN*100
            rep   movsw

            dec   word ptr cs:[IleObkt]
            pop   si
            pop   es
NieUsuwaj:  RET
ENDP

; WE: (BX, CX) = poz. nowego obkt.
;     AL = nr. rysunku
;     AH = typ
; WY: GS:[BP] = adres nowego obkt.
NowyObiekt PROC
            push   ax

            mov    ax, ObktLen
            mul    word ptr cs:[IleObkt]     ;AX = IleObkt * ObktLEN
            mov    bp, ax                    ;ES:DI = nowy adres
            pop    ax

            mov    gs:[bp + x], bx             ;BX = x
            mov    gs:[bp + y], cx             ;CX = y
            mov    gs:[bp+Rys], al             ;AL = Rysunek
            mov    gs:[bp+Typ], ah             ;AH = Typ
            mov    byte ptr gs:[bp+Kier], 1    ;Kierunek na 1

            xor    ax, ax
            mov    gs:[bp+Czyn], al
            mov    gs:[bp+Licz], al
            mov    gs:[bp+Trup], al
            mov    gs:[bp+Punkty], ax
            mov    gs:[bp+Sila], al
            mov    gs:[bp+Air], al
            mov    gs:[bp+CzyWybucha], al
            mov    gs:[bp+Drzwi], al

            call   UstawWymiary                 ;Ustaw nowe wymiary
            inc    word ptr cs:[IleObkt]
            RET
ENDP


 ; Utawia wymiary obkt. na podst. rysunku
 ;GS:[BP] = adres obiektu
UstawWymiary PROC
              push    si
              xor     bh, bh
              mov     bl, gs:[bp+Rys]        ;BX = Rysunek
              shl     bx, 1                  ;BX = adres offsetu
              mov     ds, cs:[GrafSeg]       ;DS = GrafSeg
              mov     si, ds:[bx]            ;DS:SI = adres rysunku

              lodsw                          ;Szerokosc
              mov     gs:[bp+Szer], ax
              lodsw
              mov     gs:[bp+Wys], ax        ;Wysokosc
              pop     si
              RET
ENDP

 Rnd dw 1234h
Random PROC
              mov   ax, cs:[Rnd]
              add   ax, 9248h
              add   ax, 1
              ror   ax, 3
              add   al, ah
              mov   cs:[Rnd], ax
              RET
ENDP
