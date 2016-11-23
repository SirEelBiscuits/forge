#pragma once

#include <memory>
#include <functional>

#include "keyboard.h"
#include "mouse.h"
#include "screen.h"
#include "memory.h"

namespace Anvil {
	using GetInputSignature    = void(*)();
	using RequestRedrawSignature = void(*)();
	using LogSignature         = void(*)(char*, ...);
	using QuitSignature        = std::function<void()>;

	struct Platform {
		Memory   *memory   = nullptr;
		Screen   *screen   = nullptr;
		Keyboard *keyboard = nullptr;
		Mouse    *mouse    = nullptr;

		GetInputSignature    GetInput     = nullptr;
		RequestRedrawSignature RequestRedraw  = nullptr;
		LogSignature         Log          = nullptr;
		QuitSignature        QuitCallback{};
	};
}