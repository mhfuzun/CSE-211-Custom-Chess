#pragma once

#include "ConfigReader.hpp"
#include "CoolDown.hpp"
#include "ChessBoard.hpp"
#include "MoveValidator.hpp"
#include "History.hpp"

#include <iostream>
#include <string>

class GameManager {
    public:
        enum class TurnResult {INVALID, STANDART, CHECK, CHECKMAT, DRAW};

        GameManager(const GameConfig &_config);

        ~GameManager ();

        void initTheGame( void );

    private:
        const GameConfig &config;
        ChessBoard chessBoard;
        MoveValidator* moveValidator;
        CoolDown coolDown;
        History history;
        
        void showLastMove( void );
        void showCheckStatus(Types::Color color);
        bool GameUndo( void );
        GameManager::TurnResult makeMove(Types::Position from, Types::Position to, std::string promotionType);
};
