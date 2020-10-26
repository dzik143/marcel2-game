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
;=== WYPISUJE NA EKRANIE ZAWARTOSC AL ======================================
;===========================================================================

BuforP  db '00h',10,13,'$'
Matryca db '0123456789ABCDEF'

PiszAL PROC
        push bx
        push ds
        push es
        push ax
        push dx
        push si
        push di

        mov bx, cs
        mov ds, bx
        mov es, bx

        lea bx, Matryca
        mov di, (Offset BuforP)+1
        mov byte ptr es:[di-1], '0'

        std
        mov dl, 10h
MainP:
        xor ah, ah        ;Wyczysc reszte
        div dl            ;AL=AX/DL,  AH=Reszta
        xchg al, ah       ;AL=Reszta, AH=Iloraz (AX/DL)
        xlat              ;AL=DS:[BX+AL]
        stosb

        mov al, ah
        cmp al, 0         ;Sprawdz czy iloraz = 0
        jne MainP
        cld

        push  cs
        pop   ds
        lea   dx, BuforP
        mov   ah, 09h
        int   21h

        pop di
        pop si
        pop dx
        pop ax
        pop es
        pop ds
        pop bx
        RET
ENDP

BuforPW   db '0000H$'
PiszAX PROC
        push   bx
        push   ds
        push   es
        push   ax
        push   dx
        push   si
        push   di

        mov    bx, cs
        mov    ds, bx
        mov    es, bx

        push   ax
        lea    bx, Matryca
        lea    di, BuforPW
        mov    ax, 3030h         ;'00'
        stosw
        stosb
        pop    ax

        std
        mov    cx, 10h
MainPW:
        xor    dx, dx        ;Wyczysc reszte
        div    cx            ;AX=AX/CX,  DX=Reszta
        xchg   dx, ax        ;AL=Reszta, AH=Iloraz (AX/DL)
        xlat                 ;AL=DS:[BX+AL]
        stosb

        mov    ax, dx
        cmp al, 0            ;Sprawdz czy iloraz = 0
        jne MainPW
        cld

        push  cs
        pop   ds
;        mov   dx, di
        LEA   DX, BuforPW
        mov   ah, 09h
        int   21h

        pop di
        pop si
        pop dx
        pop ax
        pop es
        pop ds
        pop bx
        RET
ENDP

;===========================================================================
;=== Zamienia liczbe zawarta w AX na ciag znakow ASCII =====================
;      Wejscie: AX: liczba do zamiany                                      =
;      Wyjscie: cs:[BP]: Ciag ASCII (6 bajtow)                             =
;===========================================================================
ZamienAX PROC
               mov    dword ptr cs:[bp], '0000'
               mov    word ptr cs:[bp+4], '$0'
               add    bp, 4

               mov    bx, 0ah
PiszMain:      xor    dx, dx
               div    bx                  ;AX = AX/10;  DX = Reszta

               mov    byte ptr cs:[bp], dl
               add    byte ptr cs:[bp], 48
               dec    bp

               or     ax, ax
               jne    PiszMain

               RET
ENDP
