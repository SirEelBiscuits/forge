#pragma once

#include "interface/keyboard.h"
#include "windows.h"

namespace Windows {
	class Keyboard : public Anvil::Keyboard {
	public:
		void SetKeyState(KeyCode key, KeyState state) {
			state = (KeyState)((int)state + (int)KeyState::NewlyChanged * (state != KeyStates[key]));
			KeyStates[key] = state;
		}

		void FlipKeyStates() {
			for(auto i = 0u; i < NumKeycodes; ++i)
				KeyStates[i] = (KeyState((int)KeyStates[i] & (~2)));
		}

		void Reset() {
			auto const numKeys = 256;
			BYTE keys[numKeys];

			GetKeyboardState(keys);

			for(auto i = 0x07u; i < numKeys; ++i) {
				auto keystate = keys[i];
				auto scan = MapVirtualKey(i, MAPVK_VK_TO_VSC);
				SetKeyState(
					MakeKeycode(scan),
					((keystate&(~1)) != 0
						? Keyboard::KeyState::Down
						: Keyboard::KeyState::Up
					)
				);
			}
		}
	};
}