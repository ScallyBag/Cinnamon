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

#include <iostream>
#include <cstring>
#include "namespaces/bits.h"
#include "namespaces/constants.h"
#include "util/Singleton.h"
#include "util/logger.h"

using namespace constants;
using namespace _logger;

class Hash : public Singleton<Hash> {
    friend class Singleton<Hash>;

public:

    typedef union _ThashData {
        u64 dataU;

        struct __dataS {
            short score;
            char depth;
            uchar from;
            uchar to;
            uchar flags;

            __dataS() {};

            __dataS(const short score, const char depth, const uchar from, const uchar to, const uchar flags) :
                    score(score), depth(depth), from(from), to(to), flags(flags) {};
        } dataS;

        _ThashData() {};

        _ThashData(const short score, const char depth, const uchar from, const uchar to,
                   const uchar flags) :
                dataS(score, depth, from, to, flags) {};
    } __Tdata;

    typedef struct {
        u64 key;
        _ThashData u;
    } _Thash;

    enum : char {
        hashfALPHA = 0, hashfEXACT = 1, hashfBETA = 2
    };

#ifdef DEBUG_MODE
    unsigned nRecordHashA, nRecordHashB, nRecordHashE, collisions, readCollisions, hashProbeCount, n_cut_hashA, n_cut_hashB, n_cut_hashE, probeHash;
#endif

    void setHashSize(const int mb);

    void clearHash();

    u64 readHash(const u64 zobristKeyR)
#ifndef DEBUG_MODE
    const
#endif
    {
        INC(hashProbeCount);
        const _Thash *hash = &(hashArray[zobristKeyR % HASH_SIZE]);
        const u64 data = hash->u.dataU;
        const u64 k = hash->key;
        if (zobristKeyR == (k ^ data)) return data;
        DEBUG(if (data) readCollisions++)
        return 0;
    }

    void recordHash(const u64 zobristKey, _ThashData &tmp) {
#ifdef DEBUG_MODE
        ASSERT(zobristKey)
        if (tmp.dataS.flags == hashfALPHA) nRecordHashA++;
        else if (tmp.dataS.flags == hashfBETA)
            nRecordHashB++;
        else nRecordHashE++;
#endif
        _Thash *hash = &(hashArray[zobristKey % HASH_SIZE]);
        DEBUG(if (hash->key && hash->key != zobristKey) collisions++)
        hash->key = (zobristKey ^ tmp.dataU);
        hash->u.dataU = tmp.dataU;
    }

private:
    Hash();

    unsigned HASH_SIZE;
#ifdef JS_MODE
    static constexpr int HASH_SIZE_DEFAULT = 1;
#else
    static constexpr int HASH_SIZE_DEFAULT = 64;
#endif

    void dispose();

    _Thash *hashArray;
};

