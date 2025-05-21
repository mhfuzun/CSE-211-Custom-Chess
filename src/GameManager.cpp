#include "Types.hpp"
#include "ConfigReader.hpp"
#include "ChessBoard.hpp"
#include "Player.hpp"
#include "GameManager.hpp"

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

    // Players
    Player player1(config, Types::Mod::USER, Types::Color::WHITE);
    Player player2(config, Types::Mod::USER, Types::Color::BLACK);

    chessBoard.displayBoard(Types::Color::WHITE);

    for (int turn=0; turn<config.getTurnLimit(); turn++)
    {
        for (Player player : {player1, player2}) {
            if (player.getColor() == Types::Color::WHITE) {
                std::cout << "Turn: " << turn + 1 << " - White:" << std::endl;
            } else if (player.getColor() == Types::Color::BLACK) {
                std::cout << "Turn: " << turn + 1 << " - Black:" << std::endl;
            }

            chessBoard.displayBoard(player.getColor());

            Types::Command com = player.Turn();
            
            if (com == Types::Command::MOVE) {
                Types::MovePos move = player.getMove();
                std::cout << "Move from (" << move.from.x << ", " << move.from.y << ") to (" << move.to.x << ", " << move.to.y << ")" << std::endl;

                makeMove(move.from, move.to);
            }
        }
    }
}

bool GameManager::makeMove(Types::Position from, Types::Position to) {
    isValidMove(chessBoard, from, to);

    chessBoard.movePiece(from, to);

    return true;
}
