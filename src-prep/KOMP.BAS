' ==============================================================================
' =                                                                            =
' = Author: Sylwester Wysocki <sw143@wp.pl>                                    =
' = Created on: 2002                                                           =
' =                                                                            =
' = This is free and unencumbered software released into the public domain.    =
' =                                                                            =
' = Anyone is free to copy, modify, publish, use, compile, sell, or            =
' = distribute this software, either in source code form or as a compiled      =
' = binary, for any purpose, commercial or non-commercial, and by any          =
' = means.                                                                     =
' =                                                                            =
' = In jurisdictions that recognize copyright laws, the author or authors      =
' = of this software dedicate any and all copyright interest in the            =
' = software to the public domain. We make this dedication for the benefit     =
' = of the public at large and to the detriment of our heirs and               =
' = successors. We intend this dedication to be an overt act of                =
' = relinquishment in perpetuity of all present and future rights to this      =
' = software under copyright law.                                              =
' =                                                                            =
' = THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,            =
' = EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF         =
' = MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     =
' = IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR          =
' = OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,      =
' = ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR      =
' = OTHER DEALINGS IN THE SOFTWARE.                                            =
' =                                                                            =
' = For more information, please refer to <https://unlicense.org>              =
' =                                                                            =
' ==============================================================================

     Rekurencja = 0: label = 0
     IleSpc = 15

     FOR i% = 1 TO LEN(COMMAND$)
         x$ = MID$(COMMAND$, i%, 1)
         IF x$ = "." THEN GOTO Wyskok
         nazwa2$ = nazwa2$ + x$
     NEXT i%
Wyskok:

     nazwa1$ = COMMAND$
     nazwa3$ = nazwa2$
     nazwa2$ = nazwa2$ + ".TMS"
'     PRINT COMMAND$; " => "; nazwa2$; "..."

Start:
     Rekurencja = 0:  CzyWar = 0
     OPEN nazwa1$ FOR INPUT AS #1
     OPEN nazwa2$ FOR OUTPUT AS #2


     DO


         bajt$ = INPUT$(1, #1)

         SELECT CASE bajt$
           CASE ";": LINE INPUT #1, a$: PRINT #2, ";" + a$
           CASE "/": PRINT #2, ""
           CASE "I":
                    bajt2$ = INPUT$(1, #1)
                    IF bajt2$ = "F" THEN
                         '==============================================
                         '================ IF ===========================
                         '==============================================
JeszczeRaz:                      CzyWar = 1
                                 Rekurencja = 1
                                 DO
                                    tms$ = INPUT$(1, 1#)
                                 LOOP UNTIL tms$ = "("

                                 war$ = "": bajt$ = "": znak$ = ""
                                 DO
                                    war$ = war$ + bajt$
                                    bajt$ = INPUT$(1, #1)

                                    IF bajt$ = "=" OR bajt$ = "<" OR bajt$ = ">" OR bajt$ = "!" THEN znak$ = bajt$
                                 LOOP UNTIL znak$ <> ""

                                 PRINT #2, "cmp    "; war$; ", ";

                                 war$ = "": bajt$ = ""
                                 DO
                                    war$ = war$ + bajt$
                                    bajt$ = INPUT$(1, #1)
                                 LOOP UNTIL bajt$ = ")"

                                 PRINT #2, war$
                                 PRINT #2, SPC(IleSpc);
                                 IF znak$ = "=" THEN PRINT #2, "jne    ";
                                 IF znak$ = "<" THEN PRINT #2, "jnb    ";
                                 IF znak$ = ">" THEN PRINT #2, "jna    ";
                                 IF znak$ = "!" THEN PRINT #2, "je    ";

                                 tms$ = INPUT$(1, #1)  'omin spacje
                                 tms$ = INPUT$(1, #1)  'omin spacje i {

                                 a$ = MID$(STR$(label), 2, 5)
                                 PRINT #2, nazwa3$; a$
                                 IF tms$ = "&" THEN
                                                 PRINT #2, SPC(IleSpc);
                                                 GOTO JeszczeRaz
                                               END IF

                                 label = label + 1
                                 c$ = MID$(STR$(label), 2, 5)

                                 bajt$ = "": war$ = ""
                                 DO
                                    bajt$ = INPUT$(1, #1)
                                    SELECT CASE bajt$
                                       CASE "{": Rekurencja = Rekurencja + 1: PRINT #2, bajt$;
                                       CASE "}": Rekurencja = Rekurencja - 1: IF Rekurencja > 0 THEN PRINT #2, bajt$;
                                       'CASE "/": PRINT #2, ""
                                       CASE "^": IF Rekurencja = 1 THEN PRINT #2, nazwa3$; c$ ELSE PRINT #2, "^";   'jmp  poza warunek
                                       CASE ELSE: PRINT #2, bajt$;
                                    END SELECT
                                 LOOP UNTIL Rekurencja = 0

                                 PRINT #2, ""

                                 label = label + 1
                                 b$ = MID$(STR$(label), 2, 5)

                           '-==================================
                           '============= ELSE =================
                           '====================================
                                 bajt$ = INPUT$(1, #1)
                                 IF bajt$ = "\" THEN
                                         label = label + 1
                                         PRINT #2, SPC(IleSpc); "jmp    "; nazwa3$; b$
                                         PRINT #2, nazwa3$; a$; ":"

                                         DO
                                             bajt$ = INPUT$(1, #1)
                                             SELECT CASE bajt$
                                       '         CASE "/": PRINT 2#, ""
                                                CASE "^": PRINT #2, nazwa3$; c$    'jmp  poza warunek
                                                CASE "\":
                                                CASE ELSE: PRINT #2, bajt$;
                                             END SELECT

                                         LOOP UNTIL bajt$ = "\"
                                         PRINT #2, ""
                                         PRINT #2, nazwa3$; b$; ":"
                                  ELSE
                                         PRINT #2, nazwa3$; a$; ":"    'label jesli nie
                                         PRINT #2, bajt$;
                                 END IF

                                 PRINT #2, nazwa3$; c$; ":"


                    ELSE PRINT #2, "I"; bajt2$;
                    END IF
               CASE "#":
                         tms$ = INPUT$(8, #1)     'omin napis include
                         DO
                             tms$ = INPUT$(1, 1#)
                         LOOP UNTIL tms$ <> " "


                         LINE INPUT #1, tms2$
                         tms$ = tms$ + tms2$
                         nazwa$ = ""
                         FOR i% = 1 TO LEN(tms$)
                            x$ = MID$(tms$, i%, 1)
                            IF x$ = " " THEN GOTO Wyskok2
                            IF ASC(x$) = 13 THEN GOTO Wyskok2
                            IF x$ = "." THEN GOTO Wyskok2
                            nazwa$ = nazwa$ + x$
                         NEXT i%
Wyskok2:
                    '     PRINT nazwa$ + ".INC => "; nazwa$ + ".tms..."
                         SHELL "komp.exe " + nazwa$ + ".INC"
                         PRINT #2, "include "; nazwa$ + ".tms"


       '===================== R E P E A T ====================
            CASE "R":  a$ = INPUT$(5, #1)
                       IF a$ = "EPEAT" THEN
                              CzyWar = 1
                              DO: tms$ = INPUT$(1, 1#): LOOP UNTIL tms$ = "("

                         '============ L I C Z N I K ==============
                              licznik$ = ""
                              DO
                                  tms$ = INPUT$(1, 1#):
                                  IF tms$ <> "," THEN licznik$ = licznik$ + tms$
                              LOOP UNTIL tms$ = ","
                        '============ S T A L A ======================
                              stala$ = ""
                              DO
                                  tms$ = INPUT$(1, 1#):
                                  IF tms$ <> ")" THEN stala$ = stala$ + tms$
                              LOOP UNTIL tms$ = ")"

                              DO: tms$ = INPUT$(1, 1#): LOOP UNTIL tms$ = "{"

                              label = label + 1
                              label$ = "R_" + nazwa3$ + MID$(STR$(label), 2, 5)
                              PRINT #2, "mov "; licznik$; ","; stala$
                              PRINT #2, label$; ":      ";


                        '============ C I A L O    P E T L I ============
                              Rekurencja = 1
                              DO
                                 tms$ = INPUT$(1, 1#)
                                 SELECT CASE tms$
                                    CASE "{": Rekurencja = Rekurencja + 1: PRINT #2, "{";
                                    CASE "/": PRINT #2, ""
                                    CASE "}": Rekurencja = Rekurencja - 1: IF Rekurencja > 0 THEN PRINT #2, tms$;
                                    CASE ELSE: PRINT #2, tms$;
                                 END SELECT
                              LOOP UNTIL Rekurencja = 0

                              PRINT #2, ""
                              PRINT #2, "dec   "; licznik$
                              PRINT #2, "jne   "; label$


                       ELSE PRINT #2, "R"; a$;
                       END IF

           '======================= TST =========================
            CASE "T":  a$ = INPUT$(2, #1)
                       IF a$ = "ST" THEN
                              CzyWar = 1
                              DO: tms$ = INPUT$(1, 1#): LOOP UNTIL tms$ = "("


                        '============== ZMIENNA ===============
                              zmienna$ = ""
                              DO
                                  tms$ = INPUT$(1, 1#):
                                  IF tms$ <> "," THEN zmienna$ = zmienna$ + tms$
                              LOOP UNTIL tms$ = ","
                        '============ S T A L A ======================
                              stala$ = ""
                              DO
                                  tms$ = INPUT$(1, 1#):
                                  IF tms$ <> ")" THEN stala$ = stala$ + tms$
                              LOOP UNTIL tms$ = ")"

                              DO: tms$ = INPUT$(1, 1#): LOOP UNTIL tms$ = "{"

                              PRINT #2, "     test      "; zmienna$; " ,"; stala$
                              labal = label + 1
                              label$ = "Tst_" + nazwa3$ + MID$(STR$(label), 2, 5)
                              PRINT #2, "     je        "; label$

                        '============ C I A L O    P E T L I ============
                              Rekurencja = 1
                              DO
                                 tms$ = INPUT$(1, 1#)
                                 SELECT CASE tms$
                                    CASE "{": Rekurencja = Rekurencja + 1: PRINT #2, "{";
                                    CASE "/": PRINT #2, ""
                                    CASE "}": Rekurencja = Rekurencja - 1: IF Rekurencja > 0 THEN PRINT #2, tms$;
                                    CASE ELSE: PRINT #2, tms$;
                                 END SELECT
                              LOOP UNTIL Rekurencja = 0

                              PRINT #2, ""
                              PRINT #2, label$; ":"

                       ELSE PRINT #2, "T"; a$;
                       END IF

            CASE ELSE:   PRINT #2, bajt$;
        END SELECT


     LOOP UNTIL EOF(1#)

     PRINT #2, ""
     PRINT #2, ";File created by Szefo ASM compiler"
     CLOSE #1, #2


     SHELL "del *.TM2"
     IF CzyWar = 1 THEN
                      SHELL "REN " + nazwa2$ + " " + nazwa3$ + ".tm2"
                      nazwa1$ = nazwa3$ + ".tm2"
                      IleSpc = IleSpc + 4
                      GOTO Start
     END IF
END
