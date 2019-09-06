#import "shared.hpp"

namespace shared {

unsigned int *player_frameCount = nullptr;

const unsigned int amount = 1;
void **shared[] = {(void**)(&player_frameCount)};

};