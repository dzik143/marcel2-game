/******************************************************************************/
/*                                                                            */
/*  Author: Sylwester Wysocki <sw143@wp.pl>                                   */
/*  Created on: December 2002 to January 2003                                 */
/*                                                                            */
/*  This is free and unencumbered software released into the public domain.   */
/*                                                                            */
/*  Anyone is free to copy, modify, publish, use, compile, sell, or           */
/*  distribute this software, either in source code form or as a compiled     */
/*  binary, for any purpose, commercial or non-commercial, and by any         */
/*  means.                                                                    */
/*                                                                            */
/*  In jurisdictions that recognize copyright laws, the author or authors     */
/*  of this software dedicate any and all copyright interest in the           */
/*  software to the public domain. We make this dedication for the benefit    */
/*  of the public at large and to the detriment of our heirs and              */
/*  successors. We intend this dedication to be an overt act of               */
/*  relinquishment in perpetuity of all present and future rights to this     */
/*  software under copyright law.                                             */
/*                                                                            */
/*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,           */
/*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF        */
/*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.    */
/*  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR         */
/*  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,     */
/*  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR     */
/*  OTHER DEALINGS IN THE SOFTWARE.                                           */
/*                                                                            */
/*  For more information, please refer to <https://unlicense.org>             */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>;

FILE *f;              //Do op. na plikach
unsigned int i;       //Licznik

char Tlo[256*40];     //Bufor na tlo
char Graf[37100];     //Grafika obiektow
char Paleta[768];

char Key;
unsigned int EKey;
int KursorX, KursorY, KursorR;
char KursorF;                   //Tlo czy obiekt
char nazwa[13];                  //Nazwa otwartego pliku


struct OpisObiektu {
  int    x,y;            //0   2+2
  char   kier;           //4   1
  int    szer, wys;      //5   2+2
  char   rys;            //9   1
  char   czyn;           //10  1
  char   licz;           //11  1
  char   typ;            //12  1
  int    ener;           //13  2
  char   sila;           //15  1
  int    punkty;         //16  2
  char   active;         //18  1
  char   CUS;            //19  1
  char   RysWyb;         //20  1
  char   Trup;           //21  1    1 = niesmiertelny
  char   OneColor;       //22  1
  char   CzyWybucha;     //23  1
  char   Grunt;          //24  1
  char   Air;            //25  1
  int    scrOffset;      //26  2  Offset na ekranie
  char   drzwi;          //28  1 ;1 jesli kolizja z drzwiami (itp.)
  char   maska;          //29  1 ;Maska do zamkow, przelacznikow itp.
                         // =30
} Obiekty[150];

int IleObkt;

int SzerEtp = 20;
int WysEtp = 14;
char Etap[5000];

int Grunt = 30;
int x,y;              //Takie sobie liczniki


void RysKwad (int x, int y, int nr);
void RysObkt (unsigned int x, unsigned int y, unsigned int nr, unsigned int kier);
void SetPal (void);
void LoadData (void);
void UsunObiekt (void);
void GetKey(void);
void Locate (char x, char y);
void Ramka (int x, int y, int szer, char wys, char kolor);
void Zapisz (void);
void Wczytaj (void);
void NowyObiekt (void);
void UstawWymiary (int nr);

void ZmniejszSzer (void);
void ZwiekszSzer (void);

int  KameraX, KameraY;     //Polozenie kamery
int  KursorX, KursorY;
unsigned int  PodKurs;     //nr obiektu pod kursorem


main ()
{
  LoadData();

MainLoop:

  /////////////////  O B R A Z     P L A N S Z Y  ////////////////

  for (x=0; x<20; x++)
     for (y=0; y<=11; y++)
       {
         i = Etap[x+KameraX + (y+KameraY)*SzerEtp];
         if (i!=-1) RysKwad (x*16, y*16, i); else Ramka (x*16, y*16, 16, 16,0);

       };

  /////////////////////// K U R S O R /////////////////////

  Locate (0,0);

  printf ("[%i x %i]", KursorX, KursorY);
  printf ("%i,%i", SzerEtp, WysEtp);

  x = (KursorX-KameraX) * 16;
  y = (KursorY-KameraY) * 16;

  if (KursorF)
  {
    RysObkt (x, y, KursorR, 1);
  }
  else
  {
    Ramka(x, y, 18,17,12);
    RysKwad(x+1, y+1, KursorR);
  };

  asm
  {
    mov   KursorF, 0
    mov   ah, 2
    int   16h            //teraz AL = stan klaw.
    test  al, 40h        //6 bit = stan CapsLock'a
    je    Zgaszony
    mov   KursorF, 1

Zgaszony:
  };

  /////////////////// O B I E K T Y /////////////////////////
  PodKurs = 0xffff;

  for (i=0; i<IleObkt; i++)
  {
    x = (Obiekty[i].x/16 - KameraX)*16;
    y = (Obiekty[i].y/16 - KameraY)*16 + Obiekty[i].y%16;

    if ((x<320) & (x>=0) & (y<192) & (y>=0))
    {
      RysObkt (x, y, Obiekty[i].rys, Obiekty[i].kier);

      if (Obiekty[i].typ==12)
      {
        RysObkt (x, y-16*3, Obiekty[i].rys, Obiekty[i].kier);
      }
    };

    if ((Obiekty[i].x/16==KursorX) & (Obiekty[i].y/16==KursorY))
    {
      PodKurs = i;
    }

    UstawWymiary(i);    //odswiez na wszelki wypadek...
  };

  ////////////////////// I N F O /////////////////////

  if (PodKurs!=0xffff)
  {
    i = PodKurs;
    x = Obiekty[i].x; y = Obiekty[i].y;

    Ramka (0,130,130,69,85);
    Locate (1,17);

    printf ("%i:[%i,%i]\n", i,x, y);

    if (Obiekty[i].typ==12)
    {
      printf ("Trasa:%i\n", Obiekty[i].ener/16);
    }
    else
    {
      printf (" Ener:%i Sila:%i\n", Obiekty[i].ener, Obiekty[i].sila);
    }

    printf (" Typ:%i(", Obiekty[i].typ);

    switch (Obiekty[i].typ)
    {
      case 0: {printf ("Nul)\n"); break;}
      case 1: {printf ("Pajak)\n"); break;}
      case 2: {printf ("Pocisk)\n"); break;}
      case 3: {printf ("Wybuch)\n"); break;}
      case 4: {printf ("RoboCop)\n"); break;}
      case 5: {printf ("Apteczka)\n"); break;}
      case 6: {printf ("Wyjscie)\n"); break;}
      case 7: {printf ("Drzwi)\n"); break;}
      case 8: {printf ("Zamek)\n"); break;}
      case 9: {printf ("Animacja)\n"); break;}
      case 10: {printf ("Acme)\n"); break;}
      case 11: {printf ("Mina)\n"); break;}
      case 12: {printf ("Winda)\n"); break;}
      case 13: {printf ("Zoombi)\n"); break;}
      case 14: {printf ("Wyspa)\n"); break;}
    }

    printf (" Punkty:%i\n", Obiekty[i].punkty);
    printf (" Czynnosc:%i\n ", Obiekty[i].czyn);

    printf ("Active:%i ", Obiekty[i].active);

    if (Obiekty[i].Trup) printf ("T");
    if (Obiekty[i].CzyWybucha) printf ("X");
    if (Obiekty[i].Air) printf ("Y");

    printf ("\n Maska:%x", Obiekty[i].maska);

    RysObkt (300, 180, Obiekty[i].RysWyb, 1);
  };

  ////////////////// S T E R O W A N I E ///////////////////

  GetKey();

  if (Key==0x1b) {                                    //ESC
    Ramka(75,75,160,20,20);
    Locate(10,10);
    printf("Czy na pewno (T/N)?");
    GetKey();
    if (Key=='T') return (0); else goto MainLoop;
  }

  if (Key=='W') if (KameraY>0) {KameraY--; KursorY--;};
  if (Key=='S') if (KameraY+12<WysEtp) {KameraY++; KursorY++;};
  if (Key=='D') if (KameraX+20<SzerEtp) {KameraX++; KursorX++;};
  if (Key=='A') if (KameraX>0) {KameraX--; KursorX--;};

  if (EKey==0x4b00) if (KursorX>KameraX) KursorX--;         //Lewo
  if (EKey==0x4d00) if (KursorX<KameraX+19) KursorX++;      //Prawo
  if (EKey==0x5000) if (KursorY<KameraY+11) KursorY++;      //Dol
  if (EKey==0x4800) if (KursorY>KameraY) KursorY--;         //Gora
  if (Key==0xd) Etap[KursorX + KursorY*SzerEtp] = KursorR;  //Ent

  if (Key==0x9) if (KursorR>0) KursorR--;                   //Tab
  if (Key=='Q') KursorR++;                                  //Q
  if (Key==8) {Etap[KursorX+KursorY*SzerEtp]=0xff; KursorX++;} //BS

  if (EKey==0x5500) {nazwa[0]=0; Zapisz ();};               //Shf+F2
  if (EKey==0x3c00) Zapisz ();                              //F2
  if (EKey==0x3d00) Wczytaj ();                             //F3

  if (EKey==0x3f00) ZmniejszSzer ();                        //F5
  if (EKey==0x4000) ZwiekszSzer ();                         //F6
  if (EKey==0x4100) WysEtp--;
  if (EKey==0x4200) {WysEtp++; KursorY++; KameraY++;};      //F8

  if (Key=='1') KursorR=0;
  if (Key=='2') KursorR=Grunt;

  if (PodKurs!=0xffff) goto JestObiekt;
  if (EKey==0x5200) NowyObiekt ();      //Ins

///// = 1 = PAJAK = //// Typ=1 Ener=3 Sila=1 Czyn=1 Punkty=10 Wyb=11 Rys=3
  if (Key=='!') {NowyObiekt (); Obiekty[i].ener=3; Obiekty[i].typ=1; Obiekty[i].sila=1; Obiekty[i].rys=7; Obiekty[i].czyn=1; Obiekty[i].RysWyb=15; Obiekty[i].punkty=10; Obiekty[i].CzyWybucha=1;};
///// = 2 = ROBOCOP //// Typ=4 Ener=5 Sila=1 Czyn=1 Punkty=50 Wyb=11 Rys=15
  if (Key=='@') {NowyObiekt (); Obiekty[i].ener=5; Obiekty[i].typ=4; Obiekty[i].sila=1; Obiekty[i].rys=19; Obiekty[i].czyn=1; Obiekty[i].RysWyb=15; Obiekty[i].punkty=50; Obiekty[i].CzyWybucha=1;};
///// = 3 = ZOOMBI //// Typ=4 Ener=5 Sila=1 Czyn=1 Punkty=50 Wyb=11 Rys=15
  if (Key=='#') {NowyObiekt (); Obiekty[i].ener=6; Obiekty[i].typ=13; Obiekty[i].sila=2; Obiekty[i].rys=43; Obiekty[i].czyn=1; Obiekty[i].RysWyb=46; Obiekty[i].punkty=50; Obiekty[i].CzyWybucha=1;};

JestObiekt:

  if (PodKurs==0xffff) goto MainLoop;    //wszytko ponizej dotyczy
                                         //obkt. pod kursorem
  i = PodKurs;
  if (EKey==0x5300) UsunObiekt ();                        //Del

  if (EKey==0x4900) Obiekty[i].active++;    //PgUp
  if (EKey==0x5100) Obiekty[i].active--;    //PgDn
  if (EKey==0x4700) Obiekty[i].maska++;    //Home
  if (EKey==0x4f00) Obiekty[i].maska--;    //End

  if (Key=='/') Obiekty[i].y--;
  if (Key=='?') Obiekty[i].y++;
  if (Key=='Y') if (Obiekty[i].Air==0) Obiekty[i].Air=1; else Obiekty[i].Air=0;
  if (Key=='.') Obiekty[i].typ++;
  if (Key==',') Obiekty[i].typ--;
  if (Key=='*') Obiekty[i].kier = -Obiekty[i].kier;
  if (Key=='=') Obiekty[i].ener++;
  if (Key=='-') Obiekty[i].ener--;
  if (Key==']') Obiekty[i].sila++;
  if (Key=='[') Obiekty[i].sila--;
  if (Key=='0') Obiekty[i].rys++;
  if (Key=='9') Obiekty[i].rys--;

  if (Key==0x27) Obiekty[i].czyn++;
  if (Key==';') Obiekty[i].czyn--;
  if (Key=='M') Obiekty[i].RysWyb++;
  if (Key=='N') Obiekty[i].RysWyb--;
  if (Key=='T') if (Obiekty[i].Trup) Obiekty[i].Trup=0; else Obiekty[i].Trup=1;
  if (Key=='Z') Obiekty[i].RysWyb--;

  if (Key=='P') Obiekty[i].punkty += 10;
  if (Key=='O') Obiekty[i].punkty -= 10;

  if (Key=='C') {Obiekty[i].punkty = 0; Obiekty[i].sila=0; Obiekty[i].ener=0; Obiekty[i].czyn=0; Obiekty[i].typ=0;};
  if (Key=='Z') IleObkt =0;
  if (Key=='I') {Obiekty[i].y -= 16; KursorY--;}
  if (Key=='K') {Obiekty[i].y += 16; KursorY++;}
  if (Key=='J') {Obiekty[i].x -= 16; KursorX--;}
  if (Key=='L') {Obiekty[i].x += 16; KursorX++;}

  if (Key=='X') if (Obiekty[i].CzyWybucha)
                  Obiekty[i].CzyWybucha = 0;
                    else Obiekty[i].CzyWybucha = 1;

  goto MainLoop;
};

void RysKwad (int x, int y, int nr)
{
  asm
  {
    push   es
    push   si
    push   di

    mov    ax, 0a000h
    mov    es, ax          //ES = 0xA000
    mov    ax, y
    xchg   ah, al          //AX = y*256
    mov    di, ax          //DI = y*256
    shr    ax, 2           //AX = y*64
    add    di, ax          //DI = y*(256*64) = y*320
    add    di, x           //DI = y*320 + x

    mov    ax, nr
    xchg   ah, al          //*256 = *16*16
    mov    si, ax
    add    si, offset Tlo  //DS:[SI] = Adres zadanego rysunku

    mov    cx, 1008h       //10h  x  8*2h

Rysuj1:
    mov    ax, ds:[si]
    add    si, 2
    mov    es:[di], ax
    add    di, 2

    dec    cl
    jne    Rysuj1

    mov    cl, 8
    add    di, 130h
    dec    ch
    jne    Rysuj1

    pop    di
    pop    si
    pop    es
  }
};

void RysObkt (unsigned int x, unsigned int y, unsigned int nr, unsigned int kier)
{
  unsigned int AddLn;    //do odwracania

  asm
  {
    push  es
    push  si
    push  di

    mov   ax, 0a000h
    mov   es, ax
    mov   ax, y
    xchg  ah, al
    mov   di, ax
    shr   ax, 2
    add   di, ax
    add   di, x

    mov   bx, nr
    shl   bx, 1                        //BX=adres rys. w tabl. offset.
    add   bx, offset Graf

    mov   si, word ptr ds:[bx]         //wczytaj adres obrazka
    add   si, offset Graf              //wzg. [Graf]

    mov   cx, word ptr ds:[si]         //CX = Szer
    mov   dx, cx                       //DX = Szer
    mov   bx, word ptr ds:[si+2]       //BL = Wys
    add   si, 4                        //omin naglowek
    mov   AddLn, 320                   //AddLn = 320

    cmp   kier, -1
    jne   Prawo
    add   AddLn, dx                    //gdy lewo to AddLn = 320+Szer
    add   di, dx                       //skoryguj poczatek
    jmp   RysLn

Prawo:
    sub   AddLn, dx                    //gdy prawo to AddLn = 320-Szer

RysLn:
    mov    al, ds:[si]
    inc    si
    cmp    al, 0ffh
    je     NieRys

    mov    byte ptr es:[di], al

NieRys:
    add    di, kier
    loop   RysLn

    add    di, AddLn                   //AddLn
    mov    cx, dx                      //Kolejna linia

    dec    bl                          //BL = Liczik Wys.
    jne    RysLn

    pop    di
    pop    si
    pop    es
  }
};

void SetPal(void)
{
  asm
  {
    lea    si, Paleta
    mov    dx, 3c8h         //DX = 0x3c8
    xor    al, al

SetRGB:
    out    dx, al           //nr. koloru
    inc    dx               //DX = 0x3c9

    outsb                   //R
    outsb                   //G
    outsb                   //B

    dec    dx               //DX = 0x3c8
    inc    al
    cmp    al, 255
    jne    SetRGB
  }
};

void LoadData(void)
{
  f = fopen ("tlo.sgb","rb");
  for (i=0; i<=256*50; i++) Tlo[i] = fgetc (f);
  fclose (f);

  f = fopen ("obiekty.sgb","rb");
  for (i=0; i<37000; i++) Graf[i] = fgetc (f);
  fclose (f);

  f = fopen ("paleta.pal","rb");
  for (i=0; i<768; i++) Paleta[i] = fgetc (f);
  fclose (f);

  asm {mov  ax, 13h; int 10h;};
  SetPal ();

  for (i=0; i<2000; i++) Etap[i] = 0xff;
};

void GetKey(void)
{
  asm
  {
    xor  ah, ah
    int  16h

    mov  EKey, ax
    cmp  al, 'a'
    jb   Big
    cmp  al, 'z'
    ja   Big

    sub  al, 20h
Big:
    mov  Key, al
  };
};

void Locate (char x, char y)
{
  asm
  {
    mov    ah, 2
    xor    bh, bh          // Strona
    mov    dl, x
    mov    dh, y
    int    10h
  };
};

void Ramka (int x, int y, int szer, char wys, char kolor)
{
  asm
  {
    push    es
    push    di

    mov     ax, y
    xchg    ah, al
    mov     di, ax
    shr     ax, 2
    add     di, ax
    add     di, x

    mov     ax, 0a000h
    mov     es, ax

    mov     al, kolor
    mov     cx, szer           //CX = szer
    rep     stosb
    sub     di, szer
    add     di, 320

    mov     bx, szer
    mov     dx, bx            //BX=DX = szer
    mov     ah, wys           //AH = wys;
    dec     ah

RysPion:
    stosb
    push    ax

    xor     al, al
    mov     cx, szer
    sub     cx, 2
    rep     stosb

    pop     ax
    stosb

    add     di, 320
    sub     di, szer
    dec     ah
    jne     RysPion

    mov     cx, szer
    rep     stosb

    pop     di
    pop     es
  };
};

void Zapisz ()
{
  if (!nazwa[0])
  {
    Ramka (0,0,120,30,4); Locate (1,1);
    printf ("Nazwa pliku:");
    Locate (3,2); scanf ("%s", nazwa);
  };

  f = fopen (nazwa,"wb+");


  fprintf (f, "PES M2c");                   //ID
  fwrite (&SzerEtp, 1, 2, f);               //Wymiary etapu
  fwrite (&WysEtp, 1, 2, f);
  fwrite (&Etap, 1, SzerEtp*WysEtp, f);     //Obraz etapu

  fwrite (&IleObkt, 1, 2, f);                     //Ile Obktow
  fwrite (&Obiekty, sizeof(Obiekty[0]), IleObkt, f);  //Dane obiektow

  fclose (f);
};

void Wczytaj ()
{
  Ramka (0,0,120,30,2); Locate (1,1);
  printf ("Nazwa pliku:");
  Locate (3,2); scanf ("%s", nazwa);

  f = fopen (nazwa,"rb+");

  fseek (f, 7, 0);                        //Omin ID (PES M2c)
  fread (&SzerEtp, 1, 2, f);
  fread (&WysEtp, 1, 2, f);               //Wymiary planszy
  fread (&Etap, 1, SzerEtp*WysEtp, f);    //Obraz etapu

  fread (&IleObkt, 1, 2, f);                          //Ile Obktow
  fread (&Obiekty, sizeof(Obiekty[0]), IleObkt, f);   //Dane obiektow
  fclose (f);

  KameraX = Obiekty[0].x/16; KameraY=Obiekty[0].y/16;
  KursorX = KameraX; KursorY = KameraY;
};

void UstawWymiary (int nr)
{
  int Szer, Wys, Rys;
  Rys = Obiekty[nr].rys;

  asm
  {
    mov     bx, Rys
    shl     bx, 1                  //BX=adres rys. w tabl. offset.
    add     bx, offset Graf

    mov     si, word ptr ds:[bx]     //wczytaj adres obrazka
    add     si, offset Graf          //wzg. [Graf]

    lodsw
    mov     Szer, ax
    lodsw
    mov     Wys, ax
  };

  Obiekty[nr].szer = Szer;
  Obiekty[nr].wys = Wys;
};

void NowyObiekt (void)
{
  i = IleObkt;

   Obiekty[i].x = KursorX*16;
   Obiekty[i].y = KursorY*16;
   Obiekty[i].rys = KursorR;
   Obiekty[i].kier = 1;
   UstawWymiary (i);
   IleObkt++;
};

void UsunObiekt (void)
{
  for (i=PodKurs; i<IleObkt; i++)
    Obiekty[i] = Obiekty[i+1];

   IleObkt --;
};

void ZwiekszSzer (void)
{
 for (y=0; y<WysEtp; y++)
   for (i=SzerEtp*WysEtp; i>SzerEtp*(y+1); i--)
     Etap[i]=Etap[i-1];

  SzerEtp++;
};

void ZmniejszSzer (void)
{
  for (y=0; y<WysEtp; y++)
    for (i=(SzerEtp-1)*(y+1); i<SzerEtp*WysEtp; i++)
      Etap[i-1]=Etap[i];


  SzerEtp--;

  if (KameraX>0) {KameraX--; KursorX--;};
};
