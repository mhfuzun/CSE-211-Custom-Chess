#include "History.hpp"

#include "Types.hpp"
#include "ConfigReader.hpp"

History::History () {

};

History::~History () {
    
};

void History::pushHistory(History::HistoryNode node) {
    history.push(node);
}

bool History::popHistory(History::HistoryNode &node) {
    if (history.empty()) {
        return false;
    }

    node = history.top();
    history.pop();
    
    return true;
}

bool History::getLastMove(Types::Position &from, Types::Position &to) {
    if (history.empty()) {
        return false;
    }

    History::HistoryNode node = history.top();
    from = node.pos_from;
    to = node.pos_to;

    return true;
}

bool History::getLastNode(History::HistoryNode &node) {
    if (history.empty()) {
        return false;
    }

    node = history.top();
    
    return true;
}

std::string getStringOfMove(History::HistoryNode node) {
    std::string ret = "";

    // Castling
    if (node.typeCastling == History::CastlingType::QUEENSIDE) {
        ret = "O-O-O";
        return ret;
    } else if (node.typeCastling == History::CastlingType::KINGSIDE) {
        ret = "O-O";
        return ret;
    }

    // Piece Type
    if (node.piece_from.type[0] != 'P') {
        ret += node.piece_from.type[0];
    }

    if (node.typeTeleport) {
        ret += node.pos_from.toString(8) + ">";
    }

    // Target Position
    ret += node.pos_to.toString(8);
    
    // Capture information
    if (node.typeCapture) {
       ret += std::string("x") + std::string(1, node.piece_to.type[0]);
    }

    // Promotion Information
    if (node.typePromotion) {
        ret += std::string("=") + std::string(1, node.promotionType[0]);
    }

    // Check Information
    if (node.typeChek) {
        ret += "+";
    }

    // Checkmat, end of the game
    if (node.typeChekmat) {
        ret += "#";
    }

    return ret;
}

int History::DumpHistoryInChessFormat(int maxLength) {
    bool printAll = (maxLength == 0);
    std::stack<History::HistoryNode> temp;
    int totalMoves = history.size();
    int currentTurn = totalMoves;

    bool firstLine = true;

    while (!history.empty()) {
        History::HistoryNode node1 = history.top();
        history.pop();
        temp.push(node1);

        bool hasSecondMove = false;
        History::HistoryNode node2;

        if (!history.empty() && !(firstLine && ((totalMoves%2) == 1))) {
            node2 = history.top();
            history.pop();
            temp.push(node2);
            hasSecondMove = true;
        }

        if (printAll || maxLength > 0) {
            // Hamle numarası yazdır
            std::cout << "[" << (currentTurn + 1) / 2 << "] ";

            if (hasSecondMove) {
                std::cout << getStringOfMove(node2) << " - ";
            }

            std::cout << getStringOfMove(node1);

            if (!hasSecondMove) {
                std::cout << " - ";
            }

            std::cout << std::endl;

            maxLength--;
            firstLine = false;
        }

        currentTurn -= hasSecondMove ? 2 : 1;

        if (!printAll && maxLength <= 0) break;
    }

    // Stack'i geri yükle
    while (!temp.empty()) {
        history.push(temp.top());
        temp.pop();
    }

    return (totalMoves + 1) / 2; // Toplam hamle çift sayısı (tur sayısı)
}

