#include "Types.hpp"
#include "ChessBoard.hpp"
#include "ConfigReader.hpp"
#include "Player.hpp"
#include "MoveValidator.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

ChessBoard::ChessBoard() {
}

ChessBoard::~ChessBoard() {
    
}

void ChessBoard::initBoard(GameConfig config) {
    gameSettings.name = config.game_settings.name;
    gameSettings.board_size = config.game_settings.board_size;
    gameSettings.board.backgroundColor = config.game_settings.board.backgroundColor;
    gameSettings.board.textColor = config.game_settings.board.textColor;
    gameSettings.board.resetColor = config.game_settings.board.resetColor;

    for (int i=0; i<gameSettings.board_size; i++) {
        board.push_back(std::vector<Types::Piece>());
        for (int j=0; j<gameSettings.board_size; j++)
            board[i].push_back(NullPiece);
    }
    
    for (const auto &piece : config.pieces) {
        Types::Piece piece_s;
        Types::Position piece_p;
        piece_s.type = piece.type;
        piece_s.display_on = piece.display_on;
        piece_s.firstMove = true;
        
        if (piece.positions.count("white") > 0) {
            piece_s.color = Types::Color::WHITE;
            piece_s.ascii = piece.display_ascii[0];
            for (const auto &pos : piece.positions.at("white")) {
                board[pos.x][pos.y] = piece_s;
            }
        }

        if (piece.positions.count("black") > 0) {
            piece_s.color = Types::Color::BLACK;
            piece_s.ascii = piece.display_ascii[1];
            for (const auto &pos : piece.positions.at("black")) {
                board[pos.x][pos.y] = piece_s;
            }
        }
    }
}

void ChessBoard::displayBoard(Types::Color perspectiveColor) {
    const std::string white_bg = "\033[47m"; // beyaz arka plan
    const std::string black_fg = "\033[30m"; // siyah yazı
    const std::string reset    = "\033[0m";  // sıfırla

    std::cout << white_bg << black_fg;
    std::cout << "   ";
    for (char c = 'a'; c < 'a' + gameSettings.board_size; ++c) {
        std::cout << c << " ";
    }
    std::cout << "   " << reset << "\n";

    for (int i = 0; i < gameSettings.board_size; i++) {
        int y;

        if (perspectiveColor == Types::Color::WHITE) {
            y = gameSettings.board_size - i - 1;
        } else {
            y = i;
        }

        std::cout << white_bg << black_fg;
        std::cout << " " << y + 1 << " "; // Satır numarası
        for (int x = 0; x < gameSettings.board_size; ++x) {
            Types::Position pos{x, y};
            if (board[x][y].display_on) {
                std::cout << getPieceSymbol(board[x][y]) << " ";
            } else {
                std::cout << ". "; // Boş kare
            }
        }
        std::cout << " " << y + 1 << " "; // Satır numarası
        std::cout << reset << "\n";
    }

    // Alt satıra sütun harfleri
    std::cout << white_bg << black_fg;
    std::cout << "   ";
    for (char c = 'a'; c < 'a' + gameSettings.board_size; ++c) {
        std::cout << c << " ";
    }
    std::cout << "   " << reset << "\n";
}

std::string ChessBoard::getPieceSymbol(const Types::Piece& piece) {
    if (!piece.display_on) return " ";
    return piece.ascii;
}

bool ChessBoard::movePiece(Types::Position from, Types::Position to) {
    board[from.x][from.y].firstMove = false;

    board[to.x][to.y] = board[from.x][from.y];
    board[from.x][from.y] = NullPiece;
    return true;
}

Types::Piece ChessBoard::getPiece(Types::Position from) {
    return board[from.x][from.y];
}

/*
void ChessBoard::initBoard(GameConfig config) {
    gameSettings.name = config.game_settings.name;
    gameSettings.board_size = config.game_settings.board_size;
    gameSettings.board.backgroundColor = config.game_settings.board.backgroundColor;
    gameSettings.board.textColor = config.game_settings.board.textColor;
    gameSettings.board.resetColor = config.game_settings.board.resetColor;

    for (const auto &piece : config.pieces) {
        Piece piece_s;
        Types::Position piece_p;
        piece_s.type = piece.type;
        piece_s.display_on = piece.display_on;
        
        if (piece.positions.count("white") > 0) {
            piece_s.color = Types::Color::WHITE;
            piece_s.ascii = piece.display_ascii[0];
            for (const auto &pos : piece.positions.at("white")) {
                piece_p = {pos.x, pos.y};
                board[piece_p] = piece_s;
            }
        }

        if (piece.positions.count("black") > 0) {
            piece_s.color = Types::Color::BLACK;
            piece_s.ascii = piece.display_ascii[1];
            for (const auto &pos : piece.positions.at("black")) {
                piece_p = {pos.x, pos.y};
                board[piece_p] = piece_s;
            }
        }
    }
}

void ChessBoard::displayBoard(Types::Color perspectiveColor) {
    const std::string white_bg = "\033[47m"; // beyaz arka plan
    const std::string black_fg = "\033[30m"; // siyah yazı
    const std::string reset    = "\033[0m";  // sıfırla


    return;

    std::cout << white_bg << black_fg;
    std::cout << "   ";
    for (char c = 'a'; c < 'a' + gameSettings.board_size; ++c) {
        std::cout << c << " ";
    }
    std::cout << "   " << reset << "\n";

    for (int i = 0; i < gameSettings.board_size; i++) {
        int y;

        if (perspectiveColor == Types::Color::WHITE) {
            y = gameSettings.board_size - i - 1;
        } else {
            y = i;
        }

        std::cout << white_bg << black_fg;
        std::cout << " " << y + 1 << " "; // Satır numarası
        for (int x = 0; x < gameSettings.board_size; ++x) {
            Types::Position pos{x, y};
            auto it = board.find(pos);
            if (it != board.end()) {
                std::cout << getPieceSymbol(it->second) << " ";
            } else {
                std::cout << ". "; // Boş kare
            }
        }
        std::cout << " " << y + 1 << " "; // Satır numarası
        std::cout << reset << "\n";
    }

    // Alt satıra sütun harfleri
    std::cout << white_bg << black_fg;
    std::cout << "   ";
    for (char c = 'a'; c < 'a' + gameSettings.board_size; ++c) {
        std::cout << c << " ";
    }
    std::cout << "   " << reset << "\n";
}
*/
