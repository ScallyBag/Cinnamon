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

#include "unistd.h"
#include "Search.h"
#include "threadPool/ThreadPool.h"
#include <condition_variable>
#include "namespaces/String.h"
#include "util/IniFile.h"
#include <algorithm>
#include <future>
#include "namespaces/bits.h"

class SearchManager : public Singleton<SearchManager> {
    friend class Singleton<SearchManager>;

public:

    bool getRes(_Tmove &resultMove, string &ponderMove, string &pvv);

    ~SearchManager();

    int loadFen(const string &fen = "");

    int getPieceAt(const uchar side, const u64 i);

    u64 getTotMoves();

    void incHistoryHeuristic(const int from, const int to, const int value);

    void startClock();

    string boardToFen();

    string decodeBoardinv(const uchar type, const int a, const uchar side);

    bool setParameter(string param, const int value);

    void clearHeuristic();

    int getForceCheck() const;

    u64 getZobristKey(const int id) const;

    u64 getEnpassant(const int id) const {
        return threadPool->getThread(id).getEnpassant();
    }

    void setForceCheck(const bool a);

    void setRunningThread(const bool r);

    string probeRootTB() const;

    void setRunning(const int i);

    int getRunning(const int i) const;

    void display() const;

    void setMaxTimeMillsec(const int i);

    void unsetSearchMoves();

    void setSearchMoves(const vector<string> &searchmoves);

    void setPonder(bool i);

    int getSide() const;

    int getScore(const uchar side);

    int getMaxTimeMillsec() const;

    void setNullMove(const bool i);

    void setChess960(const bool i);

    bool makemove(const _Tmove *i);

    void takeback(const _Tmove *move, const u64 oldkey, const uchar oldEnpassant, const bool rep);

    void setSide(const bool i);

    int getMoveFromSan(const string &string, _Tmove *ptr) const;

#ifndef JS_MODE

    int printDtmGtb(const bool dtm);

    void printDtmSyzygy();

    void printWdlSyzygy();

#endif

    void pushStackMove();

    void init();

    void setRepetitionMapCount(const int i);

    bool setNthread(const int);

#if defined(FULL_TEST)

    unsigned SZtbProbeWDL() const;

    u64 getBitmap(const int n, const uchar side) const {
        return side ? board::getBitmap<WHITE>(threadPool->getPool()[n]->getChessboard())
                    : board::getBitmap<BLACK>(threadPool->getPool()[n]->getChessboard());
    }

    const _Tchessboard &getChessboard(const int n = 0) const {
        return threadPool->getPool()[n]->getChessboard();
    }

    template<uchar side>
    u64 getPinned(const u64 allpieces, const u64 friends, const int kingPosition) const {
        return board::getPinned<side>(allpieces, friends, kingPosition, threadPool->getPool()[0]->getChessboard());
    }

#endif

#ifdef DEBUG_MODE

    unsigned getCumulativeMovesCount() const {
        unsigned i = 0;
        for (Search *s:threadPool->getPool()) {
            i += s->cumulativeMovesCount;
        }
        return i;
    }

    unsigned getNCutAB() const {
        unsigned i = 0;
        for (Search *s:threadPool->getPool()) {
            i += s->nCutAB;
        }
        return i;
    }

    double getBetaEfficiency() const {
        double b = 0;
        unsigned count = 0;
        for (Search *s:threadPool->getPool()) {
            b += s->betaEfficiency;
            count += s->betaEfficiencyCount;
        }
        return b / count;
    }

    unsigned getLazyEvalCuts() const {
        unsigned i = 0;
        for (Search *s:threadPool->getPool()) {
            i += s->lazyEvalCuts;
        }
        return i;
    }

    unsigned getNCutFp() const {
        unsigned i = 0;
        for (Search *s:threadPool->getPool()) {
            i += s->nCutFp;
        }
        return i;
    }

    unsigned getNCutRazor() const {
        unsigned i = 0;
        for (Search *s:threadPool->getPool()) {
            i += s->nCutRazor;
        }
        return i;
    }

    unsigned getTotBadCaputure() const {
        unsigned i = 0;
        for (Search *s:threadPool->getPool()) {
            i += s->nCutBadCaputure;
        }
        return i;
    }

#endif

    int search(const int mply);

private:

    SearchManager();

    ThreadPool<Search> *threadPool = nullptr;

    _TpvLine lineWin;

    void setMainPly(const int r);

    void startThread(Search &thread, const int depth);

    void stopAllThread();

};

