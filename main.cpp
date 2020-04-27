#include<iostream>
#include<fstream>
using namespace std;

#include"Protocol.h"
#include"Board.h"
#include "Position.h"


using namespace Engine;

int main(){
    srand(time(NULL));
	Engine::Board board;
	Protocol uci(board);
	uci.start();
}
