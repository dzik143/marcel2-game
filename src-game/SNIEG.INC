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

             cmp   byte ptr cs:[SniegFlag], 1
             jne   NieSnieg

             mov   es, cs:[BuforVGA]       ;ES = BuforVGA
             mov   ax, cs
             mov   ds, ax                  ;CS => DS
             mov   cx, 20                  ;CX = Ile platkow = 20
             lea   si, Snieg               ;DS:[SI] = CS:[Snieg]

RysujSnieg:
             mov   di, ds:[si]             ;DI = offset platka
             call  Random                  ;AX = random

             add   di, 320                 ;Platek w dol
             test  ah, 1                   ;Jesli 0 bit AH
             je    NoDown                  ;ustawiony to
             add   di, 320                 ;jeszcze o jeden w dol

NoDown:
             and   ax, 1                   ;AX = AddX platka (0 v 1)
             add   di, ax

             mov   ds:[si], di             ;Zapisz nowy offset
             mov   byte ptr es:[di], 15    ;Rysuj platek

             add   si, 2                   ;Nast. platek
             loop  RysujSnieg              ;Zapetl
NieSnieg:
