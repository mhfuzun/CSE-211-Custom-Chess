#pragma once

class Types {
    public:
        enum class Mod { USER, COMPUTER};
        enum class Color { WHITE, BLACK };
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

};


