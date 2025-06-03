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
            std::vector<int> portalIn;                  // Portal in    ----->  Portal out
            std::vector<std::vector<int>> portalOut;    // Portal out   ----->  move position
        };
        
        MoveValidator(ChessBoard& _board);
        bool isValidMove(Types::Position from, Types::Position to);
        std::vector<Types::Position> getValidMoveList(Types::Position pos);
        std::vector<Types::Position> validateBoardCheckStatus(Types::Color color);
        std::vector<Types::Position> testMoveForAttack(Types::Position from, Types::Position to, Types::Color enemyColor);

    private:
        ChessBoard& board;

        bool isValidSquare(Types::Position sq);
        Types::Color getSquareOwner(Types::Position sq);
        bool SquareAttack(std::vector<Types::Position> &ret, Types::Piece piece, Types::Position pos, bool onlyCapture, bool noEat);
        void createMovement(std::vector<Types::Position> &ret, Types::Piece piece, Types::Position from, int length, int step_x, int step_y, bool onlyCapture, bool noEat=false);
        MoveValidator::MovAdjL createMovementGraph(Types::Position from);
        void removeInvalidMovement(Types::Piece piece, Types::Position from, std::vector<Types::Position> &posList);
        std::vector<Types::Position> createMovementList(Types::Piece piece, Types::Position from);
        std::vector<Types::Position> createSafeMovementList(Types::Piece piece, Types::Position from);
        void DumpMovementList (MoveValidator::MovAdjL adjList);
        std::vector<Types::Position> checkAttack(Types::Position pos, Types::Color enemyColor);

};

