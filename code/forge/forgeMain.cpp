#include <cstdio>

#include "anvil/interface/platform.h"
#include "Game.h"

Anvil::Platform *platform;

extern "C" __declspec(dllexport) size_t GetObjectSize() {
	return sizeof(Game::Main);
}

extern "C" __declspec(dllexport) void Initialise(void* thisPtr, Anvil::Platform * platform) {
	::platform = platform;
	platform->Log("Initialised\n");
	new(thisPtr) Game::Main();
}

extern "C" __declspec(dllexport) void Reload(Anvil::Platform * platform) {
	::platform = platform;
}

extern "C" __declspec(dllexport) void Update(void* thisPtr) {
	reinterpret_cast<Game::Main*>(thisPtr)->Update();
}