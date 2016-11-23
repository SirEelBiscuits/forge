#include <Windows.h>
#include <chrono>
#include <iostream>

#include "interface/platform.h"
#include "interface/keyboard.h"
#include "interface/mouse.h"
#include "interface/screen.h"
#include "interface/memory.h"

using color = int;

static Anvil::Platform platform;
static WNDCLASSEX WindowClass;
static HWND window;
static BITMAPINFO bufferInfo;
static color * screenBuffer;
static int winW, winH;

BOOL Init(HINSTANCE instance);
HWND MakeWindow(HINSTANCE instance, int cmdShow);
LRESULT CALLBACK MainWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
void GetInput();
void RequestRedraw();
void FlipBuffers();
void ResizeScreenBuffer(int x, int y);
bool Resizing { false };
void Log(char* format, ...);

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE prevInstance,
	LPSTR CmdLine,
	int CmdShow
) {
	Init(instance);
	window = MakeWindow(instance, CmdShow);

	bool quit { false };
	int frame = 0;
	platform.QuitCallback = [&]() { quit = true; };

	auto time = std::chrono::high_resolution_clock::now();

	int modFrame = 0;
	int mul = 0;

	while (!quit) {
		platform.GetInput();
		if(modFrame > 0) {
			modFrame = 0;
			auto p = 0;
			for(int y = 0 ; y < winH; ++y)
				for(int x = 0; x < winW; ++x)
					screenBuffer[p++] = x * y * frame;
		}
		++modFrame;
		++frame;
		platform.RequestRedraw();
	}

	auto passed = std::chrono::high_resolution_clock::now() - time;
	auto msPassed = std::chrono::duration_cast<std::chrono::milliseconds>(passed).count();
	auto fps = frame / (std::chrono::duration_cast<std::chrono::milliseconds>(passed).count() / 1000.f);

	platform.Log("time: %dms, %d frames, %f avg fps\n"),
		(int)std::chrono::duration_cast<std::chrono::milliseconds>(passed).count(), 
		frame, 
		frame / (std::chrono::duration_cast<std::chrono::milliseconds>(passed).count() / 1000.f)
	);

}

BOOL Init(HINSTANCE instance) {
	platform.memory   = std::make_unique<Anvil::Memory>();
	platform.screen   = std::make_unique<Anvil::Screen>();
	platform.keyboard = std::make_unique<Anvil::Keyboard>();
	platform.mouse    = std::make_unique<Anvil::Mouse>();

	platform.GetInput      = &GetInput;
	platform.RequestRedraw = &RequestRedraw;
	platform.Log           = &Log;

	WindowClass.cbSize = sizeof(WindowClass);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWndProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hInstance = instance;
	WindowClass.hIcon = NULL;
	WindowClass.hCursor = NULL;
	WindowClass.hbrBackground = NULL;
	WindowClass.lpszMenuName = nullptr;
	WindowClass.lpszClassName = "WindowClass";
	WindowClass.hIconSm = NULL;

	return RegisterClassEx(&WindowClass);
}

LRESULT CALLBACK MainWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {

	switch(message) {
	case WM_SHOWWINDOW:
		ResizeScreenBuffer(winW, winH);
		break;

	case WM_PAINT:
		FlipBuffers();
		break;

	case WM_SIZE:
		winW = LOWORD(lParam);
		winH = HIWORD(lParam);
		if(!Resizing)
			ResizeScreenBuffer(winW, winH);
		FlipBuffers();
		break;

	case WM_EXITSIZEMOVE:
		ResizeScreenBuffer(winW, winH);
		break;

	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		platform.QuitCallback();
		break;

	default: break;
	}
	return DefWindowProc(window, message, wParam, lParam);
}

HWND MakeWindow(HINSTANCE instance, int cmdShow) {
	HWND hwnd = CreateWindow(
		"WindowClass",
		"Main Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		instance,
		nullptr
	);

	if (!hwnd)
		return NULL;

	ShowWindow(hwnd, cmdShow);

	return hwnd;
}

void GetInput() {
	MSG msg;
	while (PeekMessage(&msg, window, 0, 0, PM_REMOVE) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void RequestRedraw() {
	RedrawWindow(window, NULL, NULL, RDW_INVALIDATE);
}

void FlipBuffers() {
	PAINTSTRUCT ps;

	auto hdc = BeginPaint(window, &ps);

	RECT rect;
	GetClientRect(window, &rect);

	SetDIBitsToDevice(
		hdc,
		0, 0, rect.right - rect.left, rect.bottom - rect.top,
		0, 0, 0, bufferInfo.bmiHeader.biHeight,
		screenBuffer,
		&bufferInfo,
		DIB_RGB_COLORS
	);

	EndPaint(window, &ps);
}

void ResizeScreenBuffer(int x, int y) {
	bufferInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bufferInfo.bmiHeader.biWidth = x;
	bufferInfo.bmiHeader.biHeight = y;
	bufferInfo.bmiHeader.biPlanes = 1;
	bufferInfo.bmiHeader.biBitCount = sizeof(color) * 8;
	bufferInfo.bmiHeader.biCompression = BI_RGB;

	delete[] screenBuffer;
	screenBuffer = new color[x*y];

	platform.screen->SetBuffer(screenBuffer, x, y);
}

void Log(char* format, ...) {
	const auto size = 2000;
	auto buffer = new char[size];

	va_list args;
	va_start(args, format);
	vsnprintf(buffer, size, format, args);
	OutputDebugString(buffer);
	va_end(args);

	delete buffer;;
}
