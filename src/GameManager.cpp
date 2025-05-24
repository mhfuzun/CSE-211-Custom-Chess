#include "Types.hpp"
#include "ConfigReader.hpp"
#include "ChessBoard.hpp"
#include "Player.hpp"
#include "GameManager.hpp"
#include "MoveValidator.hpp"

#include <iostream>
#include <string>

GameManager::GameManager(const GameConfig &_config)
: config(_config) {

}

GameManager::~GameManager () {
    
}

void GameManager::initTheGame( void ) {
    // Chess Board
    chessBoard.initBoard(config);

    moveValidator = new MoveValidator(chessBoard);

    // Players
    Player player1(config, Types::Mod::USER, Types::Color::WHITE);
    Player player2(config, Types::Mod::USER, Types::Color::BLACK);

    for (int turn=0; turn<config.getTurnLimit(); turn++)
    {
        for (Player player : {player1, player2}) {
            bool validMove = false;
            bool drawBuf = true;
            while (!validMove) {
                if (drawBuf) {
                    if (player.getColor() == Types::Color::WHITE) {
                        std::cout << "Turn: " << turn + 1 << " - White:" << std::endl;
                    } else if (player.getColor() == Types::Color::BLACK) {
                        std::cout << "Turn: " << turn + 1 << " - Black:" << std::endl;
                    }

                    chessBoard.displayBoard(player.getColor());
                }
                
                Types::Command com = player.Turn();
                
                if (com == Types::Command::MOVE) {
                    Types::MovePos move = player.getMove();
                    std::cout << "Move from (" << move.from.x << ", " << move.from.y << ") to (" << move.to.x << ", " << move.to.y << ")" << std::endl;

                    if (!makeMove(move.from, move.to)) {
                        validMove = false;
                        drawBuf = false;
                    } else {
                        validMove = true;
                    }
                } else if (com == Types::Command::EXIT) {
                    std::cout << "Thank you!" << std::endl;
                    return;
                }
            }
        }
    }
}

bool GameManager::makeMove(Types::Position from, Types::Position to) {
    
    // there is no piece here!
    if (chessBoard.getPieceAt(from).type == Types::NULL_PIECE_TYPE) {
        std::cout << "There is no any piece here!" << std::endl;
    // it is a valid move
    } else if (moveValidator->isValidMove(from, to)) {
        chessBoard.movePiece(from, to);

        return true;
    // it is NOT a valid move, but show the valid position list
    } else {
        std::cout << "This move is NOT a valid move command. (We show you the valid moves on the board)" << std::endl;
        chessBoard.displayValidMovement(from, moveValidator->getValidMoveList(from));
    }

    return false;
}
