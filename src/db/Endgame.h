/*
    Cinnamon is a UCI chess engine
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

#include "../ChessBoard.h"

#ifdef DEBUG_MODE

#include <unordered_map>

#endif

/*
KXK
KBNK    ok ko 36%
KPK
KRKP    ok
KRKB    ok ko 37%
KRKN    ok
KQKP    ok ko
KQKR    ok
KBBKN   ok
KBPsK
KQKRPs
KRPKR
KRPPKRP
KPsK
KBPKB
KBPPKB
KBPKN
KNPK
KNPKB
KPKP
*/
class Endgame : public Singleton<Endgame> {
    friend class Singleton<Endgame>;

public:

    template<int side>
    int getEndgameValue(const _Tchessboard &chessboard, const int nPieces) {
        ASSERT(nPieces != 999);
        ASSERT_RANGE(side, 0, 1);
        const auto posKingBlack = BITScanForward(chessboard[KING_BLACK]);
        const auto posKingWhite = BITScanForward(chessboard[KING_WHITE]);
        switch (nPieces) {
            case 4 :
                if (chessboard[QUEEN_BLACK]) {
                    if (0 && chessboard[PAWN_WHITE]) {
                        int result = KQKP(WHITE, posKingBlack, posKingWhite, BITScanForward(chessboard[PAWN_WHITE]));
                        return side == BLACK ? result : -result;
                    } else if (0 && chessboard[ROOK_WHITE]) {
                        int result = KRKQ(posKingBlack, posKingWhite);
                        return side == BLACK ? result : -result;
                    }
                } else if (chessboard[QUEEN_WHITE]) {
                    if (0 && chessboard[PAWN_BLACK]) {
                        int result = KQKP(BLACK, posKingWhite, posKingBlack, BITScanForward(chessboard[PAWN_BLACK]));
                        return side == WHITE ? result : -result;
                    } else if (0 && chessboard[ROOK_BLACK]) {
                        int result = KRKQ(posKingWhite, posKingBlack);
                        return side == WHITE ? result : -result;
                    }
                } else if (chessboard[ROOK_BLACK]) {
                    if (chessboard[PAWN_WHITE]) {
                        int result = KRKP<WHITE>(side == BLACK, posKingBlack,
                                                 posKingWhite, BITScanForward(chessboard[ROOK_BLACK]),
                                                 BITScanForward(chessboard[PAWN_WHITE]));
                        return side == BLACK ? result : -result;
                    } else if (0 && chessboard[BISHOP_WHITE]) {
                        int result = KRKB(posKingWhite);
                        return side == BLACK ? result : -result;
                    } else if (0 && chessboard[KNIGHT_WHITE]) {
                        int result = KRKN(posKingWhite, BITScanForward(chessboard[KNIGHT_WHITE]));
                        return side == BLACK ? result : -result;
                    }
                } else if (chessboard[ROOK_WHITE]) {
                    if (chessboard[PAWN_BLACK]) {
                        int result = KRKP<BLACK>(side == WHITE, posKingWhite,
                                                 posKingBlack, BITScanForward(chessboard[ROOK_WHITE]),
                                                 BITScanForward(chessboard[PAWN_BLACK]));
                        return side == WHITE ? result : -result;
                    } else if (0 && chessboard[BISHOP_BLACK]) {
                        int result = KRKB(posKingBlack);
                        return side == WHITE ? result : -result;
                    } else if (0 && chessboard[KNIGHT_BLACK]) {
                        int result = KRKN(posKingBlack, BITScanForward(chessboard[KNIGHT_BLACK]));
                        return side == WHITE ? result : -result;
                    }
                } else if (0 && (chessboard[BISHOP_BLACK] && chessboard[KNIGHT_BLACK])) {
                    int result = KBNK(posKingBlack, posKingWhite);
                    return side == BLACK ? result : -result;
                } else if (0 && chessboard[BISHOP_WHITE] && chessboard[KNIGHT_WHITE]) {
                    int result = KBNK(posKingWhite, posKingBlack);
                    return side == WHITE ? result : -result;
                }
                break;
            case 5:
                if (0 && chessboard[KNIGHT_WHITE] && bitCount(chessboard[BISHOP_BLACK]) == 2) {
                    int result = KBBKN(posKingBlack, posKingWhite,
                                       BITScanForward(chessboard[KNIGHT_WHITE]));
                    return side == BLACK ? result : -result;
                } else if (0 && chessboard[KNIGHT_BLACK] && bitCount(chessboard[BISHOP_WHITE]) == 2) {
                    int result = KBBKN(posKingWhite, posKingBlack,
                                       BITScanForward(chessboard[KNIGHT_BLACK]));
                    return side == WHITE ? result : -result;
                }
                break;
            default:
                break;
        }

        return INT_MAX;
    }

private:
    
    const uchar DistanceBonus[8] = {0, 0, 100, 80, 60, 40, 20, 10};    //TODO stockfish
    const int VALUE_KNOWN_WIN = 15000;    //TODO stockfish
    const int penaltyKRKN[8] = {0, 10, 14, 20, 30, 42, 58, 80};    //TODO stockfish
    const int KBNKMateTable[64] = {    //TODO stockfish
            200, 190, 180, 170, 170, 180, 190, 200,
            190, 180, 170, 160, 160, 170, 180, 190,
            180, 170, 155, 140, 140, 155, 170, 180,
            170, 160, 140, 120, 120, 140, 160, 170,
            170, 160, 140, 120, 120, 140, 160, 170,
            180, 170, 155, 140, 140, 155, 170, 180,
            190, 180, 170, 160, 160, 170, 180, 190,
            200, 190, 180, 170, 170, 180, 190, 200};

    const int MateTable[64] = {    //TODO stockfish
            100, 90, 80, 70, 70, 80, 90, 100, 90, 70, 60, 50, 50, 60, 70, 90, 80, 60, 40, 30, 30, 40, 60, 80, 70, 50,
            30, 20, 20, 30, 50, 70, 70, 50, 30, 20, 20, 30, 50, 70, 80, 60, 40, 30, 30, 40, 60, 80, 90, 70, 60, 50, 50,
            60, 70, 90, 100, 90, 80, 70, 70, 80, 90, 100};

    template<int loserSide>
    int
    KRKP(const int tempo, const int winnerKingPos, const int loserKingPos, const int rookPos, const int pawnPos) {
        // If the stronger side's king is in front of the pawn, it's a win
        if (FILE_AT[winnerKingPos] == FILE_AT[pawnPos]) {
            if (loserSide == BLACK && winnerKingPos < pawnPos) {
                return VALUEROOK - DISTANCE[winnerKingPos][pawnPos];
            }
            if (loserSide == WHITE && winnerKingPos > pawnPos) {
                return VALUEROOK - DISTANCE[winnerKingPos][pawnPos];
            }
        }
        // If the weaker side's king is too far from the pawn and the rook, it's a win
        if (DISTANCE[loserKingPos][pawnPos] - (tempo ^ 1) >= 3 && DISTANCE[loserKingPos][rookPos] >= 3) {
            return VALUEROOK - DISTANCE[winnerKingPos][pawnPos];
        }
        // If the pawn is far advanced and supported by the defending king, the position is drawish
        if (((loserSide == BLACK && RANK_AT[loserKingPos] <= 2) || (loserSide == WHITE && RANK_AT[loserKingPos] >= 5))
            && DISTANCE[loserKingPos][pawnPos] == 1 &&
            ((loserSide == BLACK && RANK_AT[winnerKingPos] >= 3) || (loserSide == WHITE && RANK_AT[winnerKingPos] <= 4))
            && DISTANCE[winnerKingPos][pawnPos] - tempo > 2) {
            return 80 - DISTANCE[winnerKingPos][pawnPos] * 8;
        } else {
            constexpr int DELTA_S = loserSide == WHITE ? -8 : 8;

            const int queeningSq = BITScanForward(FILE_[pawnPos] & RANK_1_8[loserSide ^ 1]);

            return 200 - 8 * (DISTANCE[winnerKingPos][pawnPos + DELTA_S]
                              - DISTANCE[loserKingPos][pawnPos + DELTA_S]
                              - DISTANCE[pawnPos][queeningSq]);
        }
    }

    int KRKQ(const int winnerKingPos, const int loserKingPos) {
        return VALUEQUEEN - VALUEROOK + MateTable[loserKingPos] + DistanceBonus[DISTANCE[winnerKingPos][loserKingPos]];
    }

    int KRKB(const int loserKingPos) {
        return MateTable[loserKingPos];
    }

    int KRKN(const int loserKingPos, const int knightPos) {
        return MateTable[loserKingPos] + penaltyKRKN[DISTANCE[loserKingPos][knightPos]];
    }

    int KQKP(const int loserSide, const int winnerKingPos, const int loserKingPos, const int pawnPos) {
        int result = DistanceBonus[DISTANCE[winnerKingPos][loserKingPos]];
        if ((DISTANCE[loserKingPos][pawnPos] != 1) || (RANK_AT[pawnPos] != (loserSide == WHITE ? 6 : 1))
            /*|| (0x5a5a5a5a5a5a5a5aULL & POW2(pawnPos))*/) {// 0x5a5a5a5a5a5a5a5aULL = FILE B D E F G
            result += VALUEQUEEN - VALUEPAWN;
        }
        return result;
    }

    int KBBKN(const int winnerKingPos, const int loserKingPos, const int knightPos) {
        return VALUEBISHOP + DistanceBonus[DISTANCE[winnerKingPos][loserKingPos]] +
               (DISTANCE[loserKingPos][knightPos]) * 32;
        // Bonus for driving the defending king and knight apart
        // Bonus for restricting the knight's mobility
        //result += Value((8 - popcount<Max15>(pos.attacks_from<KNIGHT>(nsq))) * 8);
    }

    int KBNK(const int winnerKingPos, const int loserKingPos) {
        return VALUE_KNOWN_WIN + DistanceBonus[DISTANCE[winnerKingPos][loserKingPos]] + KBNKMateTable[loserKingPos];
    }

};

