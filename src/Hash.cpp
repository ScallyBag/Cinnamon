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

#include <mutex>
#include "Hash.h"

Hash::Hash() {
    HASH_SIZE = 0;
    hashArray[HASH_GREATER] = hashArray[HASH_ALWAYS] = nullptr;
#ifdef DEBUG_MODE
    n_cut_hashA = n_cut_hashB = cutFailed = probeHash = 0;
    nRecordHashA = nRecordHashB = nRecordHashE = collisions = 0;
#endif
    setHashSize(64);
}

void Hash::clearAge() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hashArray[HASH_GREATER][i].entryAge = 0;
    }
}

void Hash::clearHash() {
    if (!HASH_SIZE) {
        return;
    }
    memset(hashArray[HASH_ALWAYS], 0, sizeof(_Thash) * HASH_SIZE);
    memset(hashArray[HASH_GREATER], 0, sizeof(_Thash) * HASH_SIZE);
}

int Hash::getHashSize() {
    return HASH_SIZE / (1024 * 1000 / (sizeof(_Thash) * 2));
}

void Hash::setHashSize(int mb) {
//    if (mb < 1) {
//        cout << "info string errorzzzz\n";
//        return;
//    }
    dispose();
    if (mb) {
        int tmp = mb * 1024 * 1000 / (sizeof(_Thash) * 2);
        hashArray[HASH_GREATER] = (_Thash *) calloc(tmp, sizeof(_Thash));
        if (!hashArray[HASH_GREATER]) {
            cout << "info string error - no memory\n";
            return;
        }
        hashArray[HASH_ALWAYS] = (_Thash *) calloc(tmp, sizeof(_Thash));
        if (!hashArray[HASH_ALWAYS]) {
            cout << "info string error - no memory\n";
            return;
        }
        HASH_SIZE = tmp;
    }
}

void Hash::dispose() {
    if (hashArray[HASH_GREATER]) {
        free(hashArray[HASH_GREATER]);
    }
    if (hashArray[HASH_ALWAYS]) {
        free(hashArray[HASH_ALWAYS]);
    }
    hashArray[HASH_GREATER] = hashArray[HASH_ALWAYS] = nullptr;
    HASH_SIZE = 0;
}

Hash::~Hash() {
    dispose();
}

void Hash::recordHash(bool running, _Thash *rootHash[2], const char depth, const char flags, const u64 key, const int score, _Tmove *bestMove) {
    ASSERT(key);
    ASSERT(rootHash[HASH_GREATER]);
    ASSERT(rootHash[HASH_ALWAYS]);
    if (!running) {
        return;
    }
    ASSERT(abs(score) <= 32200);
    _Thash tmp;

    tmp.key = key;
    tmp.score = score;
    tmp.flags = flags;
    tmp.depth = depth;
    if (bestMove && bestMove->from != bestMove->to) {
        tmp.from = bestMove->from;
        tmp.to = bestMove->to;
    } else {
        tmp.from = tmp.to = 0;
    }
    {
        lock_guard<mutex> lock(MUTEX_HASH);
        memcpy(rootHash[HASH_GREATER], &tmp, sizeof(_Thash));
    }

#ifdef DEBUG_MODE
    if (flags == hashfALPHA) {
        nRecordHashA++;
    } else if (flags == hashfBETA) {
        nRecordHashB++;
    } else {
        nRecordHashE++;
    }
#endif
    tmp.entryAge = 1;
    {
        lock_guard<mutex> lock(MUTEX_HASH);
        if (rootHash[HASH_ALWAYS]->key && rootHash[HASH_ALWAYS]->depth >= depth && rootHash[HASH_ALWAYS]->entryAge) {
            INC(collisions);
            return;
        }
        memcpy(rootHash[HASH_ALWAYS], &tmp, sizeof(_Thash));
    }
}
