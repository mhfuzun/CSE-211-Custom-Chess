#pragma once

#include "ConfigReader.hpp"
#include "ChessBoard.hpp"

#include <iostream>
#include <string>

class GameManager {
    public:
        GameManager(const GameConfig &_config);

        ~GameManager ();

    private:
        const GameConfig &config;
};
