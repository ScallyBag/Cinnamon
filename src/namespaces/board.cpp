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

#include "board.h"

[[gnu::pure]]u64 board::colors(const int pos) {
    if (POW2(pos) & 0x55aa55aa55aa55aaULL)return 0x55aa55aa55aa55aaULL;
    return 0xaa55aa55aa55aa55ULL;
}


#ifdef DEBUG_MODE

u64 board::getBitmap(const uchar side, const _Tchessboard &chessboard) {
    return side ? getBitmap<WHITE>(chessboard) : getBitmap<BLACK>(chessboard);
}

int board::getPieceAt(uchar side, const u64 bitmapPos, const _Tchessboard &chessboard) {
    return side == WHITE ? getPieceAt<WHITE>(bitmapPos, chessboard) : getPieceAt<BLACK>
            (bitmapPos, chessboard);
}

#endif

u64 board::performRankFileCaptureAndShift(const int position, const u64 enemies, const u64 allpieces) {
    ASSERT_RANGE(position, 0, 63)
    const u64 rankFile = Bitboard::getRankFile(position, allpieces);
    return (rankFile & enemies) | (rankFile & ~allpieces);
}

u64 board::getDiagShiftAndCapture(const int position, const u64 enemies, const u64 allpieces) {
    ASSERT_RANGE(position, 0, 63)
    u64 nuovo = Bitboard::getDiagonalAntiDiagonal(position, allpieces);
    return (nuovo & enemies) | (nuovo & ~allpieces);
}

u64 board::getMobilityRook(const int position, const u64 enemies, const u64 friends) {
    ASSERT_RANGE(position, 0, 63)
    return performRankFileCaptureAndShift(position, enemies, enemies | friends);
}

[[gnu::pure]]bool board::isCastleRight_WhiteKing(const uchar RIGHT_CASTLE) {
    return RIGHT_CASTLE & RIGHT_KING_CASTLE_WHITE_MASK;
}

[[gnu::pure]]bool board::isCastleRight_BlackKing(const uchar RIGHT_CASTLE) {
    return RIGHT_CASTLE & RIGHT_KING_CASTLE_BLACK_MASK;
}

[[gnu::pure]]bool board::isCastleRight_WhiteQueen(const uchar RIGHT_CASTLE) {
    return RIGHT_CASTLE & RIGHT_QUEEN_CASTLE_WHITE_MASK;
}

[[gnu::pure]]bool board::isCastleRight_BlackQueen(const uchar RIGHT_CASTLE) {
    return RIGHT_CASTLE & RIGHT_QUEEN_CASTLE_BLACK_MASK;
}

[[gnu::pure]]int board::getFile(const char cc) {
    return 104 - tolower(cc);
}

bool board::isOccupied(const uchar pos, const u64 allpieces) {
    return allpieces & POW2(pos);
}

bool board::isPieceAt(const uchar pieces, const uchar pos, const _Tchessboard &chessboard) {
    return chessboard[pieces] & POW2(pos);
}
