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

;ES:DI = adres docelowy
;AL = nr. rysunku
;cs:[_Wys]= wysokosc kwadratu <0,10h>
_Wys db 0
RysujKwadrat PROC
                 cmp    byte ptr cs:[_Wys], 0
                 je     RysKwadEnd
                 push   cx
                 push   di

                 mov    ds, cs:[TloSeg]       ;DS = TloSeg
                 shl    ax, 8                 ;AX = AL*256
                 mov    si, ax                ;DS:SI = adres zadanego rysunku
                 mov    ch, _Wys              ;CH = ilosc linii
                 mov    cl, 4                 ;4 dword na linie

                 mov    al, cs:[Jasnosc]      ;AL = Jasnosc
                 shr    al, 3                 ;AL = Jasnosc/8
                 mov    ah, al                ;AX = ? ? J J
                 mov    bx, ax                ;BX = ? ? J J
                 shl    ebx, 10h              ;EAX = J J 0 0
                 mov    bx, ax                ;EBX = J J J J

RysKwadLoop:     mov    eax, ds:[si]
                 add    eax, ebx              ;dodaj wsp. jasnosci
                 add    si, 4
                 mov    es:[di], eax
                 add    di, 4
                 dec    cl
                 jne    RysKwadLoop

                 mov    cl, 4
                 add    di, 130h
                 dec    ch
                 jne    RysKwadLoop

                 pop    di
                 pop    cx
RysKwadEnd:      RET
ENDP

RysujEtap PROC
                 push   ds

                 mov    byte ptr cs:[_Wys], 16   ;ust. wysokosc kwad.
                 mov    ax, cs:[KameraY]         ;AX = KameraY
                 mul    word ptr cs:[SzerEtp]    ;AX = KameraY * SzerEtp
                 add    ax, cs:[KameraX]         ;AX = KameraY*SzerEtp+KameraX
                 mov    bp, ax                   ;BP = Adres kamery

                 mov    es, cs:[BuforVGA]        ;ES = BuforVGA
                 mov    ax, cs:[Py]              ;AX = Py
                 xchg   ah, al                   ;AX = Py*256
                 mov    di, ax                   ;AX => DI
                 shr    ax, 2                    ;AX = Py*64
                 add    di, ax                   ;DI = Py*320
                 add    di, cs:[Px]              ;DI = Py*320+Px
                 mov    cx, 0b14h                ;20 x 11

RysEtpLoop:      mov    al, fs:[bp]              ;FS:[BP] => AL
                 inc    bp
                 IF    (al=0ffh) {
                 ;THEN
                     push  di
                     mov   ds, cs:[PlenerSeg]
                     mov   si, di
                     add   si, 320
                     sub   si, cs:[PlenerX]
                     mov   bx, 1004h

FillEmpty:           mov   eax, ds:[si]
                     add   si, 4
                     mov   es:[di], eax
                     add   di, 4
                     dec   bl
                     jne   FillEmpty

                     mov   bl, 4
                     add   si, 320-16
                     add   di, 320-16
                     dec   bh
                     jne   FillEmpty
                     pop   di
                 }\
                 ;ELSE
                     call   RysujKwadrat        ;AL => EKRAN
                   \

                 add    di, 10h             ;Nast. kwadrat
                 dec    cl                  ;Czy koniec
                 jne    RysEtpLoop          ;linii?

                 mov    cl, 20              ;next. ln.
                 add    bp, cs:[SzerEtp]
                 sub    bp, 20
                 add    di, 320*15

                 dec    ch
                 jne    RysEtpLoop

    ;============ OSTATNIA LINIA ============
                 mov    ax, 10h
                 sub    ax, cs:[Py]
                 or     al, al
                 je     RysEtpEnd
                 mov    byte ptr cs:[_Wys], al       ;Wysokosc = 16-Py
                 mov    cl, 14h

Ostatnia:        mov    al, fs:[bp]      ;FS:[SI] => AL
                 inc    bp
                 IF    (al=0ffh) {
                 ;THEN
                     push  di
                     mov   ds, cs:[PlenerSeg]
                     mov   si, di
                     add   si, 320
                     sub   si, cs:[PlenerX]
                     mov   bh, cs:[_Wys]
                     mov   bl, 4

FillEmpty2:          mov   eax, ds:[si]
                     add   si, 4
                     mov   es:[di], eax
                     add   di, 4
                     dec   bl
                     jne   FillEmpty2

                     mov   bl, 4
                     add   si, 320-16
                     add   di, 320-16
                     dec   bh
                     jne   FillEmpty2
                     pop   di
                 }\
                 ;ELSE
                     call   RysujKwadrat        ;AL => EKRAN
                   \

                 add    di, 10h          ;Nast. kwadrat
                 dec    cl
                 jne    Ostatnia         ;Czy koniec linii
RysEtpEnd:       pop    ds
                 RET
ENDP


      _kier dw 1
  _OneColor db 0
      AddLn dw 0       ;do odwracania
IleWychodzi dw 0       ;do obcinania w poziomie
         _x dw 0

RysujObiekt PROC
            push   ds
            push   si
            push   di

            mov    ds, cs:[GrafSeg]
            xor    ah, ah                       ;AX = rys.
            shl    al, 1                        ;AX = Rys*2
            mov    bx, ax                       ;BX = adres nagl. rysunku

            mov    ax, cs:[IleWychodzi]         ;AX = IleWychodzi

            mov    si, ds:[bx]                  ;DS:SI dane obrazka
            mov    cx, ds:[si]                  ;CX = Szer
            sub    cx, ax                       ;CX = Szer-IleWychodzi
            js     ScreenEnd
            je     ScreenEnd

            mov    dx, cx                       ;DX = Szer
            mov    bx, ds:[si+2]                ;BL = Wys
            sub    bl, cs:[_Wys]                ;
            add    si, 4                        ;omin naglowek
            mov    AddLn, 320                   ;AddLn = 320


            IF (_Kier=-1) {
            ;THEN
                add   cs:[AddLn], dx            ;gdy lewo to AddLn = 320+Szer
                add   di, dx                    ;skoryguj poczatek
                cmp   cs:[_x], 160
                jb    ^
                add   si, ax
            }\;ELSE
                sub   cs:[AddLn], dx            ;gdy prawo to AddLn = 320-Szer
                cmp   cs:[_x], 16
                ja    ^
                add   si, cs:[IleWychodzi]
             \ ;END IF

            mov    ah, cs:[Jasnosc]
            shr    ah, 3

RysLn:      mov    al, ds:[si]
            inc    si

            IF    (al!255) {
            ;THEN
;                add    al, ah                           ;dolicz jasnosc
                IF    (cs:[_OneColor]=1) {mov al, 0fh}
                mov    byte ptr es:[di], al
            } ;END IF

            add    di, cs:[_kier]
            cmp    di, 320*193
            ja     ScreenEnd
            dec    cx
            jne    RysLn

            add    si, cs:[IleWychodzi]
            add    di, AddLn                   ;AddLn
            mov    cx, dx                      ;Kolejna linia

            dec    bl                          ;BL = Liczik Wys.
            jne    RysLn

ScreenEnd:  pop    di
            pop    si
            pop    ds
            RET
ENDP


BuforToScreen PROC
            mov    ds, cs:[BuforVGA]
            mov    si, 16*320+18
            mov    ax, 0a000h
            mov    es, ax
            mov    di, 8+320*8

            mov    cl, 4ch           ;Szerokosc = 130h/4
            mov    ch, 16*12-17      ;Wysokosc

            mov    bx, cs:[AddBlur]
            shr    bx, 2
            shl    bx, 1
            mov    dx, word ptr cs:[KameraY]
            shl    dx, 4
            sub    dx, cs:[Py]

Kopiuj:     mov    eax, ds:[si]
            add    si, 4
            mov    es:[di], eax
            add    di, 4
            dec    cl
            jne    Kopiuj

            mov    cl, 4ch           ;Odswiez szerokosc (130h/4)
            add    di, 10h
            add    si, 10h

            inc    dx
            cmp    dx, cs:[PoziomWody]
            ja     RysujWode

            dec    ch
            jne    Kopiuj
            RET
     ;=============== DOKONCZENIE WODA ==================
RysujWode:  mov    eax, ds:[si]
            add    si, 4
            add    eax, 89898989h         ;89898989h
            mov    es:[di], eax
            add    di, 4
            dec    cl
            jne    RysujWode

            add    bx, 2
            IF    (bx=12*2) {xor bx, bx}
            add    si, cs:[Blur+bx]

            mov    cl, 4ch           ;Odswiez szerokosc (130h/4)
            add    di, 10h
            add    si, 10h
            dec    ch
            jne    RysujWode
            RET
ENDP

;ES:DI = adres docelowy
;CL = Poziom <0,100>
;AL = kolor wlasciwy;  AH = kolor tla
RysujPasek PROC
             mov   ch, 100         ;CH = 100
             sub   ch, cl          ;CH = Dopelnienie do 100
             or    cl, cl
             je    PasekZero

RPLoop:      mov   byte ptr es:[di], 50
             mov   es:[di+320], al
             mov   es:[di+320*2], al
             mov   es:[di+320*3], al
             mov   es:[di+320*4], al
             mov   byte ptr es:[di+320*5], 60

             inc   di
             dec   cl
             jne   RPLoop

             or    ch, ch
             je    RPEnd
PasekZero:   mov   al, ah
             xchg  cl, ch
             jmp   RPLoop
RPEnd:       RET
ENDP


AddPoz  dw -3*320-3, -3*320-1, -3*320+1, -3*320+3
        dw -2*320-2, -2*320+0, -2*320+2
        dw -1*320-3, -1*320-1, -1*320+1, -1*320+3
        dw  0*320-2,  0*320+2
        dw  1*320-3,  1*320-1,  1*320+1,  1*320+3

RysujWybuch PROC
                   mov   es, cs:[BuforVGA]     ;ES = BuforVGA
                   mov   cx, 19*2              ;CX = Ile punktow
                   mov   ax, cs
                   mov   ds, ax                ;CS => DS
                   lea   si, WybuchPoz         ;DS:[SI] = Pozycje piksli
                   mov   al, 1bh               ;AL = kolor punktow
;                   mov   ah, 20h
                   xor   bx, bx                ;BX = licznik piklsi

RysujPiksle:       mov   di, ds:[si+bx]        ;DI = aktualna pozycja
                   add   di, ds:[AddPoz+bx]    ;przesun o AddPoz

                   cmp   di, 640               ;Jesli wychodzi
                   jb    NieZapisuj            ;z gory to nie zapisuj
                   mov   ds:[si+bx], di        ;Zapisz zmiany

NieZapisuj:        add   di, cs:[Px]           ;Uwzglednik zmiane
                   add   di, cs:[Py]           ;kamery
                   mov   es:[di], al           ;Rysuj piksel

                   add   bx, 2                 ;Nastepna kropka
                   cmp   bx, cx                ;czy to juz
                   jb    RysujPiksle           ;koniec?
                   RET
ENDP

 ;GS:[SI] = adres wybuchajacego obiektu
Wybuch PROC
                   push  es
                   push  di

                   mov   byte ptr cs:[WybuchLicz], 32
                   mov   ax, cs
                   mov   es, ax                     ;ES = CS
                   lea   di, WybuchPoz              ;ES:[DI] = CS:[WybuchPoz]

                   mov   ax, gs:[si+ScrOffset]      ;AX = offset
                   mov   cx, 20
                   rep   stosw

                   pop   di
                   pop   es
                   RET
ENDP

;ES = segment docelowy
Cls PROC
                   xor   di, di
                   mov   cx, 16000
                   xor   eax, eax
                   rep   stosd
                   RET
ENDP

 RGB db 3 dup (0)
PalDown PROC
                   push    es
                   mov     ax, cs
                   mov     ds, ax
                   mov     es, ax

                   mov     ch, 50
PalDownMain:       mov     cl, 255          ;Licznik kolorow
 ;================= O D C Z Y T ==================
SetRGBLoop:        mov     dx, 3c7h         ;DX = 3c8h
                   mov     al, cl
                   out     dx, al

                   inc     dx
                   inc     dx               ;DX = 3c9h
                   lea     di, RGB
                   mov     si, di
                   push    cx
                   insb
                   insb
                   insb
                   pop     cx
 ;================= Z A P I S  ==================
                   dec     dx               ;DX = 3c8h
                   mov     al, cl
                   out     dx, al

                   inc     dx
                   mov     ah, 3
SetRGB:            lodsb
                   dec     al
                   jns     NieZero
                   xor     al, al
NieZero:           out     dx, al
                   dec     ah
                   jne     SetRGB
        ;===================================
                   dec     cl / jne SetRGBLoop
                   call    WaitSync
                   dec     ch / jne PalDownMain
                   pop     es
                   RET
ENDP


WaitSync PROC
               mov     dx, 3dah
_Wait:         in      al, dx
               test    al, 8
               je      _Wait
               RET
ENDP

Splyw PROC

             REPEAT (bl,100) {
                 xor  dx, dx
                 mov  dl, bl
                 shl  dx, 6

                 REPEAT (cx, dx) {
                     call    Random
                     mov     di, ax
                     mov     al, es:[di]
                     IF     (di<320*198) {
                     ;THEN
                         mov     es:[di+320], al
                         mov     es:[di+640], al
                     } ;END IF
                 } ;LOOP CX
                     call    WaitSync
             } ;LOOP BL
               RET
ENDP

;DX = adres palety
SetPal PROC
              push   es
              mov    es, cs:[PalSeg]
              mov    cx, 0ffh           ;Ile Kolorow
              xor    bx, bx             ;Kolor poczatkowy
              mov    ax, 1012h
              int    10h
              pop    es
              RET
ENDP