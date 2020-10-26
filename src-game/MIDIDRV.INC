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

;CS:[DX] = adres nazwy pliku
Uchwyt dw ?
PlayMIDI PROC

      ;================ WCZYTAJ NAGLOWEK ================

             push     cs
             pop      ds
             mov      ax, 3d00h
             int      21h                 ;Otwiera plik CS:[DX]
             mov      cs:[Uchwyt], ax
             mov      bx, ax

             lea      dx, MidHead         ;DS:DX = CS:[MidHead]
             mov      cx, 14              ;CX = 14
             mov      ah, 3fh
             int      21h                 ;File => CS:[MidHead] * 14

             mov      ax, cs:[Tracks]     ;AX = Track
             xchg     ah, al              ;AX => Intel Word
             mov      cs:[Tracks], ax     ;Zapisz

      ;================ WCZYTAJ DANE SCIEZEK ================

             xor      dx, dx                      ;tu liczy adresy sciezek
             xor      di, di                      ;DI = nr. sciezki
             mov      cx, cs:[Tracks]             ;CX = ile sciezek
LoadTrack:   push     cx

                 xor      ax, ax
                 mov      cs:[TrackEnd+di], al        ;TrackEnd = 0

                 shl      di, 1
                 inc      dx
                 mov      cs:[TrackOfs+di], dx        ;zapisz adres sciezki+1
                 mov      cs:[TrackTime+di], ax       ;TrackTime = 0
                 dec      dx
                 shr      di, 1


                 push     dx
                 push     cs
                 pop      ds
                 lea      dx, TrackHead
                 mov      cx, 8
                 mov      bx, cs:[Uchwyt]
                 mov      ah, 3fh
                 int      21h
                 pop      dx


                 lea      bp, TrackLEN                ;CS:[BP] = CS:[TrackLEN]
                 call     SwapDW                      ;Zamienia bajty w DWORD

                 mov      ds, cs:[MidiSEG]              ;DS = MidiSEG
                 mov      cx, word ptr cs:[TrackLEN]    ;CX = dl. sciezki
                 mov      ah, 3fh
                 int      21h

                 add      dx, word ptr cs:[TrackLEN]    ;obl. adres nast.
                 inc      di                            ;BX = nr. sciezki

             pop      cx                          ;CX = ile sciezek
             loop     LoadTrack

             mov      bx, cs:[Uchwyt]
             mov      ah, 3eh
             int      21h



             mov     al, 36h
             out     43h, al

             mov     ax, cs:[Dzielnik]          ;AX = Dzielnik
;             SHL     AX, 2
             xchg    ah, al                     ;AX => INTEL WORD
             cwd                                ;AX => EAX
             mov     ebx, eax                   ;EBX = Dzielnik
             mov     eax, 1234DCh               ;EAX = 1234dch
             div     ebx                        ;DX:AX = 1234dch/Dzielnik
             SHR    ax, 2

;             mov     al, ah         ;112
             out     40h, al
             mov     al, ah         ;23
             out     40h, al


             mov      byte ptr cs:[MIDIFlag], 1   ;PlayMIDI ON
             RET
ENDP

;ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ    Wysyla bajt do ukladu MIDI                                         บ
;บ       Wejscie: AL = bajt do wyslania                                  บ
;ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

WriteMIDI PROC
               push    dx
               push    ax

               mov     dx, 331h    ;Status Port
MidiWaitW:
               in      al, dx
               test    al, 40h     ;czekaj az bit 7=0 (uklad gotowy do zapisu)
               jne     MidiWaitW

               pop     ax
               dec     dx          ;DX= 330h - port danych
               out     dx, al

               pop     dx
               RET
ENDP

;ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ    Odbiera bajt z ukladu MIDI                                         บ
;บ       Wyjscie: AL = bajt zwrocony przez karte                         บ
;ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
ReadMIDI PROC
               push    dx

               mov     dx, 331h    ;Status Port
MidiWaitR:
               in      al, dx
               test    al, 80h     ;czekaj az bit 6 = 0 (uklad gotowy)
               jne     MidiWaitR

               dec     dx          ;DX= 330h - port danych
               in      al, dx

               pop     dx
               RET
ENDP

;ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ    Wysyla komende do ukladu MIDI                                      บ
;บ       Wejscie: AL = bajt do wyslania                                  บ
;ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
WriteCommandMIDI PROC
               push    dx
               push    ax

               mov     dx, 331h    ;Status Port
MidiWaitWC:
               in      al, dx
               test    al, 40h     ;czekaj az bit 7=0 (uklad gotowy do zapisu)
               jne     MidiWaitWC

               pop     ax
               out     dx, al      ;331h = Command Port

               pop     dx
               RET
ENDP

;ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
;บ    Resetuje uklad MIDI i instaluje sterownik na IRQ 08h               บ
;ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

Old1ch dd ?     ;bufor na org. adres INT 0x1c

InstallMIDI PROC
                mov     al, 0ffh
                call    WriteCommandMIDI   ;komenda 0ffh = RESET KARTY

                mov     cx, 0ffffh
WaitForReady:   call    ReadMIDI
                dec     cx
                je      MidiDoDupy
                cmp     al, 0feh           ;0feh = reset O.K
                jne     WaitForReady

                mov     al, 3fh            ;kom. 3fh = tryb Uart (??!!)
                call    WriteCommandMIDI


                mov      ax, 351ch
                int      21h
                mov      word ptr cs:[Old1ch+2], es
                mov      word ptr cs:[Old1ch], bx

                push     cs
                pop      ds
                lea      dx, IntMIDI
                mov      ax, 251ch
                int      21h

                mov      ah, 48h
                mov      bx, 1000h
                int      21h
                jc       LowMem
                mov      cs:[MidiSeg], ax
                RET


  MidiFalse db 'MIDI jest do dupy!!!$'
MidiDoDupy:     push     cs
                pop      ds
                lea      dx, MidiFalse
                mov      ah, 9
                int      21h

                mov      ah, 4ch
                int      21h
ENDP

UninstallMIDI PROC
                mov     ds, word ptr cs:[Old1ch+2]
                mov     dx, word ptr cs:[Old1ch]
                mov     ax, 251ch
                int     21h
                RET
ENDP


       LiczMIDI  dw 0
      CommandLen db 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2
                 db 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 ;64

                 db 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 ;32
                 db 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,2,1,0,0,0,0,0,0,0,0,0,0,0,0 ;16  12
        MidiFlag db 0
IntMIDI PROC
             cmp      byte ptr cs:[MidiFlag], 1
             jne      IntMIDIEnd
             push     ds
             push     si
             push     ax
             push     bx
             push     cx
             push     dx
             push     bp

             mov      ds, word ptr cs:[MidiSEG]   ;DS = MidiSEG
             xor      bp, bp                      ;BP = nr. sciezki
             xor      bx, bx                      ;BX = nr. sciezki*2
             mov      cx, cs:[Tracks]             ;CX = Ilosc sciezek
PlayTrack:
                   cmp     byte ptr cs:[TrackEnd+bp], 1
                   je      NextTrack
                   mov     ax, cs:[TrackTime+bx]    ;AX = delta time
                   cmp     ax, cs:[LiczMIDI]
                   jne     NextTrack

                   mov     si, cs:[TrackOfs+bx]      ;DS:SI = adres sciezki

                   cmp     byte ptr ds:[si], 80h
                   jb      OldCommand
                   lodsb                             ;AL = bajt ze sciezki
                   mov     cs:[TrackOld+bp], al      ;ustaw nowa ostatnia kom.
OldCommand:        mov     al, cs:[TrackOld+bp]      ;AL = ostatnia komenda


                   cmp     al, 0ffh
                   jne     NieMeta
        ;============= JESLI  META KOMENDA (0xff) =================
                   lodsb                             ;odczytaj kod komendy
                   cmp     al, 2fh
                   jne     NoEndTrack
                   mov     byte ptr cs:[TrackEnd+bp], 1
NoEndTrack:
                   xor     dx, dx                    ;DX = dlugosc komendy
                   xor     ax, ax

LiczLEN:           shl     dx, 7
                   lodsb

                   and     al, 07fh                  ;0111 1111b
                   add     dx, ax
                   test    byte ptr ds:[si-1], 080h
                   jne     LiczLEN

                   add     si, dx
                   jmp     LiczDelte
     ;================= NIE META KOMENDA ===================
NieMeta:           call    WriteMIDI                 ;AL => MIDI

                   push    bx
                   sub     al, 80h                   ;AL = komenda - 80h
                   xor     bh, bh
                   mov     bl, al                    ;BX = komenda - 80h
                   mov     ah, cs:[CommandLEN+bx]    ;BL = dlugosc komendy
                   pop     bx

                   or      ah, ah
                   je      LiczDelte

SendToMIDI:        lodsb                             ;Wyslij AH kolejnych
                   call    WriteMIDI                 ;bajtow ze sciezki do MID

                   dec     ah
                   jne     SendToMIDI
   ;================== LICZ NOWA DELTE ====================
LiczDelte:         xor     dx, dx                   ;EDX = Delta time
                   xor     ax, ax

LiczDT:            shl     dx, 7                    ;Delta << 7
                   lodsb                             ;AL = DS:[SI]   SI++
                   and     al, 07fh                  ;0111 1111b
                   add     dx, ax
                   test    byte ptr ds:[si-1], 080h
                   jne     LiczDT

                   add     word ptr cs:[TrackTime+bx], dx     ;zapisz nowa delte
                   mov     word ptr cs:[TrackOfs+bx], si      ;zapisz nowy offset

                   jmp     PlayTrack

NextTrack:   add      bx, 2
             inc      bp
             dec      cx
             jne      PlayTrack

             inc     word ptr cs:[LiczMIDI]

             pop      bp
             pop      dx
             pop      cx
             pop      bx
             pop      ax
             pop      si
             pop      ds
IntMIDIEnd:  IRET
ENDP

;CS:BP = adres DWORD do zamiana
SwapDW PROC
            mov    ax, word ptr cs:[bp]           ;1 2 3 4
            xchg   ah, al                         ;2 1 3 4
            xchg   ax, word ptr cs:[bp+2]         ;3 4 2 1
            xchg   ah, al
            mov    word ptr cs:[bp], ax           ;4 3 2 1
            RET
ENDP
