#pragma once

#include "interface/screen.h"

namespace Windows {
	class Screen : public Anvil::Screen {
	public:
		void SetBuffer(int *buffer, int width, int height) {
			_buffer = buffer;
			_width = width;
			_height = height;
		}
	};
}
