#pragma once

class Types {
    public:
        enum class Mod { USER, COMPUTER};
        enum class Color { EMPTY, WHITE, BLACK };
        enum class Command { EMPTY, MOVE, UNDO, EXIT};

        struct Position {
            int x;
            int y;

            bool operator==(const Position& other) const {
                return x == other.x && y == other.y;
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

            SpecialAbilities special_abilities;
        };
};

