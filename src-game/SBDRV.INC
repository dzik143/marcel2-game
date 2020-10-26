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

SbConfig:
      SBPort   dw 220h
         DMA   db 01h
         IRQ   db 05h
    PagePort   dw 83h
          Sb   db 0ffh         ;0 = O.K;  1 = Zajety;  2 = brak karty


     SamplPlik db 'SOUND.BDS',0
      SamplSeg dw 0

;===========================================================================
;======= INICJUJE UKLAD DSP ================================================
;=    Wyjscie: CF = 0: SB O.K                                              =
;=             CF = 1: Blad inicjacji                                      =
;===========================================================================

 SbAdres dd 0

ResetSB PROC

             ;========= PRZERWANIE ===========

                mov     ah, 35h
                mov     al, byte ptr cs:[IRQ]
                add     al, 8
                int     21h
                mov     word ptr cs:[SbAdres], es
                mov     word ptr cs:[SbAdres+2], bx

                mov     ah, 25h
                push    cs
                pop     ds
                lea     dx, SbIRQ
                int     21h

             ;============= INICJACJA ===========

                mov     dx, word ptr cs:[SBPort]
                add     dx, 6                          ;DX = 2x6h (Reset Port)
                mov     al, 1
                out     dx, al                         ;0 => 2x6h

                mov     cx, 1000
                loop    $

                xor     al, al
                out     dx, al                         ;1 => 2x6h

                mov     cx, 100
WaitOnSB:       call    ReadDSP
                cmp     al, 0aah
                je      SbOK
                loop    WaitOnSB

           ;======== !! BLAD !! ========

                mov     ah, 4ch
                int     21h

SbOK:           mov     al, 0d1h
                call    WriteDSP                       ;Wlacz przetwornik DAC
                RET
ENDP
;===========================================================================
;=========== ZAPIS DANYCH DO UKLADU DSP ====================================
;=     Wejscie: AL = bajt do wyslania                                      =
;=          !! UWAGA Niszy DX !!                                           =
;===========================================================================

WriteDSP PROC
                push   ax
                mov    dx, word ptr cs:[SBPort]       ;DX = 2xCh
                add    dx, 0ch                        ;Write/Status Port

                in     al, dx
                test   al, 80h                        ;7 bit znaczy, ze DSP
                jne    $-5                            ;jest gotowy do zapisu

                pop    ax
                out    dx, al
                RET
ENDP

;===========================================================================
;=========== ODCZYT DANYCH Z UKLADU DSP ====================================
;=     Wyjscie: AL = odczytany bajt                                        =
;=          !! UWAGA Niszy DX !!                                           =
;===========================================================================

ReadDSP PROC
                mov    dx, word ptr cs:[SBPort]       ;DX = 2xEh
                add    dx, 0eh                        ;(Data Available Port)

                in     al, dx
                test   al, 80h                        ;7 bit znaczy, ze dane
                je     $-5                            ;sa gotowe do odebrania

                sub    dx, 4                          ;DX = 2xAh (Data Read
                in     al, dx                         ;              Port)
                RET
ENDP

DSP MACRO bajt
            mov      al, bajt
            call     WriteDSP
ENDM

;===========================================================================
;============ ODTWARZA PROBKE ==============================================
;=       Wejsice: AL = nr. probki                                          =
;===========================================================================

PlaySB PROC
              push  ds

              cmp   al, byte ptr cs:[Sb]
              jb    Graj
              jmp   SbZajety

Graj:         mov   byte ptr cs:[SB], al             ;zaznacz, ze zajety
              xor   ah, ah
              shl   ax, 2
              mov   bp, ax

              mov   bh, byte ptr cs:[DMA]            ;BH = DMA
              mov   al, bh                           ;bity 0-1 = nr. kanalu
              add   al, 4                            ;bit 2: 0 - Odblokuj
              out   0ah, al                          ;       1 - Zablokuj

              mov   ax, word ptr cs:[SamplSeg]       ;AX = Segment Probki
              mov   ds, ax
              mov   bl, ah                           ;BL = starszy bajt Seg.
              shr   bl, 4                            ;BL = Strona (4 najb.
                                                     ;    znaczace bity seg.)

              shl   ax, 4                            ;AX = Seg*10h
              add   ax, word ptr ds:[bp]             ;AX = Dodaj Offset

         ;=============== USTAW ADRESS PORT ==============

              xor   dh, dh
              mov   dl, bh                           ;DX = DMA
              shl   dx, 1                            ;DX = DMA*2 (Addres Port)

              out   dx, al                           ;Mlodszy bajt adresu
              mov   al, ah
              out   dx, al                           ;Starszy bajt adresu

          ;=============== USTAW LENGHT PORT ===============

              inc   dx                               ;DX=DMA*2+1 (Lenght Port)
              mov   ax, word ptr ds:[bp+2]           ;AX = Dlugosc probki
              out   dx, al                           ;Mlodszy bajt dlugosci
              mov   al, ah
              out   dx, al                           ;Starszy bajt dlugosci

          ;============== USTAW PAGE PORT =================

              mov   al, bl                           ;AL = Nr. strony
              mov   dx, word ptr cs:[PagePort]
              out   dx, al                           ;Page port
           ;=============== USTWA TRYB ===============

              mov   al, 48h                          ;Tryb 48h
              add   al, bh                           ;bity 0-1: nr. DMA
              out   0bh, al                          ;0BH = Port trybu DMA

              mov   al, bh
              out   0ah, al                          ;Odblokuj DMA

              DSP   40h                              ;Ust. czest.
              DSP   131                              ;256-1000.000/Czest.
                                                     ;ust. na 8 KHz

              DSP   14h                              ;Ust. Dl. probki
              mov   ax, word ptr ds:[bp+2]
              call  WriteDSP                         ;Mlodszy bajt dlugosci
              mov   al, ah
              call  WriteDSP                         ;Starszy bajt dlugosci
              DSP   91h                              ;Graj...

              mov   ah, 1
              mov   cl, byte ptr cs:[IRQ]
              dec   cl
              shl   ah, cl
              in    al, 21h
              and   al, ah
              out   21h, al                         ;Odblokuj odpowiednie IRQ

SbZajety:     pop   ds
              RET
ENDP

;===========================================================================
;============ PRZERWANIE SOUND BLASTERA ====================================
;===========================================================================

SbIRQ PROC
              push  dx
              push  ax

              mov   byte ptr cs:[SB], 0ffh          ;zaznacz ,ze juz koniec
              mov   dx, word ptr cs:[SBPort]
              add   dx, 0eh
              in    al, dx                          ;Znak dla karty ,ze
                                                    ;juz koniec
              mov   al, 20h
              out   20h, al

              pop   ax
              pop   dx
              IRET
ENDP
