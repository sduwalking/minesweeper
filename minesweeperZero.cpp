#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

int length, width, bombNum;

void difficultyOption(int n) {
    int difficulty = n;
    if (difficulty == 0) {
        length = 10;
        width = 10;
        bombNum = 10;
    } 
    else if (difficulty == 1) {
        length = 20;
        width = 20;
        bombNum = 60;
    } 
    else if (difficulty == 2) {
        length = 50;
        width = 25;
        bombNum = 260;
    } 
    else {
        length = 10;
        width = 10;
        bombNum = 10;
    }
}

struct Cell {
    bool isBomb;
    bool isReveal;
    bool isFlagged;
    bool isQuestion;
    int aroundBomb;

    Cell() {
        isBomb = false;
        isReveal = false;
        isFlagged = false;
        isQuestion = false;
        aroundBomb = 0;
    }

    char display() {
        if (isFlagged) return '!';
        if (isQuestion) return '?';
        if (!isReveal) return '-';
        if (isBomb) return '*';
        if (aroundBomb > 0) return '0' + aroundBomb;
        return ' ';
    }

    void reset() {
        isBomb = false;
        isReveal = false;
        isFlagged = false;
        isQuestion = false;
        aroundBomb = 0;
    }
};

class mineSweeper {
private:
    int rows;
    int cols;
    int bombNum;
    int flaggedNum;
    int revealNum;
    int totalBombs;
    bool gameOver;
    bool gameWin;

    vector<vector<Cell>> board;
    
    void placeBomb() {
        srand(time(0));
        int bombsPlaced = 0;
        
        while (bombsPlaced < bombNum) {
            int r = rand() % rows;
            int c = rand() % cols;
            
            if (!board[r][c].isBomb) {
                board[r][c].isBomb = true;
                bombsPlaced++;
            }
        }
        totalBombs = bombsPlaced;
    }

    void calculateBomb() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (board[i][j].isBomb) continue;
                
                int count = 0;
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        int ni = i + dx;
                        int nj = j + dy;
                        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                            if (board[ni][nj].isBomb) {
                                count++;
                            }
                        }
                    }
                }
                board[i][j].aroundBomb = count;
            }
        }
    }

    void revealBlankArea(int row, int col) {
        vector<pair<int, int>> stack;
        stack.push_back({row, col});
        
        while (!stack.empty()) {
            pair<int, int> current = stack.back();
            stack.pop_back();
            int r = current.first;
            int c = current.second;
            
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int ni = r + dx;
                    int nj = c + dy;
                    if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                        if (!board[ni][nj].isReveal && !board[ni][nj].isFlagged && 
                            !board[ni][nj].isQuestion) {
                            
                            board[ni][nj].isReveal = true;
                            revealNum++;
                            
                            if (board[ni][nj].aroundBomb == 0) {
                                stack.push_back({ni, nj});
                            }
                        }
                    }
                }
            }
        }
    }

public:
    mineSweeper(int r, int c, int b) : rows(r), cols(c), bombNum(b) {
        flaggedNum = 0;
        revealNum = 0;
        totalBombs = 0;
        gameOver = false;
        gameWin = false;
        
        board.resize(rows, vector<Cell>(cols));
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                board[i][j] = Cell();
            }
        }
        
        placeBomb();
        calculateBomb();
    }

    void display() {
        cout << "   ";
        for (int j = 0; j < cols; j++) {
            if (j < 10) cout << " " << j << " ";
            else cout << j << " ";
        }
        cout << endl;
        
        for (int i = 0; i < rows; i++) {
            if (i < 10) cout << " " << i << " ";
            else cout << i << " ";
            
            for (int j = 0; j < cols; j++) {
                Cell& cell = board[i][j];
                if (cell.isFlagged) {
                    cout << "|!|";
                } else if (cell.isQuestion) {
                    cout << "|?|";
                } else if (!cell.isReveal) {
                    cout << "|-|";
                } else if (cell.isBomb) {
                    cout << "|*|";
                } else {
                    if (cell.aroundBomb > 0) {
                        cout << "|" << cell.aroundBomb << "|";
                    } else {
                        cout << "| |";
                    }
                }
            }
            cout << endl;
        }
        
        cout << "\nbomb remining: " << bombNum - flaggedNum << " flagged: " << flaggedNum 
             << " revealed: " << revealNum << "/" << rows*cols - bombNum << endl;
        if (gameOver) cout << "game over\n";
        if (gameWin) cout << "you win\n";
    }

    bool reveal(int row, int col) {
        if (row < 0 || row >= rows || col < 0 || col >= cols) 
            return false;

        Cell& cell = board[row][col];

        if (cell.isFlagged || cell.isReveal || cell.isQuestion)
            return false;

        cell.isReveal = true;
        revealNum++;

        if (cell.isBomb) {
            gameOver = true;
            return true;
        }

        if (cell.aroundBomb == 0) {
            revealBlankArea(row, col);
        }
        
        checkWin();
        return false;
    }

    void flag(int row, int col) {
        if (row < 0 || row >= rows || col < 0 || col >= cols) return;
        
        Cell& cell = board[row][col];
        if (cell.isReveal) return;

        if (!cell.isFlagged && !cell.isQuestion) {
            cell.isFlagged = true;
            flaggedNum++;
        } else if (cell.isFlagged) {
            cell.isFlagged = false;
            flaggedNum--;
        } else if (cell.isQuestion) {
            cell.isQuestion = false;
            cell.isFlagged = true;
            flaggedNum++;
        }
        
        checkWin();
    }

    void question(int row, int col) {
        if (row < 0 || row >= rows || col < 0 || col >= cols) return;
        
        Cell& cell = board[row][col];
        if (cell.isReveal) return;

        if (!cell.isFlagged && !cell.isQuestion) {
            cell.isQuestion = true;
        } else if (cell.isQuestion) {
            cell.isQuestion = false;
        } else if (cell.isFlagged) {
            cell.isFlagged = false;
            cell.isQuestion = true;
            flaggedNum--;
        }
    }
    
    void checkWin() {
        if (gameOver) return;
        
        int correctFlags = 0;
        int totalCells = rows * cols;
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                Cell& cell = board[i][j];
                
                if (cell.isBomb && cell.isFlagged) {
                    correctFlags++;
                }
                
                if (cell.isBomb && cell.isReveal) {
                    return;
                }
            }
        }
        
        if (correctFlags == bombNum) {
            gameWin = true;
            return;
        }
        
        if (revealNum == totalCells - bombNum) {
            gameWin = true;
        }
    }
    
    bool isGameOver() { return gameOver; }
    bool isGameWin() { return gameWin; }
    int getRemainingBombs() { return bombNum - flaggedNum; }
    
    void revealAll() {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                board[i][j].isReveal = true;
            }
        }
    }
};

void printMenu() {
    cout << "=========================\n";
    cout << "reveal (r rows cols) ---r\n";
    cout << "flag (f rows cols) -----f\n";
    cout << "question (q rows cols) -q\n";
    cout << "restart ----------------4\n";
    cout << "display ----------------5\n";
    cout << "quit -------------------6\n";
    cout << "=========================\n";
}

int main() {
    int diff;
    cout << "please input difficulty(0-2)\n";
    cout << "easy(0) ------ 10x10, 10 bomb\n";
    cout << "normal(1) ---- 20x20, 60 bomb\n";
    cout << "difficult(2) - 50x25, 260 bomb\n";
    
    while (true) {
        cin >> diff;
        if (diff >= 0 && diff <= 2) break;
        cout << "please input valid difficulty (0-2): ";
    }
    
    difficultyOption(diff);
    
    mineSweeper game(length, width, bombNum);
    bool running = true;
    
    cout << "\nboard " << length << "x" << width 
         << " bomb remining: " << bombNum << endl;
    
    while (running) {
        game.display();
        if (game.isGameOver() || game.isGameWin()) {
            game.revealAll();
            game.display();
            
            cout << "again?(y/n): ";
            char restart;
            cin >> restart;
            if (restart == 'y' || restart == 'Y') {
                mineSweeper newGame(length, width, bombNum);
                game = newGame;
                continue;
            } else {
                break;
            }
        }
        
        printMenu();
        
        char cmd;
        int row, col;
        cout << "\ninput command: ";
        cin >> cmd;
        
        if (cmd == '6') {
            running = false;
            cout << "game over\n";
            continue;
        }
        
        if (cmd == '4') {
            cout << "restarting...\n";
            mineSweeper newGame(length, width, bombNum);
            game = newGame;
            continue;
        }
        
        if (cmd == '5') {
            continue;
        }
        
        if (cmd == 'r' || cmd == 'f' || cmd == 'q') {
            cout << "input row and column: \n";
            cin >> row >> col;
            
            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "invalid input\n";
                continue;
            }
        }
        
        switch (cmd) {
            case 'r':
                if (game.reveal(row, col)) {
                    cout << "you hit a bomb!\n";
                }
                break;
            case 'f':
                game.flag(row, col);
                break;
            case 'q':
                game.question(row, col);
                break;
            default:
                cout << "invalid command\n";
        }
    }
    
    cout << "thanks for playing!\n";
    return 0;
}