#pragma once

#include "interface/screen.h"

namespace Windows {
	class Screen : public Anvil::Screen {
	public:
		void SetBuffer(int *buffer, int width, int height) {
			buffer = buffer;
			width = width;
			height = height;
		}
	};
}
