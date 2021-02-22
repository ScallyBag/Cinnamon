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

#include "SearchManager.h"

using namespace _logger;

SearchManager::SearchManager() {

    threadPool = new ThreadPool<Search>(1);
#if defined(CLOP) || defined(DEBUG_MODE)
    IniFile iniFile("cinnamon.ini");

    while (true) {
        pair<string, string> *parameters = iniFile.get();
        if (!parameters) {
            break;
        }
        string param = parameters->first;
        int value = stoi(parameters->second);
        cout << param << "=" << value << endl;

        if (param == "threads") {
            setNthread(value);
        } else {
            if (!setParameter(param, value)) {
                cout << "error parameter " << param << " not defined\n";
            }
        }
    }
#endif
}

#if defined(FULL_TEST)
unsigned SearchManager::SZtbProbeWDL() const {
    return threadPool->getThread(0).SZtbProbeWDL();
}
#endif

string SearchManager::probeRootTB() const {
    _Tmove bestMove;
    Search &search = threadPool->getThread(0);
    if (search.probeRootTB(&bestMove)) {
        string best = string(search.decodeBoardinv(bestMove.s.type, bestMove.s.from, getSide())) +
                      string(search.decodeBoardinv(bestMove.s.type, bestMove.s.to, getSide()));

        if (bestMove.s.promotionPiece != NO_PROMOTION)
            best += tolower(bestMove.s.promotionPiece);

        return best;
    } else
        return "";
}

int SearchManager::search(const int mply) {

    constexpr int SkipStep[64] =
            {0, 1, 2, 3, 1, 1, 2, 3, 0, 1, 1, 2, 1, 1, 2, 3, 0, 1, 1, 2, 1, 1, 2, 3, 0, 1, 1, 2, 1, 1, 2, 3, 0, 1, 1, 2,
             1,
             1, 2, 3, 0, 1, 1, 2, 1, 1, 2, 3, 0, 1, 1, 2, 1, 1, 2, 3, 0, 1, 1, 2, 1, 1, 2, 3};

    debug("start singleSearch -------------------------------")
    lineWin.cmove = -1;
    setMainPly(mply);
    ASSERT(bitCount(threadPool->getBitCount()) < 2)
    debug("start lazySMP --------------------------")

    for (int ii = 1; ii < threadPool->getNthread(); ii++) {
        Search &helperThread = threadPool->getNextThread();
        if (helperThread.getId() == 0)continue;

        helperThread.setRunning(1);
        startThread(helperThread, mply + SkipStep[ii]);
    }

    debug("end lazySMP ---------------------------")
    Search &mainThread = threadPool->getThread(0);
    mainThread.setMainParam(mply);
    mainThread.run();

    auto res = mainThread.getValWindow();

    if (mainThread.getRunning()) {
        memcpy(&lineWin, &mainThread.getPvLine(), sizeof(_TpvLine));
    }
    stopAllThread();
    threadPool->joinAll();
    debug("end singleSearch -------------------------------")
    return res;
}

bool SearchManager::getRes(_Tmove &resultMove, string &ponderMove, string &pvv) {
    if (lineWin.cmove < 1) {
        return false;
    }

    pvv.clear();
    string pvvTmp;

    ASSERT(lineWin.cmove)
    for (int t = 0; t < lineWin.cmove; t++) {
        pvvTmp.clear();
        pvvTmp +=
                decodeBoardinv(lineWin.argmove[t].s.type, lineWin.argmove[t].s.from,
                               threadPool->getThread(0).sideToMove);
        if (pvvTmp.length() != 4 && pvvTmp[0] != 'O') {
            pvvTmp += decodeBoardinv(lineWin.argmove[t].s.type, lineWin.argmove[t].s.to,
                                     threadPool->getThread(0).sideToMove);
        }
        pvv.append(pvvTmp);
        if (t == 1) {
            ponderMove.assign(pvvTmp);
        }
        pvv.append(" ");
    }
    memcpy(&resultMove, lineWin.argmove, sizeof(_Tmove));

    return true;
}

SearchManager::~SearchManager() {
}

int SearchManager::loadFen(const string &fen) {
    int res = -1;

    for (uchar i = 0; i < threadPool->getPool().size(); i++) {
        res = threadPool->getThread(i).loadFen(fen);
        ASSERT_RANGE(res, 0, 1)
    }
    return res;
}

void SearchManager::startThread(Search &thread, const int depth) {

    debug("startThread: ", thread.getId(), " depth: ", depth, " isrunning: ", getRunning(thread.getId()))

    thread.setMainParam(depth);

    thread.start();
}

void SearchManager::setMainPly(const int r) {
    for (Search *s:threadPool->getPool()) {
        s->setMainPly(r);
    }
}

int SearchManager::getPieceAt(const uchar side, const u64 i) {
    return side == WHITE ? board::getPieceAt<WHITE>(i, threadPool->getThread(0).getChessboard())
                         : board::getPieceAt<BLACK>(i, threadPool->getThread(0).getChessboard());
}

u64 SearchManager::getTotMoves() {
    u64 i = 0;
    for (Search *s:threadPool->getPool()) {
        i += s->getTotMoves();
    }
    return i;
}

void SearchManager::incHistoryHeuristic(const int from, const int to, const int value) {
    for (Search *s:threadPool->getPool()) {
        s->incHistoryHeuristic(from, to, value);
    }
}

void SearchManager::startClock() {
    threadPool->getThread(0).startClock();// static variable
}

string SearchManager::boardToFen() {
    return threadPool->getThread(0).boardToFen();
}

void SearchManager::clearHeuristic() {
    for (Search *s:threadPool->getPool()) {
        s->clearHeuristic();
    }
}

int SearchManager::getForceCheck() const {
    return threadPool->getThread(0).getForceCheck();
}

u64 SearchManager::getZobristKey(const int id) const {
    return threadPool->getThread(id).getZobristKey();
}

void SearchManager::setForceCheck(const bool a) {
    for (Search *s:threadPool->getPool()) {
        s->setForceCheck(a);
    }
}

void SearchManager::setRunningThread(const bool r) {
    Search::setRunningThread(r);
}

void SearchManager::setRunning(const int i) {
    for (Search *s:threadPool->getPool()) {
        s->setRunning(i);
    }
}

int SearchManager::getRunning(const int i) const {
    return threadPool->getThread(i).getRunning();
}

void SearchManager::display() const {
    threadPool->getThread(0).display();
}

void SearchManager::setMaxTimeMillsec(const int i) {
    for (Search *s:threadPool->getPool()) {
        s->setMaxTimeMillsec(i);
    }
}

void SearchManager::unsetSearchMoves() {
    for (Search *s:threadPool->getPool()) {
        s->unsetSearchMoves();
    }
}

void SearchManager::setSearchMoves(const vector<string> &searchMov) {
    _Tmove move;
    vector<int> searchMoves;
    for (auto it = searchMov.begin(); it != searchMov.end(); ++it) {
        getMoveFromSan(*it, &move);
        const int x = move.s.to | (int) (move.s.from << 8);
        searchMoves.push_back(x);
    }
    for (Search *s:threadPool->getPool()) {
        s->setSearchMoves(searchMoves);
    }
}

void SearchManager::setPonder(const bool i) {
    for (Search *s:threadPool->getPool()) {
        s->setPonder(i);
    }
}

int SearchManager::getSide() const {
#ifdef DEBUG_MODE
    int t = threadPool->getThread(0).sideToMove;
    for (Search *s:threadPool->getPool()) {
        ASSERT(s->sideToMove == t)
    }
#endif
    return threadPool->getThread(0).sideToMove;
}

int SearchManager::getScore(const uchar side) {
    return threadPool->getThread(0).getScore(0xffffffffffffffffULL, side, -_INFINITE, _INFINITE, true);
}

int SearchManager::getMaxTimeMillsec() const {
    return threadPool->getThread(0).getMaxTimeMillsec();
}

void SearchManager::setNullMove(const bool i) {
    for (Search *s:threadPool->getPool()) {
        s->setNullMove(i);
    }
}

void SearchManager::setChess960(const bool i) {
    for (Search *s:threadPool->getPool()) {
        s->setChess960(i);
    }
}

bool SearchManager::makemove(const _Tmove *i) {
    bool b = false;
    for (Search *s:threadPool->getPool()) {
        b = s->makemove(i, true, false);
    }
    return b;
}

string SearchManager::decodeBoardinv(const uchar type, const int a, const uchar side) {
    return threadPool->getThread(0).decodeBoardinv(type, a, side);
}

void SearchManager::takeback(const _Tmove *move, const u64 oldkey,const uchar oldEnpassant, const bool rep) {
    for (Search *s:threadPool->getPool()) {
        s->takeback(move, oldkey, oldEnpassant,rep);
    }
}

void SearchManager::setSide(const bool i) {
    for (Search *s:threadPool->getPool()) {
        s->setSide(i);
    }
}

#ifndef JS_MODE

int SearchManager::printDtmGtb(const bool dtm) {
    return threadPool->getThread(0).printDtmWdlGtb(dtm);
}

void SearchManager::printDtmSyzygy() {
    threadPool->getThread(0).printDtzSyzygy();
}

void SearchManager::printWdlSyzygy() {
    threadPool->getThread(0).printWdlSyzygy();
}

#endif

int SearchManager::getMoveFromSan(const string &string, _Tmove *ptr) const {
#ifdef DEBUG_MODE
    int t = threadPool->getThread(0).getMoveFromSan(string, ptr);
    for (Search *s:threadPool->getPool()) {
        ASSERT(s->getMoveFromSan(string, ptr) == t)
    }
#endif
    return threadPool->getThread(0).getMoveFromSan(string, ptr);
}

void SearchManager::pushStackMove() {
    for (Search *s:threadPool->getPool()) {
        s->pushStackMove();
    }
}

void SearchManager::init() {
    for (Search *s:threadPool->getPool()) {
        s->init();
    }
}

void SearchManager::setRepetitionMapCount(const int i) {
    for (Search *s:threadPool->getPool()) {
        s->setRepetitionMapCount(i);
    }
}


bool SearchManager::setNthread(const int nthread) {
    return threadPool->setNthread(nthread);
}

void SearchManager::stopAllThread() {
    Search::setRunningThread(false);
}

bool SearchManager::setParameter(string param, const int value) {
    bool b = false;
    for (Search *s:threadPool->getPool()) {
        b = s->setParameter(param, value);
    }
    return b;
}




