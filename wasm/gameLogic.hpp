#include <emscripten/emscripten.h>

#ifdef __cplusplus
extern "C" {
#endif

void EMSCRIPTEN_KEEPALIVE loadMatrix(float*);
void* EMSCRIPTEN_KEEPALIVE GetPtr(int);

#ifdef __cplusplus
}
#endif
