#pragma once
#include "Game.h"

// connect four header file

class ConnectFour : public Game {
    public:
        ConnectFour();
        ~ConnectFour();

        void        setUpBoard() override;
        Player*     checkForWinner() override;
        bool        checkForDraw() override;
        std::string initialStateString() override;
        std::string stateString() override;
        void        setStateString(const std::string &s) override;
        bool        actionForEmptyHolder(BitHolder &holder) override;
        bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
        bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
        void        stopGame() override;

        void        updateAI() override;
        bool        gameHasAI() override { return true; } // Set to true when AI is implemented
        Grid*       getGrid() override { return _grid; }
    private:
        Bit*        PieceForPlayer(const int playerNumber);
        Player*     ownerAt(int index) const;
        bool        verifyCandidate(int cIndex);
        bool        aiTestForTerminal(std::string& state);
        int         aiBoardEval(std::string& state, int playerColor, int depth);
        int         negamax(std::string& state, int depth, int alpha, int beta, int playerColor);
        int         evaluatePositionalAdvantage(std::string& state, int playerColor);
        Grid*       _grid;
};