#include "gameLogic.hpp"
#include "Player.hpp"

enum SharedObj { PlayerView, LevelView, END};
void *SharedMem[SharedObj::END];
Player player(SharedMem);



void EMSCRIPTEN_KEEPALIVE loadMatrix(float *matrix) {
	for(int i = 0; i < 16; ++i) {
		matrix[i] = static_cast<float>(i);
	}
}

void* EMSCRIPTEN_KEEPALIVE GetPtr(int id) {
	return SharedMem[id];
}


