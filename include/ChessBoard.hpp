#pragma once

#include "Types.hpp"
#include "ConfigReader.hpp"
#include "Portal.hpp"

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
        std::vector<Types::Position> getPiecesWhichRoyal(Types::Color colorType);

        bool isTherePortalEntry(Types::Position pos, Portal &portal);

        int getBoardSize( void );

        bool checkCastlingForAColor( void );

        void displayValidMovement(Types::Position from, std::vector<Types::Position> pos_list);
        void resetHighlightColor( void );

        bool promoteThePiece(Types::Position from, std::string type);

        void dumpPortalInfo( void );

    private:
        struct BoardSquare
        {
            Types::SquareType type;
            Types::Piece piece;
            Portal portal;
            bool colored;
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
        
        // std::unordered_map<Types::Position, Types::Piece> board;
        std::vector<std::vector<BoardSquare>> board;
        game_settings gameSettings;
        const GameConfig& config;

        std::string getPieceSymbol(const Types::Piece& piece);
        
};
