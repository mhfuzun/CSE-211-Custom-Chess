#include "Types.hpp"
#include "ConfigReader.hpp"
#include "ChessBoard.hpp"
#include "Player.hpp"
#include "GameManager.hpp"
#include "MoveValidator.hpp"
#include "CoolDown.hpp"
#include "History.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <array>

GameManager::GameManager(const GameConfig &_config)
: config(_config), chessBoard(_config), coolDown(chessBoard) {

}

GameManager::~GameManager () {
    
}

void GameManager::showLastMove( void ) {
    History::HistoryNode historyNode;
    if (history.getLastNode(historyNode)) {
        if (historyNode.typeCastling == History::CastlingType::EMPTY) {
            chessBoard.setColorFlagSingle("lastMove_begin", historyNode.pos_from, true);
            chessBoard.setColorFlagSingle("lastMove_end", historyNode.pos_to, true);
        } else {
            // King
            chessBoard.setColorFlagSingle("lastMove_begin", historyNode.pos_from, true);
            chessBoard.setColorFlagSingle("lastMove_end", historyNode.pos_to, true);

            // Rook
            std::array<Types::Position, 2> poses;
            
            if (historyNode.typeCastling == History::CastlingType::QUEENSIDE) {
                poses = moveValidator->getRookPositionWhenCastling(historyNode.color, MoveValidator::MovementType::castling_long);
            } else {
                poses = moveValidator->getRookPositionWhenCastling(historyNode.color, MoveValidator::MovementType::castling_short);
            }
            chessBoard.setColorFlagSingle("lastMove_begin", poses[0], false);
            chessBoard.setColorFlagSingle("lastMove_end", poses[1], false);
        }
    } else {
        chessBoard.resetColorFlagAll("lastMove_begin");
        chessBoard.resetColorFlagAll("lastMove_end");
    }
}

void GameManager::showCheckStatus(Types::Color color) {
    std::vector<Types::Position> checkPosition = moveValidator->validateBoardCheckStatus(color);

    if (checkPosition.size() > 0) {
        chessBoard.setColorFlagList("check", chessBoard.getPiecesWhichRoyal(color), true);
        chessBoard.setColorFlagList("check", checkPosition, false);
    } else {
        chessBoard.resetColorFlagAll("check");
    }
}

bool GameManager::GameUndo( void ) {
    History::HistoryNode historyNode;
    if (history.popHistory(historyNode)) {
        if (historyNode.typeCastling == History::CastlingType::EMPTY) {
            chessBoard.placePieceAt(historyNode.piece_from, historyNode.pos_from);
            chessBoard.placePieceAt(historyNode.piece_to, historyNode.pos_to);
        } else {
            // King
            chessBoard.placePieceAt(historyNode.piece_from, historyNode.pos_from);
            chessBoard.placePieceAt(historyNode.piece_to, historyNode.pos_to);

            // Rook
            chessBoard.placePieceAt(historyNode.castlingRook, historyNode.castlingFrom);
            chessBoard.placeNullPieceAt(historyNode.castlingTo);
        }

        return true;
    }

    return false;
}

void GameManager::initTheGame( void ) {
    // Chess Board
    chessBoard.initBoard(config);

    moveValidator = new MoveValidator(chessBoard);

    // Players
    Player player1(config, Types::Mod::USER, Types::Color::WHITE);
    Player player2(config, Types::Mod::USER, Types::Color::BLACK);

    Player playerList[2] = {player1, player2};
    int playerCount = std::size(playerList);
    
    for (int moveCounter=0; moveCounter<(config.getTurnLimit()*2); moveCounter++) {
        int turn = moveCounter / std::size(playerList);
        Player player = playerList[moveCounter % (playerCount)];

        bool validMove = false;
        bool drawBuf = true;
        while (!validMove) {
            if (drawBuf) {
                if (player.getColor() == Types::Color::WHITE) {
                    std::cout << "[" << moveCounter << "] Turn: " << turn + 1 << " - White:" << std::endl;
                } else if (player.getColor() == Types::Color::BLACK) {
                    std::cout << "[" << moveCounter << "] Turn: " << turn + 1 << " - Black:" << std::endl;
                }

                // add last move sign
                showLastMove();
                showCheckStatus(player.getColor());
                chessBoard.displayBoard(player.getColor());
            }
            
            Types::MovePos move;
            std::string prom;
            Types::Command com = player.Turn(move, prom);

            drawBuf = true;
            if (com == Types::Command::MOVE) {
                // Types::MovePos move = player.getMove();
                std::cout << "Move from (" << move.from.x << ", " << move.from.y << ") to (" << move.to.x << ", " << move.to.y << ")" << std::endl;

                GameManager::TurnResult turnRes = makeMove(move.from, move.to, prom);
                if (turnRes == GameManager::TurnResult::INVALID) {
                    validMove = false;
                    drawBuf = false;
                } else if ((turnRes == GameManager::TurnResult::STANDART) || turnRes == GameManager::TurnResult::CHECK) {
                    validMove = true;
                } else {
                    if (turnRes == GameManager::TurnResult::CHECKMAT) {
                        std::cout << "CHECKMAT, X wins!" << std::endl;
                    } else if (turnRes == GameManager::TurnResult::DRAW) {
                        std::cout << "DRAW, In the end, friendship won!" << std::endl;
                    }

                    showLastMove();
                    chessBoard.displayBoard(Types::getEnemyColor(player.getColor()));
                    std::cout << "History:" << std::endl;
                    history.DumpHistoryInChessFormat(0);
                    std::cout << std::endl;

                    return;
                }

            } else if (com == Types::Command::UNDO) {
                if (GameUndo()) {
                    validMove = true;
                    moveCounter-=2;
                } else {
                    std::cout << "History is Empty!!" << std::endl;
                    continue;
                }
            } else if (com == Types::Command::EXIT) {
                std::cout << "Thank you!" << std::endl;
                return;
            } else if (com == Types::Command::REFRESH) {
                continue;
            } else if (com == Types::Command::PORTALINFO) {
                chessBoard.dumpPortalInfo();
                coolDown.dumpCoolDown();
                continue;
            } else if (com == Types::Command::HISTORY) {
                validMove = false;
                drawBuf = false;
                std::cout << "History:" << std::endl;
                history.DumpHistoryInChessFormat(0);
                std::cout << std::endl;
                continue;
            }
        }

        // Her hamle sonu görevleri:
        coolDown.iterateCooldown();
        chessBoard.resetColorFlagAll("check");
    }
}

GameManager::TurnResult GameManager::makeMove(Types::Position from, Types::Position to, std::string promotionType) {
    GameManager::TurnResult retType;
    retType = GameManager::TurnResult::INVALID;

    Types::Piece piece_from = chessBoard.getPieceAt(from);

    // there is no piece here!
    if (piece_from.type == Types::NULL_PIECE_TYPE) {
        std::cout << "There is no any piece here!" << std::endl;
        
    // it is a valid move
    } else {
        MoveValidator::MoveResult movRes = moveValidator->isValidMove(from, to);
        if (movRes.valid) {
            retType = GameManager::TurnResult::STANDART;
            bool promotePiece = moveValidator->isReadyForPoromotion(piece_from, to);
            
            ////////////////////////////////////////////////////////////
            // History Store
            /*
                HistoryNode:
                    Types::Position pos_from;
                    Types::Position pos_to;
                    Types::Piece piece_from;
                    Types::Piece piece_to;
                    std::vector<Portal> portalStatus;
                    std::string promotionType;
                    
                    CastlingType typeCastling;
                    Types::Position castlingFrom;
                    Types::Position castlingTo;
                    Types::Piece castlingRook;

                    bool typeCapture;
                    bool typeChek;
                    bool typeChekmat;
                    bool typePromotion;
                    bool typeTeleport;
            */
            
            History::CastlingType castlingType = History::CastlingType::EMPTY;

            if (movRes.type == MoveValidator::MovementType::castling_long) {
                castlingType = History::CastlingType::QUEENSIDE;
            } else if (movRes.type == MoveValidator::MovementType::castling_short) {
                castlingType = History::CastlingType::KINGSIDE;
            }
            
            static Types::Piece NullPiece = {
                Types::NULL_PIECE_TYPE,
                Types::Color::EMPTY,
                false,
                ". ",
                false,
                {},
                {}
            };

            History::HistoryNode historyNode = {
                piece_from.color,
                from,
                to,
                piece_from,
                chessBoard.getPieceAt(to),
                {}, //*
                promotionType,

                castlingType,
                {-1, -1},
                {-1, -1},
                NullPiece,

                (chessBoard.getPieceAt(to).type != Types::NULL_PIECE_TYPE),
                false,
                false,
                promotePiece,
                (movRes.portalUse)
            };
            ////////////////////////////////////////////////////////////

            if (
                (movRes.type == MoveValidator::MovementType::castling_long) || 
                (movRes.type == MoveValidator::MovementType::castling_short)
            ) {
                // for king
                chessBoard.movePiece(from, to);
                
                // for rook
                std::array<Types::Position, 2> poses = moveValidator->getRookPositionWhenCastling(piece_from.color, movRes.type);
                
                ///////////////////////////////////////////////
                // History Storing for The Rook in Castling: //
                ///////////////////////////////////////////////
                historyNode.castlingFrom = poses[0];
                historyNode.castlingTo = poses[1];
                historyNode.castlingRook = chessBoard.getPieceAt(poses[0]);
                ///////////////////////////////////////////////

                chessBoard.movePiece(poses[0], poses[1]);
            } else {
                chessBoard.movePiece(from, to);
            }
            
            if (promotePiece) {
                chessBoard.promoteThePiece(to, promotionType);
            }

            if (movRes.portalUse) {
                coolDown.pushCooldown(movRes.portalID);
            }

            // Her şey tamam şimdi kontrol yapılabilir. 
            // işin içine portal kullanma haklarınında dahil edilmesi gerekiyor.
            Types::Color enemyColor = Types::getEnemyColor(piece_from.color);
            std::vector<Types::Position> checkPosition = moveValidator->validateBoardCheckStatus(enemyColor);
            std::vector<Types::Position> drawPosition = moveValidator->calculateAllMovementForAColor(enemyColor);
            
            bool check_status = (checkPosition.size() > 0);
            bool draw_status = (drawPosition.size() == 0);

            if (draw_status && check_status) {
                // checkmat (tr: mat)
                historyNode.typeChekmat = true;
                retType = GameManager::TurnResult::CHECKMAT;

                std::vector<Types::Position> checkmatPosition = moveValidator->getAllPositionsInCheckmat(enemyColor);
                std::cout << "Checkmat Positions: " << std::endl;
                for (const auto& pos : checkmatPosition) {
                    std::cout << pos.toString(8) << " ";
                }
                std::cout << std::endl;

                chessBoard.setColorFlagList("check", chessBoard.getPiecesWhichRoyal(enemyColor), true);
                chessBoard.setColorFlagList("check", checkmatPosition, false);
            }
            
            if (!draw_status && check_status) {
                // check (tr: şah)
                historyNode.typeChek = true;
                retType = GameManager::TurnResult::CHECK;

                chessBoard.setColorFlagList("check", chessBoard.getPiecesWhichRoyal(enemyColor), true);
                chessBoard.setColorFlagList("check", checkPosition, false);
            }

            if (draw_status && !check_status) {
                // stalemate (tr: pat)
                retType = GameManager::TurnResult::DRAW;

                std::vector<Types::Position> checkmatPosition = moveValidator->getAllPositionsInCheckmat(enemyColor);
                std::cout << "Stalemate Positions: " << std::endl;
                for (const auto& pos : checkmatPosition) {
                    std::cout << pos.toString(8) << " ";
                }
                std::cout << std::endl;
                
                chessBoard.setColorFlagList("check", chessBoard.getPiecesWhichRoyal(enemyColor), true);
                chessBoard.setColorFlagList("check", checkmatPosition, false);
            }
            
            history.pushHistory(historyNode);
            
        // it is NOT a valid move, but show the valid position list
        } else {
            std::cout << "This move is NOT a valid move command. (We show you the valid moves on the board)" << std::endl;
            chessBoard.displayValidMovement(from, moveValidator->getValidMoveList(from));
        }
    }

    return retType;
}
