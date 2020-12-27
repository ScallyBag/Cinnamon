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

using namespace constants;

class See { // TODO
public:
    static int see(const _Tmove &move, const _Tchessboard &chessboard, const u64 allpieces) {
        const int side = move.s.side;
        const int position = move.s.to;
        const int capturedPiece = move.s.capturedPiece;
        ASSERT(allpieces == (board::getBitmap<WHITE>(chessboard) | board::getBitmap<BLACK>(chessboard)));
        int yourCap[15];
        int yourCount = _see(side, position, allpieces, yourCap, PIECES_VALUE[move.s.pieceFrom], chessboard);
        const int pieceValue = PIECES_VALUE[capturedPiece];
        if (yourCount == 0) return pieceValue;
        int myCap[15];
        int myCount = _see(side ^ 1, position, allpieces, myCap, PIECES_VALUE[move.s.pieceFrom], chessboard);

        if (!myCount && yourCount) {
            return -PIECES_VALUE[move.s.pieceFrom];
        }

        if (myCount > yourCount)myCount = yourCount + 1;
        else yourCount = myCount;

        if (yourCount == myCount) {
            int score = 0;
            for (int j = 0; j < myCount; j++) score -= myCap[j];
            for (int j = 0; j < yourCount - 1; j++) score += yourCap[j];
            return score + pieceValue;
        }

        if (yourCount < myCount) {
            int score = 0;
            for (int j = 0; j < yourCount; j++) score += yourCap[j];
            for (int j = 0; j < myCount - 1; j++) score -= myCap[j];
            return score + pieceValue;
        }
        return pieceValue;
    }

private:
    static int
    _see(const int side, const int position, const u64 allpieces, int attackersValue[], const int firstValueFrom,
         const _Tchessboard &chessboard) {
        ASSERT_RANGE(position, 0, 63);
        ASSERT_RANGE(side, 0, 1);
        int count = 0;
        const int xside = side ^1;
        ///pawn
        u64 attackers = PAWN_FORK_MASK[side][position] & chessboard[PAWN_BLACK + xside];
        for (; attackers; RESET_LSB(attackers)) {
            attackersValue[count++] = (VALUEPAWN);
        }

        ///knight
        attackers = KNIGHT_MASK[position] & chessboard[KNIGHT_BLACK + xside];
        for (; attackers; RESET_LSB(attackers)) {
            attackersValue[count++] = (VALUEKNIGHT);
        }

        if (RANK_FILE_DIAG_ANTIDIAG[position] &
            (chessboard[BISHOP_BLACK + xside] | chessboard[ROOK_BLACK + xside] |
             chessboard[QUEEN_BLACK + xside])) {
            ///bishop
            auto diagAnt = Bitboard::getDiagonalAntiDiagonal(position, allpieces);
            u64 b = diagAnt & chessboard[BISHOP_BLACK + xside];
            for (; b; RESET_LSB(b)) {
                attackersValue[count++] = (VALUEBISHOP);
            }

            ///rook
            auto rankFile = Bitboard::getRankFile(position, allpieces);
            b = rankFile & chessboard[ROOK_BLACK + xside];
            for (; b; RESET_LSB(b)) {
                attackersValue[count++] = (VALUEROOK);
            }

            ///queen
            b = (diagAnt | rankFile) & chessboard[QUEEN_BLACK + xside];
            for (; b; RESET_LSB(b)) {
                attackersValue[count++] = (VALUEQUEEN);
            }
        }
        ///king
        if (NEAR_MASK1[position] & chessboard[KING_BLACK + xside]) {
            attackersValue[count++] = (VALUEKING);
        }
        if (firstValueFrom)
            for (int i = 0; i < count; i++) {
                if (attackersValue[i] == firstValueFrom) {
                    swap(attackersValue[i], attackersValue[0]);
                    break;
                }
            }
        return count;
    }

};

