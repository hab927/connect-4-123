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
        int x = (holder.getPosition().x - 35) / 70;
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
    return square->bit()->getOwner();
}

Player* ConnectFour::checkForWinner()
{   
    bool isWinner = false;
    // iterate through every slot on the board
    for (int y=0; y<7; y++) {
        for (int x=0; x<8; x++) {
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
            int column  = cIndex % 7;
            int row     = cIndex / 7;

            if (
                // y is 0? don't need to check vertical validity
                // y is negative? check if row - 3 is below 0, if so, OOB
                // vice versa for positive
                y == 0 ? false : ((y == -1 ? row + y*3 <= 0 : row + y*3 > 6)) ||
                // x is 0? no need again for horizontal
                // x is negative? check if column - 3 is below 0, if yes, OOB
                // vice versa for positive
                x == 0 ? false : ((x == -1 ? column + x*3 <= 0 : column + x*3 > 7))
            ) {
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
            ) {
                std::cout << cIndex << "," << cIndex + offset << "," << cIndex + offset*2 << "," << cIndex + offset*3 << std::endl;
                return true;
            };
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

// just pasted the TTT AI code for now
void ConnectFour::updateAI() 
{
    BitHolder* bestMove = nullptr;
    std::string state = stateString();

    // Traverse all cells, evaluate minimax function for all empty cells
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 3 + x;
        // Check if cell is empty
        if (state[index] == '0') {
            bestMove = square;
        }
    });


    // Make the best move
    if(bestMove) {
        if (actionForEmptyHolder(*bestMove)) {
        }
    }
}
