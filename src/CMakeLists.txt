cmake_minimum_required(VERSION 3.5)
project(cinnamon_debug)

# debug
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -Wno-braced-scalar-init -mbmi2 -DUSE_BMI2 -DBENCH_MODEx -fno-omit-frame-pointer -g -pthread -pedantic -std=c++11  -DDEBUG_MODEc -fsigned-char -fno-exceptions -fno-rtti -funroll-loops -msse4.2 -DHAS_POPCNT -mpopcnt -msse3 -DHAS_BSF -DHAS_64BIT -m64 ")

# bench
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-braced-scalar-init -mbmi2 -DUSE_BMI2 -pthread -pedantic -DBENCH_MODE -std=c++11 -DDLOG_LEVEL=_FATAL -Wall -Ofast -DNDEBUG -fsigned-char -fno-exceptions -fno-rtti -funroll-loops -msse4.2 -DHAS_POPCNT -mpopcnt -msse3 -DHAS_BSF -DHAS_64BIT -m64 ")

# unit test
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-braced-scalar-init -fsanitize=address -fno-omit-frame-pointer -g -pthread -pedantic -std=c++11 -DDLOG_LEVEL=_TRACE -DFULL_TEST -DDEBUG_MODE -fsigned-char -fno-exceptions -fno-rtti -funroll-loops -msse4.2 -DHAS_POPCNT -mpopcnt -msse3 -DHAS_BSF -DHAS_64BIT -m64 ")

include_directories(db/gtb)
include_directories(db/gtb/compression)
include_directories(db/gtb/compression/lzma)
include_directories(db/gtb/sysport)

set(SOURCE_FILES
        namespaces/constants.h
        namespaces/debug.h
        namespaces/bits.h
        namespaces/random.h
        namespaces/see.h
        namespaces/board.h
        namespaces/board.cpp
        perft/_TPerftRes.h
        perft/Perft.cpp
        perft/Perft.h
        db/OpenBook.cpp
        db/OpenBook.h
        db/syzygy/SYZYGY.h
        db/syzygy/SYZYGY.cpp
        db/syzygy/tbprobe.c
        db/gaviota/GTB.h
        db/gaviota/GTB.cpp
        db/gtb/compression/lzma/Alloc.c
        db/gtb/compression/lzma/Alloc.h
        db/gtb/compression/lzma/Bra.h
        db/gtb/compression/lzma/Bra86.c
        db/gtb/compression/lzma/LzFind.c
        db/gtb/compression/lzma/LzFind.h
        db/gtb/compression/lzma/LzFindMt.h
        db/gtb/compression/lzma/LzHash.h
        db/gtb/compression/lzma/Lzma86Dec.c
        db/gtb/compression/lzma/Lzma86Dec.h
        db/gtb/compression/lzma/Lzma86Enc.c
        db/gtb/compression/lzma/Lzma86Enc.h
        db/gtb/compression/lzma/LzmaDec.c
        db/gtb/compression/lzma/LzmaDec.h
        db/gtb/compression/lzma/LzmaEnc.c
        db/gtb/compression/lzma/LzmaEnc.h
        db/gtb/compression/lzma/Types.h
        db/gtb/compression/wrap.c
        db/gtb/compression/wrap.h
        db/gtb/sysport/sysport.c
        db/gtb/sysport/sysport.h
        db/gtb/gtb-att.c
        db/gtb/gtb-att.h
        db/gtb/gtb-dec.c
        db/gtb/gtb-dec.h
        db/gtb/gtb-probe.c
        db/gtb/gtb-probe.h
        db/gtb/gtb-types.h
        db/gtb/version.h
        perft/PerftThread.cpp
        perft/PerftThread.h
        test/test.cpp
        threadPool/ObserverThread.h
        threadPool/Spinlock.h
        threadPool/Thread.h
        threadPool/ThreadPool.h
        util/Bitboard.cpp
        util/Bitboard.h
        util/FileUtil.h
        GetOpt.h
        util/IniFile.cpp
        util/IniFile.h
        util/logger.h
        util/Singleton.h
        util/String.cpp
        util/String.h
        util/bench/Time.h
        util/Timer.cpp
        util/Timer.h
        ChessBoard.cpp
        ChessBoard.h
        Eval.cpp
        Eval.h
        GenMoves.cpp
        GenMoves.h
        Hash.cpp
        Hash.h
        IterativeDeeping.cpp
        IterativeDeeping.h
        main.cpp
        Search.cpp
        Search.h
        SearchManager.cpp
        SearchManager.h
        Uci.cpp
        Uci.h)

set(CMAKE_CXX_COMPILER "clang++")
add_executable(cinnamon_debug ${SOURCE_FILES})
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
target_link_libraries(cinnamon_debug /usr/lib/libgtest.a)
