#include "ConnectFour.h"

ConnectFour::ConnectFour() {
    _grid = new Grid(7, 6);
}

ConnectFour::~ConnectFour() {
    delete _grid;
}

// yellow or red piece
Bit* ConnectFour::PieceForPlayer(const int playerNumber) {
    Bit *bit = new Bit();
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "yellow.png" : "red.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void ConnectFour::setUpBoard() {
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(70, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}

bool ConnectFour::actionForEmptyHolder(BitHolder &holder) {
    if (holder.bit()) {
        return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
    if (bit) {
        int x = ((int)holder.getPosition().x - 35) / 70;
        int targetRow = -1;
        for (int row = 5; row >= 0; row--) {
            if (_grid->getSquare(x,row)->bit() == nullptr) {
                targetRow = row;
                break;
            }
        }
        if (targetRow != -1) {
            BitHolder &finalHolder = *_grid->getSquare(x,targetRow);
            finalHolder.setBit(bit);
            bit->setPosition(finalHolder.getPosition());
            endTurn();
            return true;
        };
    }
    return false;
}

bool ConnectFour::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move anything in tic tac toe
    return false;
}

bool ConnectFour::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // you can't move anything in tic tac toe
    return false;
}

void ConnectFour::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* ConnectFour::ownerAt(int index ) const
{
    auto square = _grid->getSquare(index % 7, index / 7);
    if (!square || !square->bit()) {
        return nullptr;
    }
    if (index < 0 || index >= 42) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* ConnectFour::checkForWinner()
{   
    bool isWinner = false;
    // iterate through every slot on the board
    for (int y=0; y<6; y++) {
        for (int x=0; x<7; x++) {
            // now, ideally, for each square, if there is a piece then we want
            // to check each of the neighboring cells (kinda like minesweeper)
            // using corresponding indices. I saw a similar technique in the
            // chess Sebastian Lague video
            Player* candidate = ownerAt(y*7 + x);
            // only proceed if there is a piece in the current slot
            if (!candidate) {
                continue;
            }
            int candidateIndex = y*7 + x;
            isWinner = verifyCandidate(candidateIndex);
            if (isWinner) {
                return candidate;   // found a winner
            }
        }
    }
    return nullptr;
}

// helper function to verify candidate pieces on the connect 4 board
bool ConnectFour::verifyCandidate(int cIndex) {
    // iterate through all of the neighbors
    for (int y=-1; y<=1; y++) {
        for (int x=-1; x<=1; x++) {
            if (y == 0 && x == 0) continue; // skip the original character index
            int col     = cIndex % 7;
            int row     = cIndex / 7;

            int targetRow = row + (y * 3);
            int targetCol = col + (x * 3);

            // cleaner way to do what i was doing earlier, no big gross if statement
            if (targetRow < 0 || targetRow > 5 || targetCol < 0 || targetCol > 6) {
                continue;
            }

            // we have now verified that the direction we are checking is valid
            // and can proceed with checking if the player has won
            int offset = y*7 + x;
            Player* owner = ownerAt(cIndex);
            if ( // check for 4 in a row in the given direction
                owner &&
                owner == ownerAt(cIndex + offset) &&
                owner == ownerAt(cIndex + offset*2) &&
                owner == ownerAt(cIndex + offset*3)
            ) return true;
        }
    }
    return false;
}

bool ConnectFour::checkForDraw()
{
    bool isDraw = true;
    // check to see if the board is full
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
}

std::string ConnectFour::initialStateString() {
    return "000000000000000000000000000000000000000000"; // 42 zeroes
}

std::string ConnectFour::stateString()
{
    std::string s = "000000000000000000000000000000000000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 7 + x] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

void ConnectFour::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y*7 + x;
        int playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit( PieceForPlayer(playerNumber-1) );
        } else {
            square->setBit( nullptr );
        }
    });
}

void ConnectFour::updateAI() 
{
    // negamax AI

    std::string currentState = stateString();

    int bestMove = -10000000;
    int bestSquare = -1;

    // move through each column and determine best move
    int columns[7] = {3,2,4,1,5,0,6};
    for (int x = 0; x <= 6; x++) {
        // check each row starting from the bottom, if it's empty, then place it
        for (int row = 5; row >= 0; row--) {
            int index = columns[x] + row*7;
            if (currentState[index] == '0') {
                currentState[index] = '2';
                int newValue = -negamax(currentState, 0, -10000000, 10000000, HUMAN_PLAYER);
                currentState[index] = '0';
                if (newValue > bestMove) {
                    bestSquare = index;
                    bestMove = newValue;
                }
                break;
            }
        }
    }

    if (bestSquare != -1) {
        // make a move based on our best square
        actionForEmptyHolder(*_grid->getSquare(bestSquare%7, bestSquare/7));
    }
}

bool ConnectFour::aiTestForTerminal(std::string& state) {            // helper for the AI to find if the board is terminal using a state string
    if (state.find('0') == std::string::npos) {         // string's find function will either return the position or return npos for no existence.
        return true;        // terminal state reached
    }
    return false;           // no terminal state
}

//
// same as checkForWinner() and verifyCandidate() above except it uses the state string instead of the grid object
//
int ConnectFour::aiBoardEval(std::string& state, int playerColor, int depth) {                   // helper for the AI to find if the board is terminal
    char playerChar = (playerColor == HUMAN_PLAYER) ? '1' : '2';
    for (int y=0; y<=5; y++) {
        for (int x=0; x<=6; x++) {
            int index = x + y*7;
            char candidate = state[index];
            // only proceed if there is a piece in the current slot
            if (candidate == '0') {
                continue;
            }
            // verifyCandidate() except it uses state string
            for (int yDir=-1; yDir<=1; yDir++) {
                for (int xDir=-1; xDir<=1; xDir++) {
                    if (yDir == 0 && xDir == 0) continue; // skip the original character index
                    int col     = index % 7;
                    int row     = index / 7;

                    int targetRow = row + (yDir * 3);
                    int targetCol = col + (xDir * 3);

                    // cleaner way to do what i was doing earlier, no big gross if statement
                    if (targetRow < 0 || targetRow > 5 || targetCol < 0 || targetCol > 6) {
                        continue;
                    }

                    int offset = yDir*7 + xDir;
                    char owner = state[index];
                    if ( // check for 4 in a row in the given direction
                        owner != '0' &&
                        owner == state[index + offset] &&
                        owner == state[index + offset*2] &&
                        owner == state[index + offset*3]
                    ) {
                        int score = 100 - depth;
                        return (owner == playerChar) ? score : -score;
                    };
                }
            }
        }
    }
    return 0;
}

int ConnectFour::negamax(std::string& state, int depth, int alpha, int beta, int playerColor) {
    // std::cout << depth << std::endl;
    // depth limit because i don't trust this thing

    if (depth > 5) {
        return 0;
    }
    
    int score = aiBoardEval(state, playerColor, depth);
    if (score != 0) {
        return score; 
    }

    if (aiTestForTerminal(state)) {     // draw check
        return 0;                       
    }

    int bestVal = -100000;
    int columns[7] = {3,2,4,1,5,0,6};
    for (int x = 0; x < 7; x++) {
        for (int row = 5; row >= 0; row--) {
            int i = columns[x] + row*7;
            if (state[i] == '0') {
                state[i] = playerColor == HUMAN_PLAYER ? '1' : '2';     // push move
                int newVal = -negamax(state, depth + 1, -beta, -alpha, -playerColor);
                state[i] = '0';         // pop move
                if (newVal > bestVal) {
                    bestVal = newVal;
                }
                // alpha beta pruning
                if (newVal > alpha) {
                    alpha = newVal;
                }
                if (alpha >= beta) break;
                break;
            }
        }
    }
    return bestVal;
}
