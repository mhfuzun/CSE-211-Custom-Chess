#pragma once

#include "ConfigReader.hpp"
#include "CoolDown.hpp"
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
        CoolDown coolDown;
        
        bool makeMove(Types::Position from, Types::Position to);
};
