#include "Types.hpp"
#include "ChessBoard.hpp"
#include "ConfigReader.hpp"
#include "Player.hpp"
#include "Portal.hpp"
#include "MoveValidator.hpp"
#include "assert_utils.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <iomanip> // std::setw için gerekli
#include <cassert>

ChessBoard::ChessBoard(const GameConfig& _config) : config(_config) {
}

ChessBoard::~ChessBoard() {
    
}

void ChessBoard::initBoard(const GameConfig& _config) {
    gameSettings.name = _config.game_settings.name;
    gameSettings.board_size = _config.game_settings.board_size;
    gameSettings.board.backgroundColor = _config.game_settings.board.backgroundColor;
    gameSettings.board.textColor = _config.game_settings.board.textColor;
    gameSettings.board.resetColor = _config.game_settings.board.resetColor;

    // Board Initialization
    for (int i=0; i<gameSettings.board_size; i++) {
        board.push_back(std::vector<BoardSquare>());
        for (int j=0; j<gameSettings.board_size; j++) {
            BoardSquare square = {
                Types::SquareType::EMPTY,
                NullPiece,
                NullPortal,
                false
            };

            board[i].push_back(square);
        }
    }
    
    // Pieces
    for (const auto &piece : _config.pieces) {
        Types::Piece piece_s;
        // Types::Position piece_p;
        piece_s.type = piece.type;
        piece_s.display_on = piece.display_on;
        piece_s.firstMove = true;
        piece_s.movement = piece.movement;
        piece_s.special_abilities = piece.special_abilities;
        
        if (piece.positions.count("white") > 0) {
            piece_s.color = Types::Color::WHITE;
            piece_s.ascii = piece.display_ascii[0];
            for (const auto &pos : piece.positions.at("white")) {
                board[pos.x][pos.y].piece = piece_s;
                board[pos.x][pos.y].type = Types::SquareType::PIECE;
            }
        }

        if (piece.positions.count("black") > 0) {
            piece_s.color = Types::Color::BLACK;
            piece_s.ascii = piece.display_ascii[1];
            for (const auto &pos : piece.positions.at("black")) {
                board[pos.x][pos.y].piece = piece_s;
                board[pos.x][pos.y].type = Types::SquareType::PIECE;
            }
        }
    }

    // Portals
    int portalID=0;
    for (const auto &portal : _config.portals) {
        board[portal.positions.entry.x][portal.positions.entry.y].type = Types::SquareType::PORTAL_IN;
        board[portal.positions.entry.x][portal.positions.entry.y].portal.initPortal(portalID, Portal::PortalDirection::IN, portal);
        board[portal.positions.exit.x][portal.positions.exit.y].type = Types::SquareType::PORTAL_OUT;
        board[portal.positions.exit.x][portal.positions.exit.y].portal.initPortal(portalID, Portal::PortalDirection::OUT, portal);
        portalID++;
    }
}

void ChessBoard::displayBoard(Types::Color perspectiveColor) {
    const std::string yellow_bg     = "\033[43m"; // sarı arka plan
    const std::string blue_bg       = "\033[44m"; // Blue arka plan
    const std::string purple_bg     = "\033[45m"; // Purple arka plan
    const std::string white_bg      = "\033[47m"; // beyaz arka plan
    const std::string black_fg      = "\033[30m"; // siyah yazı
    const std::string reset         = "\033[0m";  // sıfırla

    std::cout << white_bg << black_fg;
    std::cout << "    ";
    for (char c = 'a'; c < 'a' + gameSettings.board_size; ++c) {
        std::cout << c << " ";
    }
    std::cout << "    " << reset << "\n";

    bool blackSquare = true;
    for (int i = 0; i < gameSettings.board_size; i++) {
        int y;

        if (perspectiveColor == Types::Color::WHITE) {
            y = gameSettings.board_size - i - 1;
            blackSquare = (i%2 == 1);
        } else {
            y = i;
            blackSquare = (i%2 != 1);
        }

        std::cout << white_bg << black_fg;
        std::cout << " " << std::setw(2) << y + 1 << " "; // Satır numarası
        for (int x = 0; x < gameSettings.board_size; ++x) {
            // Types::Position pos{x, y};
            if (board[x][y].colored) {
                std::cout << yellow_bg << black_fg;
            } else {
                if (blackSquare) {
                    std::cout << purple_bg << black_fg;
                } else {
                    std::cout << white_bg << black_fg;
                }
            }

            blackSquare = !blackSquare;

            if ((board[x][y].type == Types::SquareType::PIECE) && board[x][y].piece.display_on) {
                std::cout << getPieceSymbol(board[x][y].piece) << " ";
            } else if ((board[x][y].type == Types::SquareType::PORTAL_IN) || (board[x][y].type == Types::SquareType::PORTAL_OUT)) {
                std::cout << board[x][y].portal.getPortalID();
                if (board[x][y].portal.getPortalDirection() == Portal::PortalDirection::IN) {
                    std::cout << "i";
                } else if (board[x][y].portal.getPortalDirection() == Portal::PortalDirection::OUT) {
                    std::cout << "o";
                } else {
                    std::cout << "e";
                }
            } else {
                // std::cout << ". "; // Boş kare
                std::cout << "  "; // Boş kare
            }
        }

        std::cout << white_bg << black_fg;
        std::cout << " " << std::setw(2) << y + 1 << " "; // Satır numarası
        std::cout << reset << "\n";
    }

    // Alt satıra sütun harfleri
    std::cout << white_bg << black_fg;
    std::cout << "    ";
    for (char c = 'a'; c < 'a' + gameSettings.board_size; ++c) {
        std::cout << c << " ";
    }
    std::cout << "    " << reset << "\n";
}

std::string ChessBoard::getPieceSymbol(const Types::Piece& piece) {
    if (!piece.display_on) return " ";
    return piece.ascii;
}

Types::Piece ChessBoard::movePiece(Types::Position from, Types::Position to) {
    ASSERT_MSG(from.isValid(getBoardSize()), "invalid position!");
    ASSERT_MSG(to.isValid(getBoardSize()), "invalid position!");

    Types::Piece targetSquare = board[to.x][to.y].piece;

    board[to.x][to.y].piece = board[from.x][from.y].piece;
    board[from.x][from.y].piece = NullPiece;
    board[from.x][from.y].type = Types::SquareType::EMPTY;

    board[to.x][to.y].piece.firstMove = false;
    board[to.x][to.y].type = Types::SquareType::PIECE;

    return targetSquare;
}

// For Test Cases
Types::Piece ChessBoard::testMoveTo(Types::Position from, Types::Position to) {
    ASSERT_MSG(from.isValid(getBoardSize()), "invalid position!");
    ASSERT_MSG(to.isValid(getBoardSize()), "invalid position!");

    Types::Piece targetSquare = board[to.x][to.y].piece;

    board[to.x][to.y].piece = board[from.x][from.y].piece;
    board[from.x][from.y].piece = NullPiece;

    return targetSquare;
}

Types::Piece ChessBoard::putThePieceTo(Types::Piece piece, Types::Position to) {
    ASSERT_MSG(to.isValid(getBoardSize()), "invalid position!");

    Types::Piece targetSquare = board[to.x][to.y].piece;
    board[to.x][to.y].piece = piece;

    return targetSquare;
}

Types::Piece ChessBoard::getPieceAt(Types::Position from) {
    ASSERT_MSG(from.isValid(getBoardSize()), "invalid position!");
    return board[from.x][from.y].piece;
}

void ChessBoard::placePieceAt(Types::Piece piece, Types::Position to) {
    ASSERT_MSG(to.isValid(getBoardSize()), "invalid position!");
    board[to.x][to.y].piece = piece;
}

std::vector<Types::Position> ChessBoard::getPiecesWhichRoyal(Types::Color colorType) {
    std::vector<Types::Position> retList;

    for (int x=0; x<getBoardSize(); x++)
        for (int y=0; y<getBoardSize(); y++) {
            Types::Position pos = {x,y};
            Types::Piece piece = getPieceAt(pos);
            
            if ((piece.color == colorType) && piece.special_abilities.royal) {
                retList.push_back(pos);
            }
        }

    return retList;
}

Types::Piece ChessBoard::getPieceWithType(std::string type) {
    for (int x=0; x<getBoardSize(); x++)
        for (int y=0; y<getBoardSize(); y++) {
            Types::Piece piece = getPieceAt({x, y});
            if (piece.type == type) 
                return piece;
        }
            

    return NullPiece;
}

bool ChessBoard::isTherePortalEntry(Types::Position pos, Portal &portal) {
    ASSERT_MSG(pos.isValid(getBoardSize()), "invalid position!");

    if (board[pos.x][pos.y].type == Types::SquareType::PORTAL_IN) {
        if (
            board[pos.x][pos.y].portal.isValidPortal()
        ) {
            portal = board[pos.x][pos.y].portal;
            return true;
        }
    }
    
    return false;
}

int ChessBoard::getBoardSize( void ) {
    return gameSettings.board_size;
}

bool ChessBoard::checkCastlingForAColor( void ) {
    // Checkmat'ları kontrol et.
    // Aranın boş olması gerekiyor.
    // Taşların daha önce hiç haraket etmemiş olması gerekiyor.
    return false;
}

void ChessBoard::displayValidMovement(Types::Position from, std::vector<Types::Position> pos_list) {
    for (auto pos : pos_list) {
        board[pos.x][pos.y].colored = true;
    }
    
    std::cout << "Valid Movement: " << from.toString(getBoardSize()) << " --> ";
    for (auto pos : pos_list) {
        std::cout << pos.toString(getBoardSize()) << " ";
    }
    std::cout << std::endl;

    displayBoard(getPieceAt(from).color);

    for (auto pos : pos_list) {
        board[pos.x][pos.y].colored = false;
    }
}

void ChessBoard::resetHighlightColor( void ) {
    for (auto row : board) {
        for (auto sq : row) {
            sq.colored = false;
        }
    }
}

bool ChessBoard::promoteThePiece(Types::Position from, std::string type) {
    PieceConfig retPiece;

    if (config.getPieceWithType(type, retPiece)) {
        Types::Piece piece_old = getPieceAt(from);

        Types::Piece piece;
        piece.type = retPiece.type;
        piece.color = piece_old.color;
        piece.display_on = true;
        piece.firstMove = false;
        piece.movement = retPiece.movement;
        piece.special_abilities = retPiece.special_abilities;

        if (piece_old.color == Types::Color::WHITE)
            piece.ascii = retPiece.display_ascii[0];
        else
            piece.ascii = retPiece.display_ascii[1];

        placePieceAt(piece, from);
        return true;
    } else {
        std::cout << "Type Mistake!" << std::endl;
        return false;
    }
}

void ChessBoard::dumpPortalInfo( void ) {
    std::cout << "Portal Info" << std::endl;
    for (int x=0; x<getBoardSize(); x++)
        for (int y=0; y<getBoardSize(); y++) {
            Types::Position pos = {x,y};
            Portal portal;
            if (isTherePortalEntry(pos, portal)) {
                std::cout << "Portal[" << portal.getPortalID() << "]" << std::endl;
                
                std::cout << "   Entry Pos.: " << portal.getEntryPosition().toString(getBoardSize()) << std::endl;
                std::cout << "   Exit Pos.: " << portal.getExitPosition().toString(getBoardSize()) << std::endl;

                if (portal.isValidPortal())
                    std::cout << "   Valid: True" << std::endl;
                else {
                    std::cout << "   Valid: False" << std::endl;
                    std::cout << "   CoolDown: " << portal.getCoolDown() << std::endl;
                }
            }
        }
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
