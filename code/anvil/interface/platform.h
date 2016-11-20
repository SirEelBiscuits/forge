#include <memory>

#include "keyboard.h"
#include "mouse.h"
#include "screen.h"
#include "memory.h"
#include "logging.h"

namespace Anvil {
	using GetInputSignature = void(*)();
	using FlipBuffersSignature = void(*)();

	struct Platform {
		std::unique_ptr<Memory>   memory = nullptr;
		std::unique_ptr<Screen>   screen = nullptr;
		std::unique_ptr<Keyboard> keyboard = nullptr;
		std::unique_ptr<Mouse>    mouse = nullptr;
		std::unique_ptr<Logging>  logging = nullptr;

		GetInputSignature    GetInput = nullptr;
		FlipBuffersSignature FlipBuffers = nullptr;
	};
}