#pragma once

#include "Types.hpp"
#include "ConfigReader.hpp"
#include "Portal.hpp"
#include "TerminalColors.hpp"
#include "assert_utils.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class ChessBoard {
    public:
        struct game_settings {
            std::string name;
            int board_size;
            struct
            {
                std::string backgroundColor;
                std::string textColor;
                std::string resetColor;
            } board;
            
        };

        ChessBoard(const GameConfig &_config);

        ~ChessBoard();

        void initBoard(const GameConfig& _config);

        void displayBoard(Types::Color perspectiveColor);

        Types::Piece movePiece(Types::Position from, Types::Position to);
        Types::Piece testMoveTo(Types::Position from, Types::Position to);
        Types::Piece putThePieceTo(Types::Piece piece, Types::Position to);

        Types::Piece getPieceAt(Types::Position from);
        Types::Piece getPieceWithType(std::string type);
        void placePieceAt(Types::Piece piece, Types::Position to);
        void placeNullPieceAt(Types::Position to);
        std::vector<Types::Position> getPiecesWhichRoyal(Types::Color colorType);

        Portal* getPortalWithID(int id);
        bool isTherePortalEntry(Types::Position pos, Portal &portal);
        Portal* getPortalEntry(Types::Position pos);

        int getBoardSize( void );

        bool checkCastlingForAColor( void );

        void displayValidMovement(Types::Position from, std::vector<Types::Position> pos_list);
        void resetHighlightColor( void );

        bool promoteThePiece(Types::Position from, std::string type);

        void dumpPortalInfo( void );

        void resetColorFlagAll(const std::string &flagName);
        void setColorFlagList(const std::string &flagName, const std::vector<Types::Position> &positions, bool resetOthers=false);
        void setColorFlagSingle(const std::string &flagName, const Types::Position &pos, bool resetOthers=false);

        bool isValidPromotionType(std::string pType);

    private:
        struct SquareColor {
            bool lastMove_begin = false;
            bool lastMove_end = false;
            bool check = false;
            bool selected = false;
            bool possibleMove = false;

           void setFlag(const std::string &key, bool value) {
                if (key == "lastMove_begin") {
                    lastMove_begin = value;
                } else if (key == "lastMove_end") {
                    lastMove_end = value;
                } else if (key == "selected") {
                    selected = value;
                } else if (key == "possibleMove") {
                    possibleMove = value;
                } else if (key == "check") {
                    check = value;
                } else {
                    ASSERT_MSG((0), "Undefined Tag!");
                }
            }

            std::string getSquareColor(bool blackSquare) {
                using namespace TerminalColor;

                if (check) {
                    return colorize(black_fg, bright_red_bg);
                } else if (possibleMove) {
                    return colorize(black_fg, bright_green_bg);
                } else if (selected) {
                    return colorize(black_fg, bright_blue_bg);
                } else if (lastMove_begin) {
                    return colorize(black_fg, yellow_bg);
                } else if (lastMove_end) {
                    return colorize(black_fg, bright_yellow_bg);
                } else {
                    if (blackSquare) {
                        return colorize(black_fg, bright_magenta_bg);
                    } else {
                        return colorize(black_fg, white_bg);
                    }
                }
            }
        };
        
        struct BoardSquare
        {
            Types::SquareType type;
            Types::Piece piece;
            Portal portal;
            bool colored;
            SquareColor sqColor;
        };

        Types::Piece NullPiece = {
            Types::NULL_PIECE_TYPE,
            Types::Color::EMPTY,
            false,
            ". ",
            false,
            {},
            {}
        };
        
        const Portal NullPortal = Portal();

        std::unordered_map<Types::Position, BoardSquare> board;
        // std::vector<std::vector<BoardSquare>> board;
        game_settings gameSettings;
        const GameConfig& config;

        std::string getPieceSymbol(const Types::Piece& piece);
        
};
