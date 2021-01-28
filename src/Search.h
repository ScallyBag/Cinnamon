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

#include <unistd.h>
#include "Hash.h"
#include "Eval.h"
#include "namespaces/bits.h"
#include "namespaces/board.h"
#include <climits>
#include "threadPool/Thread.h"

#ifndef JS_MODE

#include "db/gaviota/GTB.h"
#include "db/syzygy/SYZYGY.h"

#endif


class Search : public Eval, public Thread<Search> {

public:

    Search();

    Search(const Search *s) { clone(s); }

    void clone(const Search *);

    virtual ~Search();

    void setRunning(const int);

    void setPonder(const bool);

    void setNullMove(const bool);

    void setMaxTimeMillsec(const int);

    bool setParameter(String& param,const int value);

    int getMaxTimeMillsec() const;

    void startClock();

    int getRunning() const;

    const _TpvLine &getPvLine() const {
        return pvLine;
    }

    void setMainParam(const int depth);

    template<bool searchMoves>
    int search(const int depth, const int alpha, const int beta);

    void run();

    void endRun() {};
#ifndef JS_MODE

    int probeWdl(const int depth, const uchar side, const int N_PIECE);

    int printDtmWdlGtb(const bool dtm);

    void printDtzSyzygy();

    void printWdlSyzygy();

#endif

    void setMainPly(const int);

    STATIC_CONST int NULL_DIVISOR = 7;
    STATIC_CONST int NULL_DEPTH = 3;
    STATIC_CONST int VAL_WINDOW = 50;

    static void setRunningThread(const bool t) {
        runningThread = t;
    }

    bool probeRootTB(_Tmove *);

    int SZtbProbeWDL() const;

    int getValWindow() const {
        return valWindow;
    }

    u64 getZobristKey() const;

#ifdef DEBUG_MODE
    unsigned cumulativeMovesCount;
    unsigned totGen;

#endif

    void unsetSearchMoves();

    void setSearchMoves(const vector<int> &v);

private:

    typedef struct {
        Hash::_ThashData phasheType[2];
    } _TcheckHash;

    Hash &hash = Hash::getInstance();
#ifndef JS_MODE
    SYZYGY *syzygy = &SYZYGY::getInstance();
#endif
    vector<int> searchMovesVector;
    int valWindow = INT_MAX;
    static volatile bool runningThread;
    _TpvLine pvLine;

    bool ponder;

#ifdef BENCH_MODE
    Times *times = &Times::getInstance();
#endif

    template<bool searchMoves>
    void aspirationWindow(const int depth, const int valWindow);

    int checkTime() const;

    int maxTimeMillsec = 5000;
    bool nullSearch;
    static high_resolution_clock::time_point startTime;

    bool checkDraw(u64);

    template<uchar side, bool checkMoves>
    int search(const int depth, int alpha, const int beta, _TpvLine *pline, const int N_PIECE, const int nRootMoves);

    template<bool checkMoves>
    bool checkSearchMoves(const _Tmove *move) const;

    template<uchar side>
    int qsearch(int alpha, const int beta, const uchar promotionPiece, const int depth);

    void updatePv(_TpvLine *pline, const _TpvLine *line, const _Tmove *move);

    int mainDepth;

    inline int checkHash(const int type,
                                            const int alpha,
                                            const int beta,
                                            const int depth,
                                            const u64 zobristKeyR, Hash::_ThashData &checkHashStruct) {

        if ((checkHashStruct.dataU = hash.readHash(type, zobristKeyR))) {
            if (checkHashStruct.dataS.depth >= depth) {
                INC(hash.probeHash);
                if (currentPly) {
                    switch (checkHashStruct.dataS.flags) {
                        case Hash::hashfEXACT:
                        case Hash::hashfBETA:
                            if (checkHashStruct.dataS.score >= beta) {
                                INC(hash.n_cut_hashB);
                                return beta;
                            }
                            break;
                        case Hash::hashfALPHA:
                            if (checkHashStruct.dataS.score <= alpha) {
                                INC(hash.n_cut_hashA);
                                return alpha;
                            }
                            break;
                        default:
                            fatal("error checkHash")
                            break;
                    }
                }
            }
        }
        INC(hash.cutFailed);
        return INT_MAX;
    }

    template<uchar side>
    bool badCapure(const _Tmove &move, const u64 allpieces);
};
