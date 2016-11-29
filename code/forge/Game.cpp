#include "Game.h"
#include "anvil/interface/platform.h"

extern Anvil::Platform *platform;

namespace Game {
	void Main::Update() {
		auto w = platform->screen->GetBufferWidth();
		auto h = platform->screen->GetBufferHeight();
		auto screen = platform->screen->GetBuffer();

		if(modFrame > 0) {
			modFrame = 0;
			auto p = 0;
			for(auto y = 0u ; y < h; ++y)
				for(auto x = 0u; x < w; ++x)
					screen[p++] = (x * y * frame);
		}
		++modFrame;
		++frame;
	}
}