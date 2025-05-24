#pragma once
#include "Types.hpp"
#include "ChessBoard.hpp"

#include <vector>

class MoveValidator {
    public:
        struct MovAdjL
        {
            std::vector<Types::Position> pos_list;      // includes portal points and standart square points
            std::vector<int> moveList;
            std::vector<std::vector<int>> portalIn;     // Portal in    ----->  Portal out
            std::vector<std::vector<int>> portalOut;    // Portal out   ----->  move position
        };
        
        MoveValidator(ChessBoard& _board);
        bool isValidMove(Types::Position from, Types::Position to);
        std::vector<Types::Position> getValidMoveList(Types::Position pos);

    private:
        ChessBoard& board;

        bool isValidSquare(Types::Position sq);
        Types::Color getSquareOwner(Types::Position sq);
        void createMovement(std::vector<Types::Position> &ret, Types::Color color, Types::Position from, int length, int step_x, int step_y, bool onlyCapture);
        MoveValidator::MovAdjL createMovementGraph(Types::Position from);
        std::vector<Types::Position> createMovementList(Types::Position from);
        void DumpMovementList (std::vector<Types::Position> list);

};

