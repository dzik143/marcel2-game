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

              mov    ah, 4ah
              mov    bx, 400h          ;8 KB na kod
              int    21h

              mov    ah, 48h
              mov    bx, 40h           ;512 bajtow na stos
              int    21h
              mov    ss, ax
              mov    sp, 200h
     ;====================================================
     ;================= SOUND BLASTER ====================
     ;====================================================
             mov     ah, 48h
             mov     bx, 2000h
             int     21h
             jc      LowMem
             dec     ax

SearchMem:   inc     ax
             mov     bx, ax
             shl     bx, 1

             jne     SearchMem
             mov     word ptr ds:[SamplSeg], ax

             lea     dx, SamplPlik
             mov     ax, 3d00h
             int     21h
             mov     bx, ax

             push    ds
             mov     ds, ds:[SamplSeg]
             xor     dx, dx
             mov     cx, 0ffffh
             mov     ah, 3fh
             int     21h
             pop     ds

             mov     ah, 3eh
             int     21h
;==================== WCZYTANIE DANYCH ==========================
              lea    dx, TloPlik        ;CS:DX = CS:[TloPlik]    !Tlo!
              lea    bp, TloSeg         ;CS:BP = CS:[TloSeg]
              mov    bx, 600h           ;BX = Ile par. wczytac
              call   LoadFile

              lea    dx, GrafPlik       ;CS:DX = CS:[GrafPlik]   !Obiekty!
              lea    bp, GrafSeg        ;CS:BP = CS:[GrafSeg]
              mov    bx, 900h          ;BX = Ile par. wczytac
              call   LoadFile

              lea    dx, FontyPlik      ;CS:DX = CS:[FontyPlik]  !Fonty!
              lea    bp, FontySeg       ;CS:BP = CS:[FontySeg]
              mov    bx, 22h            ;BX = Ile par. wczytac
              call   LoadFile
;============================ PALETA ================================
              mov    ax, 13h
              int    10h
              lea    dx, PalPlik        ;CS:DX = CS:[PalPlik]    !Paleta!
              lea    bp, PalSeg         ;CS:BP = CS:[PalSeg]
              mov    bx, 60h            ;BX = Ile par. wczytac
              call   LoadFile
;=========================== BUFORY ================================
              mov    ah, 48h
              mov    bx, 1000h          ;Caly segment
              int    21h                ;na buforowanie
              jc     LowMem
              mov    ds:[BuforVGA], ax  ;ekranu

              mov    ah, 48h
              mov    bx, 400h           ;32 KB obraz
              int    21h                ;etapu
              jc     LowMem
              mov    ds:[EtapSeg], ax
              mov    fs, ax             ;! FS = Etap !

              mov    ah, 48h
              mov    bx, 800h           ;32 KB na dane
              int    21h                ;obiektow
              jc     LowMem
              mov    ds:[Obiekty], ax
              mov    gs, ax             ;! GS = Obiekty !

              mov    ah, 48h
              mov    bx, 1000h
              int    21h
              jc     LowMem
              mov    ds:[PlenerSeg], ax
  ;============= LOSUJ POZYCJE DLA SNIEGU ============
             mov     cx, 20            ;CX = ile platkow = 20
             lea     si, Snieg         ;DS:SI = CS:[Snieg]
FillLoop:    call    Random            ;AX = liczba losowa
             mov     ds:[si], ax       ;AX => [SI]
             add     si, 2             ;Nast. platek
             loop    FillLoop          ;Zapetl

             call    InstallKey
             call    InstallMIDI

             lea     dx, MidiPlik
             call    PlayMIDI
             call    ResetSB

         ;    mov     ah, 4ch
          ;   int     21h
           ;  jmp     Koniec
