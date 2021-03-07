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
#include <limits.h>

using namespace constants;
using namespace _logger;

class Hash : public Singleton<Hash> { //TODO cambiare nome
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
    unsigned nRecordHashA, nRecordHashB, nRecordHashE, collisions, readCollisions, n_cut_hashA, n_cut_hashB, n_cut_hashE, readHashCount;
#endif

    void setHashSize(const int mb);

    void clearHash();

    inline int readHash(
            const int alpha,
            const int beta,
            const int depth,
            const u64 zobristKeyR, Hash::_ThashData &checkHashStruct, const bool currentPly) {
        INC(readHashCount);
        const Hash::_Thash *hash = &(hashArray[zobristKeyR % HASH_SIZE]);
        DEBUG(u64 d = 0)
        checkHashStruct.dataU = 0;
        bool found = false;
        for (int i = 0; i < BUCKETS; i++, hash++) {
            if (found)break;
            const u64 data = hash->u.dataU;
            DEBUG(d |= data)
            const u64 k = hash->key;
            if (zobristKeyR == (k ^ data)) {
                found = true;
                checkHashStruct.dataU = data;
                if (checkHashStruct.dataS.depth >= depth) {
                    if (currentPly) { //TODO
                        switch (checkHashStruct.dataS.flags) {
                            case Hash::hashfEXACT:  //TODO
                            case Hash::hashfBETA:
                                if (checkHashStruct.dataS.score >= beta) {
                                    INC(n_cut_hashB);
                                    return beta;
                                }
                                break;
                            case Hash::hashfALPHA:
                                if (checkHashStruct.dataS.score <= alpha) {
                                    INC(n_cut_hashA);
                                    return alpha;
                                }
                                break;
                            default:
                                fatal("Error checkHash")
                                exit(1);
                        }
                    }
                }
            }
        }
        DEBUG(if (d && !found)readCollisions++)
        return INT_MAX;
    }

    void recordHash(const u64 zobristKey, const _ThashData &toStore) {
#ifdef DEBUG_MODE
        ASSERT(zobristKey)
        if (toStore.dataS.flags == hashfALPHA) nRecordHashA++;
        else if (toStore.dataS.flags == hashfBETA) nRecordHashB++;
        else nRecordHashE++;
#endif
        const unsigned kMod = zobristKey % HASH_SIZE;

        _Thash *empty = nullptr;

        {
            _Thash *hash = &(hashArray[kMod]);
            bool found = false;
            for (int i = 0; i < BUCKETS; i++, hash++) {
                const u64 data = hash->u.dataU;
                const u64 k = hash->key;
                if (zobristKey == (k ^ data)) {
                    found = true;
                    if (hash->u.dataS.depth <= toStore.dataS.depth) {
                        hash->key = (zobristKey ^ toStore.dataU);
                        hash->u.dataU = toStore.dataU;
                        return;
                    }
                } else if (!hash->key) {
                    empty = hash;
                    if (found)break;
                }
            }
        }
        if (empty) {
            empty->key = (zobristKey ^ toStore.dataU);
            empty->u.dataU = toStore.dataU;
            return;
        }
        {
            _Thash *hash = &(hashArray[kMod]);
            _Thash *old = &(hashArray[kMod]);
            int i;
            for (i = 0; i < BUCKETS; i++, hash++) {
                if (hash->u.dataS.depth <= toStore.dataS.depth && hash->u.dataS.depth < old->u.dataS.depth) old = hash;
            }
            if (i == BUCKETS) hash = old;

            DEBUG(if (hash->key && hash->key != (zobristKey ^ toStore.dataU)) INC(collisions))
            hash->key = (zobristKey ^ toStore.dataU);
            hash->u.dataU = toStore.dataU;
        }
    }

private:
    Hash();

    static constexpr int BUCKETS = 4; //TODO provare 3
    unsigned HASH_SIZE;
#ifdef JS_MODE
    static constexpr int HASH_SIZE_DEFAULT = 1;
#else
    static constexpr int HASH_SIZE_DEFAULT = 64;
#endif

    void dispose();

    _Thash *hashArray;
};

