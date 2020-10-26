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

   ;=================== P I O N O W O =====================
               mov    ax, word ptr gs:[y]          ;AX = y gracza
               shr    ax, 4                        ;AX = y /16
               mov    bx, word ptr cs:[KameraY]    ;BX = KameraY
               sub    ax, bx                       ;AX = y wzg. kamery

               IF (ax<5) {
               ;THEN
                   or   bx, bx         ;jesli poczatek ekranu
                   je   ^              ;to wyskok z warunku

                   IF (cs:[Py]=16) {
                   ;THEN
                        dec cs:[KameraY]
                        mov cs:[Py], 0}\
                   ;ELSE
                        add   cs:[Py], 4\          ;2
                 }

               IF (ax>6) {
               ;THEN
                   mov   ax, cs:[WysEtp]  ;AX = wysokosc etapu
                   add   bx, 11           ;BX = KameraY+11
                   cmp   bx, ax           ;jesli juz koniec ekranu
                   jae   ^                ;to wyskok z warunku

                   IF (cs:[Py]=0) {
                   ;THEN
                        inc cs:[KameraY]
                        mov cs:[Py], 16}\
                   ;ELSE
                        sub   cs:[Py], 4\
                 }

 ;======================= P O Z I O M O =======================
                 mov    ax, gs:[x]         ;AX = X gracza
                 shr    ax, 4              ;AX = x/16
                 sub    ax, cs:[KameraX]   ;AX = X wzg. Kamery

                 IF (ax>9) {
                 ;THEN
                     mov  ax, cs:[KameraX]
                     add  ax, 20
                     cmp  ax, cs:[SzerEtp]
                     jae  ^
                     IF (cs:[Px]=0) {
                     ;THEN
                        inc    word ptr cs:[KameraX]
                        mov    word ptr cs:[Px], 14
                     }\
                     ;ELSE
                        sub    word ptr cs:[Px], 2
                     \
                     IF (cs:[PlenerX]=0) {mov  cs:[PlenerX], 140h}
                     dec  word ptr cs:[PlenerX]
                  } ; End If

                 mov    ax, word ptr gs:[x]      ;AX = X
                 shr    ax, 4                    ;AX = x/16
                 sub    ax, cs:[KameraX]         ;AX = X wzg. Kamery

                 IF (ax<9) {
                 ;THEN
                    mov  bx, cs:[KameraX]        ;BX = KameraX
                    or   bx, bx                  ;Jesli poczatek ekranu
                    je   ^                       ;to wyskok z warunku

                    IF (cs:[Px]=16) {
                    ;THEN
                       dec    word ptr cs:[KameraX]
                       mov    word ptr cs:[Px], 2
                    }\
                    ;ELSE
                       add    word ptr cs:[Px], 2
                     \

                    inc  word ptr cs:[PlenerX]
                    cmp  word ptr cs:[PlenerX], 140h
                    jne  ^
                    mov  word ptr cs:[PlenerX], 0
                 } ; END if