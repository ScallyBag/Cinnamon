Cinnamon
==========
### UCI Chess Engine

Cinnamon is a chess program for Windows, Linux, Mac OS, Android and Raspberry Pi, is a console-based chess engine for use with [xboard][4], [Arena][5], [Tarrasch][6], [Droidfish][7] or any UCI-compatible GUI. Cinnamon is also a javascript library to play with [chessboardjs][8] or any js GUI.

- [Play on line](https://gekomad.github.io/Cinnamon/)

Version
----------
2.4.0

News
----------
- Reverse Futility Pruning
- Stalemate improvement
- Fix bug in castle
- Speeded up Perft


Useful links
----------
- [Bitboard Calculator](https://gekomad.github.io/Cinnamon/BitboardCalculator/)
- [Auriga Distributed Perft](https://github.com/gekomad/Auriga)
- [Web Eval debugger](https://github.com/gekomad/chess-engine-eval-debugger)
- [Android App](https://play.google.com/store/apps/details?id=com.github.gekomad.cinnamonengine)
- [Cinnamon on DGT Pi](https://www.digitalgametechnology.com/index.php/products/revelation-ii/533-dgt-pi-chess-computer-for-dgt-e-boards)
- [Clion IDE](https://www.jetbrains.com/?from=CINNAMON_CHESS_ENGINE)


Features
----------

- Available for both Unix, Windows, Mac, Android, ARM and Javascript
- UCI protocol
- GPL 3 License
- Chess 960
- C++11 source
- PVS
- Rotated bitboards
- Null moves
- Futility pruning
- Reverse Futility Pruning
- Delta pruning
- Razoring
- Interruptible multithread Perft test [standard][9] and [chess960][10]
- 32/64 bit architectures
- Iterative deeping
- History heuristics
- Killer heuristics
- Lazy evaluation
- MVV/LVA
- Two-tier Transposition Table
- Aspiration Windows
- Late Move Reduction
- Reverse Futility Pruning
- Ponder
- Open Book (Polyglot)
- Gaviota Tablebases
- Syzygy Tablebases
- [Elo ratings][3]

Binaries
----------

Binaries are available [here][1].
All files are compiled statically, no further libraries are necessary.

Command line tools
----------
#### Perft
`cinnamon.exe -perft [-d depth] [-c nCpu] [-h hash size (mb) [-F dump file]] [-Chess960] [-f "fen position"] `

Setting `-F` and `-h` you can stop (Ctrl-c) and restart the perft process.

| **perft 7**  | qperft  | stockfish 12 | cinnamon |  vajolet 2.8.0  |  cheng4 4.39   |
| ------------ | ------------ |  ------------ | ------------ | ------------ | ----- | 
| i7-9750H 2.6GHz  | 13 sec.  | 14 sec. | 17 sec. | 25 sec. |  32 sec. | 

| **perft 7**  |   cinnamon 8 core 512M hash    |  qperft 512M hash | cinnamon 512M hash |
| ------------ | ------------ | ------------------ | ------ |
| i7-9750H 2.6GHz | 1.76 sec. |  1.93 sec. | 5.23 sec. |


#### Gaviota DTM (distance to mate)

`cinnamon.exe -dtm-gtb -f "fen position" -p path`

#### Gaviota WDL (win/draw/loss)

`cinnamon.exe -wdl-gtb -f "fen position" -p path`

#### SYZYFY DTZ (distance to zero)

`cinnamon.exe -dtz-syzygy -f "fen position" -p path`

#### SYZYFY WDL (win/draw/loss)

`cinnamon.exe -wdl-syzygy -f "fen position" -p path`

#### EPD generator
`cinnamon.exe -puzzle_epd -t K?K?`
 example: `cinnamon.exe -puzzle_epd -t KRKP`

Compiling
---------

Cinnamon requires C++11 or greater, use unique Makefile to compile for many architectures:

    $ make


    Makefile for cross-compile Linux/Windows/OSX/ARM/Javascript

    make cinnamon64-modern-INTEL     > 64-bit optimized for modern Intel cpu
    make cinnamon64-BMI2             > 64-bit optimized for Haswell Intel cpu
    make cinnamon64-modern-AMD       > 64-bit optimized for modern Amd cpu
    make cinnamon64-modern           > 64-bit with popcnt bsf sse3 support
    make cinnamon64-generic          > Unspecified 64-bit
    make cinnamon64-ARM              > Optimized for ARM cpu

    make cinnamon32-modern           > 32-bit with sse support
    make cinnamon32-generic          > Unspecified 32-bit
    make cinnamon32-ARM              > Optimized for ARM cpu

    make cinnamon-js                 > Javascript build

    add:
     COMP=compiler                   > Use another compiler
     FULL_TEST=yes                   > Unit test (googletest)


    g++ is the default compiler, add COMP=yourcompiler to use another compiler
     example: make cinnamon64-modern-INTEL COMP=clang++


License
-------

Cinnamon is released under the GPLv3+ license.

Credits
-------

Cinnamon was written by Giuseppe Cannella at gmail dot com.

  [1]: https://github.com/gekomad/Cinnamon/releases
  [3]: https://www.computerchess.org.uk/ccrl/404/cgi/compare_engines.cgi?family=Cinnamon
  [4]: https://www.gnu.org/software/xboard
  [5]: http://www.playwitharena.de
  [6]: https://triplehappy.com
  [7]: https://play.google.com/store/apps/details?hl=it&id=org.petero.droidfish
  [8]: https://chessboardjs.com
  [9]: https://gekomad.github.io/Cinnamon/perft.html
  [10]: https://www.chessprogramming.org/Chess960_Perft_Results

