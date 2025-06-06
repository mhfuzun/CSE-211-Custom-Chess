#pragma once
#include "Types.hpp"
#include "ChessBoard.hpp"

#include <vector>
#include <queue>

class CoolDown {
    public:
        CoolDown(ChessBoard &_board);
        ~CoolDown();

        void iterateCooldown();
        void pushCooldown(int portalID);

        void dumpCoolDown( void );

    private:
        std::queue<int> coolQueue;
        ChessBoard &board;
};