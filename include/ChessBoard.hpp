#pragma once

#include "Types.hpp"
#include "ConfigReader.hpp"

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

        ChessBoard();

        ~ChessBoard();

        void initBoard(GameConfig config);

        void displayBoard(Types::Color perspectiveColor);

        bool movePiece(Types::Position from, Types::Position to);

        Types::Piece getPieceAt(Types::Position from);
        Types::Piece getPieceWithType(std::string type);

        int getBoardSize( void );

        bool checkCastlingForAColor( void );

        void displayValidMovement(Types::Position from, std::vector<Types::Position> pos_list);
        void resetHighlightColor( void );

    private:
        struct BoardSquare
        {
            Types::Piece piece;
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
        
        // std::unordered_map<Types::Position, Types::Piece> board;
        std::vector<std::vector<BoardSquare>> board;
        game_settings gameSettings;

        std::string getPieceSymbol(const Types::Piece& piece);
        
};
