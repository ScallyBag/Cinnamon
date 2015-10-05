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

#include "../../Search.h"
#include <iomanip>
#include <atomic>
#include <fstream>
#include <unistd.h>

#include <mutex>

#include "../../blockingThreadPool/ThreadPool.h"

#include <set>

#include "../remote/RemoteNode.h"
#include "../../util/IniFile.h"


class PerftDistributed : public Thread, public ThreadPool<RemoteNode>, public Singleton<PerftDistributed> {
    friend class Singleton<PerftDistributed>;

public:


    void setParam(string fen1, int depth1, string distributedFile, int port);

    ~PerftDistributed();

    virtual void run();

    virtual void endRun();


private:
    std::vector<tuple<string, int, int, string>> nodesSet;

    PerftDistributed() : ThreadPool(1) { };

    std::vector<tuple<string, int, int, string>> getRemoteNodes(string distributedFile);

    int depth;
    string fen;
    int port;
};
