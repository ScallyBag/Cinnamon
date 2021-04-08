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

#include "Eval.h"

using namespace _eval;
u64 *Eval::evalHash;

Eval::Eval() {
    memset(&structureEval, 0, sizeof(_Tboard));
    if (evalHash == nullptr)
        evalHash = (u64 *) calloc(hashSize, sizeof(u64));
}

Eval::~Eval() {
    free(evalHash);
    evalHash = nullptr;
}

/**
 * evaluate pawns for color at phase
 * 1. if no pawns returns -NO_PAWNS
 * 3. if other side has all pawns substracts ENEMIES_ALL_PAWNS
 * 4. add ATTACK_KING * number of attacking pawn to other king
 * 5. space - in OPEN phase PAWN_CENTER * CENTER_MASK
 * 7. *king security* - in OPEN phase add at kingSecurity FRIEND_NEAR_KING * each pawn near to king and substracts ENEMY_NEAR_KING * each enemy pawn near to king
 * 8. pawn in promotion - if pawn is in 7' add PAWN_7H. If pawn can go forward add PAWN_IN_PROMOTION for each pawn
 * 9. unprotected - no friends pawn protect it
 * 10. blocked - pawn can't go on
 * 11. isolated - there aren't friend pawns on the two sides - subtracts PAWN_ISOLATED for each pawn
 * 12. doubled - there aren't friend pawns on the two sides - subtracts DOUBLED_PAWNS for each pawn. If it is isolated too substracts DOUBLED_ISOLATED_PAWNS
 * 13. backward - if there isn't friend pawns on sides or on sides in 1 rank below subtracts BACKWARD_PAWN
 * 14. passed - if there isn't friend pawns forward and forward on sides until 8' rank add PAWN_PASSED[side][pos]

 */
template<uchar side>
pair<int, int> Eval::evaluatePawn(const _Tchessboard &chessboard) {
    INC(evaluationCount[side]);
    int result[2] = {0, 0};
    constexpr int xside = X(side);

    const u64 ped_friends = chessboard[side];

    // 5. space
//    if (phase == OPEN) {
//        result += PAWN_CENTER * bitCount(ped_friends & CENTER_MASK);
//        ADD(SCORE_DEBUG.PAWN_CENTER[side], PAWN_CENTER * bitCount(ped_friends & CENTER_MASK));
//    }

    // 7.

//        if (structureEval.pinned[side] & ped_friends) result -= PAWN_PINNED;
//        ADD(SCORE_DEBUG.PAWN_PINNED[side], -PAWN_PINNED);
    structureEval.kingSecurity[MG][side] +=
            FRIEND_NEAR_KING[MG] * bitCount(NEAR_MASK2[structureEval.posKing[side]] & ped_friends);
    structureEval.kingSecurity[EG][side] +=
            FRIEND_NEAR_KING[EG] * bitCount(NEAR_MASK2[structureEval.posKing[side]] & ped_friends);

//        structureEval.kingSecurity[side] -=
//                ENEMY_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[xside]] & ped_friends);


    // 8.  pawn in 8th

    const u64 pawnsIn7 = PAWNS_7_2[side] & ped_friends;
    result[MG] += PAWN_IN_7TH[MG] * bitCount(pawnsIn7);
    result[EG] += PAWN_IN_7TH[EG] * bitCount(pawnsIn7);
    ADD(SCORE_DEBUG.PAWN_7H[side], PAWN_IN_7TH * bitCount(pawnsIn7));

    const u64 pawnsIn8 = (shiftForward<side, 8>(pawnsIn7) & (~structureEval.allPieces)) |
                         (structureEval.allPiecesSide[xside] &
                          (shiftForward<side, 7>(pawnsIn7) | shiftForward<side, 9>(pawnsIn7)));

    result[MG] += PAWN_IN_PROMOTION[MG] * bitCount(pawnsIn8); //try to decrease PAWN_IN_PROMOTION
    result[EG] += PAWN_IN_PROMOTION[EG] * bitCount(pawnsIn8); //try to decrease PAWN_IN_PROMOTION
    ADD(SCORE_DEBUG.PAWN_IN_PROMOTION[side], PAWN_IN_PROMOTION * (bitCount(pawnsIn8)));


    for (u64 p = ped_friends; p; RESET_LSB(p)) {
        bool isolated = false;
        const int o = BITScanForward(p);
        const u64 pos = POW2(o);

        // 4. attack king
        if (structureEval.posKingBit[xside] & PAWN_FORK_MASK[side][o]) {
            structureEval.kingAttackers[xside] |= pos;
            result[EG] += ATTACK_KING[EG];
            result[MG] += ATTACK_KING[MG];
        }

        /// blocked
        result[EG] -= (!(PAWN_FORK_MASK[side][o] & structureEval.allPiecesSide[xside])) &&
                      (structureEval.allPieces & (shiftForward<side, 8>(pos))) ? PAWN_BLOCKED[EG] : 0;
        result[MG] -= (!(PAWN_FORK_MASK[side][o] & structureEval.allPiecesSide[xside])) &&
                      (structureEval.allPieces & (shiftForward<side, 8>(pos))) ? PAWN_BLOCKED[MG] : 0;
        ADD(SCORE_DEBUG.PAWN_BLOCKED[side], (!(PAWN_FORK_MASK[side][o] & structureEval.allPiecesSide[xside])) &&
                                            (structureEval.allPieces & (shiftForward<side, 8>(pos))) ? -PAWN_BLOCKED
                                                                                                     : 0);
        /// unprotected
        if (!(ped_friends & PAWN_PROTECTED_MASK[side][o])) {
            result[EG] -= UNPROTECTED_PAWNS[EG];
            result[MG] -= UNPROTECTED_PAWNS[MG];
            ADD(SCORE_DEBUG.UNPROTECTED_PAWNS[side], -UNPROTECTED_PAWNS);
        }
        /// isolated
        if (!(ped_friends & PAWN_ISOLATED_MASK[o])) {
//            result -= PAWN_ISOLATED;
//            ADD(SCORE_DEBUG.PAWN_ISOLATED[side], -PAWN_ISOLATED);
            isolated = true;
        }
        /// doubled
        if (NOTPOW2(o) & FILE_[o] & ped_friends) {
//            result -= DOUBLED_PAWNS;
//            ADD(SCORE_DEBUG.DOUBLED_PAWNS[side], -DOUBLED_PAWNS);
            /// doubled and isolated
            if (isolated) {
                ADD(SCORE_DEBUG.DOUBLED_ISOLATED_PAWNS[side], -DOUBLED_ISOLATED_PAWNS);
                result[EG] -= DOUBLED_ISOLATED_PAWNS[EG];
                result[MG] -= DOUBLED_ISOLATED_PAWNS[MG];
            }
        }
        /// backward
        if (!(ped_friends & PAWN_BACKWARD_MASK[side][o])) {
            ADD(SCORE_DEBUG.BACKWARD_PAWN[side], -BACKWARD_PAWN);
            result[EG] -= BACKWARD_PAWN[EG];
            result[MG] -= BACKWARD_PAWN[MG];
        }
        /// passed
        if (!(chessboard[xside] & PAWN_PASSED_MASK[side][o])) {
            ADD(SCORE_DEBUG.PAWN_PASSED[side], PAWN_PASSED[side][o]);
            result[EG] += PAWN_PASSED[side][o];
            result[MG] += PAWN_PASSED[side][o];
        }
    }
    return pair<int, int>(result[MG], result[EG]);
}

/**
 * evaluate bishop for color at phase TODO rivedere doc
 * 1. if no bishops returns 0
 * 2. if two bishops add BONUS2BISHOP
 * 3 *king security* - in OPEN phase substracts at kingSecurity ENEMY_NEAR_KING for each bishop close to enmey king
 * 4. undevelop - substracts UNDEVELOPED_BISHOP for each undeveloped bishop
 * 5. mobility add MOB_BISHOP[phase][???]
 * 6. if only one bishop and pawns on same square color substracts n_pawns * BISHOP_PAWN_ON_SAME_COLOR
 * 7. outposts
 * 8. bishop on big diagonal
 * 9. pinned
 */
template<uchar side>
pair<int, int> Eval::evaluateBishop(const _Tchessboard &chessboard, const u64 enemies) {
    INC(evaluationCount[side]);
    constexpr int xside = X(side);
    u64 bishop = chessboard[BISHOP_BLACK + side];

    // 1.
    if (!bishop) return pair<int, int>(0, 0);

    int result[2] = {0, 0};
    const int nBishop = bitCount(bishop);

    // 2.
    if (nBishop == 1) {
        result[MG] -=
                BISHOP_PAWN_ON_SAME_COLOR[MG] * bitCount(chessboard[side] & board::colors(BITScanForward(bishop)));
        result[EG] -=
                BISHOP_PAWN_ON_SAME_COLOR[EG] * bitCount(chessboard[side] & board::colors(BITScanForward(bishop)));
    } else {
        // 2.
        assert(nBishop > 1);

        result[MG] += BONUS2BISHOP[MG];
        result[EG] += BONUS2BISHOP[EG];
        ADD(SCORE_DEBUG.BONUS2BISHOP[side], BONUS2BISHOP);

    }

    // 3. *king security*

    // 9. pinned
    if (structureEval.pinned[side] & bishop) result[MG] -= BISHOP_PINNED[MG];
    if (structureEval.pinned[side] & bishop) result[EG] -= BISHOP_PINNED[EG];
    ADD(SCORE_DEBUG.BISHOP_PINNED[side], -BISHOP_PINNED);
//        structureEval.kingSecurity[side] -=
//                ENEMY_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[xside]] & bishop);
//        ADD(SCORE_DEBUG.KING_SECURITY_BISHOP[side],
//            -ENEMY_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[xside]] & bishop));


    // 4. undevelop
//    if (phase != END) {
//        result -= UNDEVELOPED_BISHOP * bitCount(BISHOP_HOME[side] & bishop);
//        ADD(SCORE_DEBUG.UNDEVELOPED_BISHOP[side], UNDEVELOPED_BISHOP * bitCount(BISHOP_HOME[side] & bishop));
//    }

    for (; bishop; RESET_LSB(bishop)) {
        const int o = BITScanForward(bishop);
        // 5. mobility

        const u64 x = Bitboard::getDiagonalAntiDiagonal(o, structureEval.allPieces);
        const u64 captured = x & enemies;
        assert(bitCount(captured) + bitCount(x & ~structureEval.allPieces) < (int) (sizeof(MOB_BISHOP) / sizeof(int)));
        if (captured & structureEval.posKingBit[xside]) structureEval.kingAttackers[xside] |= POW2(o);

        result[MG] += MOB_BISHOP[MG][bitCount(captured) + bitCount(x & ~structureEval.allPieces)];;
        result[EG] += MOB_BISHOP[EG][bitCount(captured) + bitCount(x & ~structureEval.allPieces)];;

        ADD(SCORE_DEBUG.MOB_BISHOP[side],
            MOB_BISHOP[phase][bitCount(captured) + bitCount(x & ~structureEval.allPieces)]);

        // 6.
//        if (phase != OPEN) {
//            if ((BIG_DIAGONAL & structureEval.allPieces) == POW2(o)) {
//                ADD(SCORE_DEBUG.OPEN_DIAG_BISHOP[side], OPEN_FILE);
//                result += OPEN_FILE;
//            }
//            if ((BIG_ANTIDIAGONAL & structureEval.allPieces) == POW2(o)) {
//                ADD(SCORE_DEBUG.OPEN_DIAG_BISHOP[side], OPEN_FILE);
//                result += OPEN_FILE;
//            }
//        }

        // 7. outposts
        const auto p = BISHOP_OUTPOST[side][o];

        //enemy pawn doesn't attack bishop
        if (p && !(PAWN_FORK_MASK[xside][o] & chessboard[xside])) {
            //friend paws defends bishop
            if (PAWN_FORK_MASK[X(side)][o] & chessboard[side]) {
                result[MG] += p;
                result[EG] += p;
                if (!(chessboard[KNIGHT_BLACK + xside]) && !(chessboard[BISHOP_BLACK + xside] & board::colors(o))) {
                    result[MG] += p;
                    result[EG] += p;
                }
            }
        }
    }
    return pair<int, int>(result[MG], result[EG]);
}

/**
 * evaluate queen for color at phase
 * 2. *king security* - in OPEN phase add at kingSecurity FRIEND_NEAR_KING for each queen near to king and substracts ENEMY_NEAR_KING for each queen near to enemy king
 * 3. mobility - MOB_QUEEN[phase][position]
 * 4. half open file - if there is a enemy pawn on same file add HALF_OPEN_FILE_Q
 * 5. open file - if there is any pieces on same file add OPEN_FILE_Q
 * 6. 5. bishop on queen - if there is a bishop on same diagonal add BISHOP_ON_QUEEN
 */
template<uchar side>
pair<int, int> Eval::evaluateQueen(const _Tchessboard &chessboard, const u64 enemies) {
    INC(evaluationCount[side]);
    u64 queen = chessboard[QUEEN_BLACK + side];
    int result[2] = {0, 0};
    constexpr int xside = X(side);
    // 2. *king security*

    if (structureEval.pinned[side] & queen) result[MG] -= QUEEN_PINNED[MG];
    if (structureEval.pinned[side] & queen) result[EG] -= QUEEN_PINNED[EG];
    ADD(SCORE_DEBUG.QUEEN_PINNED[side], -QUEEN_PINNED);
    structureEval.kingSecurity[MG][side] +=
            FRIEND_NEAR_KING[MG] * bitCount(NEAR_MASK2[structureEval.posKing[side]] & queen);
    structureEval.kingSecurity[EG][side] +=
            FRIEND_NEAR_KING[EG] * bitCount(NEAR_MASK2[structureEval.posKing[side]] & queen);
    ADD(SCORE_DEBUG.KING_SECURITY_QUEEN[side],
        FRIEND_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[side]] & queen));

//        structureEval.kingSecurity[side] -=
//                ENEMY_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[xside]] & queen);
//        ADD(SCORE_DEBUG.KING_SECURITY_QUEEN[xside],
//            -ENEMY_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[xside]] & queen));


    for (; queen; RESET_LSB(queen)) {
        const int o = BITScanForward(queen);
        // 3. mobility
        const u64 x = board::performRankFileCaptureAndShift(o, enemies, structureEval.allPieces) |
                      board::getDiagShiftAndCapture(o, enemies, structureEval.allPieces);

        result[MG] += MOB_QUEEN[MG][bitCount(x)];
        result[EG] += MOB_QUEEN[EG][bitCount(x)];
        ADD(SCORE_DEBUG.MOB_QUEEN[side], MOB_QUEEN[phase][bitCount(x)]);

        if (x & structureEval.posKingBit[xside]) structureEval.kingAttackers[xside] |= POW2(o);
        // 4. half open file
//        if ((chessboard[xside] & FILE_[o])) {
//            ADD(SCORE_DEBUG.HALF_OPEN_FILE_Q[side], HALF_OPEN_FILE_Q);
//            result += HALF_OPEN_FILE_Q;
//        }

        // 5. open file
        if ((FILE_[o] & structureEval.allPieces) == POW2(o)) {
            ADD(SCORE_DEBUG.OPEN_FILE_Q[side], OPEN_FILE_Q);
            result[MG] += OPEN_FILE_Q[MG];
            result[EG] += OPEN_FILE_Q[EG];
        }

        // 6. bishop on queen
        if (DIAGONAL_ANTIDIAGONAL[o] & chessboard[BISHOP_BLACK + side]) {
            ADD(SCORE_DEBUG.BISHOP_ON_QUEEN[side], BISHOP_ON_QUEEN);
            result[MG] += BISHOP_ON_QUEEN[MG];
            result[EG] += BISHOP_ON_QUEEN[EG];
        }
    }
    return pair<int, int>(result[MG], result[EG]);
}

/**
 * evaluate knight for color at phase
 * 1. // pinned
 * 2. undevelop - substracts UNDEVELOPED_KNIGHT for each undeveloped knight
 * 4. *king security* - in OPEN phase add at kingSecurity FRIEND_NEAR_KING for each knight near to king and substracts ENEMY_NEAR_KING for each knight near to enemy king
 * 5. mobility
 * 6. outposts
*/

template<uchar side>
pair<int, int> Eval::evaluateKnight(const _Tchessboard &chessboard, const u64 notMyBits) {
    INC(evaluationCount[side]);
    u64 knight = chessboard[KNIGHT_BLACK + side];
    if (!knight) return pair<int, int>(0, 0);
    constexpr int xside = X(side);
    // 1. pinned
    int result[2] = {0, 0};

    // 2. undevelop
//    if (phase == OPEN) {
//        result -= bitCount(knight & KNIGHT_HOME[side]) * UNDEVELOPED_KNIGHT;
//        ADD(SCORE_DEBUG.UNDEVELOPED_KNIGHT[side],
//            bitCount(knight & KNIGHT_HOME[side]) * UNDEVELOPED_KNIGHT);
//    }

    // 4. king security

    if (structureEval.pinned[side] & knight) result[MG] -= KNIGHT_PINNED[MG];
    if (structureEval.pinned[side] & knight) result[EG] -= KNIGHT_PINNED[EG];
    ADD(SCORE_DEBUG.KNIGHT_PINNED[side], -KNIGHT_PINNED);
    structureEval.kingSecurity[MG][side] +=
            FRIEND_NEAR_KING[MG] * bitCount(NEAR_MASK2[structureEval.posKing[side]] & knight);
    structureEval.kingSecurity[EG][side] +=
            FRIEND_NEAR_KING[EG] * bitCount(NEAR_MASK2[structureEval.posKing[side]] & knight);
    ADD(SCORE_DEBUG.KING_SECURITY_KNIGHT[side],
        FRIEND_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[side]] & knight));

//        structureEval.kingSecurity[side] -=
//                ENEMY_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[xside]] & knight);
//        ADD(SCORE_DEBUG.KING_SECURITY_KNIGHT[xside],
//            -ENEMY_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[xside]] & knight));

    for (; knight; RESET_LSB(knight)) {
        const int pos = BITScanForward(knight);

        // 5. mobility
        assert(bitCount(notMyBits & KNIGHT_MASK[pos]) < (int) (sizeof(MOB_KNIGHT) / sizeof(int)));
        u64 mob = notMyBits & KNIGHT_MASK[pos];
        result[MG] += MOB_KNIGHT[bitCount(mob)];
        result[EG] += MOB_KNIGHT[bitCount(mob)];
        if (mob & structureEval.posKingBit[xside]) structureEval.kingAttackers[xside] |= POW2(pos);
        ADD(SCORE_DEBUG.MOB_KNIGHT[side], MOB_KNIGHT[bitCount(mob)]);

        // 6. outposts
        auto p = KNIGHT_OUTPOST[side][pos];

        //enemy pawn doesn't attack knight
        if (p && !(PAWN_FORK_MASK[xside][pos] & chessboard[xside])) {
            //friend paws defends knight
            if (PAWN_FORK_MASK[xside][pos] & chessboard[side]) {
                result[MG] += p;
                result[EG] += p;
                if (!(chessboard[KNIGHT_BLACK + xside]) &&
                    !(chessboard[BISHOP_BLACK + xside] & board::colors(pos))) {
                    result[MG] += p;
                    result[EG] += p;
                }
            }
        }
    }
    return pair<int, int>(result[MG], result[EG]);
}


/**
 * evaluate rook for color at phase
 * 1. if no rooks returns 0
 * 3. in middle if in 7th - add ROOK_7TH_RANK for each rook in 7th
 * 4. *king security* - in OPEN phase add at kingSecurity FRIEND_NEAR_KING for each rook near to king and substracts ENEMY_NEAR_KING for each rook near to enemy king
 * 5. add OPEN_FILE/HALF_OPEN_FILE if the rook is on open/semiopen file
 * 6. trapped
 * 7. 2 linked towers
 * 8. Penalise if Rook is Blocked Horizontally
*/
template<uchar side>
pair<int, int> Eval::evaluateRook(const _Tchessboard &chessboard, const u64 enemies, const u64 friends) {
    INC(evaluationCount[side]);

    u64 rook = chessboard[ROOK_BLACK + side];
    if (!rook) return pair<int, int>(0, 0);

//    const int nRooks = bitCount(rook);
    // 2.
    int result[2] = {0, 0};
    constexpr int xside = X(side);
    // 3. in 7th

    result[MG] += ROOK_7TH_RANK[MG] * bitCount(rook & RANK_1_7[side]);
    result[EG] += ROOK_7TH_RANK[EG] * bitCount(rook & RANK_1_7[side]);
    ADD(SCORE_DEBUG.ROOK_7TH_RANK[side], ROOK_7TH_RANK * bitCount(rook & RANK_1_7[side]));

    // 4. king security
    if (structureEval.pinned[side] & rook) result[MG] -= ROOK_PINNED[MG];
    if (structureEval.pinned[side] & rook) result[EG] -= ROOK_PINNED[EG];
    ADD(SCORE_DEBUG.ROOK_PINNED[side], -ROOK_PINNED);
    structureEval.kingSecurity[MG][side] +=
            FRIEND_NEAR_KING[MG] * bitCount(NEAR_MASK2[structureEval.posKing[side]] & rook);
    structureEval.kingSecurity[EG][side] +=
            FRIEND_NEAR_KING[EG] * bitCount(NEAR_MASK2[structureEval.posKing[side]] & rook);
    ADD(SCORE_DEBUG.KING_SECURITY_ROOK[side],
        FRIEND_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[side]] & rook));

//        structureEval.kingSecurity[side] -=
//                ENEMY_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[xside]] & rook);
//        ADD(SCORE_DEBUG.KING_SECURITY_ROOK[xside],
//            -ENEMY_NEAR_KING * bitCount(NEAR_MASK2[structureEval.posKing[xside]] & rook));


    // .6
//    if (((F1G1bit[side] & king) && (H1H2G1bit[side] & rook)) || ((C1B1bit[side] & king) && (A1A2B1bit[side] & rook))) {
//        ADD(SCORE_DEBUG.ROOK_TRAPPED[side], -ROOK_TRAPPED);
//        result -= ROOK_TRAPPED;
//    }

    // .7
//    if (nRooks == 2) {
//        const int firstRook = BITScanForward(rook);
//        const int secondRook = BITScanReverse(rook);
//        if ((!(LINK_ROOKS[firstRook][secondRook] & structureEval.allPieces))) {
//            ADD(SCORE_DEBUG.CONNECTED_ROOKS[side], CONNECTED_ROOKS);
//            result += CONNECTED_ROOKS;
//        }
//    }

    for (; rook; RESET_LSB(rook)) {
        const int o = BITScanForward(rook);
        //mobility
        u64 mob = board::getMobilityRook(o, enemies, friends);
        if (mob & structureEval.posKingBit[xside]) structureEval.kingAttackers[xside] |= POW2(o);

        assert(bitCount(mob) < (int) (sizeof(MOB_ROOK[phase]) / sizeof(int)));
        result[MG] += MOB_ROOK[MG][bitCount(mob)];
        result[EG] += MOB_ROOK[EG][bitCount(mob)];
        ADD(SCORE_DEBUG.MOB_ROOK[side], MOB_ROOK[phase][bitCount(mob)]);

//        if (phase != OPEN) {
//            // .8 Penalise if Rook is Blocked Horizontally
//            if ((RANK_BOUND[o] & structureEval.allPieces) == RANK_BOUND[o]) {
//                ADD(SCORE_DEBUG.ROOK_BLOCKED[side], -ROOK_BLOCKED);
//                result -= ROOK_BLOCKED;
//            }
//        }

        // .5
//        if (!(chessboard[side] & FILE_[o])) {
//            ADD(SCORE_DEBUG.ROOK_OPEN_FILE[side], OPEN_FILE);
//            result += OPEN_FILE;
//        }
//        if (!(chessboard[xside] & FILE_[o])) {
//            ADD(SCORE_DEBUG.ROOK_OPEN_FILE[side], OPEN_FILE);
//            result += OPEN_FILE;
//        }
    }
    return pair<int, int>(result[MG], result[EG]);
}

pair<int, int> Eval::evaluateKing(const _Tchessboard &chessboard, const uchar side, const u64 squares) {
    assert(evaluationCount[side] == 5);
    int result[2] = {0, 0};
    uchar pos_king = structureEval.posKing[side];

    ADD(SCORE_DEBUG.DISTANCE_KING[side], DISTANCE_KING_ENDING[pos_king]);
    result[MG] = DISTANCE_KING_OPENING[pos_king];
    result[EG] = DISTANCE_KING_ENDING[pos_king];


    //mobility
    assert(bitCount(squares & NEAR_MASK1[pos_king]) < (int) (sizeof(MOB_KING[phase]) / sizeof(int)));
    result[MG] += MOB_KING[MG][bitCount(squares & NEAR_MASK1[pos_king])];
    result[EG] += MOB_KING[EG][bitCount(squares & NEAR_MASK1[pos_king])];
    ADD(SCORE_DEBUG.MOB_KING[side], MOB_KING[phase][bitCount(squares & NEAR_MASK1[pos_king])]);

    assert(pos_king < 64);
    if (!(NEAR_MASK1[pos_king] & chessboard[side])) {
        ADD(SCORE_DEBUG.PAWN_NEAR_KING[side], -PAWN_NEAR_KING);
        result[MG] -= PAWN_NEAR_KING[MG];
        result[EG] -= PAWN_NEAR_KING[EG];
    }
    result[MG] += structureEval.kingSecurity[MG][side];
    result[EG] += structureEval.kingSecurity[EG][side];
    return pair<int, int>(result[MG], result[EG]);
}

void Eval::storeHashValue(const u64 key, const short value) {
    evalHash[key % hashSize] = (key & keyMask) | (value & valueMask);
    assert(value == getHashValue(key));
}

short Eval::getHashValue(const u64 key) {
    const u64 kv = evalHash[key % hashSize];
    if ((kv & keyMask) == (key & keyMask))
        return (short) (kv & valueMask);

    return noHashValue;
}

short Eval::getScore(const _Tchessboard &chessboard, const u64 key, const uchar side, const int alpha, const int beta,
                     const bool trace) {
    const short hashValue = getHashValue(key);
    if (hashValue != noHashValue && !trace) {
        return side ? -hashValue : hashValue;
    }
    int lazyscore_white = lazyEvalSide<WHITE>(chessboard);
    int lazyscore_black = lazyEvalSide<BLACK>(chessboard);

#ifndef TUNING
    const int lazyscore = side ? lazyscore_white - lazyscore_black : lazyscore_black - lazyscore_white;
    if (lazyscore > (beta + FUTIL_MARGIN) || lazyscore < (alpha - FUTIL_MARGIN)) {
        INC(lazyEvalCuts);
        return lazyscore;
    }
#endif
    DEBUG(evaluationCount[WHITE] = evaluationCount[BLACK] = 0)
    DEBUG(memset(&SCORE_DEBUG, 0, sizeof(_TSCORE_DEBUG)))

    structureEval.kingSecurity[MG][WHITE] = structureEval.kingSecurity[EG][BLACK] = 0;
    const auto w = board::getBitmapNoPawnsNoKing<WHITE>(chessboard);
    const auto b = board::getBitmapNoPawnsNoKing<BLACK>(chessboard);
//    const int npieces = bitCount(w | b);
//    _Tphase phase;
//    if (npieces < 6) {
//        phase = END;
//    } else if (npieces < 11) {
//        phase = MIDDLE;
//    } else {
//        phase = OPEN;
//    }
    structureEval.allPiecesNoPawns[BLACK] = b | chessboard[KING_BLACK];
    structureEval.allPiecesNoPawns[WHITE] = w | chessboard[KING_WHITE];
    structureEval.allPiecesSide[BLACK] = structureEval.allPiecesNoPawns[BLACK] | chessboard[PAWN_BLACK];
    structureEval.allPiecesSide[WHITE] = structureEval.allPiecesNoPawns[WHITE] | chessboard[PAWN_WHITE];
    structureEval.allPieces = structureEval.allPiecesSide[BLACK] | structureEval.allPiecesSide[WHITE];
    structureEval.posKing[BLACK] = (uchar) BITScanForward(chessboard[KING_BLACK]);
    structureEval.posKing[WHITE] = (uchar) BITScanForward(chessboard[KING_WHITE]);
    structureEval.posKingBit[BLACK] = POW2(structureEval.posKing[BLACK]);
    structureEval.posKingBit[WHITE] = POW2(structureEval.posKing[WHITE]);
    structureEval.kingAttackers[WHITE] = structureEval.kingAttackers[BLACK] = 0;


    structureEval.pinned[WHITE] = board::getPinned<WHITE>(structureEval.allPieces,
                                                          structureEval.allPiecesSide[WHITE],
                                                          structureEval.posKing[WHITE], chessboard);
    structureEval.pinned[BLACK] = board::getPinned<BLACK>(structureEval.allPieces,
                                                          structureEval.allPiecesSide[BLACK],
                                                          structureEval.posKing[BLACK], chessboard);

    _Tresult tResult[2];

    getRes(chessboard, tResult);

    const int bonus_attack_king_black_eg = BONUS_ATTACK_KING[bitCount(structureEval.kingAttackers[WHITE])];
    const int bonus_attack_king_white_eg = BONUS_ATTACK_KING[bitCount(structureEval.kingAttackers[BLACK])];

    const int attack_king_white[2] = {ATTACK_KING[MG] * bitCount(structureEval.kingAttackers[WHITE]),
                                      ATTACK_KING[EG] * bitCount(structureEval.kingAttackers[WHITE])};

    const int attack_king_black[2] = {ATTACK_KING[MG] * bitCount(structureEval.kingAttackers[BLACK]),
                                      ATTACK_KING[EG] * bitCount(structureEval.kingAttackers[BLACK])};
    side == WHITE ? lazyscore_black -= 5 : lazyscore_white += 5;

    const int result_mg =
            (attack_king_black[MG] + lazyscore_black + tResult[MG].pawns[BLACK] +
             tResult[MG].knights[BLACK] + tResult[MG].bishop[BLACK] + tResult[MG].rooks[BLACK] + tResult[MG].queens[BLACK] +
             tResult[MG].kings[BLACK]) -
            (attack_king_white[MG] + lazyscore_white + tResult[MG].pawns[WHITE] +
             tResult[MG].knights[WHITE] + tResult[MG].bishop[WHITE] + tResult[MG].rooks[WHITE] + tResult[MG].queens[WHITE] +
             tResult[MG].kings[WHITE]);
    const int result_eg =
            (attack_king_black[EG] + bonus_attack_king_black_eg + lazyscore_black + tResult[EG].pawns[BLACK] +
             tResult[EG].knights[BLACK] + tResult[EG].bishop[BLACK] + tResult[EG].rooks[BLACK] + tResult[EG].queens[BLACK] +
             tResult[EG].kings[BLACK]) -
            (attack_king_white[EG] + bonus_attack_king_white_eg + lazyscore_white + tResult[EG].pawns[WHITE] +
             tResult[EG].knights[WHITE] + tResult[EG].bishop[WHITE] + tResult[EG].rooks[WHITE] + tResult[EG].queens[WHITE] +
             tResult[EG].kings[WHITE]);
    const float phase =
            ((MAX_VALUE_TAPERED - (lazyscore_white + lazyscore_black)) * 256 + (MAX_VALUE_TAPERED / 2)) /
            MAX_VALUE_TAPERED;
    const short finalScore = ((result_mg * (256 - phase)) + (result_eg * phase)) / 256;

#ifdef DEBUG_MODE
    if (trace) {
        const string HEADER = "\n|\t\t\t\t\tTOT (white)\t\t  WHITE\t\tBLACK\n";
        cout << "|PHASE: ";
        if (phase == OPEN) {
            cout << " OPEN\n";
        } else if (phase == MIDDLE) {
            cout << " MIDDLE\n";
        } else {
            cout << " END\n";
        }

        cout << "|VALUES:";
        cout << "\tPAWN: " << (double) constants::VALUEPAWN / 100.0;
        cout << " ROOK: " << (double) constants::VALUEROOK / 100.0;
        cout << " BISHOP: " << (double) constants::VALUEBISHOP / 100.0;
        cout << " KNIGHT: " << (double) constants::VALUEKNIGHT / 100.0;
        cout << " QUEEN: " << (double) constants::VALUEQUEEN / 100.0 << "\n\n";

        cout << HEADER;
        cout << "|Material:         " << setw(10) << (double) (lazyscore_white - lazyscore_black) / 100.0 << setw(15) <<
             (double) (lazyscore_white) / 100.0 << setw(10) << (double) (lazyscore_black) / 100.0 << "\n";
        cout << "|Bonus attack king:" << setw(10) <<
             (double) (bonus_attack_king_white - bonus_attack_king_black) / 100.0 << setw(15) <<
             (double) (bonus_attack_king_white) / 100.0 << setw(10) << (double) (bonus_attack_king_black) / 100.0
             << "\n";

        cout << HEADER;
        cout << "|Pawn:             " << setw(10) << (double) (Tresult.pawns[WHITE] - Tresult.pawns[BLACK]) / 100.0 <<
             setw(15) << (double) (Tresult.pawns[WHITE]) / 100.0 << setw(10) << (double) (Tresult.pawns[BLACK]) / 100.0
             <<
             "\n";
        cout << "|       attack king:              " << setw(10) <<
             (double) (SCORE_DEBUG.ATTACK_KING_PAWN[WHITE]) / 100.0 << setw(10) <<
             (double) (SCORE_DEBUG.ATTACK_KING_PAWN[BLACK]) / 100.0 << "\n";
//        cout << "|       center:                   " << setw(10) << (double) (SCORE_DEBUG.PAWN_CENTER[WHITE]) / 100.0 <<
//             setw(10) << (double) (SCORE_DEBUG.PAWN_CENTER[BLACK]) / 100.0 << "\n";
        cout << "|       in 7th:                   " << setw(10) << (double) (SCORE_DEBUG.PAWN_7H[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.PAWN_7H[BLACK]) / 100.0 << "\n";
        cout << "|       in promotion:             " << setw(10) <<
             (double) (SCORE_DEBUG.PAWN_IN_PROMOTION[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.PAWN_IN_PROMOTION[BLACK]) / 100.0 << "\n";
        cout << "|       blocked:                  " << setw(10) <<
             (double) (SCORE_DEBUG.PAWN_BLOCKED[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.PAWN_BLOCKED[BLACK]) / 100.0 << "\n";
        cout << "|       unprotected:              " << setw(10) <<
             (double) (SCORE_DEBUG.UNPROTECTED_PAWNS[WHITE]) / 100.0 << setw(10) <<
             (double) (SCORE_DEBUG.UNPROTECTED_PAWNS[BLACK]) / 100.0 << "\n";
//        cout << "|       isolated                  " << setw(10) <<
//             (double) (SCORE_DEBUG.PAWN_ISOLATED[WHITE]) / 100.0 << setw(10) <<
//             (double) (SCORE_DEBUG.PAWN_ISOLATED[BLACK]) / 100.0 << "\n";
//        cout << "|       double                    " << setw(10) <<
//             (double) (SCORE_DEBUG.DOUBLED_PAWNS[WHITE]) / 100.0 << setw(10) <<
//             (double) (SCORE_DEBUG.DOUBLED_PAWNS[BLACK]) / 100.0 << "\n";
        cout << "|       double isolated           " << setw(10) <<
             (double) (SCORE_DEBUG.DOUBLED_ISOLATED_PAWNS[WHITE]) / 100.0 << setw(10) <<
             (double) (SCORE_DEBUG.DOUBLED_ISOLATED_PAWNS[BLACK]) / 100.0 << "\n";
        cout << "|       backward                  " << setw(10) <<
             (double) (SCORE_DEBUG.BACKWARD_PAWN[WHITE]) / 100.0 << setw(10) <<
             (double) (SCORE_DEBUG.BACKWARD_PAWN[BLACK]) / 100.0 << "\n";
        cout << "|       fork:                     " << setw(10) << (double) (SCORE_DEBUG.FORK_SCORE[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.FORK_SCORE[BLACK]) / 100.0 << "\n";
        cout << "|       passed:                   " << setw(10) << (double) (SCORE_DEBUG.PAWN_PASSED[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.PAWN_PASSED[BLACK]) / 100.0 << "\n";
        cout << "|       all enemies:              " << setw(10) <<
             (double) (SCORE_DEBUG.ENEMIES_PAWNS_ALL[WHITE]) / 100.0 << setw(10) <<
             (double) (SCORE_DEBUG.ENEMIES_PAWNS_ALL[BLACK]) / 100.0 << "\n";
        cout << "|       none:                     " << setw(10) << (double) (SCORE_DEBUG.NO_PAWNS[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.NO_PAWNS[BLACK]) / 100.0 << "\n";
//        cout << "|       pinned:                   " << setw(10) << (double) (SCORE_DEBUG.PAWN_PINNED[WHITE]) / 100.0 <<
//             setw(10) << (double) (SCORE_DEBUG.PAWN_PINNED[BLACK]) / 100.0 << "\n";
        cout << HEADER;
        cout << "|Knight:           " << setw(10) <<
             (double) (Tresult.knights[WHITE] - Tresult.knights[BLACK]) / 100.0 << setw(15) <<
             (double) (Tresult.knights[WHITE]) / 100.0 << setw(10) << (double) (Tresult.knights[BLACK]) / 100.0 << "\n";
//        cout << "|       undevelop:                " << setw(10) <<
//             (double) (SCORE_DEBUG.UNDEVELOPED_KNIGHT[WHITE]) / 100.0 << setw(10) <<
//             (double) (SCORE_DEBUG.UNDEVELOPED_KNIGHT[BLACK]) / 100.0 << "\n";
        cout << "|       trapped:                  " << setw(10) <<
             (double) (SCORE_DEBUG.KNIGHT_TRAPPED[WHITE]) / 100.0 << setw(10) <<
             (double) (SCORE_DEBUG.KNIGHT_TRAPPED[BLACK]) / 100.0 << "\n";
        cout << "|       mobility:                 " << setw(10) << (double) (SCORE_DEBUG.MOB_KNIGHT[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.MOB_KNIGHT[BLACK]) / 100.0 << "\n";
        cout << "|       pinned:                   " << setw(10) << (double) (SCORE_DEBUG.KNIGHT_PINNED[WHITE]) / 100.0
             <<
             setw(10) << (double) (SCORE_DEBUG.KNIGHT_PINNED[BLACK]) / 100.0 << "\n";
        cout << HEADER;
        cout << "|Bishop:           " << setw(10) << (double) (Tresult.bishop[WHITE] - Tresult.bishop[BLACK]) / 100.0 <<
             setw(15) << (double) (Tresult.bishop[WHITE]) / 100.0 << setw(10)
             << (double) (Tresult.bishop[BLACK]) / 100.0 <<
             "\n";
        cout << "|       bad:                      " << setw(10) << (double) (SCORE_DEBUG.BAD_BISHOP[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.BAD_BISHOP[BLACK]) / 100.0 << "\n";
        cout << "|       mobility:                 " << setw(10) << (double) (SCORE_DEBUG.MOB_BISHOP[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.MOB_BISHOP[BLACK]) / 100.0 << "\n";
//        cout << "|       undevelop:                " << setw(10) <<
//             (double) (SCORE_DEBUG.UNDEVELOPED_BISHOP[WHITE]) / 100.0 << setw(10) <<
//             (double) (SCORE_DEBUG.UNDEVELOPED_BISHOP[BLACK]) / 100.0 << "\n";
//        cout << "|       open diag:                " << setw(10) <<
//             (double) (SCORE_DEBUG.OPEN_DIAG_BISHOP[WHITE]) / 100.0 << setw(10) <<
//             (double) (SCORE_DEBUG.OPEN_DIAG_BISHOP[BLACK]) / 100.0 << "\n";
        cout << "|       bonus 2 bishops:          " << setw(10) <<
             (double) (SCORE_DEBUG.BONUS2BISHOP[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.BONUS2BISHOP[BLACK]) / 100.0 << "\n";
        cout << "|       pinned:                   " << setw(10) << (double) (SCORE_DEBUG.BISHOP_PINNED[WHITE]) / 100.0
             <<
             setw(10) << (double) (SCORE_DEBUG.BISHOP_PINNED[BLACK]) / 100.0 << "\n";
        cout << HEADER;
        cout << "|Rook:             " << setw(10) << (double) (Tresult.rooks[WHITE] - Tresult.rooks[BLACK]) / 100.0 <<
             setw(15) << (double) (Tresult.rooks[WHITE]) / 100.0 << setw(10) << (double) (Tresult.rooks[BLACK]) / 100.0
             <<
             "\n";
        cout << "|       7th:                      " << setw(10) <<
             (double) (SCORE_DEBUG.ROOK_7TH_RANK[WHITE]) / 100.0 << setw(10) <<
             (double) (SCORE_DEBUG.ROOK_7TH_RANK[BLACK]) / 100.0 << "\n";
//        cout << "|       trapped:                  " << setw(10) <<
//             (double) (SCORE_DEBUG.ROOK_TRAPPED[WHITE]) / 100.0 <<
//             setw(10) << (double) (SCORE_DEBUG.ROOK_TRAPPED[BLACK]) / 100.0 << "\n";
        cout << "|       mobility:                 " << setw(10) << (double) (SCORE_DEBUG.MOB_ROOK[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.MOB_ROOK[BLACK]) / 100.0 << "\n";
//        cout << "|       blocked:                  " << setw(10) <<
//             (double) (SCORE_DEBUG.ROOK_BLOCKED[WHITE]) / 100.0 <<
//             setw(10) << (double) (SCORE_DEBUG.ROOK_BLOCKED[BLACK]) / 100.0 << "\n";
//        cout << "|       open file:                " << setw(10) <<
//             (double) (SCORE_DEBUG.ROOK_OPEN_FILE[WHITE]) / 100.0 << setw(10) <<
//             (double) (SCORE_DEBUG.ROOK_OPEN_FILE[BLACK]) / 100.0 << "\n";
//        cout << "|       connected:                " << setw(10) <<
//             (double) (SCORE_DEBUG.CONNECTED_ROOKS[WHITE]) / 100.0 << setw(10) <<
//             (double) (SCORE_DEBUG.CONNECTED_ROOKS[BLACK]) / 100.0 << "\n";
        cout << "|       pinned:                   " << setw(10) << (double) (SCORE_DEBUG.ROOK_PINNED[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.ROOK_PINNED[BLACK]) / 100.0 << "\n";
        cout << HEADER;
        cout << "|Queen:            " << setw(10) << (double) (Tresult.queens[WHITE] - Tresult.queens[BLACK]) / 100.0 <<
             setw(15) << (double) (Tresult.queens[WHITE]) / 100.0 << setw(10)
             << (double) (Tresult.queens[BLACK]) / 100.0 <<
             "\n";
        cout << "|       mobility:                 " << setw(10) << (double) (SCORE_DEBUG.MOB_QUEEN[WHITE]) / 100.0 <<
             setw(10) << (double) (SCORE_DEBUG.MOB_QUEEN[BLACK]) / 100.0 << "\n";
        cout << "|       bishop on queen:          " << setw(10) <<
             (double) (SCORE_DEBUG.BISHOP_ON_QUEEN[WHITE]) / 100.0 << setw(10) <<
             (double) (SCORE_DEBUG.BISHOP_ON_QUEEN[BLACK]) / 100.0 << "\n";
        cout << "|       pinned:                   " << setw(10) << (double) (SCORE_DEBUG.QUEEN_PINNED[WHITE]) / 100.0
             <<
             setw(10) << (double) (SCORE_DEBUG.QUEEN_PINNED[BLACK]) / 100.0 << "\n";
        cout << HEADER;
        cout << "|King:             " << setw(10) << (double) (Tresult.kings[WHITE] - Tresult.kings[BLACK]) / 100.0 <<
             setw(15) << (double) (Tresult.kings[WHITE]) / 100.0 << setw(10) << (double) (Tresult.kings[BLACK]) / 100.0
             <<
             "\n";
        cout << "|       distance:                 " << setw(10) <<
             (double) (SCORE_DEBUG.DISTANCE_KING[WHITE]) / 100.0 << setw(10) <<
             (double) (SCORE_DEBUG.DISTANCE_KING[BLACK]) / 100.0 << "\n";

        cout << "|       pawn near:                " << setw(10) <<
             (double) (SCORE_DEBUG.PAWN_NEAR_KING[WHITE]) / 100.0 << setw(10) <<
             (double) (SCORE_DEBUG.PAWN_NEAR_KING[BLACK]) / 100.0 << "\n";
        cout << endl;
        cout << "\n|Total (white)..........   " << (side ? result / 100.0 : -result / 100.0) << endl;
        cout << flush;
    }
#endif
#ifndef TUNING
    storeHashValue(key, finalScore);
#endif
    return side ? -finalScore : finalScore;
//    return side ? finalScore + 5 : -finalScore - 5;
}

