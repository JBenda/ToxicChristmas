#include "gameLogic.hpp"

#include <chrono>

#include "Player.hpp"
#include "World.hpp"
#include "Utility.hpp"

enum SharedObj { PlayerView, LevelView, InputStruct, END};

void *SharedMem[SharedObj::END];
void **itr = SharedMem;

Player player(itr++);
World world(itr++, player);
Input input(itr++);

unsigned long long lastUpdate = 0;

// return World Modi 
unsigned int EMSCRIPTEN_KEEPALIVE Update() {
	unsigned long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	unsigned int result = 0;
	if (lastUpdate) {
		result = world.Update(input, now - lastUpdate);
	}
	lastUpdate = now;
	return result;
}

void EMSCRIPTEN_KEEPALIVE loadMatrix(float *matrix) {
	for(int i = 0; i < 16; ++i) {
		matrix[i] = static_cast<float>(i);
	}
}

void* EMSCRIPTEN_KEEPALIVE GetPtr(int id) {
	return SharedMem[id];
}


