#pragma once

#include "Types.hpp"
#include "ConfigReader.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

class ChessBoard {
    public:
        struct game_settings {
            std::string name;
            int board_size;
            struct
            {
                std::string backgroundColor;
                std::string textColor;
                std::string resetColor;
            } board;
            
        };
        
        struct PositionHash {
            std::size_t operator()(const Types::Position& p) const {
                return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
            }
        };

        struct Piece {
            std::string type;   // "King", "Pawn", etc.
            Types::Color color;
            bool display_on = true;
            std::string ascii;
        };

        ChessBoard();

        ~ChessBoard();

        void initBoard(GameConfig config);

        void displayBoard(Types::Color perspectiveColor);

    private:
        std::unordered_map<Types::Position, Piece, PositionHash> board;
        game_settings gameSettings;

        std::string getPieceSymbol(const Piece& piece);
        
};
