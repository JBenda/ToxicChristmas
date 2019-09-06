#import "shared.hpp"

namespace shared {

const char str_player_frameCount[] = "player_frameCount";

unsigned int *player_frameCount = nullptr;

const unsigned int amount = 1;
void **shared[] = {
    (void**)str_player_frameCount,
    (void**)&player_frameCount
};

};