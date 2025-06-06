#include "Player.hpp"
#include "ChessBoard.hpp"
#include "ChessBoard.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

Player::Player (const GameConfig &_config, Types::Mod mod, Types::Color color)
: config(_config), playerMod(mod), playerColor(color)
{}

Player::~Player () {
    
}

Types::Command Player::Turn(Types::MovePos& retMovePos,  std::string& prom) {
    return getCommand(retMovePos, prom);
}

Types::Command Player::getCommand(Types::MovePos& retMovePos,  std::string& prom) {
    std::string line;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        
        // Boş satır kontrolü
        if (line.empty()) continue;

        // Komutu ayrıştır
        std::vector<std::string> args = tokenize(line);

        if (args[0] == "move") {
            bool succ;

            // move test command
            if (args.size() == 2) {
                succ = translatePositon_command2board(args[1], retMovePos.from);
                retMovePos.to = {-1, -1};
            } else if (args.size() == 3) {
                succ = 
                translatePositon_command2board(args[1], retMovePos.from) &
                translatePositon_command2board(args[2], retMovePos.to);
            }

            if (args.size() == 4) {
                prom = args[3];
            } else {
                prom = "";
            }

            if (succ)
                return Types::Command::MOVE;
            else {
                std::cout << "Again; EX: move a1 a2 [k]" << std::endl;
                continue;
            }
        } else if (args[0] == "undo") {
            return Types::Command::UNDO;
        } else if (args[0] == "exit") {
            return Types::Command::EXIT;
        } else if (args[0] == "refresh") {
            return Types::Command::REFRESH;
        } else if (args[0] == "portal") {
            return Types::Command::PORTALINFO;
        } else if (args[0] == "history") {
            return Types::Command::HISTORY;
        } else {
            std::cout << "Unknown Command!" << std::endl;
            continue;
        }
    }
}

Types::MovePos Player::getMove( void ) {
    return movePos;
}

Types::Mod Player::getMod( void ) const {
    return playerMod;
}

Types::Color Player::getColor( void ) const {
    return playerColor;
}

// Komutu ve argümanlarını ayrıştırmak için yardımcı fonksiyon
std::vector<std::string> Player::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

bool Player::translatePositon_command2board(std::string pos1, Types::Position &pos2) {
    if (pos1.length() != 2) return false;
    char a = pos1[0];
    char b = pos1[1] - '0';

    if ('a' <= a && a < ('a' + config.getBoardSize())) {
        pos2.x = a - 'a';
    } else if ('A' <= a && a < ('A' + config.getBoardSize())) {
        pos2.x = a - 'A';
    } else {
        return false;
    }

    if (1 <= b && b <= config.getBoardSize()) {
        pos2.y = b-1;
    } else {
        return false;
    }

    return true;
}
