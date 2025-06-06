#include "Types.hpp"
#include "MoveValidator.hpp"
#include "ChessBoard.hpp"

#include <vector>
#include <array>
#include <algorithm> // remove_if

MoveValidator::MoveValidator(ChessBoard& _board) : board(_board) {

}

MoveValidator::MoveResult MoveValidator::isValidMove(Types::Position from, Types::Position to) {
    ////////////////////////////////////////////////////////////
    if (!isValidSquare(from) || !isValidSquare(to)) 
        return {
            false,
            MoveValidator::MovementType::none,
            false,
            -1
        };

    ////////////////////////////////////////////////////////////
    MoveValidator::MovAdjL adjL = createMovementGraph(from);

    // DumpMovementList(adjL);

    for (auto j : adjL.moveList) {
        if (adjL.pos_list[j].pos == to) {
            return {
                true,
                adjL.pos_list[j].type,
                false,
                -1
            };
        }
    }

    for (auto portalIn : adjL.portalIn) {
        for (auto portalOut : adjL.portalOut[portalIn.portalOutID]) {
            MoveValidator::MovementData moveData = adjL.pos_list[portalOut];
            if (moveData.pos == to) {
                return {
                    true,
                    moveData.type,
                    true,
                    portalIn.portalID
                };
            }
        }
    }

    return {
        false,
        MoveValidator::MovementType::none,
        false,
        -1
    };
}

std::vector<Types::Position> MoveValidator::getValidMoveList(Types::Position pos) {
    std::vector<Types::Position> ret;
    for (const auto &movData : createMovementGraph(pos).pos_list) {
        ret.push_back(movData.pos);
    }

    return ret;
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

    if (piece.type == Types::NULL_PIECE_TYPE) return Types::Color::EMPTY;

    return piece.color;
}

// True dönerse -> iterasyon tamamlandı
// False dönerse -> devam et
bool MoveValidator::SquareAttack(MoveValidator::MovementType movType, std::vector<MoveValidator::MovementData> &ret, Types::Piece piece, Types::Position pos, bool onlyCapture, bool noEat) {
    if (!isValidSquare(pos)) return true;
    
    Types::Color targetColor = getSquareOwner(pos);
        
    if (targetColor == piece.color) {
        // std::cout << "Att: " << pos.toString(8) << " -> same color, color: " << ((targetColor == Types::Color::WHITE) ? "White" : "Black") << std::endl;
        return true;
    } else if (targetColor == Types::Color::EMPTY) {
        // std::cout << "Att: " << pos.toString(8) << " -> empty, capture: " << onlyCapture << std::endl;

        if (onlyCapture) return true;

        ret.push_back({movType, pos});
    } else {
        // std::cout << "Att: " << pos.toString(8) << " -> enemy, noEat: " << noEat << std::endl;

        if (noEat) return true;

        ret.push_back({movType, pos});
        return true;
    }

    return false;
}

void MoveValidator::createMovement(MoveValidator::MovementType movType, std::vector<MoveValidator::MovementData> &ret, Types::Piece piece, Types::Position from, int length, int step_x, int step_y, bool onlyCapture, bool noEat) {
    Types::Position newPos;

    newPos.x = from.x;
    newPos.y = from.y;

    while (length--) {
        newPos.x += step_x;
        newPos.y += step_y;
        
        if (!isValidSquare(newPos)) break;
       
        if (SquareAttack(movType, ret, piece, newPos, onlyCapture, noEat))
            break;
    }
}

void MoveValidator::removeInvalidMovement(Types::Piece piece, Types::Position from, std::vector<MoveValidator::MovementData> &posList) {
    posList.erase(
        std::remove_if(posList.begin(), posList.end(),
        [&](const MoveValidator::MovementData& movData) {
            // size sıfır değilse hamlede şah durumu oluşuyor -> bunu alın buradan.
            return (testMoveForAttack(from, movData.pos, piece.color).size() != 0);
        }),
        posList.end()
    );
}

std::vector<MoveValidator::MovementData> MoveValidator::createMovementList(Types::Piece piece, Types::Position from) {

    std::vector<MoveValidator::MovementData> ret;
    
    // forward
    if (piece.movement.forward > 0) {
        if (piece.movement.backward || (piece.color == Types::Color::WHITE)) {
            createMovement(MoveValidator::MovementType::forward, ret, piece, from, piece.movement.forward, 0, +1, false, (piece.movement.diagonal_capture > 0));
        }
        
        if (piece.movement.backward || (piece.color == Types::Color::BLACK)) {
            createMovement(MoveValidator::MovementType::forward, ret, piece, from, piece.movement.forward, 0, -1, false, (piece.movement.diagonal_capture > 0));
        }
    }

    // sideways
    if (piece.movement.sideways > 0) {
        createMovement(MoveValidator::MovementType::sideways, ret, piece, from, piece.movement.sideways, +1, 0, false);
        createMovement(MoveValidator::MovementType::sideways, ret, piece, from, piece.movement.sideways, -1, 0, false);
    }

    // diagonal 
    if (piece.movement.diagonal > 0) {
        createMovement(MoveValidator::MovementType::diagonal, ret, piece, from, piece.movement.diagonal, +1, +1, false);
        createMovement(MoveValidator::MovementType::diagonal, ret, piece, from, piece.movement.diagonal, +1, -1, false);
        createMovement(MoveValidator::MovementType::diagonal, ret, piece, from, piece.movement.diagonal, -1, +1, false);
        createMovement(MoveValidator::MovementType::diagonal, ret, piece, from, piece.movement.diagonal, -1, -1, false);
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
            SquareAttack(MoveValidator::MovementType::l_shape, ret, piece, Lpos, false, false);
        }
    }

    // diagonal_capture
    if (piece.movement.diagonal_capture > 0) {
        if (piece.color == Types::Color::WHITE) {
            createMovement(MoveValidator::MovementType::diagonal_capture, ret, piece, from, piece.movement.diagonal_capture, +1, +1, true);
            createMovement(MoveValidator::MovementType::diagonal_capture, ret, piece, from, piece.movement.diagonal_capture, -1, +1, true);
        }

        if (piece.color == Types::Color::BLACK) {
            createMovement(MoveValidator::MovementType::diagonal_capture, ret, piece, from, piece.movement.diagonal_capture, +1, -1, true);
            createMovement(MoveValidator::MovementType::diagonal_capture, ret, piece, from, piece.movement.diagonal_capture, -1, -1, true);
        }
    }

    // first_move_forward
    if (piece.firstMove && piece.movement.first_move_forward) {
        if (piece.color == Types::Color::WHITE) {
            createMovement(MoveValidator::MovementType::first_move_forward, ret, piece, from, piece.movement.first_move_forward, 0, +1, false, true);
        }

        if (piece.color == Types::Color::BLACK) {
            createMovement(MoveValidator::MovementType::first_move_forward, ret, piece, from, piece.movement.first_move_forward, 0, -1, false, true);
        }
    }
    
    // Ignore repeated positions
    std::vector<MoveValidator::MovementData> posList;
    bool founded;
    for (auto movData : ret) {
        founded = false;
        for (auto safe : posList) {
            if (safe.pos == movData.pos) {
                founded = true;
                break;
            }
        }

        if (!founded) posList.push_back(movData);
    }

    return posList;
}

/*
    Oluşturulan hamleler haraket ettirildiğinde şah durumunu kontrol eder.
*/
std::vector<MoveValidator::MovementData> MoveValidator::createSafeMovementList(Types::Piece piece, Types::Position from) {
    std::vector<MoveValidator::MovementData> moveList = createMovementList(piece, from);
    
    // special_abilities.castling
    if ((piece.type == "King") && piece.special_abilities.castling) {
        int yAdd;
        if (piece.color == Types::Color::WHITE) {
            yAdd = 0;
        } else if (piece.color == Types::Color::BLACK) {
            yAdd = 7;
        }

        if (isValidCastling(piece.color, "queen-side")) {
            moveList.push_back({MoveValidator::MovementType::castling_long, {2, yAdd}});
        } else if (isValidCastling(piece.color, "king-side")) {
            moveList.push_back({MoveValidator::MovementType::castling_short, {6, yAdd}});
        }
    }

    removeInvalidMovement(piece, from, moveList);
    return moveList;
}

MoveValidator::MovAdjL MoveValidator::createMovementGraph(Types::Position from) {
    MoveValidator::MovAdjL adjL;
    Types::Piece piece = board.getPieceAt(from);

    // Position üret.
    adjL.pos_list = createSafeMovementList(piece, from);

    // standart hareketleri kaydet.
    for (long unsigned int i=0; i<adjL.pos_list.size(); i++) {
        adjL.moveList.push_back(i);
    }

    // Portal girişlerini bul
    int posCount = adjL.pos_list.size();
    for (int i=0, j=posCount, portalID=0; i<posCount; i++) {
        const auto pData = adjL.pos_list[i];

        Portal portal;
        //  is there a portal?                    and          (empty square or enemy)?
        if (board.isTherePortalEntry(pData.pos, portal) && (getSquareOwner(pData.pos) != piece.color)) {
            if (portal.validatePortalUse(piece)) {
                adjL.portalIn.push_back({portalID, portal.getPortalID()});
                adjL.portalOut.push_back(std::vector<int>());

                adjL.pos_list.push_back({MoveValidator::MovementType::portal_in, pData.pos});
                adjL.portalOut[portalID].push_back(j);
                j++;

                for (const auto &moveData : createSafeMovementList(piece, portal.getExitPosition())) {
                    adjL.pos_list.push_back(moveData);
                    adjL.portalOut[portalID].push_back(j);
                    j++;
                }

                portalID++;
            }
        }
    }

    return adjL;
}

void MoveValidator::DumpMovementList (MoveValidator::MovAdjL adjList) {
    std::cout << "Movement List: " << std::endl;
    for (auto moveID : adjList.moveList) {
        Types::Position move = adjList.pos_list[moveID].pos;
        std::cout << move.toString(board.getBoardSize()) << " ";
    }
    std::cout << std::endl;

    std::cout << "Portal List: " << std::endl;
    for (auto portalIn : adjList.portalIn) {
        std::cout << "Portal: " << portalIn.portalOutID << std::endl;
        std::cout << "Portal ID: " << portalIn.portalID << std::endl;
        for (auto portalOut : adjList.portalOut[portalIn.portalOutID]) {
            Types::Position move = adjList.pos_list[portalOut].pos;
            std::cout << move.toString(board.getBoardSize()) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

std::vector<Types::Position> MoveValidator::checkAttack(Types::Position pos, Types::Color enemyColor) {
    std::vector<Types::Position> ret;

    for (int x=0; x<board.getBoardSize(); x++) {
        for (int y=0; y<board.getBoardSize(); y++) {
            Types::Position piecePos = {x, y};

            Types::Piece piece = board.getPieceAt(piecePos);
            if ((piece.type != Types::NULL_PIECE_TYPE) && (piece.color == enemyColor)) {
                std::vector<MoveValidator::MovementData> targetSq = createMovementList(piece, piecePos);

                for (const auto& cPos : targetSq) {
                    if (cPos.pos == pos) ret.push_back(piecePos);
                }
            }
        }
    }

    return ret;
}

std::vector<Types::Position> MoveValidator::validateBoardCheckStatus(Types::Color color) {
    std::vector<Types::Position> ret;

    for (const auto &pos : board.getPiecesWhichRoyal(color)) {
        std::vector<Types::Position> attackCheck = checkAttack(pos, Types::getEnemyColor(color));

        if (attackCheck.size() > 0) {
            ret.insert(ret.end(), attackCheck.begin(), attackCheck.end());
        }
    }

    return ret;
}

std::vector<Types::Position> MoveValidator::testMoveForAttack(Types::Position from, Types::Position to, Types::Color color) {
    Types::Piece old = board.testMoveTo(from, to);

    std::vector<Types::Position> ret = validateBoardCheckStatus(color);

    board.testMoveTo(to, from);
    board.putThePieceTo(old, to);

    return ret;
}

std::vector<Types::Position> MoveValidator::calculateAllMovementForAColor(Types::Color color) {
    std::vector<Types::Position> allowedMovement;

    for (int x=0; x<board.getBoardSize(); x++) {
        for (int y=0; y<board.getBoardSize(); y++) {
            Types::Position piecePos = {x, y};

            Types::Piece piece = board.getPieceAt(piecePos);
            if ((piece.type != Types::NULL_PIECE_TYPE) && (piece.color == color)) {
                std::vector<Types::Position> movements = getValidMoveList(piecePos);

                if (movements.size() > 0) {
                    allowedMovement.insert(allowedMovement.end(), movements.begin(), movements.end());
                }
            }
        }
    }

    return allowedMovement; 
}

std::vector<Types::Position> MoveValidator::getAllPositionsInCheckmat(Types::Color color) {
    std::vector<Types::Position> ret;

    for (const auto &pos : board.getPiecesWhichRoyal(color)) {
        Types::Piece piece = board.getPieceAt(pos);
        std::vector<MoveValidator::MovementData> standartMovement = createMovementList(piece, pos);

        for (const auto &cMovData : standartMovement) {
            std::vector<Types::Position> checkPos = checkAttack(cMovData.pos, Types::getEnemyColor(piece.color));
            
            if (checkPos.size() > 0) {
                ret.insert(ret.end(), checkPos.begin(), checkPos.end());
            }
        }
    }

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

    return posList;
}

bool MoveValidator::isReadyForPoromotion(Types::Piece piece, Types::Position pos) {
    if (
        piece.special_abilities.promotion &&
        (
            (
                (piece.color == Types::Color::WHITE) &&
                (pos.y == 7)
            ) 
            ||
            (
                (piece.color == Types::Color::BLACK) &&
                (pos.y == 0)
            )
        )
    ) {
        return true;
    }

    return false;
}

bool MoveValidator::isValidCastling(Types::Color color, std::string type) {
    Types::Position Rook1_P, Rook2_P, King_P;

    if (color == Types::Color::WHITE) {
        Rook1_P = {0, 0};
        King_P = {4, 0};
        Rook2_P = {7, 0};
    } else if (color == Types::Color::BLACK) {
        Rook1_P = {0, 7};
        King_P = {4, 7};
        Rook2_P = {7, 7};
    }

    Types::Piece Rook1, Rook2, King;
    Rook1 = board.getPieceAt(Rook1_P);
    King = board.getPieceAt(King_P);
    Rook2 = board.getPieceAt(Rook2_P);

    if ((King.type == Types::NULL_PIECE_TYPE) || !King.special_abilities.castling) return false;
    if (!King.firstMove) return false;

    // 1. şah ve kale haraket etmemiş olmalı
    if (type == "queen-side") {
        if ((Rook1.type == Types::NULL_PIECE_TYPE) || !Rook1.special_abilities.castling) return false;
        if (!Rook1.firstMove) return false;
    } else if (type == "king-side") {
        if ((Rook2.type == Types::NULL_PIECE_TYPE) || !Rook2.special_abilities.castling) return false;
        if (!Rook2.firstMove) return false;
    }

    // 2. arada taş olmamalı
    int yAdd;
    if (color == Types::Color::WHITE) {
        yAdd = 0;
    } else {
        yAdd = 7;
    }
    
    if (type == "queen-side") {
        for (int x=1; x<4; x++) {
            Types::Piece piece = board.getPieceAt({x, yAdd});
            if (piece.type != Types::NULL_PIECE_TYPE)
                return false;
        }
    } else {
        for (int x=5; x<7; x++) {
            Types::Piece piece = board.getPieceAt({x, yAdd});
            if (piece.type != Types::NULL_PIECE_TYPE)
                return false;
        }
    }

    // 3. şah ve kale'ye kadar olan kareler tehdit altında olmamalı (kale önemli değil)
    if (type == "queen-side") {
        for (int x=1; x<=4; x++) {
            if (checkAttack({x, yAdd}, Types::getEnemyColor(color)).size() > 0)
                return false;
        }
    } else {
        for (int x=4; x<7; x++) {
            if (checkAttack({x, yAdd}, Types::getEnemyColor(color)).size() > 0)
                return false;
        }
    }

    // her şey iyi
    return true;
}

std::array<Types::Position, 2> MoveValidator::getRookPositionWhenCastling(Types::Color color, MoveValidator::MovementType castlingType) {
    int yAdd = (color == Types::Color::WHITE) ? 0 : 7;

    if (castlingType == MoveValidator::MovementType::castling_long) {
        return {{{0, yAdd}, {3, yAdd}}}; // for rook when queenside
    } else {
        return {{{7, yAdd}, {5, yAdd}}}; // for rook when kingside
    }
}
