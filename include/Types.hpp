#pragma once

#include "ConfigReader.hpp"

class Types {
    public:
        enum class Mod { USER, COMPUTER};
        enum class Color { EMPTY, WHITE, BLACK };
        enum class Command { EMPTY, MOVE, UNDO, EXIT};

        static constexpr  std::string NULL_PIECE_TYPE = "NULL";

        struct Position {
            int x;
            int y;

            bool operator==(const Position& other) const {
                return x == other.x && y == other.y;
            }

            std::string toString(int chessBoard) const {
                if (x < 0 || x >= chessBoard || y < 0 || y >= chessBoard) {
                    throw std::out_of_range("Position coordinates must be in the range [0,n].");
                }

                char file = static_cast<char>('a' + x);   // x: 0 -> 'a', ..., 7 -> 'h', ...
                char rank = static_cast<char>('1' + y);   // y: 0 -> '1', ..., 7 -> '8', ...

                return std::string{file, rank};
            }
        };

        struct MovePos
        {
            Position from;
            Position to;
        };

        struct Piece {
            std::string type;   // "King", "Pawn", etc.
            Types::Color color;
            bool display_on = true;
            std::string ascii;
            bool firstMove = true;

            Movement movement;
            SpecialAbilities special_abilities;
        };
};

