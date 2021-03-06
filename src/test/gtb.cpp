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


#if defined(FULL_TEST)

#include <gtest/gtest.h>
#include <set>
#include "../SearchManager.h"
#include "../IterativeDeeping.h"
#include "../db/gaviota/GTB.h"

/*****************************
 * memory leak on gaviota lib
 ****************************/
TEST(gtb, wdl) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    GTB &tablebase = GTB::getInstance();
    if (!tablebase.setPath("/gtb4")) {
        FAIL() << "path error";
    }
    searchManager.loadFen("3r1k2/8/8/1Q6/8/8/8/2K5 w - - 0 1");
    EXPECT_EQ(4, searchManager.printDtmGtb(false)); //win
}

TEST(gtb, dtm) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    GTB &tablebase = GTB::getInstance();
    if (!tablebase.setPath("/gtb4")) {
        FAIL() << "path error";
    }
    searchManager.loadFen("3r1k2/8/8/1Q6/8/8/8/2K5 w - - 0 1");
    EXPECT_EQ(4, searchManager.printDtmGtb(true)); //win

}



#endif
