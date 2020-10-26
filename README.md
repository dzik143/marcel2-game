# [ARCHIVE/2003] Marcel II: 2D platform game
- 2D platform **GAME ENGINE** written in **PURE ASSEMBLY**,
- **ARCHIVAL** code created mainly between **December 2002 and January 2003**,
- [gameplay video](https://www.youtube.com/watch?v=W15LYIK44t8) is available on youtube,
- legacy **16-bit code** for DOS,
- **7528 bytes** of code,
- a lot of **NAIVE CODE** - don't repeat it,
- example showing that just write something in assembly **DOES NOT MAKE IT FAST**,
- released as **PUBLIC DOMAIN** - use for any purpose.

# Main features:
- [MPU-401](https://en.wikipedia.org/wiki/MPU-401) driver for music,
- plays standard [MIDI files](http://www.music.mcgill.ca/~ich/classes/mumt306/StandardMIDIfileformat.html) using **CLOCK INTERRUPT** (IRQ8) for time synchronization,
- **SB16** driver in [DMA mode](https://www.techopedia.com/definition/2767/direct-memory-access-dma) for sound,
- track **KEYBOARD STATE** in buffer updated when [keyboard interrupt](https://en.wikipedia.org/wiki/Interrupt) (IRQ9) arrived,
- **LEVEL EDITOR** written in C,
- macroassembler-like **PREPROCESSOR** written in qbasic.
