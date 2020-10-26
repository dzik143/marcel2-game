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

KLAW_TSR PROC
             push     ax
             push     bx

             xor      bh, bh
             in       al, 60h
             test     al, 80h
             je      _push
  ;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ;³ KLAWISZ PUSZCZANY ³
  ;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
_pop:        and      al, 01111111b
             mov      bl, al
             mov      byte ptr cs:[Key+BX], 0
             jmp     _Koniec_Key
  ;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ;³ KLAWISZ WCISKANY ³
  ;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
_push:       mov      bl, al
             mov      byte ptr cs:[Key+BX], 1
_Koniec_Key:
             mov      al, 20h
             out      20h, al

             pop      bx
             pop      ax
             IRET
ENDP

Old09h dd 0
InstallKey PROC
         mov     ax, 3509h
         int     21h
         mov     word ptr cs:[Old09h+2], es
         mov     word ptr cs:[Old09h], bx

         mov     ax, cs
         mov     ds, ax
         mov     es, ax

         lea     dx, KLAW_TSR
         mov     ax, 2509h
         int     21h

         lea     di, Key
         mov     cx, 40h
         xor     ax, ax
         rep     stosw
         RET
ENDP

UnInstallKey  PROC
             lds   dx, Old09h          ;DS:DX = [Old09h+2] : [Old09h]
             mov   ax, 2509h
             int   21h
             RET
ENDP
