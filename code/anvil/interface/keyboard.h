#pragma once

namespace Anvil {
	class Keyboard {
	public:
		using KeyCode = int;
		static const KeyCode NumKeycodes = 1<<10;

		enum class KeyState {
			Up           = 0,
			Down         = 1,
			NewlyChanged = 2,
			Released     = 2,
			Pressed      = 3,
		};

		static KeyCode MakeKeycode(int ScanCode, bool E0 = false, bool E1 = false) {
			return ScanCode + (E0?1<<8:0) + (E1?1<<9:0);
		};

		Keyboard()
			: KeyStates{}
		{}

		KeyState GetKeyState(KeyCode key) {
			return KeyStates[key];
		}

	protected:
		KeyState KeyStates[NumKeycodes];
	};
}