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

#include "../../namespaces/def.h"
#include "../../network/Server.h"
#include "../../namespaces/def.h"

using namespace _debug;
using namespace _def;

class Message {
private:
    const static char SEPARATOR = 1;
    string host;
    string fen;
    string dumpFile;
    int depth;
    int hashsize;
    int Ncpu;
    int from;
    int to;
    unsigned long long partial;
    unsigned long long tot;

public:
    void print();

    Message(const Message &message);

    Message(const string host1, const string fen1, const int depth1, const int hashsize1, const int Ncpu, const string dumpFile1, const int from1, const int to1, const u64 partial1, const u64 tot1);

    Message(string m);

    bool compare(Message &b);

    void setNcpu(int Ncpu) {
        Message::Ncpu = Ncpu;
    }

    const string &getHost() const {
        return host;
    }

    const string &getFen() const {
        return fen;
    }

    const string &getDumpFile() const {
        return dumpFile;
    }

    int getDepth() const {
        return depth;
    }

    int getHashsize() const {
        return hashsize;
    }

    int getFrom() const {
        return from;
    }

    int getTo() const {
        return to;
    }

    unsigned long long int getPartial() const {
        return partial;
    }

    unsigned long long int getTot() const {
        return tot;
    }

    const string getSerializedString() const;

    int getNcpu() const {
        return Ncpu;
    }

    void setHost(const string &host) {
        Message::host = host;
    }

    void setFen(const string &fen) {
        Message::fen = fen;
    }

    void setDumpFile(const string &dumpFile) {
        Message::dumpFile = dumpFile;
    }

    void setDepth(int depth) {
        Message::depth = depth;
    }

    void setHashsize(int hashsize) {
        Message::hashsize = hashsize;
    }

    void setFrom(int from) {
        Message::from = from;
    }

    void setTo(int to) {
        Message::to = to;
    }

    void setPartial(unsigned long long int partial) {
        Message::partial = partial;
    }

    void setTot(unsigned long long int tot) {
        Message::tot = tot;
    }
};

