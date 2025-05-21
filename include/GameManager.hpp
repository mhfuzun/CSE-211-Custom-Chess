#pragma once

#include "ConfigReader.hpp"
#include "ChessBoard.hpp"

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
        
        bool makeMove(Types::Position from, Types::Position to);
};
