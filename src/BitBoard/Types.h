
#ifndef BITENGINE_TYPES_H
#define BITENGINE_TYPES_H

#include <cstdint>
namespace BitEngine{
    enum PieceType{
        WPawn,
        BPawn,
        WKnight,
        BKnight,
        WBishop,
        BBishop,
        WRook,
        BRook,
        WQueen,
        BQueen,
        WKing,
        BKing,
        None,
    };
    enum Color {
        White,
        Black,
    };

    enum State : uint8_t {
        turnColor = 1,//0 for white 1 for black
        canCastleKingSideWhite = 1 << 1,
        canCastleQueenSideWhite = 1 << 2,
        canCastleKingSideBlack = 1 << 3,
        canCastleQueenSideBlack = 1 << 4,
    };


    enum MoveType{
        Null,
        Normal,//can be a quiet move or a capture
        QueenSideCastle,
        KingSideCastle,
        DoublePawnPush,
        EnPassant,
        Promote,
    };
    char getPieceChar(PieceType type);
}

#endif