#pragma once

#include "Types.hpp"
#include "Portal.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <assert.h>

class History {
    public:
        enum class CastlingType {EMPTY, QUEENSIDE, KINGSIDE};
        struct HistoryNode
        {
            Types::Color color;
            Types::Position pos_from;
            Types::Position pos_to;
            Types::Piece piece_from;
            Types::Piece piece_to;
            std::vector<Portal> portalStatus;
            std::string promotionType;
            
            CastlingType typeCastling;
            Types::Position castlingFrom;
            Types::Position castlingTo;
            Types::Piece castlingRook;

            bool typeCapture;
            bool typeChek;
            bool typeChekmat;
            bool typePromotion;
            bool typeTeleport;
        };
        
        
        History ();
        ~History ();

        void pushHistory(HistoryNode node);
        bool popHistory(HistoryNode &node);
        bool getLastMove(Types::Position &from, Types::Position &to);
        bool getLastNode(History::HistoryNode &node);

        int DumpHistoryInChessFormat(int maxLength);

    private:
        std::stack<HistoryNode> history;
        
};
