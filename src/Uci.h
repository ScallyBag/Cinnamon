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

#include "IterativeDeeping.h"
#include "perft/Perft.h"

#include <string.h>
#include "util/String.h"

class Uci : public Singleton<Uci> {
    friend class Singleton<Uci>;

public:
    Uci(const string &fen, int perftDepth, int nCpu, int perftHashSize, const string &dumpFile);

    virtual ~Uci();

private:
    Uci();

    Perft *perft = nullptr;

    SearchManager &searchManager = Singleton<SearchManager>::getInstance();

    bool uciMode;
    Tablebase *tablebase = nullptr;

    void listner(IterativeDeeping *it);

    void getToken(istringstream &uip, String &token);

    void startListner();

    bool runPerft = false;

};

