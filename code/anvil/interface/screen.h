#pragma once

namespace Anvil {
	class Screen {
	public:
		using color = int;
		using bufferDimension = unsigned;

		color           *GetBuffer()       { return _buffer; }
		bufferDimension  GetBufferWidth()  { return _width;  }
		bufferDimension  GetBufferHeight() { return _height; }
	
	protected:
		color *_buffer;
		bufferDimension _width;
		bufferDimension _height;
	};
}