#ifndef BOARD_H
#define BOARD_H

#include<cstdint>
#include<iostream>
#include<stack>
#include "GameState.h"
#include "Move.h"
#include "Types.h"
namespace Engine{

	class Board{
		private:
			Piece pieces[8][8];
			std::stack<GameState> history;
			GameState current_game_state;
			void initBoard();
		public:
			Board();
			void print() const;
			Piece getPieceAt(Position position) const;
			void makeMove(Move move);
			void undoLastMove();
			Color getTurn() const;
	};
}


#endif
