#pragma once
#include "Types.hpp"

enum class MovementType {
    forward,
    sideways,
    diagonal,
    l_shape,
    diagonal_capture,
    first_move_forward
};

struct Movement
{
    int boardSize;
    MovementType type;
    int direction;
    int length;
};

bool isValidMove(ChessBoard &chessBoard, Types::Position from, Types::Position to);
