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

#ifdef TUNING
#pragma once

#include "../def.h"
#include <set>
#include <array>

/*
 Download a big strong main.pgn file
 pgn-extract -Tr0-1 main.pgn >win_black.pgn 2>/dev/null
 pgn-extract -Tr1-0 main.pgn >win_white.pgn 2>/dev/null
 pgn-extract -Tr1/2-1/2 main.pgn >draw.pgn 2>/dev/null

 pgn-extract -Wepd win_black.pgn >win_black.epd 2>/dev/null
 pgn-extract -Wepd win_white.pgn >win_white.epd 2>/dev/null
 pgn-extract -Wepd draw.pgn >draw.epd 2>/dev/null

 cat win_white.epd  | awk -F " " '{print $1" "$2" "$3}' >foo;sort -u foo |grep "k" > win_white.epd
 cat win_black.epd  | awk -F " " '{print $1" "$2" "$3}' >foo;sort -u foo |grep "k" > win_black.epd
 cat draw.epd       | awk -F " " '{print $1" "$2" "$3}' >foo;sort -u foo |grep "k" > draw.epd
 rm foo win_black.pgn win_white.pgn draw.pgn
*/

class Texel {

private:
    constexpr static int N_PARAM = 30;
    SearchManager &searchManager = Singleton<SearchManager>::getInstance();
public:
    static constexpr auto help = "Texel method tuning\n\n"
                                 " Download a big strong main.pgn file\n"
                                 " pgn-extract -Tr0-1 main.pgn >win_black.pgn 2>/dev/null\n"
                                 " pgn-extract -Tr1-0 main.pgn >win_white.pgn 2>/dev/null\n"
                                 " pgn-extract -Tr1/2-1/2 main.pgn >draw.pgn 2>/dev/null\n"
                                 "\n"
                                 " pgn-extract -Wepd win_black.pgn >win_black.epd 2>/dev/null\n"
                                 " pgn-extract -Wepd win_white.pgn >win_white.epd 2>/dev/null\n"
                                 " pgn-extract -Wepd draw.pgn >draw.epd 2>/dev/null\n"
                                 "\n"
                                 " cat win_white.epd  | awk -F \" \" '{print $1\" \"$2\" \"$3}' >foo;sort -u foo |grep \"k\" > win_white.epd\n"
                                 " cat win_black.epd  | awk -F \" \" '{print $1\" \"$2\" \"$3}' >foo;sort -u foo |grep \"k\" > win_black.epd\n"
                                 " cat draw.epd       | awk -F \" \" '{print $1\" \"$2\" \"$3}' >foo;sort -u foo |grep \"k\" > draw.epd\n"
                                 " rm foo win_black.pgn win_white.pgn draw.pgn\n";

    Texel(const string &path) {
        this->path = path;

        cout << Time::getLocalTime() << " Start" << endl;
        set <string> draw;
        set <string> black;
        set <string> white;
        cout << "Fetch epd files..." << endl << flush;

        fetch(path + "/draw.epd", draw);
        fetch(path + "/win_white.epd", white);
        fetch(path + "/win_black.epd", black);

        cout << "ok" << flush << endl;
        cout << "draw size: " << draw.size() << endl;
        cout << "white size: " << white.size() << endl;
        cout << "black size: " << black.size() << endl;
        cout << "Purge fen..." << flush;
        clean(draw, black, white);

        cout << "ok, new size:" << endl;
        cout << "draw size: " << draw.size() << endl;
        cout << "white size: " << white.size() << endl;
        cout << "black size: " << black.size() << endl;

        cout << "Run tuning..." << flush;
        set < FEN * > fens;
        populateFens(white, fens, 1);
        cout << "." << flush;
        populateFens(black, fens, 0);
        cout << "." << flush;
        populateFens(draw, fens, 0.5);
        cout << "." << flush;
        draw.clear();
        white.clear();
        black.clear();

        tune(fens);

        cout << endl << endl << Time::getLocalTime() << " End\n";
        for (auto itr = fens.begin(); itr != fens.end(); itr++) delete *itr;
        fens.clear();
    }

private:
    string path;
    const string iniFile = "tuning.ini";

    struct FEN {
        string fen;
        double win; // 1=WHITE, 0=BLACK, 0.5=DRAW
        FEN(string f, int w) : fen(f), win(w) {}
    };

    struct PARAMS {
    private :
        int startValue;
    public:
        int getStartValue() const { return startValue; }

        string name;

        PARAMS(string n, const SearchManager &searchManager) : name(n) { startValue = searchManager.getParameter(n); }

        void print(SearchManager &searchManager) const {
            printf("\nname: %s, startValue: %d, newValue: %d", name.c_str(), startValue,
                   searchManager.getParameter(name));
            if (startValue != searchManager.getParameter(name))cout << " (*)";
        }
    };

    void populateFens(const set <string> &s, set<FEN *> &fens, const double win) {
        for (auto itr = s.begin(); itr != s.end(); itr++) {
            const string fen = *itr;
            FEN *x = new FEN(fen, win);
            fens.insert(x);
        }
    }

    bool inCheck(string fen) {
        const int side = searchManager.loadFen(fen);
        if (side == WHITE) return board::inCheck1<WHITE>(searchManager.getChessboard());
        else return board::inCheck1<BLACK>(searchManager.getChessboard());
    }

    void clean(set <string> &draw, set <string> &black, set <string> &white) {
        set<string>::iterator itr = draw.begin();
        set<string>::iterator tmp;

        for (; itr != draw.end();) {
            const auto check = inCheck(*itr);
            if (check || white.find(*itr) != white.end() || black.find(*itr) != black.end()) {
                white.erase(*itr);
                black.erase(*itr);
                tmp = itr;
                ++tmp;
                draw.erase(*itr);
                itr = tmp;
            } else itr++;
        }

        itr = white.begin();
        for (; itr != white.end();) {
            const auto check = inCheck(*itr);
            if (check || black.find(*itr) != black.end()) {
                black.erase(*itr);
                draw.erase(*itr);
                tmp = itr;
                ++tmp;
                white.erase(*itr);
                itr = tmp;
            } else itr++;
        }

        itr = black.begin();
        for (; itr != black.end();) {
            const auto check = inCheck(*itr);
            if (check) {
                white.erase(*itr);
                draw.erase(*itr);
                tmp = itr;
                ++tmp;
                black.erase(*itr);
                itr = tmp;
            } else itr++;
        }
    }

    void fetch(const string file, set <string> &s) {
        if (!FileUtil::fileExists(file)) {
            cout << "Unable to open file " << file << endl;
            exit(1);
        }
        ifstream epdFile(file);
        string line;
        if (epdFile.is_open()) {
            while (getline(epdFile, line)) s.insert(line);
            epdFile.close();
        }
    }

    double E(const set<FEN *> &fens) {
        constexpr double K = 1.13;
        double currentError = 0.0;
        for (auto itr = fens.begin(); itr != fens.end(); itr++) {
            const FEN *fen = *itr;
            searchManager.loadFen(fen->fen);
            searchManager.setRunning(2);
            searchManager.setRunningThread(true);
            searchManager.setMaxTimeMillsec(2500);
            const double score = searchManager.getQscore();
//            const double score = searchManager.getScore(WHITE);
            if (abs(score) > _INFINITE - 1000) {
                cout << "skip mate score " << score << endl;
                continue;
            }
            const auto sigmoid = 1.0 / (1.0 + pow(10.0, -K * score / 400.0));
            const auto result = fen->win;
            currentError += pow(result - sigmoid, 2.0);
        }
        return 1.0 / fens.size() * currentError;
    }

    void saveParams(const array<PARAMS, N_PARAM> &params) {
        cout << endl << Time::getLocalTime() << " save parameters to " << path + FOLDERSEP + iniFile << endl;
        ofstream myfile;
        myfile.open(path + FOLDERSEP + iniFile);
        myfile << "#" << Time::getLocalTime() << endl;
        for (auto &param:params) {
            myfile << param.name << "=" << searchManager.getParameter(param.name) << endl;
        }
        myfile.close();
    }

    void loadParams() {
        cout << "\nload parameters from " << path + FOLDERSEP + iniFile << endl;
        map<string, string> map = IniFile(path + FOLDERSEP + iniFile).map;
        for (std::map<string, string>::iterator it = map.begin(); it != map.end(); ++it) {
            std::cout << it->first << " => " << it->second << endl;
            searchManager.setParameter(it->first, stoi(it->second));
        }
    }

    void tune(const set<FEN *> &fens) {
        searchManager.setMaxTimeMillsec(2500);
        cout.precision(17);
        loadParams();
        const array<PARAMS, N_PARAM> params{
                PARAMS("ATTACK_KING", searchManager),
                PARAMS("BISHOP_ON_QUEEN", searchManager),
                PARAMS("BACKWARD_PAWN", searchManager),
                PARAMS("DOUBLED_ISOLATED_PAWNS", searchManager),
                PARAMS("DOUBLED_PAWNS", searchManager),
                PARAMS("PAWN_IN_7TH", searchManager),
                PARAMS("PAWN_CENTER", searchManager),
                PARAMS("PAWN_IN_PROMOTION", searchManager),
                PARAMS("PAWN_ISOLATED", searchManager),
                PARAMS("PAWN_NEAR_KING", searchManager),
                PARAMS("PAWN_BLOCKED", searchManager),
                PARAMS("UNPROTECTED_PAWNS", searchManager),
                PARAMS("ENEMY_NEAR_KING", searchManager),
                PARAMS("FRIEND_NEAR_KING", searchManager),
                PARAMS("HALF_OPEN_FILE_Q", searchManager),
                PARAMS("BONUS2BISHOP", searchManager),
                PARAMS("BISHOP_PAWN_ON_SAME_COLOR", searchManager),
                PARAMS("CONNECTED_ROOKS", searchManager),
                PARAMS("OPEN_FILE", searchManager),
                PARAMS("OPEN_FILE_Q", searchManager),
                PARAMS("ROOK_7TH_RANK", searchManager),
                PARAMS("ROOK_BLOCKED", searchManager),
                PARAMS("ROOK_TRAPPED", searchManager),
                PARAMS("UNDEVELOPED_KNIGHT", searchManager),
                PARAMS("UNDEVELOPED_BISHOP", searchManager),
                PARAMS("KNIGHT_PINNED", searchManager),
                PARAMS("ROOK_PINNED", searchManager),
                PARAMS("BISHOP_PINNED", searchManager),
                PARAMS("QUEEN_PINNED", searchManager),
                PARAMS("PAWN_PINNED", searchManager)
        };
        bool fullImproved;
        int cycle = 1;
        double bestError;
        do {
            cout << "***************************************** Cycle #" << (cycle++) << " " << Time::getLocalTime()
                 << " *****************************************" << endl;
            fullImproved = false;
            const double startError = E(fens);
            for (auto &param:params) param.print(searchManager);
            cout << "\nstartError: " << startError << endl << flush;
            bestError = startError;
            for (auto &param:params) {
                int bestValue = -1;
                for (int dir = 0; dir < 2; dir++) {
                    if (!dir)cout << "\nUP\n" << flush; else cout << "\nDOWN\n" << flush;

                    auto oldValue = searchManager.getParameter(param.name);
                    int newValue;
                    if (dir == 0) newValue = searchManager.getParameter(param.name) + 1;
                    else {
                        if (searchManager.getParameter(param.name) <= 0)continue;
                        else newValue = searchManager.getParameter(param.name) - 1;
                    }
                    searchManager.setParameter(param.name, newValue);

                    double currentError;

                    int eq = 0;
                    while (true) {
                        currentError = E(fens);
                        cout << param.name << " try value: " << newValue << "\terror: " << currentError
                             << "\tbestError: "
                             << bestError;
                        if (currentError < bestError)cout << "\t(improved)";
                        else if (currentError > bestError)cout << "\t(got worse)";
                        else cout << "\t(same)";
                        cout << endl << flush;
                        if (currentError <= bestError && eq < 3) {
                            if (currentError == bestError) eq++; else eq = 0;
                            if (currentError < bestError) {
                                bestValue = newValue;
                                bestError = currentError;
                                fullImproved = true;
                            }
                            if (dir == 0) newValue++; else { if (newValue <= 0)break; else newValue--; }
                            searchManager.setParameter(param.name, newValue);
                        } else break;
                    }
                    searchManager.setParameter(param.name, oldValue);
                    if (bestValue >= 0) {
                        cout << "\n** OK ** bestError: " << bestError << " bestValue " << bestValue << " was "
                             << param.getStartValue() << flush;
                        searchManager.setParameter(param.name, bestValue);
                        for (auto &param:params) param.print(searchManager);
                        saveParams(params);
                        assert(E(fens) == bestError);
                    } else cout << "\n** ko **" << flush;
                }
            }
        } while (fullImproved);
        for (auto &param:params) param.print(searchManager);
        saveParams(params);
    }
};

#endif
