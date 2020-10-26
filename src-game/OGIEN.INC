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
;========== RYSUJE OGIEN ===================================================
;==    Wejscie: ES = segment docelowy                                      =
;===========================================================================

         ;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
         ;³ LOSUJ 1 LINIE ³
         ;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
             call   Random

             mov    bp, 320*198
             mov    cx, 320

LosujMain:   ror    ax, 3
             add    al, ah
             cmp    al, 110     ;100
             ja     NieRysuj

             mov    byte ptr es:[bp], 255
NieRysuj:    inc    bp
             loop   LosujMain
          ;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
          ;³ RYSUJ RESZTE ³
          ;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
             mov    bp, 320*120
             mov    cx, 79*320
             xor    dh, dh
             xor    ah, ah
@1:
             mov    al, es:[bp]
             mov    dl, es:[bp+320]
             add    ax, dx
             mov    dl, es:[bp+319]
             add    ax, dx
             mov    dl, es:[bp+321]
             add    ax, dx
             shr    ax, 2
             je     NieDec
             dec    ax
NieDec:
             mov    es:[bp], al
             inc    bp
             loop   @1

       ;======= Dolna czesc buforu => gorna (widoczna) czesc =======
             mov    ax, es
             mov    ds, ax
             mov    si, 320*120
             mov    di, 320*40
             mov    cx, 78*320/4
             rep    movsd
