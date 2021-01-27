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
#include <string>
#include <algorithm>
#include <sstream>
#include <cstring>

using namespace std;

class String: public string {
public:
    String(const string &s) : string(s) { };

    String(const char *s) : string(s) { };

    String() = default;

    String &trim();

    String &trimLeft();

    String &trimRight();

    String &replace(const char c1, const char c2);

    String &replace(const string &s1, const string &s2);

    String &toUpper();

    String &toLower();

    template<class T>
    String(T d, const string tohex = "") {
        stringstream ss;
        if (tohex == "int64tohex") {
            stringstream ss2;
            ss2 << std::hex << d;
            ss << "0x";
            for (unsigned i = 0; i < 16 - ss2.str().length(); i++)
                ss << "0";
            ss << std::hex << d << "ULL";
        }
        else if (tohex == "int32tohex") {
            stringstream ss2;
            ss2 << std::hex << d;
            ss << "0x";
            for (unsigned i = 0; i < 8 - ss2.str().length(); i++)
                ss << "0";
            ss << std::hex << d;
        }
        else {
            ss << d;
        }
        assign(ss.str());
    }

};
