#include "BoardEval.h"


namespace Engine {

    BoardEval::BoardEval(Engine::Board &board) : internal_board(board) {

    }

    int BoardEval::getPhase() const {
        int knights_count =
                popCount(internal_board.getBitboard(WKnight)) + popCount(internal_board.getBitboard(BKnight));
        int bishops_count =
                popCount(internal_board.getBitboard(WBishop)) + popCount(internal_board.getBitboard(BBishop));
        int rooks_count = popCount(internal_board.getBitboard(WRook)) + popCount(internal_board.getBitboard(BRook));
        int queens_count = popCount(internal_board.getBitboard(WQueen)) + popCount(internal_board.getBitboard(BQueen));
        int phase = 24 - bishops_count - knights_count - 2 * rooks_count - 4 * queens_count;
        phase = (phase * 256 + 12) / 24;
        return phase;
    }


    int BoardEval::getMaterialScore(Color color) const {
        int pawn_score =
                popCount(internal_board.getBitboard(getPiece(PieceType::Pawn, color))) * getPieceValue(PieceType::Pawn);
        int knight_score = popCount(internal_board.getBitboard(getPiece(PieceType::Knight, color))) *
                           getPieceValue(PieceType::Knight);
        int bishop_score = popCount(internal_board.getBitboard(getPiece(PieceType::Bishop, color))) *
                           getPieceValue(PieceType::Bishop);
        int rook_score =
                popCount(internal_board.getBitboard(getPiece(PieceType::Rook, color))) * getPieceValue(PieceType::Rook);
        int queen_score = popCount(internal_board.getBitboard(getPiece(PieceType::Queen, color))) *
                          getPieceValue(PieceType::Queen);
        return pawn_score + knight_score + bishop_score + rook_score + queen_score;
    }

    int BoardEval::getBonusPieceScore(PieceType piece, Color color) const {
        int score = 0, bonus, pos;
        int piece_index = static_cast<int>(piece);
        uint64_t positions = internal_board.getBitboard(getPiece(piece, color));

        while (positions) {
            pos = bitScanForward(popLsb(positions));
            if (color == White)
                bonus = Tables::Bonus[piece_index][63 - pos];
            else bonus = Tables::Bonus[piece_index][pos];

            score += bonus;
        }
        return score;

    }

    int BoardEval::interpolate(int opening, int ending, int phase) const {
        return  ((opening* (256 - phase)) + (ending * phase)) / 256;
    }

    int BoardEval::getBonusKingScore(Color color, int phase) const {
        int king_opening, king_ending;
        if (color == White) {
            int pos = bitScanForward(internal_board.getBitboard(WKing));
            king_opening = Tables::KingBonus[0][63 - pos];
            king_ending = Tables::KingBonus[1][63 - pos];
        } else {
            int pos = bitScanForward(internal_board.getBitboard(BKing));
            king_opening = Tables::KingBonus[0][pos];
            king_ending = Tables::KingBonus[1][pos];
        }
        return interpolate(king_opening,king_ending,phase);
    }

    int BoardEval::getDoubledPawnCount(Color color) const {
        Piece pawn = getPiece(PieceType::Pawn, color);
        uint64_t pawn_bitboard = internal_board.getBitboard(pawn);
        int result = 0;

        for (int i = 0; i < 8; i++) {
            uint64_t pawns_on_file = pawn_bitboard & Tables::MaskFile[i];
            if (pawns_on_file) {
                result += popCount(pawns_on_file) - 1;
            }

        }
        return result;
    }
    int BoardEval::getPassedPawnCount(Color color) const {
        Piece pawn = getPiece(PieceType::Pawn, color);
        Piece opposite_pawn=getPiece(PieceType::Pawn,getOpposite(color));
        int result=0;
        uint64_t pawn_bitboard = internal_board.getBitboard(pawn);
        uint64_t opposite_pawn_bitboard=internal_board.getBitboard(opposite_pawn);
        uint64_t front_fill,pawn_pos;
        Tables::Direction up=(color==White? Tables::North : Tables::South);
        while(pawn_bitboard){
            pawn_pos=popLsb(pawn_bitboard);
            int position=bitScanForward(pawn_pos);
            front_fill=Tables::AttackTables[position][up];
            if(position%8!=0)
                front_fill|=Tables::AttackTables[position-1][up];
            if(position%8!=7)
                front_fill|=Tables::AttackTables[position+1][up];

            if((front_fill&opposite_pawn_bitboard)==0)
                result++;
        }
        return result;
    }

    int BoardEval::getPawnStructureScore(Color color,int phase) const {
        int opening=0,ending=0;
        int num_doubled_pawns=getDoubledPawnCount(color);

        opening+=num_doubled_pawns*doubled_pawn_penalty[0];
        ending+=num_doubled_pawns*doubled_pawn_penalty[1];

        return interpolate(opening,ending,phase);
    }

    int BoardEval::getBonusScore(Color color, int phase) const {
        return getBonusPieceScore(PieceType::Pawn, color) + getBonusPieceScore(PieceType::Knight, color) +
               getBonusPieceScore(PieceType::Bishop, color) + getBonusPieceScore(PieceType::Rook, color) +
               getBonusPieceScore(PieceType::Queen, color) + getBonusKingScore(color, phase);
    }

    int BoardEval::getScore() const {
        int phase = getPhase();
        return getMaterialScore(White) - getMaterialScore(Black)
               + getBonusScore(White, phase) - getBonusScore(Black, phase)
               + getPawnStructureScore(White,phase)-getPawnStructureScore(Black,phase);
    }

}