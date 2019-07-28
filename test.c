#include <stdio.h>
#include <emscripten/emscripten.h>

int main(int _argc, char** _argv)  {
	printf("Hello world\n");
	return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

void EMSCRIPTEN_KEEPALIVE foo(int _argc, char** _argv) {
	printf("foo is called\n");
}

int EMSCRIPTEN_KEEPALIVE  add(int a, int b) {
	return a + b;
}


int  c = -1;

int EMSCRIPTEN_KEEPALIVE count () {
	return ++c;
}

#ifdef __cplusplus
}
#endif
