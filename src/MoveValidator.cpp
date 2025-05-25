#include "Types.hpp"
#include "MoveValidator.hpp"
#include "ChessBoard.hpp"

#include <vector>
#include <algorithm> // remove_if

MoveValidator::MoveValidator(ChessBoard& _board) : board(_board) {

}

bool MoveValidator::isValidMove(Types::Position from, Types::Position to) {
    MoveValidator::MovAdjL adjL = createMovementGraph(from);

    // DumpMovementList(adjL.pos_list);

    for (auto j : adjL.moveList) {
        if (adjL.pos_list[j] == to) {
            return true;
        }
    }

    return false;
}

bool MoveValidator::isValidSquare(Types::Position sq) {
    int boardSize = board.getBoardSize();

    if (sq.x < 0 || sq.x >= boardSize) return false;
    if (sq.y < 0 || sq.y >= boardSize) return false;

    return true;
}

Types::Color MoveValidator::getSquareOwner(Types::Position sq) {
    Types::Piece piece;

    piece = board.getPieceAt(sq);

    return piece.color;
}

void MoveValidator::createMovement(std::vector<Types::Position> &ret, Types::Color color, Types::Position from, int length, int step_x, int step_y, bool onlyCapture, bool noEat) {
    Types::Position newPos;

    newPos.x = from.x;
    newPos.y = from.y;

    while (length--) {
        newPos.x += step_x;
        newPos.y += step_y;

        if (!isValidSquare(newPos)) break;

        Types::Color targetColor = getSquareOwner(newPos);
        
        if (targetColor == color)
            break;
        else if (targetColor == Types::Color::EMPTY) {
            if (onlyCapture) break;

            ret.push_back(newPos);
        }
        else {
            if (noEat) break;

            ret.push_back(newPos);
            break;
        }
    }
}

std::vector<Types::Position> MoveValidator::createMovementList(Types::Position from) {

    std::vector<Types::Position> ret;
    Types::Piece piece;

    piece = board.getPieceAt(from);

    // forward
    if (piece.movement.forward > 0) {
        if (piece.movement.backward || (piece.color == Types::Color::WHITE)) {
            createMovement(ret, piece.color, from, piece.movement.forward, 0, +1, false, (piece.movement.diagonal_capture > 0));
        }
        
        if (piece.movement.backward || (piece.color == Types::Color::BLACK)) {
            createMovement(ret, piece.color, from, piece.movement.forward, 0, -1, false, (piece.movement.diagonal_capture > 0));
        }
    }

    // sideways
    if (piece.movement.sideways > 0) {
        createMovement(ret, piece.color, from, piece.movement.sideways, +1, 0, false);
        createMovement(ret, piece.color, from, piece.movement.sideways, -1, 0, false);
    }

    // diagonal 
    if (piece.movement.diagonal > 0) {
        createMovement(ret, piece.color, from, piece.movement.diagonal, +1, +1, false);
        createMovement(ret, piece.color, from, piece.movement.diagonal, +1, -1, false);
        createMovement(ret, piece.color, from, piece.movement.diagonal, -1, +1, false);
        createMovement(ret, piece.color, from, piece.movement.diagonal, -1, -1, false);
    }
    
    // l_shape
    std::vector<Types::Position> LposArray = {
        {from.x + 1, from.y + 2},
        {from.x - 1, from.y + 2},
        {from.x + 2, from.y + 1},
        {from.x - 2, from.y + 1},

        {from.x + 2, from.y - 1},
        {from.x - 2, from.y - 1},
        {from.x - 1, from.y - 2},
        {from.x + 1, from.y - 2}
    };

    if (piece.movement.l_shape) {
        for (auto Lpos : LposArray) {
            if (isValidSquare(Lpos) && (getSquareOwner(Lpos) != piece.color)) {
                ret.push_back(Lpos);
            }
        }
    }

    // diagonal_capture
    if (piece.movement.diagonal_capture > 0) {
        if (piece.color == Types::Color::WHITE) {
            createMovement(ret, piece.color, from, piece.movement.diagonal_capture, +1, +1, true);
            createMovement(ret, piece.color, from, piece.movement.diagonal_capture, -1, +1, true);
        }

        if (piece.color == Types::Color::BLACK) {
            createMovement(ret, piece.color, from, piece.movement.diagonal_capture, +1, -1, true);
            createMovement(ret, piece.color, from, piece.movement.diagonal_capture, -1, -1, true);
        }
    }

    // first_move_forward
    if (piece.firstMove && piece.movement.first_move_forward) {
        if (piece.color == Types::Color::WHITE) {
            createMovement(ret, piece.color, from, piece.movement.first_move_forward, 0, +1, false, true);
        }

        if (piece.color == Types::Color::BLACK) {
            createMovement(ret, piece.color, from, piece.movement.first_move_forward, 0, -1, false, true);
        }
    }
    
    // SpecialAbilities.castling
    // ChessBoard üzerinden "check" yapılmalı. fonksiyon tam eklenmedi.

    // Ignore repeated positions
    std::vector<Types::Position> posList;
    bool founded;
    for (auto pos : ret) {
        founded = false;
        for (auto safe : posList) {
            if (safe == pos) {
                founded = true;
                break;
            }
        }

        if (!founded) posList.push_back(pos);
    }

    // SpecialAbilities.royal
    if (piece.special_abilities.royal) {
        posList.erase(
            std::remove_if(posList.begin(), posList.end(),
            [&](const Types::Position& pos) {
                return checkMatForPos(pos, Types::getEnemyColor(piece.color));
            }),
            posList.end()
        );
    }

    return posList;
}

MoveValidator::MovAdjL MoveValidator::createMovementGraph(Types::Position from) {
    MovAdjL adjL;
    adjL.pos_list = createMovementList(from);

    for (long unsigned int i=0; i<adjL.pos_list.size(); i++) {
        adjL.moveList.push_back(i);
    }

    return adjL;
}

std::vector<Types::Position> MoveValidator::getValidMoveList(Types::Position pos) {
    return createMovementList(pos);
}

void MoveValidator::DumpMovementList (std::vector<Types::Position> list) {
    std::cout << "Movement List: " << std::endl;
    for (auto move : list) {
        // std::cout << "  (" << move.x << ", " << move.y << ")" << std::endl;
        std::cout << move.toString(board.getBoardSize()) << " ";
    }
    std::cout << std::endl;
}

std::optional<Types::Position> MoveValidator::checkMatForPos(Types::Position pos, Types::Color enemyColor) {
    for (int x=0; x<board.getBoardSize(); x++) {
        for (int y=0; y<board.getBoardSize(); y++) {
            Types::Position piecePos = {x, y};

            Types::Piece piece = board.getPieceAt(piecePos);
            if ((piece.type != Types::NULL_PIECE_TYPE) && (!piece.special_abilities.royal) && (piece.color == enemyColor)) {
                std::vector<Types::Position> targetSq = createMovementList(piecePos);

                for (const auto& cPos : targetSq) {
                    if (cPos == pos) return piecePos;
                }
            }
        }
    }

    return std::nullopt;
}
