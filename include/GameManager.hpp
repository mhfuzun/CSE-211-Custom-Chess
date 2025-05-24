#pragma once

#include "ConfigReader.hpp"
#include "ChessBoard.hpp"
#include "MoveValidator.hpp"

#include <iostream>
#include <string>

class GameManager {
    public:
        GameManager(const GameConfig &_config);

        ~GameManager ();

        void initTheGame( void );

    private:
        const GameConfig &config;
        ChessBoard chessBoard;
        MoveValidator* moveValidator;
        
        bool makeMove(Types::Position from, Types::Position to);
};
