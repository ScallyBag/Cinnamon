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
#include "../namespaces/see.h"

TEST(see1, test1) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    searchManager.loadFen("rnbqk1nr/pppp1ppp/8/4p3/1bP5/3P4/PP2PPPP/RNBQKBNR w KQkq - 0 1");
    _Tmove move;
    move.s.side = WHITE;
    move.s.from = B1;
    move.s.pieceFrom = KNIGHT_WHITE;
    move.s.to = A3;
    move.s.capturedPiece = SQUARE_EMPTY;
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const auto p = GenMoves::see1(move, searchManager.getChessboard(), allpieces);
    EXPECT_TRUE(p);
}

TEST(see1, test2) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = C3;
    move.s.pieceFrom = ROOK_BLACK;
    move.s.to = C6;
    move.s.capturedPiece = PAWN_WHITE;
    searchManager.loadFen("k7/8/1RP5/1P1P4/8/2r5/8/6K1 b - - 0 9");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const auto p = GenMoves::see1(move, searchManager.getChessboard(), allpieces);
    EXPECT_TRUE(p);
}

TEST(see1, test3) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = F6;
    move.s.pieceFrom = KING_BLACK;
    move.s.to = G5;
    move.s.capturedPiece = KNIGHT_WHITE;
    searchManager.loadFen("8/8/5k2/6N1/8/5N2/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const auto p = GenMoves::see1(move, searchManager.getChessboard(), allpieces);
    EXPECT_TRUE(p);
}

TEST(see1, test4) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = F6;
    move.s.pieceFrom = PAWN_BLACK;
    move.s.to = G5;
    move.s.capturedPiece = KNIGHT_WHITE;
    searchManager.loadFen("8/8/5p2/6N1/8/k4n2/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const auto p = GenMoves::see1(move, searchManager.getChessboard(), allpieces);
    EXPECT_FALSE(p);
}

TEST(see1, test5) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = C8;
    move.s.pieceFrom = BISHOP_BLACK;
    move.s.to = G4;
    move.s.capturedPiece = PAWN_BLACK;
    searchManager.loadFen("rnbqkb1r/ppp1pppp/8/3p4/4P1P1/6P1/PPPP3P/RNBQKBNR w KQkq - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const auto p = GenMoves::see1(move, searchManager.getChessboard(), allpieces);
    EXPECT_TRUE(p);
}

TEST(see1, test6) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = F3;
    move.s.pieceFrom = KING_BLACK;
    move.s.to = G5;
    move.s.capturedPiece = KNIGHT_WHITE;
    searchManager.loadFen("8/8/5p1Q/2R3N1/7B/k4n2/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const auto p = GenMoves::see1(move, searchManager.getChessboard(), allpieces);
    EXPECT_TRUE(p);
}

TEST(see1, test7) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = F6;
    move.s.pieceFrom = PAWN_BLACK;
    move.s.to = G5;
    move.s.capturedPiece = KNIGHT_WHITE;
    searchManager.loadFen("8/8/5p1Q/2R3N1/7B/k4n2/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const auto p = GenMoves::see1(move, searchManager.getChessboard(), allpieces);
    EXPECT_TRUE(p);
}

TEST(see1, test8) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = F6;
    move.s.pieceFrom = PAWN_BLACK;
    move.s.to = G5;
    move.s.capturedPiece = KNIGHT_WHITE;
    searchManager.loadFen("8/8/5p2/6N1/5b1P/k7/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const auto p = GenMoves::see1(move, searchManager.getChessboard(), allpieces);
    EXPECT_TRUE(p);
}

TEST(see1, test9) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = WHITE;
    move.s.from = E5;
    move.s.pieceFrom = PAWN_WHITE;
    move.s.to = E6;
    move.s.capturedPiece = SQUARE_EMPTY;
    searchManager.loadFen("8/3p4/8/4P3/8/k7/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const auto p = GenMoves::see1(move, searchManager.getChessboard(), allpieces);
    EXPECT_TRUE(p);
}

TEST(see, test1) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    searchManager.loadFen("rnbqk1nr/pppp1ppp/8/4p3/1bP5/3P4/PP2PPPP/RNBQKBNR w KQkq - 0 1");
    _Tmove move;
    move.s.side = WHITE;
    move.s.from = B1;
    move.s.pieceFrom = KNIGHT_WHITE;
    move.s.to = A3;
    move.s.capturedPiece = SQUARE_EMPTY;
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const int p = See::see(move, searchManager.getChessboard(), allpieces);
    EXPECT_EQ(5, p);
}

TEST(see, test2) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = C3;
    move.s.pieceFrom = ROOK_BLACK;
    move.s.to = C6;
    move.s.capturedPiece = PAWN_WHITE;
    searchManager.loadFen("k7/8/1RP5/1P1P4/8/2r5/8/6K1 b - - 0 9");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const int p = See::see(move, searchManager.getChessboard(), allpieces);
    EXPECT_EQ(-420, p);
}

TEST(see, test3) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = F6;
    move.s.pieceFrom = KING_BLACK;
    move.s.to = G5;
    move.s.capturedPiece = KNIGHT_WHITE;
    searchManager.loadFen("8/8/5k2/6N1/8/5N2/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const int p = See::see(move, searchManager.getChessboard(), allpieces);
    EXPECT_EQ(-31670, p);
}

TEST(see, test4) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = F6;
    move.s.pieceFrom = PAWN_BLACK;
    move.s.to = G5;
    move.s.capturedPiece = KNIGHT_WHITE;
    searchManager.loadFen("8/8/5p2/6N1/8/k4n2/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const int p = See::see(move, searchManager.getChessboard(), allpieces);
    EXPECT_EQ(330, p);
}

TEST(see, test5) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = C8;
    move.s.pieceFrom = BISHOP_BLACK;
    move.s.to = G4;
    move.s.capturedPiece = PAWN_BLACK;
    searchManager.loadFen("rnbqkb1r/ppp1pppp/8/3p4/4P1P1/6P1/PPPP3P/RNBQKBNR w KQkq - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const int p = See::see(move, searchManager.getChessboard(), allpieces);
    EXPECT_EQ(-235, p);
}

TEST(see, test6) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = F3;
    move.s.pieceFrom = KING_BLACK;
    move.s.to = G5;
    move.s.capturedPiece = KNIGHT_WHITE;
    searchManager.loadFen("8/8/5p1Q/2R3N1/7B/k4n2/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const int p = See::see(move, searchManager.getChessboard(), allpieces);
    EXPECT_EQ(235, p);
}

TEST(see, test7) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = F6;
    move.s.pieceFrom = PAWN_BLACK;
    move.s.to = G5;
    move.s.capturedPiece = KNIGHT_WHITE;
    searchManager.loadFen("8/8/5p1Q/2R3N1/7B/k4n2/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const int p = See::see(move, searchManager.getChessboard(), allpieces);
    EXPECT_EQ(235, p);
}

TEST(see, test8) {
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
    _Tmove move;
    move.s.side = BLACK;
    move.s.from = F6;
    move.s.pieceFrom = PAWN_BLACK;
    move.s.to = G5;
    move.s.capturedPiece = KNIGHT_WHITE;
    searchManager.loadFen("8/8/5p2/6N1/5b1P/k7/8/K7 w - - 0 1");
    const u64 allpieces = board::getBitmap<WHITE>(searchManager.getChessboard()) |
                          board::getBitmap<BLACK>(searchManager.getChessboard());
    const int p = See::see(move, searchManager.getChessboard(), allpieces);
    EXPECT_EQ(330, p);
}


#endif
