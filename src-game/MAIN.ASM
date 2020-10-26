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

.386
ASSUME cs:CODE
CODE SEGMENT use16
org  100h
            include  OBIEKTY.INM              ;Makra do obiektow
Start:      include  LOAD.INC                 ;wczytaj dane
            include  INTRO.TMS
    ;        call     Menu


Main:       call     RysujEtap
            include  KAMERA.TMS
            include  OBIEKTY.TMS
            include  SNIEG.INC
            include  DESZCZ.INC

            mov      dx, 3dah
Wait_:      in       al, dx / test al, 8 / je  Wait_
            call     BuforToScreen

   ;==================================================================
   ;============= W Y P I S Y W A N I E   P U N K T O W ==============
   ;==================================================================

             mov    ax, cs / mov ds, ax          ;CS => DS
             mov    ax, 0a000h / mov es, ax      ;ES = 0a000h

             mov    di, 320*185                  ;DI = 320*185
             mov    cx, gs:[Ener] / shl cl,2     ;CX = Energia gracza <0,100>
             mov    ax, 2e1ah
             call   RysujPasek                   ;Rysje pasek w ES:DI o dl. CL

             mov    al, ds:[Stan]
             TST    (al,1) {
             ;THEN
                mov    ds:[_Kier], 1
                mov    di, 320*185+300 / mov al, 31 / call RysujObiekt
                jmp    JestKarta
             }
            ;ELSE
                mov    di, 320*185+300
                mov    cx, 1405h
                xor    eax, eax
Czysc:          mov    es:[di], eax
                add    di, 4
                dec    cl
                jne    Czysc

                mov    cl, 5
                add    di, 140h-20
                dec    ch
                jne    Czysc


            ;END TEST
JestKarta:

      ;====================================================
      ;=================== W O D A ========================
      ;====================================================

            IF   (ds:[AddBlur]=11*4) {mov ds:[AddBlur],0}
            add   ds:[AddBlur], 2

            mov   ax, gs:[y]              ;AX = y gracza
            sub   ax, 8

            IF (ax=ds:[PoziomWody]) {
            ;THEN
                mov  di, 320*192           ;DI = 320*190
                mov  cx, 819h
                xor  eax, eax
ClearPasek:     mov  es:[di], eax
                add  di, 4
                dec  cl
                jne  ClearPasek

                add  di, 220
                mov  cl, 19h
                dec  ch
                jne  ClearPasek
            } ;END IF

            IF   (ax>ds:[PoziomWody]) {
            ;THEN
                mov  di, 320*192           ;DI = 320*190
                mov  cx, ds:[Tlen]         ;CX = Tlen <0,800>
                shr  cx, 3                 ;Zamien na skale <0,100>
                mov  ah, 48
                mov  al, 66
                call RysujPasek            ;Rysje pasek w ES:DI o dl. CL

                dec  word ptr ds:[Tlen]/jne ^
                inc  word ptr ds:[Tlen]
                dec  word ptr gs:[Ener]
            }\
            ;ELSE
                mov  word ptr ds:[Tlen], 800
             \ ;END IF

    ;==================================================
    ;============ T E K S T   I N F O =================
    ;==================================================

            IF (ds:[TekstLicz]=255) {
            ;THEN
               mov   bp, ds:[TekstOfs]
               mov   bh, 85
               mov   di, 320*187+110
               call  PiszG
            } ;END IF

            mov  al, ds:[TekstLicz]
            IF  (al>1) {dec ds:[TekstLicz]}\
            ;ELSE
              or     al, al / je ^
              dec    ds:[TekstLicz]
              mov    di, 320*187+110
              mov    eax, 2e2e2e2eh
              REPEAT (cx, 8*80) {mov es:[di], eax / add di, 4}
            \ ;END IF

            include  STER.TMS
            jmp      Main

Koniec:
            call    UninstallKey
            call    UninstallMIDI
            mov     ah, 4ch
            int     21h


            include FILE.TMS
            include GRAF.TMS
            include KEYBOARD.INC
            include OBKTPROC.TMS
            include PISZ.INC
            include FONTY.INC
            include MIDIDRV.INC
            include SBDRV.INC
            include MENU.TMS

          MidiPlik  db 'WATRWLD1.MID',0
           PalPlik  db 'PALETA.PAL',0
          GrafPlik  db 'OBIEKTY.SGB',0
           TloPlik  db 'TLO.SGB',0
         FontyPlik  db 'FONTY.SGF',0


        ClosedTekst db 'NIE MASZ CHUJU KARTY!$'
        OpenedTekst db 'DRZWI OTWARTE...$'
         TekstLicz  db  0        ;cs:[TekstLicz]>0 => wysw. TekstOfs
         TekstOfs   dw  0        ;Adres wyswietlenego tekstu

              Stan  db  0             ;Stan gracza
               Blur dw  0, 1, 1, 1, 1, 0, 0, 0, -1, -1, -1, -1
            AddBlur dw  0
               Tlen dw  0              ;Ilosc powietrza pod woda

         PoziomWody dw  16*16+8        ;Wszystko ponizej jest pod woda
      GraczNaWyspie db  0              ;1 jesli jedzie na "wyspie"
           MigaLicz db  0              ;>0 <=> gracz miga (niesmiertelny)


           Jasnosc  db  0             ;wsp. jasnosci
           GunReady db  1             ;1 jesli mozna strzelac
           PlenerX  dw  0             ;przesuniecie pleneru
           ChodAnm  db 0,1,2,1              ;wzorzec chodzenia
              Skok  db -4,-4,-5,-6,-6,-7,-6,-5 ;wzorzec skoku
                Px  dw  0            ;Przsuniecie kamery w poziomie
                Py  dw  0            ;Przesuniecie kamery w pionie
        WybuchLicz  db  0            ;Licznik wybuchu (ile do konca)

           KameraY  dw  0            ;Polozenie kamery w pionie
           KameraX  dw  ?            ;Polozenie kamery w poziomie
         WybuchPoz  dw  20 dup (?)   ;pozycje piksli w wybuchu
            PalSeg  dw  ?            ;paleta
           GrafSeg  dw  ?            ;rysunki obiektow
            TloSeg  dw  ?            ;rysunki tla (16x16)
          BuforVGA  dw  ?            ;buforowanie ekranu
         PlenerSeg  dw  ?            ;bufor na plener
           Obiekty  dw  ?            ;dane obiektow
           IleObkt  dw  ?            ;ilosc obiektow

           EtapSeg  dw  ?            ;obraz planszy
          FontySeg  dw  ?            ;segment na dane fontow
             Tekst  db 6 dup (?)     ;Pomocniczy bufor do wypisywania liczb

         SniegFlag  db  ?            ;Jesli =1 to pada snieg
        DeszczFlag  db  ?            ;Jesli =1 to pada deszcz
       PunktyGracz  dw  ?

            Sciana  EQU 29

             IdEtp  db  7 dup (?)    ;Identyfikator 'PES M2 '
           SzerEtp  dw  ?            ;szerokosc planszy
            WysEtp  dw  ?            ;wysokosc planszy
               Key  db  80h dup (?)  ;bufor na stan przyciskow
             Snieg  dw  20 dup (?)   ;offsety 20 platkow sniegu (lub kropli)

; =============================================================
; ===================== M I D I ===============================
; =============================================================

MidHead:     MidID    db 4 dup (?)       ;ID  'MThd'
             LEN      dd ?               ;Dlugosc naglowka (powinnow byc 6)
             Format   dw ?               ;Format zapisu
             Tracks   dw ?               ;Liczba sciezek
             Dzielnik dw ?               ;Dlugosc jednego taktu midi

             MidiSEG   dw ?              ;segment danych MIDI

TrackHead:   TrackID  db 4 dup (?)       ;ID 'MTrk'
             TrackLEN dd ?               ;dlugosc sciezki

             TrackOfs dw 64 dup (?)      ;adresy sciezek
             TrackTime dw 64 dup (?)     ;delta time
             TrackOld  db 64 dup (?)     ;ostatnia komenda
             TrackEnd  db 64 dup (?)     ;Flaga zakonczenia sciezki

CODE ENDS
END Start