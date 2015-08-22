/*
    Cinnamon UCI chess engine
    Copyright (C) Giuseppe Cannella

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sstream>
#include <climits>
#include <fstream>
#include <iostream>
#include "util/stacktrace.h"
#include <iostream>
#include <chrono>
#include <mutex>

using namespace std::chrono;

using namespace std;

#ifdef DEBUG_MODE

template<typename T>
void _debug(T a) {
    cout << a << " ";
}

template<typename T, typename... Args>
void _debug(T t, Args... args) {
    cout << t << " ";
    _debug(args...);
}

static mutex _CoutSyncMutex;

template<typename T, typename... Args>
void debug(T t, Args... args) {
    lock_guard<mutex> lock1(_CoutSyncMutex);
    nanoseconds ms = duration_cast<nanoseconds>(system_clock::now().time_since_epoch());
    cout << "info string TIME: " << ms.count() << " ";

    _debug(t, args...);
    cout << endl;
}

#else

#define debug(...)

#endif
namespace _board {

    static const string NAME = "Cinnamon 1.2c-smp.x";
    static const string STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    typedef unsigned char uchar;
    typedef long long unsigned u64;
    typedef u64 _Tchessboard[16];

    static const int RIGHT_CASTLE_IDX = 12;
    static const int ENPASSANT_IDX = 13;
    static const int SIDETOMOVE_IDX = 14;
    static const int ZOBRISTKEY_IDX = 15;

#define assert(a) if(!(a)){  print_stacktrace();cout<<dec<<endl<<_time::getLocalTime()<<" ********************************** assert error in "<<_file::extractFileName(__FILE__)<< " line "<<__LINE__<<" "<<" **********************************"<<endl;cerr<<flush;exit(1);};

#ifdef DEBUG_MODE
#define ASSERT(a) assert(a)
#define ASSERT_RANGE(value, from, to) {if ((value)<(from) || (value)>(to)){cout<<"VALUE: "<<value<<endl;assert(0)};}
#define INC(a) (a++)
#define ADD(a, b) (a+=(b))
#else

#define ASSERT(a)
#define ASSERT_RANGE(value, from, to)
#define INC(a)
#define ADD(a, b)

#endif
    static const int MAX_PLY = 96;
#if defined(CLOP)
#define STATIC_CONST
#else
#define STATIC_CONST static const
#endif
    STATIC_CONST int VAL_WINDOW = 50;
    typedef struct {
        char promotionPiece;
        char pieceFrom;
        uchar capturedPiece;
        uchar from;
        uchar to;
        char side;
        uchar type;
        int score;
        bool used;
    } _Tmove;

    typedef struct {
        _Tmove *moveList;
        int size;
    } _TmoveP;

    typedef struct {
        int cmove;
        _Tmove argmove[MAX_PLY];
    } _TpvLine;


    static const int BLACK = 0;
    static const int WHITE = 1;
    static const int _INFINITE = 32000;

    static const u64 POW2_0 = 0x1ULL;
    static const u64 POW2_1 = 0x2ULL;
    static const u64 POW2_2 = 0x4ULL;
    static const u64 POW2_3 = 0x8ULL;
    static const u64 POW2_4 = 0x10ULL;
    static const u64 POW2_5 = 0x20ULL;
    static const u64 POW2_6 = 0x40ULL;
    static const u64 POW2_7 = 0x80ULL;
    static const u64 POW2_56 = 0x100000000000000ULL;
    static const u64 POW2_57 = 0x200000000000000ULL;
    static const u64 POW2_58 = 0x400000000000000ULL;
    static const u64 POW2_59 = 0x800000000000000ULL;
    static const u64 POW2_60 = 0x1000000000000000ULL;
    static const u64 POW2_61 = 0x2000000000000000ULL;
    static const u64 POW2_62 = 0x4000000000000000ULL;
    static const u64 POW2_63 = 0x8000000000000000ULL;
    static const u64 NOTPOW2_0 = 0xfffffffffffffffeULL;
    static const u64 NOTPOW2_1 = 0xfffffffffffffffdULL;
    static const u64 NOTPOW2_2 = 0xfffffffffffffffbULL;
    static const u64 NOTPOW2_3 = 0xfffffffffffffff7ULL;
    static const u64 NOTPOW2_4 = 0xffffffffffffffefULL;
    static const u64 NOTPOW2_5 = 0xffffffffffffffdfULL;
    static const u64 NOTPOW2_7 = 0xffffffffffffff7fULL;
    static const u64 NOTPOW2_56 = 0xfeffffffffffffffULL;
    static const u64 NOTPOW2_57 = 0xfdffffffffffffffULL;
    static const u64 NOTPOW2_58 = 0xfbffffffffffffffULL;
    static const u64 NOTPOW2_59 = 0xf7ffffffffffffffULL;
    static const u64 NOTPOW2_60 = 0xefffffffffffffffULL;
    static const u64 NOTPOW2_61 = 0xdfffffffffffffffULL;
    static const u64 NOTPOW2_63 = 0x7fffffffffffffffULL;

    static const u64 RANDSIDE[2] = {0x1cf0862fa4118029ULL, 0xd2a5cab966b3d6cULL};
//    static const u64 BIG_CENTER = 0x3c3c3c3c0000ULL;
    static const string BOARD[64] = {"h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1", "h2", "g2", "f2", "e2", "d2", "c2",
                                     "b2", "a2", "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3", "h4", "g4", "f4", "e4",
                                     "d4", "c4", "b4", "a4", "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5", "h6", "g6",
                                     "f6", "e6", "d6", "c6", "b6", "a6", "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7",
                                     "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8"};

    static const char FEN_PIECE[] = {'p', 'P', 'r', 'R', 'b', 'B', 'n', 'N', 'k', 'K', 'q', 'Q', '-'};

    static const int INV_FEN[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 5, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 9, 0xFF, 0xFF, 7, 0xFF, 1, 11, 3, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 4, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 8, 0xFF, 0xFF, 6, 0xFF, 0, 10, 2, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    static const uchar ORIZ_RIGHT[] = {0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 16, 16, 16, 16, 16, 16, 16, 16,
                                       24, 24, 24, 24, 24, 24, 24, 24, 32, 32, 32, 32, 32, 32, 32, 32, 40, 40, 40, 40,
                                       40, 40, 40, 40, 48, 48, 48, 48, 48, 48, 48, 48, 56, 56, 56, 56, 56, 56, 56, 56};

    static const uchar ORIZ_LEFT[] = {7, 7, 7, 7, 7, 7, 7, 7, 15, 15, 15, 15, 15, 15, 15, 15, 23, 23, 23, 23, 23, 23,
                                      23, 23, 31, 31, 31, 31, 31, 31, 31, 31, 39, 39, 39, 39, 39, 39, 39, 39, 47, 47,
                                      47, 47, 47, 47, 47, 47, 55, 55, 55, 55, 55, 55, 55, 55, 63, 63, 63, 63, 63, 63,
                                      63, 63};

    static const uchar VERT_UPPER[] = {56, 57, 58, 59, 60, 61, 62, 63, 56, 57, 58, 59, 60, 61, 62, 63, 56, 57, 58, 59,
                                       60, 61, 62, 63, 56, 57, 58, 59, 60, 61, 62, 63, 56, 57, 58, 59, 60, 61, 62, 63,
                                       56, 57, 58, 59, 60, 61, 62, 63, 56, 57, 58, 59, 60, 61, 62, 63, 56, 57, 58, 59,
                                       60, 61, 62, 63};

    static const uchar VERT_LOWER[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2,
                                       3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5,
                                       6, 7, 0, 1, 2, 3, 4, 5, 6, 7};

    static const uchar RIGHT_UPPER[] = {63, 55, 47, 39, 31, 23, 15, 7, 62, 63, 55, 47, 39, 31, 23, 15, 61, 62, 63, 55,
                                        47, 39, 31, 23, 60, 61, 62, 63, 55, 47, 39, 31, 59, 60, 61, 62, 63, 55, 47, 39,
                                        58, 59, 60, 61, 62, 63, 55, 47, 57, 58, 59, 60, 61, 62, 63, 55, 56, 57, 58, 59,
                                        60, 61, 62, 63};

    static const uchar RIGHT_LOWER[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 5, 6, 16, 8, 0, 1, 2, 3, 4, 5, 24, 16,
                                        8, 0, 1, 2, 3, 4, 32, 24, 16, 8, 0, 1, 2, 3, 40, 32, 24, 16, 8, 0, 1, 2, 48, 40,
                                        32, 24, 16, 8, 0, 1, 56, 48, 40, 32, 24, 16, 8, 0};

    static const uchar LEFT_UPPER[] = {0, 8, 16, 24, 32, 40, 48, 56, 8, 16, 24, 32, 40, 48, 56, 57, 16, 24, 32, 40, 48,
                                       56, 57, 58, 24, 32, 40, 48, 56, 57, 58, 59, 32, 40, 48, 56, 57, 58, 59, 60, 40,
                                       48, 56, 57, 58, 59, 60, 61, 48, 56, 57, 58, 59, 60, 61, 62, 56, 57, 58, 59, 60,
                                       61, 62, 63};

    static const uchar LEFT_LOWER[] = {0, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 15, 2, 3, 4, 5, 6, 7, 15, 23, 3, 4,
                                       5, 6, 7, 15, 23, 31, 4, 5, 6, 7, 15, 23, 31, 39, 5, 6, 7, 15, 23, 31, 39, 47, 6,
                                       7, 15, 23, 31, 39, 47, 55, 7, 15, 23, 31, 39, 47, 55, 63};

    static const u64 MASK_BIT_UNSET_RIGHT_UP[] = {0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL,
                                                  0x0ULL, 0x1ULL, 0x2ULL, 0x4ULL, 0x8ULL, 0x10ULL, 0x20ULL, 0x40ULL,
                                                  0x0ULL, 0x100ULL, 0x201ULL, 0x402ULL, 0x804ULL, 0x1008ULL, 0x2010ULL,
                                                  0x4020ULL, 0x0ULL, 0x10000ULL, 0x20100ULL, 0x40201ULL, 0x80402ULL,
                                                  0x100804ULL, 0x201008ULL, 0x402010ULL, 0x0ULL, 0x1000000ULL,
                                                  0x2010000ULL, 0x4020100ULL, 0x8040201ULL, 0x10080402ULL,
                                                  0x20100804ULL, 0x40201008ULL, 0x0ULL, 0x100000000ULL, 0x201000000ULL,
                                                  0x402010000ULL, 0x804020100ULL, 0x1008040201ULL, 0x2010080402ULL,
                                                  0x4020100804ULL, 0x0ULL, 0x10000000000ULL, 0x20100000000ULL,
                                                  0x40201000000ULL, 0x80402010000ULL, 0x100804020100ULL,
                                                  0x201008040201ULL, 0x402010080402ULL, 0x0ULL, 0x1000000000000ULL,
                                                  0x2010000000000ULL, 0x4020100000000ULL, 0x8040201000000ULL,
                                                  0x10080402010000ULL, 0x20100804020100ULL, 0x40201008040201ULL};

    static const u64 MASK_BIT_UNSET_RIGHT_DOWN[] = {0x8040201008040200ULL, 0x80402010080400ULL, 0x804020100800ULL,
                                                    0x8040201000ULL, 0x80402000ULL, 0x804000ULL, 0x8000ULL, 0x0ULL,
                                                    0x4020100804020000ULL, 0x8040201008040000ULL, 0x80402010080000ULL,
                                                    0x804020100000ULL, 0x8040200000ULL, 0x80400000ULL, 0x800000ULL,
                                                    0x0ULL, 0x2010080402000000ULL, 0x4020100804000000ULL,
                                                    0x8040201008000000ULL, 0x80402010000000ULL, 0x804020000000ULL,
                                                    0x8040000000ULL, 0x80000000ULL, 0x0ULL, 0x1008040200000000ULL,
                                                    0x2010080400000000ULL, 0x4020100800000000ULL, 0x8040201000000000ULL,
                                                    0x80402000000000ULL, 0x804000000000ULL, 0x8000000000ULL, 0x0ULL,
                                                    0x804020000000000ULL, 0x1008040000000000ULL, 0x2010080000000000ULL,
                                                    0x4020100000000000ULL, 0x8040200000000000ULL, 0x80400000000000ULL,
                                                    0x800000000000ULL, 0x0ULL, 0x402000000000000ULL,
                                                    0x804000000000000ULL, 0x1008000000000000ULL, 0x2010000000000000ULL,
                                                    0x4020000000000000ULL, 0x8040000000000000ULL, 0x80000000000000ULL,
                                                    0x0ULL, 0x200000000000000ULL, 0x400000000000000ULL,
                                                    0x800000000000000ULL, 0x1000000000000000ULL, 0x2000000000000000ULL,
                                                    0x4000000000000000ULL, 0x8000000000000000ULL, 0x0ULL,
                                                    0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL};

    static const u64 MASK_BIT_UNSET_LEFT_UP[] = {0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x2ULL,
                                                 0x4ULL, 0x8ULL, 0x10ULL, 0x20ULL, 0x40ULL, 0x80ULL, 0x0ULL, 0x204ULL,
                                                 0x408ULL, 0x810ULL, 0x1020ULL, 0x2040ULL, 0x4080ULL, 0x8000ULL, 0x0ULL,
                                                 0x20408ULL, 0x40810ULL, 0x81020ULL, 0x102040ULL, 0x204080ULL,
                                                 0x408000ULL, 0x800000ULL, 0x0ULL, 0x2040810ULL, 0x4081020ULL,
                                                 0x8102040ULL, 0x10204080ULL, 0x20408000ULL, 0x40800000ULL,
                                                 0x80000000ULL, 0x0ULL, 0x204081020ULL, 0x408102040ULL, 0x810204080ULL,
                                                 0x1020408000ULL, 0x2040800000ULL, 0x4080000000ULL, 0x8000000000ULL,
                                                 0x0ULL, 0x20408102040ULL, 0x40810204080ULL, 0x81020408000ULL,
                                                 0x102040800000ULL, 0x204080000000ULL, 0x408000000000ULL,
                                                 0x800000000000ULL, 0x0ULL, 0x2040810204080ULL, 0x4081020408000ULL,
                                                 0x8102040800000ULL, 0x10204080000000ULL, 0x20408000000000ULL,
                                                 0x40800000000000ULL, 0x80000000000000ULL, 0x0ULL};

    static const u64 MASK_BIT_UNSET_LEFT_DOWN[] = {0x0ULL, 0x100ULL, 0x10200ULL, 0x1020400ULL, 0x102040800ULL,
                                                   0x10204081000ULL, 0x1020408102000ULL, 0x102040810204000ULL, 0x0ULL,
                                                   0x10000ULL, 0x1020000ULL, 0x102040000ULL, 0x10204080000ULL,
                                                   0x1020408100000ULL, 0x102040810200000ULL, 0x204081020400000ULL,
                                                   0x0ULL, 0x1000000ULL, 0x102000000ULL, 0x10204000000ULL,
                                                   0x1020408000000ULL, 0x102040810000000ULL, 0x204081020000000ULL,
                                                   0x408102040000000ULL, 0x0ULL, 0x100000000ULL, 0x10200000000ULL,
                                                   0x1020400000000ULL, 0x102040800000000ULL, 0x204081000000000ULL,
                                                   0x408102000000000ULL, 0x810204000000000ULL, 0x0ULL, 0x10000000000ULL,
                                                   0x1020000000000ULL, 0x102040000000000ULL, 0x204080000000000ULL,
                                                   0x408100000000000ULL, 0x810200000000000ULL, 0x1020400000000000ULL,
                                                   0x0ULL, 0x1000000000000ULL, 0x102000000000000ULL,
                                                   0x204000000000000ULL, 0x408000000000000ULL, 0x810000000000000ULL,
                                                   0x1020000000000000ULL, 0x2040000000000000ULL, 0x0ULL,
                                                   0x100000000000000ULL, 0x200000000000000ULL, 0x400000000000000ULL,
                                                   0x800000000000000ULL, 0x1000000000000000ULL, 0x2000000000000000ULL,
                                                   0x4000000000000000ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL,
                                                   0x0ULL, 0x0ULL, 0x0ULL};

    static const u64 MASK_BIT_UNSET_DOWN[] = {0x101010101010100ULL, 0x202020202020200ULL, 0x404040404040400ULL,
                                              0x808080808080800ULL, 0x1010101010101000ULL, 0x2020202020202000ULL,
                                              0x4040404040404000ULL, 0x8080808080808000ULL, 0x101010101010000ULL,
                                              0x202020202020000ULL, 0x404040404040000ULL, 0x808080808080000ULL,
                                              0x1010101010100000ULL, 0x2020202020200000ULL, 0x4040404040400000ULL,
                                              0x8080808080800000ULL, 0x101010101000000ULL, 0x202020202000000ULL,
                                              0x404040404000000ULL, 0x808080808000000ULL, 0x1010101010000000ULL,
                                              0x2020202020000000ULL, 0x4040404040000000ULL, 0x8080808080000000ULL,
                                              0x101010100000000ULL, 0x202020200000000ULL, 0x404040400000000ULL,
                                              0x808080800000000ULL, 0x1010101000000000ULL, 0x2020202000000000ULL,
                                              0x4040404000000000ULL, 0x8080808000000000ULL, 0x101010000000000ULL,
                                              0x202020000000000ULL, 0x404040000000000ULL, 0x808080000000000ULL,
                                              0x1010100000000000ULL, 0x2020200000000000ULL, 0x4040400000000000ULL,
                                              0x8080800000000000ULL, 0x101000000000000ULL, 0x202000000000000ULL,
                                              0x404000000000000ULL, 0x808000000000000ULL, 0x1010000000000000ULL,
                                              0x2020000000000000ULL, 0x4040000000000000ULL,
                                              0x8080000000000000ULL, 0x100000000000000ULL, 0x200000000000000ULL,
                                              0x400000000000000ULL, 0x800000000000000ULL, 0x1000000000000000ULL,
                                              0x2000000000000000ULL, 0x4000000000000000ULL, 0x8000000000000000ULL,
                                              0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL};

    static const u64 MASK_BIT_UNSET_UP[] = {0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x1ULL,
                                            0x2ULL, 0x4ULL, 0x8ULL, 0x10ULL, 0x20ULL, 0x40ULL, 0x80ULL, 0x101ULL,
                                            0x202ULL, 0x404ULL, 0x808ULL, 0x1010ULL, 0x2020ULL, 0x4040ULL, 0x8080ULL,
                                            0x10101ULL, 0x20202ULL, 0x40404ULL, 0x80808ULL, 0x101010ULL, 0x202020ULL,
                                            0x404040ULL, 0x808080ULL, 0x1010101ULL, 0x2020202ULL, 0x4040404ULL,
                                            0x8080808ULL, 0x10101010ULL, 0x20202020ULL, 0x40404040ULL, 0x80808080ULL,
                                            0x101010101ULL, 0x202020202ULL, 0x404040404ULL, 0x808080808ULL,
                                            0x1010101010ULL, 0x2020202020ULL, 0x4040404040ULL, 0x8080808080ULL,
                                            0x10101010101ULL, 0x20202020202ULL, 0x40404040404ULL, 0x80808080808ULL,
                                            0x101010101010ULL, 0x202020202020ULL, 0x404040404040ULL, 0x808080808080ULL,
                                            0x1010101010101ULL, 0x2020202020202ULL, 0x4040404040404ULL,
                                            0x8080808080808ULL, 0x10101010101010ULL, 0x20202020202020ULL,
                                            0x40404040404040ULL, 0x80808080808080ULL};

    static const u64 MASK_BIT_UNSET_LEFT[] = {0x0ULL, 0x1ULL, 0x3ULL, 0x7ULL, 0xfULL, 0x1fULL, 0x3fULL, 0x7fULL, 0x0ULL,
                                              0x100ULL, 0x300ULL, 0x700ULL, 0xf00ULL, 0x1f00ULL, 0x3f00ULL, 0x7f00ULL,
                                              0x0ULL, 0x10000ULL, 0x30000ULL, 0x70000ULL, 0xf0000ULL, 0x1f0000ULL,
                                              0x3f0000ULL, 0x7f0000ULL, 0x0ULL, 0x1000000ULL, 0x3000000ULL,
                                              0x7000000ULL, 0xf000000ULL, 0x1f000000ULL, 0x3f000000ULL, 0x7f000000ULL,
                                              0x0ULL, 0x100000000ULL, 0x300000000ULL, 0x700000000ULL, 0xf00000000ULL,
                                              0x1f00000000ULL, 0x3f00000000ULL, 0x7f00000000ULL, 0x0ULL,
                                              0x10000000000ULL, 0x30000000000ULL, 0x70000000000ULL, 0xf0000000000ULL,
                                              0x1f0000000000ULL, 0x3f0000000000ULL, 0x7f0000000000ULL, 0x0ULL,
                                              0x1000000000000ULL, 0x3000000000000ULL, 0x7000000000000ULL,
                                              0xf000000000000ULL, 0x1f000000000000ULL, 0x3f000000000000ULL,
                                              0x7f000000000000ULL, 0x0ULL, 0x100000000000000ULL, 0x300000000000000ULL,
                                              0x700000000000000ULL, 0xf00000000000000ULL, 0x1f00000000000000ULL,
                                              0x3f00000000000000ULL, 0x7f00000000000000ULL};

    static const u64 MASK_BIT_UNSET_RIGHT[] = {0xfeULL, 0xfcULL, 0xf8ULL, 0xf0ULL, 0xe0ULL, 0xc0ULL, 0x80ULL, 0x0ULL,
                                               0xfe00ULL, 0xfc00ULL, 0xf800ULL, 0xf000ULL, 0xe000ULL, 0xc000ULL,
                                               0x8000ULL, 0x0ULL, 0xfe0000ULL, 0xfc0000ULL, 0xf80000ULL, 0xf00000ULL,
                                               0xe00000ULL, 0xc00000ULL, 0x800000ULL, 0x0ULL, 0xfe000000ULL,
                                               0xfc000000ULL, 0xf8000000ULL, 0xf0000000ULL, 0xe0000000ULL,
                                               0xc0000000ULL, 0x80000000, 0x0ULL, 0xfe00000000ULL, 0xfc00000000ULL,
                                               0xf800000000ULL, 0xf000000000ULL, 0xe000000000ULL, 0xc000000000ULL,
                                               0x8000000000ULL, 0x0ULL, 0xfe0000000000ULL, 0xfc0000000000ULL,
                                               0xf80000000000ULL, 0xf00000000000ULL, 0xe00000000000ULL,
                                               0xc00000000000ULL, 0x800000000000ULL, 0x0ULL, 0xfe000000000000ULL,
                                               0xfc000000000000ULL, 0xf8000000000000ULL, 0xf0000000000000ULL,
                                               0xe0000000000000ULL, 0xc0000000000000ULL, 0x80000000000000ULL, 0x0ULL,
                                               0xfe00000000000000ULL, 0xfc00000000000000ULL, 0xf800000000000000ULL,
                                               0xf000000000000000ULL, 0xe000000000000000ULL, 0xc000000000000000ULL,
                                               0x8000000000000000ULL, 0x0ULL};

//    static const u64 RANK_FILE[64] = {0x1010101010101feULL, 0x2020202020202fdULL, 0x4040404040404fbULL, 0x8080808080808f7ULL, 0x10101010101010efULL, 0x20202020202020dfULL, 0x40404040404040bfULL, 0x808080808080807fULL, 0x10101010101fe01ULL, 0x20202020202fd02ULL, 0x40404040404fb04ULL, 0x80808080808f708ULL, 0x101010101010ef10ULL, 0x202020202020df20ULL, 0x404040404040bf40ULL, 0x8080808080807f80ULL, 0x101010101fe0101ULL, 0x202020202fd0202ULL, 0x404040404fb0404ULL, 0x808080808f70808ULL, 0x1010101010ef1010ULL, 0x2020202020df2020ULL, 0x4040404040bf4040ULL, 0x80808080807f8080ULL, 0x1010101fe010101ULL, 0x2020202fd020202ULL, 0x4040404fb040404ULL, 0x8080808f7080808ULL, 0x10101010ef101010ULL, 0x20202020df202020ULL, 0x40404040bf404040ULL, 0x808080807f808080ULL, 0x10101fe01010101ULL, 0x20202fd02020202ULL, 0x40404fb04040404ULL, 0x80808f708080808ULL, 0x101010ef10101010ULL, 0x202020df20202020ULL, 0x404040bf40404040ULL, 0x8080807f80808080ULL, 0x101fe0101010101ULL, 0x202fd0202020202ULL, 0x404fb0404040404ULL, 0x808f70808080808ULL, 0x1010ef1010101010ULL, 0x2020df2020202020ULL, 0x4040bf4040404040ULL, 0x80807f8080808080ULL,
//                                      0x1fe010101010101ULL, 0x2fd020202020202ULL, 0x4fb040404040404ULL, 0x8f7080808080808ULL, 0x10ef101010101010ULL, 0x20df202020202020ULL, 0x40bf404040404040ULL, 0x807f808080808080ULL, 0xfe01010101010101ULL, 0xfd02020202020202ULL, 0xfb04040404040404ULL, 0xf708080808080808ULL, 0xef10101010101010ULL, 0xdf20202020202020ULL, 0xbf40404040404040ULL, 0x7f80808080808080ULL};

    static const u64 RANK_BOUND[64] = {0x2ULL, 0x5ULL, 0xaULL, 0x14ULL, 0x28ULL, 0x50ULL, 0xa0ULL, 0x40ULL, 0x200ULL,
                                       0x500ULL, 0xa00ULL, 0x1400ULL, 0x2800ULL, 0x5000ULL, 0xa000ULL, 0x4000ULL,
                                       0x20000ULL, 0x50000ULL, 0xa0000ULL, 0x140000ULL, 0x280000ULL, 0x500000ULL,
                                       0xa00000ULL, 0x400000ULL, 0x2000000ULL, 0x5000000ULL, 0xa000000ULL,
                                       0x14000000ULL, 0x28000000ULL, 0x50000000ULL, 0xa0000000ULL, 0x40000000ULL,
                                       0x200000000ULL, 0x500000000ULL, 0xa00000000ULL, 0x1400000000ULL, 0x2800000000ULL,
                                       0x5000000000ULL, 0xa000000000ULL, 0x4000000000ULL, 0x20000000000ULL,
                                       0x50000000000ULL, 0xa0000000000ULL, 0x140000000000ULL, 0x280000000000ULL,
                                       0x500000000000ULL, 0xa00000000000ULL, 0x400000000000ULL, 0x2000000000000ULL,
                                       0x5000000000000ULL, 0xa000000000000ULL, 0x14000000000000ULL, 0x28000000000000ULL,
                                       0x50000000000000ULL, 0xa0000000000000ULL, 0x40000000000000ULL,
                                       0x200000000000000ULL, 0x500000000000000ULL, 0xa00000000000000ULL,
                                       0x1400000000000000ULL, 0x2800000000000000ULL, 0x5000000000000000ULL,
                                       0xa000000000000000ULL, 0x4000000000000000ULL};

    static const u64 RANK[64] = {0xffULL, 0xffULL, 0xffULL, 0xffULL, 0xffULL, 0xffULL, 0xffULL, 0xffULL, 0xff00ULL,
                                 0xff00ULL, 0xff00ULL, 0xff00ULL, 0xff00ULL, 0xff00ULL, 0xff00ULL, 0xff00ULL,
                                 0xff0000ULL, 0xff0000ULL, 0xff0000ULL, 0xff0000ULL, 0xff0000ULL, 0xff0000ULL,
                                 0xff0000ULL, 0xff0000ULL, 0xff000000ULL, 0xff000000ULL, 0xff000000ULL, 0xff000000ULL,
                                 0xff000000ULL, 0xff000000ULL, 0xff000000ULL, 0xff000000ULL, 0xff00000000ULL,
                                 0xff00000000ULL, 0xff00000000ULL, 0xff00000000ULL, 0xff00000000ULL, 0xff00000000ULL,
                                 0xff00000000ULL, 0xff00000000ULL, 0xff0000000000ULL, 0xff0000000000ULL,
                                 0xff0000000000ULL, 0xff0000000000ULL, 0xff0000000000ULL, 0xff0000000000ULL,
                                 0xff0000000000ULL, 0xff0000000000ULL, 0xff000000000000ULL, 0xff000000000000ULL,
                                 0xff000000000000ULL, 0xff000000000000ULL, 0xff000000000000ULL, 0xff000000000000ULL,
                                 0xff000000000000ULL, 0xff000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL,
                                 0xff00000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL,
                                 0xff00000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL};

    static const u64 RIGHT_DIAG[64] = {0x8040201008040200ULL, 0x80402010080400ULL, 0x804020100800ULL, 0x8040201000ULL,
                                       0x80402000ULL, 0x804000ULL, 0x8000ULL, 0x0ULL, 0x4020100804020000ULL,
                                       0x8040201008040001ULL, 0x80402010080002ULL, 0x804020100004ULL, 0x8040200008ULL,
                                       0x80400010ULL, 0x800020ULL, 0x40ULL, 0x2010080402000000ULL,
                                       0x4020100804000100ULL, 0x8040201008000201ULL, 0x80402010000402ULL,
                                       0x804020000804ULL, 0x8040001008ULL, 0x80002010ULL, 0x4020ULL,
                                       0x1008040200000000ULL, 0x2010080400010000ULL, 0x4020100800020100ULL,
                                       0x8040201000040201ULL, 0x80402000080402ULL, 0x804000100804ULL, 0x8000201008ULL,
                                       0x402010ULL, 0x804020000000000ULL, 0x1008040001000000ULL, 0x2010080002010000ULL,
                                       0x4020100004020100ULL, 0x8040200008040201ULL, 0x80400010080402ULL,
                                       0x800020100804ULL, 0x40201008ULL, 0x402000000000000ULL, 0x804000100000000ULL,
                                       0x1008000201000000ULL, 0x2010000402010000ULL, 0x4020000804020100ULL,
                                       0x8040001008040201ULL, 0x80002010080402ULL, 0x4020100804ULL,
                                       0x200000000000000ULL, 0x400010000000000ULL, 0x800020100000000ULL,
                                       0x1000040201000000ULL, 0x2000080402010000ULL, 0x4000100804020100ULL,
                                       0x8000201008040201ULL,
                                       0x402010080402ULL, 0x0ULL, 0x1000000000000ULL, 0x2010000000000ULL,
                                       0x4020100000000ULL, 0x8040201000000ULL, 0x10080402010000ULL, 0x20100804020100ULL,
                                       0x40201008040201ULL};

    static const u64 LEFT_DIAG[64] = {0x0ULL, 0x100ULL, 0x10200ULL, 0x1020400ULL, 0x102040800ULL, 0x10204081000ULL,
                                      0x1020408102000ULL, 0x102040810204000ULL, 0x2ULL, 0x10004ULL, 0x1020008ULL,
                                      0x102040010ULL, 0x10204080020ULL, 0x1020408100040ULL, 0x102040810200080ULL,
                                      0x204081020400000ULL, 0x204ULL, 0x1000408ULL, 0x102000810ULL, 0x10204001020ULL,
                                      0x1020408002040ULL, 0x102040810004080ULL, 0x204081020008000ULL,
                                      0x408102040000000ULL, 0x20408ULL, 0x100040810ULL, 0x10200081020ULL,
                                      0x1020400102040ULL, 0x102040800204080ULL, 0x204081000408000ULL,
                                      0x408102000800000ULL, 0x810204000000000ULL, 0x2040810ULL, 0x10004081020ULL,
                                      0x1020008102040ULL, 0x102040010204080ULL, 0x204080020408000ULL,
                                      0x408100040800000ULL, 0x810200080000000ULL, 0x1020400000000000ULL, 0x204081020ULL,
                                      0x1000408102040ULL, 0x102000810204080ULL, 0x204001020408000ULL,
                                      0x408002040800000ULL, 0x810004080000000ULL, 0x1020008000000000ULL,
                                      0x2040000000000000ULL, 0x20408102040ULL, 0x100040810204080ULL,
                                      0x200081020408000ULL, 0x400102040800000ULL, 0x800204080000000ULL,
                                      0x1000408000000000ULL, 0x2000800000000000ULL, 0x4000000000000000ULL,
                                      0x2040810204080ULL,
                                      0x4081020408000ULL, 0x8102040800000ULL, 0x10204080000000ULL, 0x20408000000000ULL,
                                      0x40800000000000ULL, 0x80000000000000ULL, 0x0ULL};

    static const u64 LEFT_RIGHT_DIAG[64] = {0x8040201008040200ULL, 0x80402010080500ULL, 0x804020110a00ULL,
                                            0x8041221400ULL, 0x182442800ULL, 0x10204885000ULL, 0x102040810a000ULL,
                                            0x102040810204000ULL, 0x4020100804020002ULL, 0x8040201008050005ULL,
                                            0x804020110a000aULL, 0x804122140014ULL, 0x18244280028ULL,
                                            0x1020488500050ULL, 0x102040810a000a0ULL, 0x204081020400040ULL,
                                            0x2010080402000204ULL, 0x4020100805000508ULL, 0x804020110a000a11ULL,
                                            0x80412214001422ULL, 0x1824428002844ULL, 0x102048850005088ULL,
                                            0x2040810a000a010ULL, 0x408102040004020ULL, 0x1008040200020408ULL,
                                            0x2010080500050810ULL, 0x4020110a000a1120ULL, 0x8041221400142241ULL,
                                            0x182442800284482ULL, 0x204885000508804ULL, 0x40810a000a01008ULL,
                                            0x810204000402010ULL, 0x804020002040810ULL, 0x1008050005081020ULL,
                                            0x20110a000a112040ULL, 0x4122140014224180ULL, 0x8244280028448201ULL,
                                            0x488500050880402ULL, 0x810a000a0100804ULL, 0x1020400040201008ULL,
                                            0x402000204081020ULL, 0x805000508102040ULL, 0x110a000a11204080ULL,
                                            0x2214001422418000ULL, 0x4428002844820100ULL, 0x8850005088040201ULL,
                                            0x10a000a010080402ULL, 0x2040004020100804ULL, 0x200020408102040ULL,
                                            0x500050810204080ULL, 0xa000a1120408000ULL, 0x1400142241800000ULL,
                                            0x2800284482010000ULL, 0x5000508804020100ULL, 0xa000a01008040201ULL,
                                            0x4000402010080402ULL, 0x2040810204080ULL, 0x5081020408000ULL,
                                            0xa112040800000ULL, 0x14224180000000ULL, 0x28448201000000ULL,
                                            0x50880402010000ULL, 0xa0100804020100ULL, 0x40201008040201ULL};

//    static const u64 LEFT_RIGHT_RANK_FILE[64] = {0x81412111090503feULL, 0x2824222120a07fdULL, 0x404844424150efbULL, 0x8080888492a1cf7ULL, 0x10101011925438efULL, 0x2020212224a870dfULL, 0x404142444850e0bfULL, 0x8182848890a0c07fULL, 0x412111090503fe03ULL, 0x824222120a07fd07ULL, 0x4844424150efb0eULL, 0x80888492a1cf71cULL, 0x101011925438ef38ULL, 0x20212224a870df70ULL, 0x4142444850e0bfe0ULL, 0x82848890a0c07fc0ULL, 0x2111090503fe0305ULL, 0x4222120a07fd070aULL, 0x844424150efb0e15ULL, 0x888492a1cf71c2aULL, 0x1011925438ef3854ULL, 0x212224a870df70a8ULL, 0x42444850e0bfe050ULL, 0x848890a0c07fc0a0ULL, 0x11090503fe030509ULL, 0x22120a07fd070a12ULL, 0x4424150efb0e1524ULL, 0x88492a1cf71c2a49ULL, 0x11925438ef385492ULL, 0x2224a870df70a824ULL, 0x444850e0bfe05048ULL, 0x8890a0c07fc0a090ULL, 0x90503fe03050911ULL, 0x120a07fd070a1222ULL, 0x24150efb0e152444ULL, 0x492a1cf71c2a4988ULL, 0x925438ef38549211ULL, 0x24a870df70a82422ULL, 0x4850e0bfe0504844ULL, 0x90a0c07fc0a09088ULL, 0x503fe0305091121ULL, 0xa07fd070a122242ULL, 0x150efb0e15244484ULL, 0x2a1cf71c2a498808ULL, 0x5438ef3854921110ULL, 0xa870df70a8242221ULL, 0x50e0bfe050484442ULL,
//                                                 0xa0c07fc0a0908884ULL, 0x3fe030509112141ULL, 0x7fd070a12224282ULL, 0xefb0e1524448404ULL, 0x1cf71c2a49880808ULL, 0x38ef385492111010ULL, 0x70df70a824222120ULL, 0xe0bfe05048444241ULL, 0xc07fc0a090888482ULL, 0xfe03050911214181ULL, 0xfd070a1222428202ULL, 0xfb0e152444840404ULL, 0xf71c2a4988080808ULL, 0xef38549211101010ULL, 0xdf70a82422212020ULL, 0xbfe0504844424140ULL, 0x7fc0a09088848281ULL};

    static const char FILE_AT[64] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3,
                                     4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7,
                                     0, 1, 2, 3, 4, 5, 6, 7};

    static const char RANK_AT[64] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
                                     3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6,
                                     7, 7, 7, 7, 7, 7, 7, 7};
    static const char MASK_BIT_SET_COUNT_VERT_UPPER[64] = {7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5,
                                                           5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,
                                                           2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
                                                           0, 0, 0, 0};
    static const char MASK_BIT_SET_COUNT_ORIZ_LEFT[64] = {7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3,
                                                          2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6,
                                                          5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1,
                                                          0};
    static const u64 MASK_BIT_SET_VERT_LOWER[64] = {0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL,
                                                    0x1ULL, 0x2ULL, 0x4ULL, 0x8ULL, 0x10ULL, 0x20ULL, 0x40ULL, 0x80ULL,
                                                    0x101ULL, 0x202ULL, 0x404ULL, 0x808ULL, 0x1010ULL, 0x2020ULL,
                                                    0x4040ULL, 0x8080ULL, 0x10101ULL, 0x20202ULL, 0x40404ULL,
                                                    0x80808ULL, 0x101010ULL, 0x202020ULL, 0x404040ULL, 0x808080ULL,
                                                    0x1010101ULL, 0x2020202ULL, 0x4040404ULL, 0x8080808ULL,
                                                    0x10101010ULL, 0x20202020ULL, 0x40404040ULL, 0x80808080ULL,
                                                    0x101010101ULL, 0x202020202ULL, 0x404040404ULL, 0x808080808ULL,
                                                    0x1010101010ULL, 0x2020202020ULL, 0x4040404040ULL, 0x8080808080ULL,
                                                    0x10101010101ULL, 0x20202020202ULL, 0x40404040404ULL,
                                                    0x80808080808ULL, 0x101010101010ULL, 0x202020202020ULL,
                                                    0x404040404040ULL, 0x808080808080ULL, 0x1010101010101ULL,
                                                    0x2020202020202ULL, 0x4040404040404ULL, 0x8080808080808ULL,
                                                    0x10101010101010ULL, 0x20202020202020ULL, 0x40404040404040ULL,
                                                    0x80808080808080ULL};

    static const u64 MASK_BIT_SET_VERT_UPPER[64] = {0x101010101010100ull, 0x202020202020200ull, 0x404040404040400ull,
                                                    0x808080808080800ull, 0x1010101010101000ull, 0x2020202020202000ull,
                                                    0x4040404040404000ull, 0x8080808080808000ull, 0x101010101010000ull,
                                                    0x202020202020000ull, 0x404040404040000ull, 0x808080808080000ull,
                                                    0x1010101010100000ull, 0x2020202020200000ull, 0x4040404040400000ull,
                                                    0x8080808080800000ull, 0x101010101000000ull, 0x202020202000000ull,
                                                    0x404040404000000ull, 0x808080808000000ull, 0x1010101010000000ull,
                                                    0x2020202020000000ull, 0x4040404040000000ull, 0x8080808080000000ull,
                                                    0x101010100000000ull, 0x202020200000000ull, 0x404040400000000ull,
                                                    0x808080800000000ull, 0x1010101000000000ull, 0x2020202000000000ull,
                                                    0x4040404000000000ull, 0x8080808000000000ull, 0x101010000000000ull,
                                                    0x202020000000000ull, 0x404040000000000ull, 0x808080000000000ull,
                                                    0x1010100000000000ull, 0x2020200000000000ull, 0x4040400000000000ull,
                                                    0x8080800000000000ull, 0x101000000000000ull, 0x202000000000000ull,
                                                    0x404000000000000ull, 0x808000000000000ull, 0x1010000000000000ull,
                                                    0x2020000000000000ull, 0x4040000000000000ull,
                                                    0x8080000000000000ull, 0x100000000000000ull, 0x200000000000000ull,
                                                    0x400000000000000ull, 0x800000000000000ull, 0x1000000000000000ull,
                                                    0x2000000000000000ull, 0x4000000000000000ull, 0x8000000000000000ull,
                                                    0x0ull, 0x0ull, 0x0ull, 0x0ull, 0x0ull, 0x0ull, 0x0ull, 0x0ull};

    static const u64 MASK_BIT_SET_ORIZ_LEFT[64] = {0xfeULL, 0xfcULL, 0xf8ULL, 0xf0ULL, 0xe0ULL, 0xc0ULL, 0x80ULL,
                                                   0x0ULL, 0xfe00ULL, 0xfc00ULL, 0xf800ULL, 0xf000ULL, 0xe000ULL,
                                                   0xc000ULL, 0x8000ULL, 0x0ULL, 0xfe0000ULL, 0xfc0000ULL, 0xf80000ULL,
                                                   0xf00000ULL, 0xe00000ULL, 0xc00000ULL, 0x800000ULL, 0x0ULL,
                                                   0xfe000000ULL, 0xfc000000ULL, 0xf8000000ULL, 0xf0000000ULL,
                                                   0xe0000000ULL, 0xc0000000ULL, 0x80000000ULL, 0x0ULL, 0xfe00000000ULL,
                                                   0xfc00000000ULL, 0xf800000000ULL, 0xf000000000ULL, 0xe000000000ULL,
                                                   0xc000000000ULL, 0x8000000000ULL, 0x0ULL, 0xfe0000000000ULL,
                                                   0xfc0000000000ULL, 0xf80000000000ULL, 0xf00000000000ULL,
                                                   0xe00000000000ULL, 0xc00000000000ULL, 0x800000000000ULL, 0x0ULL,
                                                   0xfe000000000000ULL, 0xfc000000000000ULL, 0xf8000000000000ULL,
                                                   0xf0000000000000ULL, 0xe0000000000000ULL, 0xc0000000000000ULL,
                                                   0x80000000000000ULL, 0x0ULL, 0xfe00000000000000ULL,
                                                   0xfc00000000000000ULL, 0xf800000000000000ULL, 0xf000000000000000ULL,
                                                   0xe000000000000000ULL, 0xc000000000000000ULL, 0x8000000000000000ULL,
                                                   0x0ULL};

    static const u64 MASK_BIT_SET_ORIZ_RIGHT[64] = {0x0ULL, 0x1ULL, 0x3ULL, 0x7ULL, 0xfULL, 0x1fULL, 0x3fULL, 0x7fULL,
                                                    0x0ULL, 0x100ULL, 0x300ULL, 0x700ULL, 0xf00ULL, 0x1f00ULL,
                                                    0x3f00ULL, 0x7f00ULL, 0x0ULL, 0x10000ULL, 0x30000ULL, 0x70000ULL,
                                                    0xf0000ULL, 0x1f0000ULL, 0x3f0000ULL, 0x7f0000ULL, 0x0ULL,
                                                    0x1000000ULL, 0x3000000ULL, 0x7000000ULL, 0xf000000ULL,
                                                    0x1f000000ULL, 0x3f000000ULL, 0x7f000000ULL, 0x0ULL, 0x100000000ULL,
                                                    0x300000000ULL, 0x700000000ULL, 0xf00000000ULL, 0x1f00000000ULL,
                                                    0x3f00000000ULL, 0x7f00000000ULL, 0x0ULL, 0x10000000000ULL,
                                                    0x30000000000ULL, 0x70000000000ULL, 0xf0000000000ULL,
                                                    0x1f0000000000ULL, 0x3f0000000000ULL, 0x7f0000000000ULL, 0x0ULL,
                                                    0x1000000000000ULL, 0x3000000000000ULL, 0x7000000000000ULL,
                                                    0xf000000000000ULL, 0x1f000000000000ULL, 0x3f000000000000ULL,
                                                    0x7f000000000000ULL, 0x0ULL, 0x100000000000000ULL,
                                                    0x300000000000000ULL, 0x700000000000000ULL, 0xf00000000000000ULL,
                                                    0x1f00000000000000ULL, 0x3f00000000000000ULL,
                                                    0x7f00000000000000ULL};

    static const char MASK_BIT_SET_COUNT_LEFT_LOWER[64] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2,
                                                           2, 2, 1, 0, 3, 3, 3, 3, 3, 2, 1, 0, 4, 4, 4, 4, 3, 2, 1, 0,
                                                           5, 5, 5, 4, 3, 2, 1, 0, 6, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4,
                                                           3, 2, 1, 0};

    static const char MASK_BIT_SET_COUNT_LEFT_UPPER[64] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 6, 0, 1, 2, 3,
                                                           4, 5, 5, 5, 0, 1, 2, 3, 4, 4, 4, 4, 0, 1, 2, 3, 3, 3, 3, 3,
                                                           0, 1, 2, 2, 2, 2, 2, 2, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
                                                           0, 0, 0, 0};

    static const char MASK_BIT_SET_COUNT_RIGHT_LOWER[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 2,
                                                            2, 2, 2, 2, 0, 1, 2, 3, 3, 3, 3, 3, 0, 1, 2, 3, 4, 4, 4, 4,
                                                            0, 1, 2, 3, 4, 5, 5, 5, 0, 1, 2, 3, 4, 5, 6, 6, 0, 1, 2, 3,
                                                            4, 5, 6, 7};

    static const char MASK_BIT_SET_COUNT_RIGHT_UPPER[64] = {7, 6, 5, 4, 3, 2, 1, 0, 6, 6, 5, 4, 3, 2, 1, 0, 5, 5, 5, 4,
                                                            3, 2, 1, 0, 4, 4, 4, 4, 3, 2, 1, 0, 3, 3, 3, 3, 3, 2, 1, 0,
                                                            2, 2, 2, 2, 2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
                                                            0, 0, 0, 0};

    static const u64 MASK_BIT_SET_LEFT_LOWER[64] = {0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL,
                                                    0x2ULL, 0x4ULL, 0x8ULL, 0x10ULL, 0x20ULL, 0x40ULL, 0x80ULL, 0x0ULL,
                                                    0x204ULL, 0x408ULL, 0x810ULL, 0x1020ULL, 0x2040ULL, 0x4080ULL,
                                                    0x8000ULL, 0x0ULL, 0x20408ULL, 0x40810ULL, 0x81020ULL, 0x102040ULL,
                                                    0x204080ULL, 0x408000ULL, 0x800000ULL, 0x0ULL, 0x2040810ULL,
                                                    0x4081020ULL, 0x8102040ULL, 0x10204080ULL, 0x20408000ULL,
                                                    0x40800000ULL, 0x80000000ULL, 0x0ULL, 0x204081020ULL,
                                                    0x408102040ULL, 0x810204080ULL, 0x1020408000ULL, 0x2040800000ULL,
                                                    0x4080000000ULL, 0x8000000000ULL, 0x0ULL, 0x20408102040ULL,
                                                    0x40810204080ULL, 0x81020408000ULL, 0x102040800000ULL,
                                                    0x204080000000ULL, 0x408000000000ULL, 0x800000000000ULL, 0x0ULL,
                                                    0x2040810204080ULL, 0x4081020408000ULL, 0x8102040800000ULL,
                                                    0x10204080000000ULL, 0x20408000000000ULL, 0x40800000000000ULL,
                                                    0x80000000000000ULL, 0x0ULL};

    static const u64 MASK_BIT_SET_LEFT_UPPER[64] = {0x0ULL, 0x100ULL, 0x10200ULL, 0x1020400ULL, 0x102040800ULL,
                                                    0x10204081000ULL, 0x1020408102000ULL, 0x102040810204000ULL, 0x0ULL,
                                                    0x10000ULL, 0x1020000ULL, 0x102040000ULL, 0x10204080000ULL,
                                                    0x1020408100000ULL, 0x102040810200000ULL, 0x204081020400000ULL,
                                                    0x0ULL, 0x1000000ULL, 0x102000000ULL, 0x10204000000ULL,
                                                    0x1020408000000ULL, 0x102040810000000ULL, 0x204081020000000ULL,
                                                    0x408102040000000ULL, 0x0ULL, 0x100000000ULL, 0x10200000000ULL,
                                                    0x1020400000000ULL, 0x102040800000000ULL, 0x204081000000000ULL,
                                                    0x408102000000000ULL, 0x810204000000000ULL, 0x0ULL,
                                                    0x10000000000ULL, 0x1020000000000ULL, 0x102040000000000ULL,
                                                    0x204080000000000ULL, 0x408100000000000ULL, 0x810200000000000ULL,
                                                    0x1020400000000000ULL, 0x0ULL, 0x1000000000000ULL,
                                                    0x102000000000000ULL, 0x204000000000000ULL, 0x408000000000000ULL,
                                                    0x810000000000000ULL, 0x1020000000000000ULL, 0x2040000000000000ULL,
                                                    0x0ULL, 0x100000000000000ULL, 0x200000000000000ULL,
                                                    0x400000000000000ULL, 0x800000000000000ULL, 0x1000000000000000ULL,
                                                    0x2000000000000000ULL, 0x4000000000000000ULL, 0x0ULL, 0x0ULL,
                                                    0x0ULL, 0x0ULL, 0x0ULL,
                                                    0x0ULL, 0x0ULL, 0x0ULL};

    static const u64 MASK_BIT_SET_RIGHT_LOWER[64] = {0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL,
                                                     0x0ULL, 0x1ULL, 0x2ULL, 0x4ULL, 0x8ULL, 0x10ULL, 0x20ULL, 0x40ULL,
                                                     0x0ULL, 0x100ULL, 0x201ULL, 0x402ULL, 0x804ULL, 0x1008ULL,
                                                     0x2010ULL, 0x4020ULL, 0x0ULL, 0x10000ULL, 0x20100ULL, 0x40201ULL,
                                                     0x80402ULL, 0x100804ULL, 0x201008ULL, 0x402010ULL, 0x0ULL,
                                                     0x1000000ULL, 0x2010000ULL, 0x4020100ULL, 0x8040201ULL,
                                                     0x10080402ULL, 0x20100804ULL, 0x40201008ULL, 0x0ULL,
                                                     0x100000000ULL, 0x201000000ULL, 0x402010000ULL, 0x804020100ULL,
                                                     0x1008040201ULL, 0x2010080402ULL, 0x4020100804ULL, 0x0ULL,
                                                     0x10000000000ULL, 0x20100000000ULL, 0x40201000000ULL,
                                                     0x80402010000ULL, 0x100804020100ULL, 0x201008040201ULL,
                                                     0x402010080402ULL, 0x0ULL, 0x1000000000000ULL, 0x2010000000000ULL,
                                                     0x4020100000000ULL, 0x8040201000000ULL, 0x10080402010000ULL,
                                                     0x20100804020100ULL, 0x40201008040201ULL};

    static const u64 MASK_BIT_SET_RIGHT_UPPER[64] = {0x8040201008040200ULL, 0x80402010080400ULL, 0x804020100800ULL,
                                                     0x8040201000ULL, 0x80402000ULL, 0x804000ULL, 0x8000ULL, 0x0ULL,
                                                     0x4020100804020000ULL, 0x8040201008040000ULL, 0x80402010080000ULL,
                                                     0x804020100000ULL, 0x8040200000ULL, 0x80400000ULL, 0x800000ULL,
                                                     0x0ULL, 0x2010080402000000ULL, 0x4020100804000000ULL,
                                                     0x8040201008000000ULL, 0x80402010000000ULL, 0x804020000000ULL,
                                                     0x8040000000ULL, 0x80000000ULL, 0x0ULL, 0x1008040200000000ULL,
                                                     0x2010080400000000ULL, 0x4020100800000000ULL,
                                                     0x8040201000000000ULL, 0x80402000000000ULL, 0x804000000000ULL,
                                                     0x8000000000ULL, 0x0ULL, 0x804020000000000ULL,
                                                     0x1008040000000000ULL, 0x2010080000000000ULL,
                                                     0x4020100000000000ULL, 0x8040200000000000ULL, 0x80400000000000ULL,
                                                     0x800000000000ULL, 0x0ULL, 0x402000000000000ULL,
                                                     0x804000000000000ULL, 0x1008000000000000ULL, 0x2010000000000000ULL,
                                                     0x4020000000000000ULL, 0x8040000000000000ULL, 0x80000000000000ULL,
                                                     0x0ULL, 0x200000000000000ULL, 0x400000000000000ULL,
                                                     0x800000000000000ULL, 0x1000000000000000ULL, 0x2000000000000000ULL,
                                                     0x4000000000000000ULL, 0x8000000000000000ULL, 0x0ULL,
                                                     0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL};


//    static const u64 PAWNS_8_1[2] = {0xFFULL, 0xFF00000000000000ULL};

    static const u64 PAWNS_JUMP[2] = {0xFF000000000000ULL, 0xFF00ULL};
    static const u64 PAWNS_7_2[2] = {0xFF00ULL, 0xFF000000000000ULL};

    static const u64 FILE_[64] = {0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL,
                                  0x808080808080808ULL, 0x1010101010101010ULL, 0x2020202020202020ULL,
                                  0x4040404040404040ULL, 0x8080808080808080ULL, 0x101010101010101ULL,
                                  0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL,
                                  0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL,
                                  0x8080808080808080ULL, 0x101010101010101ULL, 0x202020202020202ULL,
                                  0x404040404040404ULL, 0x808080808080808ULL, 0x1010101010101010ULL,
                                  0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL,
                                  0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL,
                                  0x808080808080808ULL, 0x1010101010101010ULL, 0x2020202020202020ULL,
                                  0x4040404040404040ULL, 0x8080808080808080ULL, 0x101010101010101ULL,
                                  0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL,
                                  0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL,
                                  0x8080808080808080ULL, 0x101010101010101ULL, 0x202020202020202ULL,
                                  0x404040404040404ULL, 0x808080808080808ULL, 0x1010101010101010ULL,
                                  0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL,
                                  0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL,
                                  0x808080808080808ULL, 0x1010101010101010ULL, 0x2020202020202020ULL,
                                  0x4040404040404040ULL, 0x8080808080808080ULL, 0x101010101010101ULL,
                                  0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL,
                                  0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL,
                                  0x8080808080808080ULL};

    static const u64 POW2[64] = {0x1ULL, 0x2ULL, 0x4ULL, 0x8ULL, 0x10ULL, 0x20ULL, 0x40ULL, 0x80ULL, 0x100ULL, 0x200ULL,
                                 0x400ULL, 0x800ULL, 0x1000ULL, 0x2000ULL, 0x4000ULL, 0x8000ULL, 0x10000ULL, 0x20000ULL,
                                 0x40000ULL, 0x80000ULL, 0x100000ULL, 0x200000ULL, 0x400000ULL, 0x800000ULL,
                                 0x1000000ULL, 0x2000000ULL, 0x4000000ULL, 0x8000000ULL, 0x10000000ULL, 0x20000000ULL,
                                 0x40000000ULL, 0x80000000ULL, 0x100000000ULL, 0x200000000ULL, 0x400000000ULL,
                                 0x800000000ULL, 0x1000000000ULL, 0x2000000000ULL, 0x4000000000ULL, 0x8000000000ULL,
                                 0x10000000000ULL, 0x20000000000ULL, 0x40000000000ULL, 0x80000000000ULL,
                                 0x100000000000ULL, 0x200000000000ULL, 0x400000000000ULL, 0x800000000000ULL,
                                 0x1000000000000ULL, 0x2000000000000ULL, 0x4000000000000ULL, 0x8000000000000ULL,
                                 0x10000000000000ULL, 0x20000000000000ULL, 0x40000000000000ULL, 0x80000000000000ULL,
                                 0x100000000000000ULL, 0x200000000000000ULL, 0x400000000000000ULL, 0x800000000000000ULL,
                                 0x1000000000000000ULL, 0x2000000000000000ULL, 0x4000000000000000ULL,
                                 0x8000000000000000ULL};

    static const u64 NOTPOW2[64] = {0xfffffffffffffffeULL, 0xfffffffffffffffdULL, 0xfffffffffffffffbULL,
                                    0xfffffffffffffff7ULL, 0xffffffffffffffefULL, 0xffffffffffffffdfULL,
                                    0xffffffffffffffbfULL, 0xffffffffffffff7fULL, 0xfffffffffffffeffULL,
                                    0xfffffffffffffdffULL, 0xfffffffffffffbffULL, 0xfffffffffffff7ffULL,
                                    0xffffffffffffefffULL, 0xffffffffffffdfffULL, 0xffffffffffffbfffULL,
                                    0xffffffffffff7fffULL, 0xfffffffffffeffffULL, 0xfffffffffffdffffULL,
                                    0xfffffffffffbffffULL, 0xfffffffffff7ffffULL, 0xffffffffffefffffULL,
                                    0xffffffffffdfffffULL, 0xffffffffffbfffffULL, 0xffffffffff7fffffULL,
                                    0xfffffffffeffffffULL, 0xfffffffffdffffffULL, 0xfffffffffbffffffULL,
                                    0xfffffffff7ffffffULL, 0xffffffffefffffffULL, 0xffffffffdfffffffULL,
                                    0xffffffffbfffffffULL, 0xffffffff7fffffffULL, 0xfffffffeffffffffULL,
                                    0xfffffffdffffffffULL, 0xfffffffbffffffffULL, 0xfffffff7ffffffffULL,
                                    0xffffffefffffffffULL, 0xffffffdfffffffffULL, 0xffffffbfffffffffULL,
                                    0xffffff7fffffffffULL, 0xfffffeffffffffffULL, 0xfffffdffffffffffULL,
                                    0xfffffbffffffffffULL, 0xfffff7ffffffffffULL, 0xffffefffffffffffULL,
                                    0xffffdfffffffffffULL, 0xffffbfffffffffffULL,
                                    0xffff7fffffffffffULL, 0xfffeffffffffffffULL, 0xfffdffffffffffffULL,
                                    0xfffbffffffffffffULL, 0xfff7ffffffffffffULL, 0xffefffffffffffffULL,
                                    0xffdfffffffffffffULL, 0xffbfffffffffffffULL, 0xff7fffffffffffffULL,
                                    0xfeffffffffffffffULL, 0xfdffffffffffffffULL, 0xfbffffffffffffffULL,
                                    0xf7ffffffffffffffULL, 0xefffffffffffffffULL, 0xdfffffffffffffffULL,
                                    0xbfffffffffffffffULL, 0x7fffffffffffffffULL};


    static const u64 PAWN_FORK_MASK[2][64] = {{0,                     0,                     0,                     0,                     0,                     0,                     0,                     0,

                                                      0x2ULL,     0x5ULL,     0xaULL,     0x14ULL,     0x28ULL,     0x50ULL,     0xa0ULL,     0x40ULL,

                                                      0x200ULL,     0x500ULL,     0xa00ULL,     0x1400ULL,     0x2800ULL,     0x5000ULL,     0xa000ULL,     0x4000ULL,

                                                      0x20000ULL,     0x50000ULL,     0xa0000ULL,     0x140000ULL,     0x280000ULL,     0x500000ULL,     0xa00000ULL,     0x400000ULL,

                                                      0x2000000ULL,     0x5000000ULL,     0xa000000ULL,     0x14000000ULL,     0x28000000ULL,     0x50000000ULL,     0xa0000000ULL,     0x40000000ULL,

                                                      0x200000000ULL,     0x500000000ULL,     0xa00000000ULL,     0x1400000000ULL,     0x2800000000ULL,     0x5000000000ULL,     0xa000000000ULL,     0x4000000000ULL,

                                                      0x20000000000ULL,     0x50000000000ULL,     0xa0000000000ULL,     0x140000000000ULL,     0x280000000000ULL,     0x500000000000ULL,     0xa00000000000ULL,     0x400000000000ULL,

                                                      0x0002000000000000ULL, 0x0005000000000000ULL, 0x000A000000000000ULL, 0x0014000000000000ULL, 0x0028000000000000ULL, 0x0050000000000000ULL, 0x00A0000000000000ULL, 0x0040000000000000ULL},
                                              {0x0000000000000200ULL, 0x0000000000000500ULL, 0x0000000000000A00ULL, 0x0000000000001400ULL, 0x0000000000002800ULL, 0x0000000000005000ULL, 0x000000000000A000ULL, 0x0000000000004000ULL,

                                                      0x20000ULL, 0x50000ULL, 0xa0000ULL, 0x140000ULL, 0x280000ULL, 0x500000ULL, 0xa00000ULL, 0x400000ULL,

                                                      0x2000000ULL, 0x5000000ULL, 0xa000000ULL, 0x14000000ULL, 0x28000000ULL, 0x50000000ULL, 0xa0000000ULL, 0x40000000ULL,

                                                      0x200000000ULL, 0x500000000ULL, 0xa00000000ULL, 0x1400000000ULL, 0x2800000000ULL, 0x5000000000ULL, 0xa000000000ULL, 0x4000000000ULL,

                                                      0x20000000000ULL, 0x50000000000ULL, 0xa0000000000ULL, 0x140000000000ULL, 0x280000000000ULL, 0x500000000000ULL, 0xa00000000000ULL, 0x400000000000ULL,

                                                      0x2000000000000ULL, 0x5000000000000ULL, 0xa000000000000ULL, 0x14000000000000ULL, 0x28000000000000ULL, 0x50000000000000ULL, 0xa0000000000000ULL, 0x40000000000000ULL,

                                                      0x200000000000000ULL, 0x500000000000000ULL, 0xa00000000000000ULL, 0x1400000000000000ULL, 0x2800000000000000ULL, 0x5000000000000000ULL, 0xa000000000000000ULL, 0x4000000000000000ULL,

                                                      0,                     0,                     0,                     0,                     0,                     0,                     0,                     0}};

    static const u64 NEAR_MASK1[64] = {0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000E0AULL,
                                       0x0000000000001C14ULL, 0x0000000000003828ULL, 0x0000000000007050ULL,
                                       0x000000000000E0A0ULL, 0x000000000000C040ULL, 0x0000000000030203ULL,
                                       0x0000000000070507ULL, 0x00000000000E0A0EULL, 0x00000000001C141CULL,
                                       0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000E0A0E0ULL,
                                       0x0000000000C040C0ULL, 0x0000000003020300ULL, 0x0000000007050700ULL,
                                       0x000000000E0A0E00ULL, 0x000000001C141C00ULL, 0x0000000038283800ULL,
                                       0x0000000070507000ULL, 0x00000000E0A0E000ULL, 0x00000000C040C000ULL,
                                       0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000E0A0E0000ULL,
                                       0x0000001C141C0000ULL, 0x0000003828380000ULL, 0x0000007050700000ULL,
                                       0x000000E0A0E00000ULL, 0x000000C040C00000ULL, 0x0000030203000000ULL,
                                       0x0000070507000000ULL, 0x00000E0A0E000000ULL, 0x00001C141C000000ULL,
                                       0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000E0A0E0000000ULL,
                                       0x0000C040C0000000ULL, 0x0003020300000000ULL, 0x0007050700000000ULL,
                                       0x000E0A0E00000000ULL, 0x001C141C00000000ULL, 0x0038283800000000ULL,
                                       0x0070507000000000ULL, 0x00E0A0E000000000ULL,
                                       0x00C040C000000000ULL, 0x0302030000000000ULL, 0x0705070000000000ULL,
                                       0x0E0A0E0000000000ULL, 0x1C141C0000000000ULL, 0x3828380000000000ULL,
                                       0x7050700000000000ULL, 0xE0A0E00000000000ULL, 0xC040C00000000000ULL,
                                       0x0203000000000000ULL, 0x0507000000000000ULL, 0x0A0E000000000000ULL,
                                       0x141C000000000000ULL, 0x2838000000000000ULL, 0x5070000000000000ULL,
                                       0xA0E0000000000000ULL, 0x40C0000000000000ULL};

    static const u64 NEAR_MASK2[64] = {0x70706ULL, 0xf0f0dULL, 0x1f1f1bULL, 0x3e3e36ULL, 0x7c7c6cULL, 0xf8f8d8ULL,
                                       0xf0f0b0ULL, 0xe0e060ULL, 0x7070607ULL, 0xf0f0d0fULL, 0x1f1f1b1fULL,
                                       0x3e3e363eULL, 0x7c7c6c7cULL, 0xf8f8d8f8ULL, 0xf0f0b0f0ULL, 0xe0e060e0ULL,
                                       0x707060707ULL, 0xf0f0d0f0fULL, 0x1f1f1b1f1fULL, 0x3e3e363e3eULL,
                                       0x7c7c6c7c7cULL, 0xf8f8d8f8f8ULL, 0xf0f0b0f0f0ULL, 0xe0e060e0e0ULL,
                                       0x70706070700ULL, 0xf0f0d0f0f00ULL, 0x1f1f1b1f1f00ULL, 0x3e3e363e3e00ULL,
                                       0x7c7c6c7c7c00ULL, 0xf8f8d8f8f800ULL, 0xf0f0b0f0f000ULL, 0xe0e060e0e000ULL,
                                       0x7070607070000ULL, 0xf0f0d0f0f0000ULL, 0x1f1f1b1f1f0000ULL, 0x3e3e363e3e0000ULL,
                                       0x7c7c6c7c7c0000ULL, 0xf8f8d8f8f80000ULL, 0xf0f0b0f0f00000ULL,
                                       0xe0e060e0e00000ULL, 0x707060707000000ULL, 0xf0f0d0f0f000000ULL,
                                       0x1f1f1b1f1f000000ULL, 0x3e3e363e3e000000ULL, 0x7c7c6c7c7c000000ULL,
                                       0xf8f8d8f8f8000000ULL, 0xf0f0b0f0f0000000ULL, 0xe0e060e0e0000000ULL,
                                       0x706070700000000ULL, 0xf0d0f0f00000000ULL, 0x1f1b1f1f00000000ULL,
                                       0x3e363e3e00000000ULL, 0x7c6c7c7c00000000ULL, 0xf8d8f8f800000000ULL,
                                       0xf0b0f0f000000000ULL, 0xe060e0e000000000ULL, 0x607070000000000ULL,
                                       0xd0f0f0000000000ULL,
                                       0x1b1f1f0000000000ULL, 0x363e3e0000000000ULL, 0x6c7c7c0000000000ULL,
                                       0xd8f8f80000000000ULL, 0xb0f0f00000000000ULL, 0x60e0e00000000000ULL};

    static const u64 KNIGHT_MASK[64] = {0x0000000000020400ULL, 0x0000000000050800ULL, 0x00000000000A1100ULL,
                                        0x0000000000142200ULL, 0x0000000000284400ULL, 0x0000000000508800ULL,
                                        0x0000000000A01000ULL, 0x0000000000402000ULL, 0x0000000002040004ULL,
                                        0x0000000005080008ULL, 0x000000000A110011ULL, 0x0000000014220022ULL,
                                        0x0000000028440044ULL, 0x0000000050880088ULL, 0x00000000A0100010ULL,
                                        0x0000000040200020ULL, 0x0000000204000402ULL, 0x0000000508000805ULL,
                                        0x0000000A1100110AULL, 0x0000001422002214ULL, 0x0000002844004428ULL,
                                        0x0000005088008850ULL, 0x000000A0100010A0ULL, 0x0000004020002040ULL,
                                        0x0000020400040200ULL, 0x0000050800080500ULL, 0x00000A1100110A00ULL,
                                        0x0000142200221400ULL, 0x0000284400442800ULL, 0x0000508800885000ULL,
                                        0x0000A0100010A000ULL, 0x0000402000204000ULL, 0x0002040004020000ULL,
                                        0x0005080008050000ULL, 0x000A1100110A0000ULL, 0x0014220022140000ULL,
                                        0x0028440044280000ULL, 0x0050880088500000ULL, 0x00A0100010A00000ULL,
                                        0x0040200020400000ULL, 0x0204000402000000ULL, 0x0508000805000000ULL,
                                        0x0A1100110A000000ULL, 0x1422002214000000ULL, 0x2844004428000000ULL,
                                        0x5088008850000000ULL, 0xA0100010A0000000ULL,
                                        0x4020002040000000ULL, 0x0400040200000000ULL, 0x0800080500000000ULL,
                                        0x1100110A00000000ULL, 0x2200221400000000ULL, 0x4400442800000000ULL,
                                        0x8800885000000000ULL, 0x100010A000000000ULL, 0x2000204000000000ULL,
                                        0x0004020000000000ULL, 0x0008050000000000ULL, 0x00110A0000000000ULL,
                                        0x0022140000000000ULL, 0x0044280000000000ULL, 0x0088500000000000ULL,
                                        0x0010A00000000000ULL, 0x0020400000000000ULL};

    static const u64 ENPASSANT_MASK[2][64] = {{0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,         0ULL,         0ULL,         0ULL,          0ULL,          0ULL,          0ULL,          0ULL,          0x200000000ULL, 0x500000000ULL, 0xA00000000ULL, 0x1400000000ULL, 0x2800000000ULL, 0x5000000000ULL, 0xA000000000ULL, 0x4000000000ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0},
                                              {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0x2000000ULL, 0x5000000ULL, 0xA000000ULL, 0x14000000ULL, 0x28000000ULL, 0x50000000ULL, 0xA0000000ULL, 0x40000000ULL, 0ULL,           0ULL,           0ULL,           0ULL,            0ULL,            0ULL,            0ULL,            0ULL,            0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0}};
}

using namespace _board;

namespace _time {
    string getLocalTime();

    int diffTime(struct timeb a, struct timeb b);

    int getYear();

    int getMonth();

    int getDay();
}

namespace _file {
    bool fileExists(string filename);

    int fileSize(const string &FileName);

    string extractFileName(string path);
}

namespace _random {
    static const u64 RANDOM_KEY[15][64] = {
#include "random.inc"
    };
}

namespace _eval {

    static const int VALUEPAWN = 100;
    static const int VALUEROOK = 520;
    static const int VALUEBISHOP = 335;
    static const int VALUEKNIGHT = 330;
    static const int VALUEQUEEN = 980;
    static const int VALUEKING = _INFINITE;

    static const int PIECES_VALUE[] = {VALUEPAWN, VALUEPAWN, VALUEROOK, VALUEROOK, VALUEBISHOP, VALUEBISHOP,
                                       VALUEKNIGHT, VALUEKNIGHT, VALUEKING, VALUEKING, VALUEQUEEN, VALUEQUEEN, 0};

    enum _Tstatus {
        OPEN, MIDDLE, END
    };

    static const int MOB_QUEEN[][29] = {{0,   1,   1,   1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
                                        {-10, -9,  -5,  0, 3, 6, 7, 10, 11, 12, 15, 18, 28, 30, 32, 35, 40, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61},
                                        {-20, -15, -10, 0, 1, 3, 4, 9,  11, 12, 15, 18, 28, 30, 32, 33, 34, 36, 37, 39, 40, 41, 42, 43, 44, 45, 56, 47, 48}};

    static const int MOB_ROOK[][15] = {{-1,  0,   1,  4, 5, 6,  7,  9,  12, 14, 19, 22, 23, 24, 25},
                                       {-9,  -8,  1,  8, 9, 10, 15, 20, 28, 30, 40, 45, 50, 51, 52},
                                       {-15, -10, -5, 0, 9, 11, 16, 22, 30, 32, 40, 45, 50, 51, 52}};

    static const int MOB_KNIGHT[] = {-8, -4, 7, 10, 15, 20, 30, 35, 40};

    static const int MOB_BISHOP[][14] = {{-8,  -7,  2,  8, 9, 10, 15, 20, 28, 30, 40, 45, 50, 50},
                                         {-20, -10, -4, 0, 5, 10, 15, 20, 28, 30, 40, 45, 50, 50},
                                         {-20, -10, -4, 0, 3, 8,  13, 18, 25, 30, 40, 45, 50, 50}};

    static const int MOB_KING[][9] = {{1,   2,   2,   1,  0,  0,  0,  0,  0},
                                      {-5,  0,   5,   5,  5,  0,  0,  0,  0},
                                      {-50, -30, -10, 10, 25, 40, 50, 55, 60}};

    static const int MOB_CASTLE[][3] = {{-50, 30, 50},
                                        {-1,  10, 10},
                                        {0,   0,  0}};

    static const int MOB_PAWNS[] = {-1, 2, 3, 4, 5, 10, 12, 14, 18, 22, 25, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50,
                                    52, 54, 56, 58, 60, 70, 75, 80, 90, 95, 100, 110};

    static const int BONUS_ATTACK_KING[] = {-1, 2, 8, 64, 128, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512,
                                            512, 512};

    static const u64 PAWN_PROTECTED_MASK[2][64] = {{0x200ULL, 0x500ULL, 0xa00ULL, 0x1400ULL, 0x2800ULL, 0x5000ULL, 0xa000ULL, 0x4000ULL, 0x20000ULL, 0x50000ULL, 0xa0000ULL, 0x140000ULL, 0x280000ULL, 0x500000ULL, 0xa00000ULL, 0x400000ULL, 0x2000000ULL, 0x5000000ULL, 0xa000000ULL, 0x14000000ULL, 0x28000000ULL, 0x50000000ULL, 0xa0000000ULL, 0x40000000ULL, 0x200000000ULL, 0x500000000ULL, 0xa00000000ULL, 0x1400000000ULL, 0x2800000000ULL, 0x5000000000ULL, 0xa000000000ULL, 0x4000000000ULL, 0x20000000000ULL, 0x50000000000ULL, 0xa0000000000ULL, 0x140000000000ULL, 0x280000000000ULL, 0x500000000000ULL, 0xa00000000000ULL, 0x400000000000ULL, 0x2000000000000ULL, 0x5000000000000ULL, 0xa000000000000ULL, 0x14000000000000ULL, 0x28000000000000ULL, 0x50000000000000ULL, 0xa0000000000000ULL, 0x40000000000000ULL, 0xFF000000000000ULL, 0xFF000000000000ULL, 0xFF000000000000ULL, 0xFF000000000000ULL, 0xFF000000000000ULL, 0xFF000000000000ULL, 0xFF000000000000ULL, 0xFF000000000000ULL, 0,                     0,                     0,                     0,                     0,                     0,                     0,                     0},
                                                   {0,        0,        0,        0,         0,         0,         0,         0,         0xFF00ULL,  0xFF00ULL,  0xFF00ULL,  0xFF00ULL,   0xFF00ULL,   0xFF00ULL,   0xFF00ULL,   0xFF00ULL,   0x200ULL,     0x500ULL,     0xa00ULL,     0x1400ULL,     0x2800ULL,     0x5000ULL,     0xa000ULL,     0x4000ULL,     0x20000ULL,     0x50000ULL,     0xa0000ULL,     0x140000ULL,     0x280000ULL,     0x500000ULL,     0xa00000ULL,     0x400000ULL,     0x2000000ULL,     0x5000000ULL,     0xa000000ULL,     0x14000000ULL,     0x28000000ULL,     0x50000000ULL,     0xa0000000ULL,     0x40000000ULL,     0x200000000ULL,     0x500000000ULL,     0xa00000000ULL,     0x1400000000ULL,     0x2800000000ULL,     0x5000000000ULL,     0xa000000000ULL,     0x4000000000ULL,     0x20000000000ULL,    0x50000000000ULL,    0xa0000000000ULL,    0x140000000000ULL,   0x280000000000ULL,   0x500000000000ULL,   0xa00000000000ULL,   0x400000000000ULL,   0xFF00000000000000ULL, 0xFF00000000000000ULL, 0xFF00000000000000ULL, 0xFF00000000000000ULL, 0xFF00000000000000ULL, 0xFF00000000000000ULL, 0xFF00000000000000ULL, 0xFF00000000000000ULL}};

    static const u64 PAWN_BACKWARD_MASK[2][64] = {{0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 131584ULL, 328960ULL, 657920ULL, 1315840ULL, 2631680ULL, 5263360ULL, 10526720ULL, 4210688ULL, 33685504ULL, 84213760ULL, 168427520ULL, 336855040ULL, 673710080ULL, 1347420160ULL, 2694840320ULL, 1077936128ULL, 8623489024ULL, 21558722560ULL, 43117445120ULL, 86234890240ULL, 172469780480ULL, 344939560960ULL, 689879121920ULL, 275951648768ULL, 2207613190144ULL, 5519032975360ULL, 11038065950720ULL, 22076131901440ULL, 44152263802880ULL, 88304527605760ULL, 176609055211520ULL, 70643622084608ULL, 565148976676864ULL, 1412872441692160ULL, 2825744883384320ULL, 5651489766768640ULL, 11302979533537280ULL, 22605959067074560ULL, 45211918134149120ULL, 18084767253659648ULL, 562949953421312ULL, 1407374883553280ULL, 2814749767106560ULL, 5629499534213120ULL, 11258999068426240ULL, 22517998136852480ULL, 45035996273704960ULL, 18014398509481984ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0},
                                                  {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 512ULL,    1280ULL,   2560ULL,   5120ULL,    10240ULL,   20480ULL,   40960ULL,    16384ULL,   131584ULL,   328960ULL,   657920ULL,    1315840ULL,   2631680ULL,   5263360ULL,    10526720ULL,   4210688ULL,    33685504ULL,   84213760ULL,    168427520ULL,   336855040ULL,   673710080ULL,    1347420160ULL,   2694840320ULL,   1077936128ULL,   8623489024ULL,    21558722560ULL,   43117445120ULL,    86234890240ULL,    172469780480ULL,   344939560960ULL,   689879121920ULL,    275951648768ULL,   2207613190144ULL,   5519032975360ULL,    11038065950720ULL,   22076131901440ULL,   44152263802880ULL,    88304527605760ULL,    176609055211520ULL,   70643622084608ULL,    565148976676864ULL, 1412872441692160ULL, 2825744883384320ULL, 5651489766768640ULL, 11302979533537280ULL, 22605959067074560ULL, 45211918134149120ULL, 18084767253659648ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0}};

    static const u64 PAWN_PASSED_MASK[2][64] = {{0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x3ULL,               0x7ULL,               0xeULL,               0x1cULL,               0x38ULL,               0x70ULL,               0xe0ULL,               0xc0ULL,               0x303ULL,             0x707ULL,             0xe0eULL,             0x1c1cULL,             0x3838ULL,             0x7070ULL,             0xe0e0ULL,             0xc0c0ULL,             0x30303ULL,           0x70707ULL,           0xe0e0eULL,           0x1c1c1cULL,           0x383838ULL,           0x707070ULL,           0xe0e0e0ULL,           0xc0c0c0ULL,           0x3030303ULL,         0x7070707ULL,         0xe0e0e0eULL,         0x1c1c1c1cULL,         0x38383838ULL,         0x70707070ULL,         0xe0e0e0e0ULL,         0xc0c0c0c0ULL,         0x303030303ULL,       0x707070707ULL,       0xe0e0e0e0eULL,       0x1c1c1c1c1cULL,       0x3838383838ULL,       0x7070707070ULL,       0xe0e0e0e0e0ULL,       0xc0c0c0c0c0ULL,       0x30303030303ULL,     0x70707070707ULL,     0xe0e0e0e0e0eULL,     0x1c1c1c1c1c1cULL,     0x383838383838ULL,     0x707070707070ULL,     0xe0e0e0e0e0e0ULL,     0xc0c0c0c0c0c0ULL,     0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0},
                                                {0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x303030303030000ULL, 0x707070707070000ULL, 0xe0e0e0e0e0e0000ULL, 0x1c1c1c1c1c1c0000ULL, 0x3838383838380000ULL, 0x7070707070700000ULL, 0xe0e0e0e0e0e00000ULL, 0xc0c0c0c0c0c00000ULL, 0x303030303000000ULL, 0x707070707000000ULL, 0xe0e0e0e0e000000ULL, 0x1c1c1c1c1c000000ULL, 0x3838383838000000ULL, 0x7070707070000000ULL, 0xe0e0e0e0e0000000ULL, 0xc0c0c0c0c0000000ULL, 0x303030300000000ULL, 0x707070700000000ULL, 0xe0e0e0e00000000ULL, 0x1c1c1c1c00000000ULL, 0x3838383800000000ULL, 0x7070707000000000ULL, 0xe0e0e0e000000000ULL, 0xc0c0c0c000000000ULL, 0x303030000000000ULL, 0x707070000000000ULL, 0xe0e0e0000000000ULL, 0x1c1c1c0000000000ULL, 0x3838380000000000ULL, 0x7070700000000000ULL, 0xe0e0e00000000000ULL, 0xc0c0c00000000000ULL, 0x303000000000000ULL, 0x707000000000000ULL, 0xe0e000000000000ULL, 0x1c1c000000000000ULL, 0x3838000000000000ULL, 0x7070000000000000ULL, 0xe0e0000000000000ULL, 0xc0c0000000000000ULL, 0x300000000000000ULL, 0x700000000000000ULL, 0xe00000000000000ULL, 0x1c00000000000000ULL, 0x3800000000000000ULL, 0x7000000000000000ULL, 0xe000000000000000ULL, 0xc000000000000000ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0}};

    static const uchar PAWN_PASSED[2][64] = {{200, 200, 200, 200, 200, 200, 200, 200, 100, 100, 100, 100, 100, 100, 100, 100, 40, 40, 40, 40, 40, 40, 40, 40, 19, 19, 19, 21, 21, 19, 19, 19, 13, 13, 13, 25, 25, 13, 13, 13, 0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
                                             {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  0,  13, 13, 13, 25, 25, 13, 13, 13, 19, 19, 19, 21, 21, 19, 19, 19, 40, 40, 40, 40, 40, 40, 40, 40, 100, 100, 100, 100, 100, 100, 100, 100, 200, 200, 200, 200, 200, 200, 200, 200}};

    static const u64 PAWN_ISOLATED_MASK[64] = {0x202020202020202ULL, 0x505050505050505ULL, 0xA0A0A0A0A0A0A0AULL,
                                               0x1414141414141414ULL, 0x2828282828282828ULL, 0x5050505050505050ULL,
                                               0xA0A0A0A0A0A0A0A0ULL, 0x4040404040404040ULL, 0x202020202020202ULL,
                                               0x505050505050505ULL, 0xA0A0A0A0A0A0A0AULL, 0x1414141414141414ULL,
                                               0x2828282828282828ULL, 0x5050505050505050ULL, 0xA0A0A0A0A0A0A0A0ULL,
                                               0x4040404040404040ULL, 0x202020202020202ULL, 0x505050505050505ULL,
                                               0xA0A0A0A0A0A0A0AULL, 0x1414141414141414ULL, 0x2828282828282828ULL,
                                               0x5050505050505050ULL, 0xA0A0A0A0A0A0A0A0ULL, 0x4040404040404040ULL,
                                               0x202020202020202ULL, 0x505050505050505ULL, 0xA0A0A0A0A0A0A0AULL,
                                               0x1414141414141414ULL, 0x2828282828282828ULL, 0x5050505050505050ULL,
                                               0xA0A0A0A0A0A0A0A0ULL, 0x4040404040404040ULL, 0x202020202020202ULL,
                                               0x505050505050505ULL, 0xA0A0A0A0A0A0A0AULL, 0x1414141414141414ULL,
                                               0x2828282828282828ULL, 0x5050505050505050ULL, 0xA0A0A0A0A0A0A0A0ULL,
                                               0x4040404040404040ULL, 0x202020202020202ULL, 0x505050505050505ULL,
                                               0xA0A0A0A0A0A0A0AULL, 0x1414141414141414ULL, 0x2828282828282828ULL,
                                               0x5050505050505050ULL, 0xA0A0A0A0A0A0A0A0ULL,
                                               0x4040404040404040ULL, 0x202020202020202ULL, 0x505050505050505ULL,
                                               0xA0A0A0A0A0A0A0AULL, 0x1414141414141414ULL, 0x2828282828282828ULL,
                                               0x5050505050505050ULL, 0xA0A0A0A0A0A0A0A0ULL, 0x4040404040404040ULL,
                                               0x202020202020202ULL, 0x505050505050505ULL, 0xA0A0A0A0A0A0A0AULL,
                                               0x1414141414141414ULL, 0x2828282828282828ULL, 0x5050505050505050ULL,
                                               0xA0A0A0A0A0A0A0A0ULL, 0x4040404040404040ULL};

    static const char DISTANCE_KING_OPENING[64] = {-8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8,
                                                   -8, -12, -12, -12, -12, -8, -8, -8, -8, -12, -16, -16, -12, -8, -8,
                                                   -8, -8, -12, -16, -16, -12, -8, -8, -8, -8, -12, -12, -12, -12, -8,
                                                   -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8, -8};

    static const char DISTANCE_KING_ENDING[64] = {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
                                                  12, 16, 16, 16, 16, 12, 12, 12, 12, 16, 20, 20, 16, 12, 12, 12, 12,
                                                  16, 20, 20, 16, 12, 12, 12, 12, 16, 16, 16, 16, 12, 12, 12, 12, 12,
                                                  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12};

}
