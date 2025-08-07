#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

const int MY_TERRITORY = 1; // 내 영토
const int OPPONENT_TERRITORY = 2; // 상대방 영토

const int MAX_DEPTH = 7; // Minimax 탐색 최대 깊이
const vector<int> SEARCH_SIZES = {10, 10, 10, 10, 10, 10, 10}; // 각 깊이별 탐색 개수 (0 ~ 6. depth 7은 최대 깊이라서 탐색하지 않음)

class MinimaxGameBoard {
private:
    vector<vector<int>> board; // 게임 보드 (2차원 벡터)
    bool passed;               // 마지막 턴에 패스했는지 여부

    vector<vector<int>> territoryBoard; // 영토 보드 (2차원 벡터)
    int myScore;                          // 내 점수
    int opponentScore;                  // 상대 점수

public:
    MinimaxGameBoard(const vector<vector<int>> &board, const vector<vector<int>> &territoryBoard, bool passed, int myScore, int opponentScore)
        : board(board), territoryBoard(territoryBoard), passed(passed), myScore(myScore), opponentScore(opponentScore) {
            int rows = board.size();
            int cols = board[0].size();
        };
    
    vector<vector<int>> getBoard() const {
        return board; // 현재 보드 상태 반환
    }
    vector<vector<int>> getTerritoryBoard() const {
        return territoryBoard; // 현재 영토 보드 상태 반환
    }
    int getBoardScore() const {
        return myScore - opponentScore; // 내 점수에서 상대 점수를 뺀 값 반환
    }

    int getMyScore() const {
        return myScore; // 내 점수 반환
    }
    int getOpponentScore() const {
        return opponentScore; // 상대 점수 반환
    }

    bool setPassed(bool passed) {
        this->passed = passed; // 패스 여부 설정
        return this->passed;
    }
    bool isPassed() const {
        return passed; // 패스 여부 반환
    }

    bool isValid(int r1, int c1, int r2, int c2) const {
        bool r1fit = false, c1fit = false, r2fit = false, c2fit = false;

        for (int r = r1; r <= r2; r++)
            for (int c = c1; c <= c2; c++)
                if (board[r][c] != 0) { // 숫자가 있는 경우 (아무도 차지하지 않음))
                    if (r == r1)
                        r1fit = true;
                    if (r == r2)
                        r2fit = true;
                    if (c == c1)
                        c1fit = true;
                    if (c == c2)
                        c2fit = true;
                }
        return r1fit && r2fit && c1fit && c2fit;
    }

    // 주어진 수를 보드에 반영 (칸을 0으로 지움)
    void updateMove(int r1, int c1, int r2, int c2, bool isMyMove) {
        if (r1 == -1 && c1 == -1 && r2 == -1 && c2 == -1) {
            passed = true;
            return;
        }

        const int currentPlayerTerritory = isMyMove ? MY_TERRITORY : OPPONENT_TERRITORY; // 업데이트하는 수의 플레이어 영토

        for (int r = r1; r <= r2; r++)
            for (int c = c1; c <= c2; c++) {
                // 1. 아무도 차지하지 않은 영역을 차지하는 경우
                if (board[r][c] != 0) {
                    board[r][c] = 0; // 해당 칸을 차지
                    territoryBoard[r][c] = currentPlayerTerritory; // 영토 보드에 현재 플레이어의 반영
                    
                    isMyMove ? myScore++ : opponentScore++; // 현재 플레이어의 점수를 1 증가
                }
                // 2. 이미 누군가 차지한 영역을 빼앗으려는 경우
                else if (territoryBoard[r][c] != currentPlayerTerritory) { // 내 영토가 아니라면 (즉, 상대방 영토라면)
                    territoryBoard[r][c] = currentPlayerTerritory; // 내 영토로 변경
                    
                    if (isMyMove) {
                        myScore++;       // 내 점수는 1 증가
                        opponentScore--; // 상대 점수는 1 감소
                    } else {
                        myScore--;       // 내 점수는 1 감소
                        opponentScore++; // 상대 점수는 1 증가
                    }
                }
            }
        passed = false;
    }

    vector<vector<int>> getAvailableMoves(bool isMyTurn) const {
        vector<vector<int>> availableMoves;
        int rows = board.size();
        int cols = board[0].size();

        const int currentPlayerTerritory = isMyTurn ? MY_TERRITORY : OPPONENT_TERRITORY;
        const int opponentTerritory = isMyTurn ? OPPONENT_TERRITORY : MY_TERRITORY;

        // 누적 합 배열을 만든다.
        vector<vector<int>> prefixSum(rows + 1, vector<int>(cols + 1, 0));
        for (int r = 1; r <= rows; r++)
            for (int c = 1; c <= cols; c++)
                prefixSum[r][c] = board[r - 1][c - 1] + prefixSum[r - 1][c] + prefixSum[r][c - 1] - prefixSum[r - 1][c - 1];

        // 모든 가능한 사각형을 탐색
        for (int r1 = 0; r1 < rows; r1++) {
            for (int c1 = 0; c1 < cols; c1++) {
                for (int r2 = r1; r2 < rows; r2++) {
                    // int sum = getSum(r1, c1, r2, c1); // 사각형 (r1, c1) ~ (r2, c1)의 합 계산
                    int sum = prefixSum[r2 + 1][c1 + 1] - prefixSum[r1][c1 + 1] - prefixSum[r2 + 1][c1] + prefixSum[r1][c1];
                    if (sum > 10)
                        break;
                    for (int c2 = c1; c2 < cols; c2++) {
                        // int sum = getSum(r1, c1, r2, c2); // 사각형 (r1, c1) ~ (r2, c2)의 합 계산
                        int sum = prefixSum[r2 + 1][c2 + 1] - prefixSum[r1][c2 + 1] - prefixSum[r2 + 1][c1] + prefixSum[r1][c1];
                        if (sum > 10)
                            break;
                        // 사각형의 네 변이 모두 포함되어 있는지 확인
                        if (sum == 10 && isValid(r1, c1, r2, c2)) {
                            int size = (r2 - r1 + 1) * (c2 - c1 + 1); // 사각형의 크기 계산
                            for (int r = r1; r <= r2; r++)
                                for (int c = c1; c <= c2; c++) {
                                    if (getTerritoryBoard()[r][c] == currentPlayerTerritory)
                                        size--; // 현재 차례의 영토는 크기에 포함하지 않도록 조정
                                    else if (getTerritoryBoard()[r][c] == opponentTerritory)
                                        size++; // 현재 차례 상대방의 영토를 뺏을 경우 하나 더 추가하도록 조정
                                }

                            availableMoves.push_back({r1, c1, r2, c2, size}); // 유효한 사각형을 추가
                        }
                    }
                }
            }
        }

        return availableMoves;
    }
};










// 게임 상태를 관리하는 클래스
class Game {
private:
    vector<vector<int>> board; // 게임 보드 (2차원 벡터)
    bool first;                // 선공 여부
    bool passed;               // 마지막 턴에 패스했는지 여부

    vector<vector<int>> territoryBoard; // 영토 보드 (2차원 벡터)
    int myScore;                          // 내 점수
    int opponentScore;                  // 상대 점수

public:
    Game() {}

    Game(const vector<vector<int>> &board, bool first)
        : board(board), first(first), passed(false), myScore(0), opponentScore(0) {
            int rows = board.size();
            int cols = board[0].size();
            territoryBoard.resize(rows, vector<int>(cols, 0)); // 영토 보드 초기화
        }

    int minimax(MinimaxGameBoard gameBoard, int depth, bool isMyTurn, int alpha, int beta) {
        // 현재 점수 계산
        int boardScore = gameBoard.getBoardScore();

        // 최대 깊이에 도달했거나 게임이 끝났을 때 점수 반환
        if (depth == MAX_DEPTH) {
            return boardScore;
        }

        int bestScore = isMyTurn ? -1000+depth : 1000-depth; // 초기값 설정

        if (gameBoard.isPassed()) {
            if (isMyTurn && gameBoard.getBoardScore() > 0) {
                return 1000 - depth;
            } else if (!isMyTurn && gameBoard.getBoardScore() < 0) {
                return -1000 + depth;
            }
        }

        // 모든 가능한 사각형을 탐색
        vector<vector<int>> availableMoves = gameBoard.getAvailableMoves(isMyTurn);
        int moveCount = availableMoves.size();
        if (moveCount == 0) { // 유효한 사각형이 없으면
            if (gameBoard.getBoardScore() > 0) {
                return 1000 - depth; // 내 점수가 높으면 승리
            } else if (gameBoard.getBoardScore() < 0) {
                return -1000 + depth; // 상대 점수가 높으면 패배
            } else {
                return 0; // 무승부
            }
        }

        int searchSize = min(moveCount, SEARCH_SIZES[depth]); // 깊이에 따라 탐색 개수 제한

        // 사각형 크기를 기준으로 내림차순 정렬
        partial_sort(availableMoves.begin(), availableMoves.begin() + searchSize, availableMoves.end(), [](const vector<int> &a, const vector<int> &b) {
            return a[4] > b[4]; // 크기가 큰 순서로 정렬
        });

        for (int i = 0; i < searchSize; i++) {
            const vector<int> &move = availableMoves[i]; // 현재 사각형의 좌표
            int r1 = move[0], c1 = move[1], r2 = move[2], c2 = move[3];
            MinimaxGameBoard newGameboard = gameBoard;
            newGameboard.updateMove(r1, c1, r2, c2, isMyTurn); // 현재 수를 적용

            int score = minimax(newGameboard, depth + 1, !isMyTurn, alpha, beta);

            // cout << "Depth: " << depth << ", Move: (" << r1 << ", " << c1 << ") to (" << r2 << ", " << c2 << "), Score: " << score << endl;

            if (isMyTurn) {
                bestScore = max(bestScore, score);
                alpha = max(alpha, bestScore);
            }
            else {
                bestScore = min(bestScore, score);
                beta = min(beta, bestScore);
            }

            // Alpha-Beta Pruning 조건
            if (alpha >= beta)
                return bestScore;
        }

        if ((isMyTurn && gameBoard.getBoardScore() > 0) || (!isMyTurn && gameBoard.getBoardScore() < 0)) {
            MinimaxGameBoard newGameboard = gameBoard;
            newGameboard.setPassed(true); // 패스 상태로 설정
            int passedScore = minimax(newGameboard, depth + 1, !isMyTurn, alpha, beta);

            if (isMyTurn) {
                bestScore = max(bestScore, passedScore);
                alpha = max(alpha, bestScore);
            } else {
                bestScore = min(bestScore, passedScore);
                beta = min(beta, bestScore);
            }
        }


        return bestScore; // 최적의 수의 점수 반환
    }

    // ================================================================
    // ===================== [필수 구현] ===============================
    // 합이 10인 유효한 사각형을 찾아 {r1, c1, r2, c2} 벡터로 반환
    // 없으면 {-1, -1, -1, -1}을 반환하여 패스를 의미함
    // ================================================================
    vector<int> calculateMove(int myTime, int oppTime) {
        // 상대방이 패스를 했고 내 점수가 더 높으면 패스
        if (passed && myScore > opponentScore)
            return {-1, -1, -1, -1};

        // // 누적 합 배열을 만든다.
        // int rows = board.size();
        // int cols = board[0].size();
        // vector<vector<int>> prefixSum(rows + 1, vector<int>(cols + 1, 0));
        // for (int r = 1; r <= rows; r++)
        //     for (int c = 1; c <= cols; c++)
        //         prefixSum[r][c] = board[r - 1][c - 1] + prefixSum[r - 1][c] + prefixSum[r][c - 1] - prefixSum[r - 1][c - 1];

        vector<int> bestMove = {-1, -1, -1, -1}; // 초기값: 유효한 사각형이 없을 때 반환할 값
        int bestScore = -1000; // 최적의 수의 점수

        MinimaxGameBoard simBoard = MinimaxGameBoard(board, territoryBoard, passed, myScore, opponentScore);
        vector<vector<int>> availableMoves = simBoard.getAvailableMoves(true); // 현재 턴에 가능한 모든 사각형을 가져옴
        int searchSize = availableMoves.size();
        searchSize = min(searchSize, SEARCH_SIZES[0]); // 탐색 개수 제한

        partial_sort(availableMoves.begin(), availableMoves.begin() + searchSize, availableMoves.end(), [](const vector<int> &a, const vector<int> &b) {
            return a[4] > b[4]; // size. 크기가 큰 순서로 정렬
        });

        for (int i = 0; i < searchSize; i++)
        {
            const vector<int> &move = availableMoves[i]; // 현재 사각형의 좌표
            int r1 = move[0], c1 = move[1], r2 = move[2], c2 = move[3];
            MinimaxGameBoard newGameboard = simBoard;
            newGameboard.updateMove(r1, c1, r2, c2, true); // 현재 수를 적용

            int score = minimax(newGameboard, 1, false, -1000, 1000); // 현재 사각형을 적용한 후의 점수 계산
            // cout << "Valid Move: (" << r1 << ", " << c1 << ") to (" << r2 << ", " << c2 << "), Score: " << score << endl;
            if (score > bestScore) {
                bestScore = score; // 최적의 수의 점수 갱신
                bestMove = {r1, c1, r2, c2}; // 최적의 수 갱신
            }
        }

        if (myScore > opponentScore) {
            MinimaxGameBoard newGameboard = simBoard;
            newGameboard.setPassed(true); // 패스 상태로 설정
            int passedScore = minimax(newGameboard, 1, false, -1000, 1000);

            if (passedScore > bestScore) {
                bestScore = passedScore; // 최적의 수의 점수 갱신
                bestMove = {-1, -1, -1, -1}; // 최적의 수 갱신
            }
        }


        return bestMove; // 최적의 수 반환
    }
    // =================== [필수 구현 끝] =============================

    // 상대방의 수를 받아 보드에 반영
    void updateOpponentAction(const vector<int> &action, int time) {
        updateMove(action[0], action[1], action[2], action[3], false);
    }

    // 주어진 수를 보드에 반영 (칸을 0으로 지움)
    void updateMove(int r1, int c1, int r2, int c2, bool isMyMove) {
        if (r1 == -1 && c1 == -1 && r2 == -1 && c2 == -1) {
            passed = true;
            return;
        }

        const int currentPlayerTerritory = isMyMove ? MY_TERRITORY : OPPONENT_TERRITORY; // 업데이트하는 수의 플레이어 영토

        for (int r = r1; r <= r2; r++) {
            for (int c = c1; c <= c2; c++) {
                // 1. 아무도 차지하지 않은 영역을 차지하는 경우
                if (board[r][c] != 0) {
                    board[r][c] = 0; // 해당 칸을 차지
                    territoryBoard[r][c] = currentPlayerTerritory; // 영토 보드에 현재 플레이어의 반영
                    
                    isMyMove ? myScore++ : opponentScore++; // 현재 플레이어의 점수를 1 증가
                }
                // 2. 이미 누군가 차지한 영역을 빼앗으려는 경우
                else if (territoryBoard[r][c] != currentPlayerTerritory) { // 내 영토가 아니라면 (즉, 상대방 영토라면)
                    territoryBoard[r][c] = currentPlayerTerritory; // 내 영토로 변경
                    
                    if (isMyMove) {
                        myScore++;       // 내 점수는 1 증가
                        opponentScore--; // 상대 점수는 1 감소
                    } else {
                        myScore--;       // 내 점수는 1 감소
                        opponentScore++; // 상대 점수는 1 증가
                    }
                }
            }
        }
        passed = false;
    }
};

// 표준 입력을 통해 명령어를 처리하는 메인 함수
int main() {
    Game game;
    bool first = false;

    while (true) {
        string line;
        getline(cin, line);

        istringstream iss(line);
        string command;
        if (!(iss >> command))
            continue;

        if (command == "READY") {
            // 선공 여부 확인
            string turn;
            iss >> turn;
            first = (turn == "FIRST");
            cout << "OK" << endl;
            continue;
        }

        if (command == "INIT") {
            // 보드 초기화
            vector<vector<int>> board;
            string row;
            while (iss >> row)
            {
                vector<int> boardRow;
                for (char c : row)
                {
                    boardRow.push_back(c - '0'); // 문자 → 숫자 변환
                }
                board.push_back(boardRow);
            }
            game = Game(board, first);
            continue;
        }

        if (command == "TIME") {
            // 내 차례: 수 계산 및 출력
            int myTime, oppTime;
            iss >> myTime >> oppTime;

            vector<int> ret = game.calculateMove(myTime, oppTime);
            cout << ret[0] << " " << ret[1] << " " << ret[2] << " " << ret[3] << endl; // 내 행동 출력

            game.updateMove(ret[0], ret[1], ret[2], ret[3], true);
            continue;
        }

        if (command == "OPP") {
            // 상대 행동 반영
            int r1, c1, r2, c2, time;
            iss >> r1 >> c1 >> r2 >> c2 >> time;
            game.updateOpponentAction({r1, c1, r2, c2}, time);
            continue;
        }

        if (command == "FINISH") {
            // 게임 종료
            break;
        }

        // 알 수 없는 명령 처리
        cerr << "Invalid command: " << command << endl;
        return 1;
    }

    return 0;
}