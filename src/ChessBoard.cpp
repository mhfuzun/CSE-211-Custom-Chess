#include "Types.hpp"
#include "ChessBoard.hpp"
#include "ConfigReader.hpp"
#include "Player.hpp"
#include "Portal.hpp"
#include "MoveValidator.hpp"
#include "assert_utils.hpp"
#include "TerminalColors.hpp"

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
        for (int j=0; j<gameSettings.board_size; j++) {
            Types::Position Pos = {i, j};
            SquareColor sqColor;
            BoardSquare square = {
                Types::SquareType::EMPTY,
                NullPiece,
                NullPortal,
                false,
                sqColor
            };

            board[Pos] = square;
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
                Types::Position ppos = {pos.x, pos.y};
                board[ppos].piece = piece_s;
                board[ppos].type = Types::SquareType::PIECE;
            }
        }

        if (piece.positions.count("black") > 0) {
            piece_s.color = Types::Color::BLACK;
            piece_s.ascii = piece.display_ascii[1];
            for (const auto &pos : piece.positions.at("black")) {
                Types::Position ppos = {pos.x, pos.y};
                board[ppos].piece = piece_s;
                board[ppos].type = Types::SquareType::PIECE;
            }
        }
    }

    // Portals
    int portalID=0;
    for (const auto &portal : _config.portals) {
        Types::Position portal_entry = {portal.positions.entry.x, portal.positions.entry.y};
        Types::Position portal_exit = {portal.positions.exit.x, portal.positions.exit.y};
        board[portal_entry].type = Types::SquareType::PORTAL_IN;
        board[portal_entry].portal.initPortal(portalID, Portal::PortalDirection::IN, portal);
        board[portal_exit].type = Types::SquareType::PORTAL_OUT;
        board[portal_exit].portal.initPortal(portalID, Portal::PortalDirection::OUT, portal);
        portalID++;
    }
}

void ChessBoard::displayBoard(Types::Color perspectiveColor) {
    using namespace TerminalColor;

    std::cout << colorize(black_fg, white_bg);
    std::cout << "    ";
    for (char c = 'a'; c < 'a' + gameSettings.board_size; ++c) {
        std::cout << c << " ";
    }
    std::cout << "    " << colorize(reset) << "\n";

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

        std::cout << colorize(black_fg, white_bg);
        std::cout << " " << std::setw(2) << y + 1 << " "; // Satır numarası
        for (int x = 0; x < gameSettings.board_size; ++x) {
            Types::Position sPos = {x, y};
            std::cout << board[sPos].sqColor.getSquareColor(blackSquare);
            blackSquare = !blackSquare;

            if ((board[sPos].type == Types::SquareType::PIECE) && board[sPos].piece.display_on) {
                std::cout << getPieceSymbol(board[sPos].piece) << " ";
            } else if (
                    board[sPos].portal.isValidPortal() &&
                    (
                        (board[sPos].type == Types::SquareType::PORTAL_IN) || 
                        (board[sPos].type == Types::SquareType::PORTAL_OUT)
                    )
                ) {
                std::cout << board[sPos].portal.getPortalID();
                if (board[sPos].portal.getPortalDirection() == Portal::PortalDirection::IN) {
                    std::cout << "i";
                } else if (board[sPos].portal.getPortalDirection() == Portal::PortalDirection::OUT) {
                    std::cout << "o";
                } else {
                    std::cout << "e";
                }
            } else {
                // std::cout << ". "; // Boş kare
                std::cout << "  "; // Boş kare
            }
        }

        std::cout << colorize(black_fg, white_bg);
        std::cout << " " << std::setw(2) << y + 1 << " "; // Satır numarası
        std::cout << colorize(reset) << "\n";
    }

    // Alt satıra sütun harfleri
    std::cout << colorize(black_fg, white_bg);
    std::cout << "    ";
    for (char c = 'a'; c < 'a' + gameSettings.board_size; ++c) {
        std::cout << c << " ";
    }
    std::cout << "    " << colorize(reset) << "\n";
}

std::string ChessBoard::getPieceSymbol(const Types::Piece& piece) {
    if (!piece.display_on) return " ";
    return piece.ascii;
}

Types::Piece ChessBoard::movePiece(Types::Position from, Types::Position to) {
    ASSERT_MSG(from.isValid(getBoardSize()), "invalid position!");
    ASSERT_MSG(to.isValid(getBoardSize()), "invalid position!");

    Types::Piece targetSquare = board[to].piece;

    board[to].piece = board[from].piece;
    board[from].piece = NullPiece;
    board[from].type = Types::SquareType::EMPTY;

    board[to].piece.firstMove = false;
    board[to].type = Types::SquareType::PIECE;

    return targetSquare;
}

// For Test Cases
Types::Piece ChessBoard::testMoveTo(Types::Position from, Types::Position to) {
    ASSERT_MSG(from.isValid(getBoardSize()), "invalid position!");
    ASSERT_MSG(to.isValid(getBoardSize()), "invalid position!");

    Types::Piece targetSquare = board[to].piece;

    board[to].piece = board[from].piece;
    board[from].piece = NullPiece;

    return targetSquare;
}

Types::Piece ChessBoard::putThePieceTo(Types::Piece piece, Types::Position to) {
    ASSERT_MSG(to.isValid(getBoardSize()), "invalid position!");

    Types::Piece targetSquare = board[to].piece;
    board[to].piece = piece;

    return targetSquare;
}

Types::Piece ChessBoard::getPieceAt(Types::Position from) {
    ASSERT_MSG(from.isValid(getBoardSize()), "invalid position!");
    
    return board[from].piece;
}

void ChessBoard::placePieceAt(Types::Piece piece, Types::Position to) {
    ASSERT_MSG(to.isValid(getBoardSize()), "invalid position!");

    if (piece.type == Types::NULL_PIECE_TYPE) {
        board[to].type = Types::SquareType::EMPTY;
    } else {
        board[to].type = Types::SquareType::PIECE;
    }
    board[to].piece = piece;
}

void ChessBoard::placeNullPieceAt(Types::Position to) {
    ASSERT_MSG(to.isValid(getBoardSize()), "invalid position!");
    
    board[to].type = Types::SquareType::EMPTY;
    board[to].piece = ChessBoard::NullPiece;
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

Portal* ChessBoard::getPortalWithID(int id) {
    for (int x = 0; x < getBoardSize(); x++) {
        for (int y = 0; y < getBoardSize(); y++) {
            Portal* por = getPortalEntry({x, y}); // ya da erişim biçimine göre uygun şekilde al
            if ((por != nullptr) && (por->getPortalID() == id)) {
                return por; // Orijinal nesneye işaret eder
            }
        }
    }
    return nullptr;
}

bool ChessBoard::isTherePortalEntry(Types::Position pos, Portal &portal) {
    Portal *por = getPortalEntry(pos);
    if (por != nullptr) {
        portal = *por;
        return true;
    }
    
    return false;
}

Portal* ChessBoard::getPortalEntry(Types::Position pos) {
    ASSERT_MSG(pos.isValid(getBoardSize()), "invalid position!");

    if (board[pos].type == Types::SquareType::PORTAL_IN) {
        return &board[pos].portal;
    }
    
    return nullptr;
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
    setColorFlagList("possibleMove", pos_list, true);
    setColorFlagSingle("selected", from, true);
    std::cout << "Valid Movement: " << from.toString(getBoardSize()) << " --> ";
    for (auto pos : pos_list) {
        std::cout << pos.toString(getBoardSize()) << " ";
    }
    std::cout << std::endl;

    displayBoard(getPieceAt(from).color);
    resetColorFlagAll("possibleMove");
    resetColorFlagAll("selected");
}

void ChessBoard::resetColorFlagAll(const std::string &flagName) {
    for (auto& [pos, sq] : board) {
        sq.sqColor.setFlag(flagName, false);
    }
}

void ChessBoard::setColorFlagList(const std::string &flagName, const std::vector<Types::Position> &positions, bool resetOthers) {
    if (resetOthers)
        resetColorFlagAll(flagName);

    for (const auto &pos : positions) {
        ASSERT_MSG(pos.isValid(getBoardSize()), "invalid position!");
        board[pos].sqColor.setFlag(flagName, true);
    }
}

void ChessBoard::setColorFlagSingle(const std::string &flagName, const Types::Position &pos, bool resetOthers) {
    if (resetOthers)
        resetColorFlagAll(flagName);

    ASSERT_MSG(pos.isValid(getBoardSize()), "invalid position!");
    board[pos].sqColor.setFlag(flagName, true);
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
                std::cout << "   Preserve Direction: " << ((portal.isPreserveDirection()) ? "True" : "False") << std::endl;
                std::cout << "   Allowed Colors: [ ";
                for (const auto &color : portal.getAllowedColors()) {
                    std::cout << Types::getColorText(color);
                }
                std::cout << "]" << std::endl;
                
                if (portal.isValidPortal()) {
                    std::cout << "   Entry Pos.: " << portal.getEntryPosition().toString(getBoardSize()) << std::endl;
                    std::cout << "   Exit Pos.: " << portal.getExitPosition().toString(getBoardSize()) << std::endl;
                    std::cout << "   Valid: True" << std::endl;
                } else {
                    std::cout << "   Valid: False" << std::endl;
                    std::cout << "   CoolDown: " << portal.getCoolDown() << std::endl;
                }
            }
        }
}

bool ChessBoard::isValidPromotionType(std::string pType) {
    return (
                (pType == "Queen") ||
                (pType == "Rook") ||
                (pType == "Knight") ||
                (pType == "Bishop")
            );
}
