#pragma once
#include "Types.hpp"
#include "ChessBoard.hpp"
#include "Portal.hpp"

#include <vector>
#include <array>

class MoveValidator {
    public:
        struct PortalInConf {
            int portalOutID;
            int portalID;
        };

        enum class MovementType {
            none,
            forward, 
            sideways, 
            diagonal, 
            l_shape,
            diagonal_capture, 
            first_move_forward, 
            castling_long, 
            castling_short,
            portal_in,
            portal_out
        };

        struct MovementData
        {
            MovementType type;
            Types::Position pos;
        };

        struct MoveResult {
            bool valid;
            MovementType type;
            bool portalUse;
            int portalID;
        };

        struct MovAdjL
        {
            std::vector<MovementData> pos_list;      // includes portal points and standart square points
            std::vector<int> moveList;
            std::vector<PortalInConf> portalIn;         // Portal in    ----->  Portal out
            std::vector<std::vector<int>> portalOut;    // Portal out   ----->  move position
        };
        
        MoveValidator(ChessBoard& _board);
        MoveResult isValidMove(Types::Position from, Types::Position to);
        std::vector<Types::Position> getValidMoveList(Types::Position pos);
        std::vector<Types::Position> validateBoardCheckStatus(Types::Color color);
        std::vector<Types::Position> testMoveForAttack(Types::Position from, Types::Position to, Types::Color color);
        std::vector<Types::Position> calculateAllMovementForAColor(Types::Color color);
        std::vector<Types::Position> getAllPositionsInCheckmat(Types::Color color);
        bool isReadyForPoromotion(Types::Piece piece, Types::Position pos);
        std::array<Types::Position, 2> getRookPositionWhenCastling(Types::Color color, MoveValidator::MovementType castlingType);

    private:
        ChessBoard& board;

        bool isValidSquare(Types::Position sq);
        Types::Color getSquareOwner(Types::Position sq);
        bool SquareAttack(MovementType movType, std::vector<MovementData> &ret, Types::Piece piece, Types::Position pos, bool onlyCapture, bool noEat);
        void createMovement(MoveValidator::MovementType movType, std::vector<MoveValidator::MovementData> &ret, Types::Piece piece, Types::Position from, int length, int step_x, int step_y, bool onlyCapture, bool noEat=false);
        void removeInvalidMovement(Types::Piece piece, Types::Position from, std::vector<MovementData> &posList);
        std::vector<MovementData> createMovementList(Types::Piece piece, Types::Position from);
        std::vector<MovementData> createSafeMovementList(Types::Piece piece, Types::Position from);
        MovAdjL createMovementGraph(Types::Position from);
        void DumpMovementList (MovAdjL adjList);
        std::vector<Types::Position> checkAttack(Types::Position pos, Types::Color enemyColor);
        bool isValidCastling(Types::Color color, std::string type);

};
