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

            mov   dx, 768            ;Ustaw palete
            call  SetPal             ;ognia
            mov   dx, 3d4h
            mov   ax, 0209h          ;Wysokosc
            out   dx, ax
  ;===============================================================
  ;============== S Z E F O  &  S Z E F O  (C) 2002 ==============
  ;===============================================================
            mov   dx, 3c8h / mov al,48 / out dx, al       ;23 => 3c8h
            xor   al, al / inc dx                         ;AL=0 ^ DX = 3c9h
            out   dx, al / out dx, al / out dx, al        ;0 => 3c9h rep 3

            push  0a000h / pop  es                        ;ES = 0a000h
            mov   di, 320*55+35 / mov  al, 36             ;AL = nr. rys
            call  RysujObiekt                             ;"Szefo & Szefo"
            call  Sleep

            xor   ah, ah
            REPEAT  (cl, 50) {
            ;DO
               mov   dx, 3c8h                             ;DX = 3c8h
               mov   al, 48 / out dx, al                   ;23 => 3c8h
               inc   dx                                   ;DX = 3c9h
               mov   al, ah / out dx, al                  ;AH => 3c9h (R)
               xor   al, al / out dx, al / out dx, al
               inc   ah

              REPEAT (ch,255) {call WaitSync}
            } ;LOOP CL


             call Sleep / call Splyw
             mov  dx, 768 / call SetPal                   ;Ustaw palete ognia

             mov      di, 320*55+90 / mov  al, 37          ;AL = nr. rys
             call     RysujObiekt                          ;"Marcel2"
             call     Sleep / call Splyw / call PalDown
             mov      dx, 768 / call SetPal / call Cls    ;Ustaw palete ognia

             call     Menu
