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


#include "RemoteNode.h"

void RemoteNode::run() {
    string a = Message::serialize(m);
    c.sendMsg(host, port, a);
    sleep
}

void RemoteNode::endRun() {
    cout << "fine " << host << endl;
}

void RemoteNode::setParam(const int port1, const string fen, const int depth, const int from, const int to, const tuple<string, int, int, string> node) {
    host = get<0>(node);
    port = port1;
    int Ncpu = get<1>(node);
    int hashsize = get<2>(node);
    string dumpFile = get<3>(node);

    m.fen = fen;
    m.depth = depth;
    m.dumpFile = dumpFile;
    m.hashsize = hashsize;
    m.from = from;
    m.to = to;

}