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

    typedef struct _Thash {
        u64 key;
        // 123456789ABCDEF|12345678|12345678|12345678|12345678|123456789ABCDEF|
        // age            | flags  | from   |   to   | depth  |    score      |
        u64 data;

        _Thash(const short score, const char depth, const uchar from, const uchar to, const uchar flags) {
            data = score;
            data &= 0xffffULL;
            data |= (u64) depth << 16;
            data |= (u64) to << (16 + 8);
            data |= (u64) from << (16 + 8 + 8);
            data |= (u64) flags << (16 + 8 + 8 + 8);
        }
    } _Thash;

    enum : char {
        hashfALPHA = 0, hashfEXACT = 1, hashfBETA = 2
    };

#ifdef DEBUG_MODE
    static unsigned nRecordHashA, nRecordHashB, nRecordHashE, collisions, readCollisions, n_cut_hashA, n_cut_hashB, n_cut_hashE, readHashCount;
#endif

    static void setHashSize(const int mb);

    static void clearHash();

    static void setAge(u64 *u, const short v) {
        (*u = (*u & 0xffffffffffffULL) | (((u64) v) << (16 + 8 + 8 + 8 + 8)));
    }

    static uchar getDepth(const u64 v) { return v >> 16; }

    static uchar getFlags(const u64 v) { return v >> (16 + 8 + 8 + 8); }

    static uchar getFrom(const u64 v) { return v >> (16 + 8 + 8); }

    static uchar getTo(const u64 v) { return v >> (16 + 8); }

    static short getScore(const u64 v) { return v; }

    static unsigned short getAge(const u64 v) { return v >> (16 + 8 + 8 + 8 + 8); }

    static u64 getKey(const u64 key, const u64 data) { return (key ^ (data & 0xffffffffffffULL)); }

    static int readHash(
            const int alpha,
            const int beta,
            const int depth,
            const u64 zobristKeyR, u64 &checkHashStruct, const bool currentPly) {
        DEBUG(u64 d = 0;readHashCount++;)
        const Hash::_Thash *hash = &(hashArray[zobristKeyR % HASH_SIZE]);
        checkHashStruct = 0;
        bool found = false;
        for (int i = 0; i < BUCKETS; i++, hash++) {
            if (found)break;
            u64 data = hash->data;
            u64 k = hash->key;
            DEBUG(d |= data)
            if (zobristKeyR == getKey(k, data)) {
                found = true;
                checkHashStruct = data;
                if (getDepth(checkHashStruct) >= depth) {
                    if (currentPly) {
                        switch (getFlags(checkHashStruct)) {
                            case Hash::hashfEXACT:  //TODO
                            case Hash::hashfBETA:
                                if (getScore(checkHashStruct) >= beta) {
                                    INC(n_cut_hashB);
                                    return beta;
                                }
                                break;
                            case Hash::hashfALPHA:
                                if (getScore(checkHashStruct) <= alpha) {
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

    static void recordHash(const u64 zobristKey, const _Thash &toStore, const int ply) {
#ifdef DEBUG_MODE
        ASSERT(zobristKey)
        if (getFlags(toStore.data) == hashfALPHA) nRecordHashA++;
        else if (getFlags(toStore.data) == hashfBETA) nRecordHashB++;
        else nRecordHashE++;
        ASSERT (getDepth(toStore.data) < MAX_PLY)
#endif
        const unsigned kMod = zobristKey % HASH_SIZE;
        _Thash *empty = nullptr;
        { // update
            _Thash *hash = &(hashArray[kMod]);
            bool found = false;
            for (int i = 0; i < BUCKETS; i++, hash++) {
                u64 data = hash->data;
                u64 k = hash->key;
                if (zobristKey == getKey(k, data)) {
                    found = true;
                    if (getDepth(data) <= getDepth(toStore.data)) {
                        hash->key = (zobristKey ^ toStore.data);
                        hash->data = toStore.data;
                        setAge(&hash->data, ply);
                        return;
                    }
                } else if (!hash->key) {
                    empty = hash;
                    if (found)break;
                }
            }
        }
        if (empty) { //empty slot
            empty->key = (zobristKey ^ toStore.data);
            empty->data = toStore.data;
            setAge(&empty->data, ply);
            return;
        }

        { // age
            _Thash *hash = &(hashArray[kMod]);
            _Thash *old = &(hashArray[kMod]);
            int i;
            int oldTT = -INT_MAX;
            for (i = 0; i < BUCKETS; i++, hash++) {
                const u64 data = hash->data;
                const int age = ((pow(getDepth(data) - getDepth(old->data), 2)) + (ply - getAge(data)));
                if (age > oldTT) {
                    old = hash;
                    oldTT = age;
                }
            }
            if (i == BUCKETS) hash = old;
            DEBUG(if (hash->key && hash->key != (zobristKey ^ toStore.data)) INC(collisions))
            hash->key = (zobristKey ^ toStore.data);
            hash->data = toStore.data;
            setAge(&hash->data, ply);
        }
    }

private:
    Hash();

    static constexpr int BUCKETS = 4;
    static unsigned HASH_SIZE;
#ifdef JS_MODE
    static constexpr int HASH_SIZE_DEFAULT = 1;
#else
    static constexpr int HASH_SIZE_DEFAULT = 64;
#endif

    static void dispose();

    static _Thash *hashArray;
};

