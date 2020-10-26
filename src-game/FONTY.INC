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

;===========================================================================
;======== WYPISUJE TEKST NA EKRANIE (GRAFICZNIE) ===========================
;=     Wejscie: CS:BP = adres tekstu do wypisania zakonczony. '$'          =
;=              ES:DI = adres docelowy                                     =
;=              BH = Kolor tekstu                                          =
;===========================================================================
Kursor_Tmp     dw 0
PiszG PROC
               push     ax
               push     dx
               push     di

               mov      dx, di                       ;DX = polozenie kursora

CzytZnak:      mov      al, cs:[bp]
               inc      bp

               cmp      al, '$'
               je       KoniecP
      ;อออออออออ spacja ออออออออ
               cmp      al, ' '
               jne      NieSpc
               add      di, 8
               jmp      CzytZnak
NieSpc:      ;อออออออออ enter อออออออออ
               cmp      al, 0feh
               jne      NieEnt

               add      dx, 320*8                         ;320*8
               mov      di, dx
               jmp      CzytZnak
NieEnt:  ;อออออออออ zmiana koloru ออออออออ
               cmp     al, 0fdh
               jne     NieKolor
NieKolor:;อออออออออ pauza ออออออออออ
               cmp     al, 0fch
               jne     NieStop

NieStop:       call    RysujZnak
               add     di, 8
               jmp     CzytZnak

KoniecP:       pop     di
               pop     dx
               pop     ax
               RET
ENDP


;===========================================================================
;====== RYSUJE POJEDYNCZY ZNAK =============================================
;=   Wejscie: AL = kod ascii                                               =
;=            ES:DI = adres docelowy                                       =
;===========================================================================
RysujZnak PROC
          push    ds
          push    si
          push    di
          push    cx
          push    bx             ;KOLOR!!

          sub     al, 33         ;Przelicza kod ascii na numer znaku w tablicy}
          xor     ah, ah
          shl     ax, 3          ;AL:= AL*8

          mov     ds, cs:[FontySeg]
          mov     si, ax
          mov     cx, 808h         ;Kazdy znak jest opisany przez 8 bajtow

MainRZ:   lodsb                    ;Laduje bajt z DS:SI do AL i zwieksza SI
          mov     ah, 1            ;AH = 0000 0001b
LineRZ:   test    al, ah
          je      NieRysF
          mov     byte ptr es:[di], bh
NieRysF:  inc     di
          shl     ah, 1            ;Kolejny bit
          dec     cl
          jne     LineRZ

          mov     cl, 8
          inc     bh               ;kolor
          add     di, 312          ;Nastepna linia
          dec     ch
          jne     MainRZ

          pop     bx
          pop     cx
          pop     di
          pop     si
          pop     ds
          RET
ENDP
