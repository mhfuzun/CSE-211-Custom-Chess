#pragma once

#include "Types.hpp"
#include "ConfigReader.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

class Player {
    public:
        Player (const GameConfig &_config, Types::Mod mod, Types::Color color);

        ~Player ();

        Types::Command Turn(Types::MovePos& retMovePos,  std::string& prom);
        
        Types::Command getCommand(Types::MovePos& retMovePos,  std::string& prom);

        Types::MovePos getMove( void );

        Types::Mod getMod( void ) const;

        Types::Color getColor( void ) const;

    private:
        const GameConfig &config;
        
        Types::MovePos movePos;

        Types::Mod playerMod;
        Types::Color playerColor;

        // Komutu ve argümanlarını ayrıştırmak için yardımcı fonksiyon
        std::vector<std::string> tokenize(const std::string& input);

        bool translatePositon_command2board(std::string pos1, Types::Position &pos2);
};
