#pragma once

#include "ConfigReader.hpp"

class Types {
    public:
        enum class Mod { USER, COMPUTER};
        enum class Color { EMPTY, WHITE, BLACK };
        enum class Command { EMPTY, MOVE, UNDO, EXIT, REFRESH, PORTALINFO};
        enum class SquareType { EMPTY, PIECE, PORTAL_IN, PORTAL_OUT};

        struct Position {
            int x;
            int y;

            bool operator==(const Position& other) const {
                return x == other.x && y == other.y;
            }

            bool isValid(int chessBoardSize) const {
                return (((0 <= x) && (x < chessBoardSize)) && ((0 <= y) && (y < chessBoardSize)));
            }

            std::string toString(int chessBoardSize) const {
                if (!(isValid(chessBoardSize))) {
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

        static constexpr  std::string NULL_PIECE_TYPE = "NULL";

        static Color getEnemyColor(Types::Color ally) {
            if (ally == Types::Color::WHITE) {
                return Types::Color::BLACK;
            } else if (ally == Types::Color::BLACK) {
                return Types::Color::WHITE;
            } else {
                return Types::Color::EMPTY;
            }
        }
};

