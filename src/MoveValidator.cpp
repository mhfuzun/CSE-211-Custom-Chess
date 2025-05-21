#include "Types.hpp"
#include "MoveValidator.hpp"
#include "ChessBoard.hpp"

#include <vector>

bool isValidMove(Types::Position from, Types::Position to) {

}

std::vector<Types::Movement> createMovementList(Types::Piece piece) {
    
}

std::vector<Types::Position> createMovementTree(ChessBoard &chessBoard, Types::Position from, std::vector<Movement> movement) {
    std::vector<Types::Position> ret;
}

void createMovementTree_recursion(ChessBoard &chessBoard, Types::Position from, std::vector<Types::Position> &ret, Movement mov) {
    Types::Position newPos;

    if (mov == mov.type == MovementType::forward) {
        if (mov > 0) {
            newPos
            ret.push_back();
            createMovementTree_recursion(chessBoard, newPos, ret, mov--);
        } else {
            return ret;
        }
    }
}