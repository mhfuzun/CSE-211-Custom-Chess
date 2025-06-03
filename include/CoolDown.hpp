#pragma once
#include "Types.hpp"
#include "ChessBoard.hpp"

#include <vector>
#include <queue>

class CoolDown {
    public:
        CoolDown();
        ~CoolDown();

        void iterateCooldown();
        void pushCooldown(Portal *portal);

    private:
        std::queue<Portal*> coolQueue;
};