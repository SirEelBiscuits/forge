#include <Windows.h>
#include <chrono>
#include <iostream>

#include "interface/platform.h"
#include "interface/mouse.h"
#include "interface/memory.h"

#include "keyboard.h"
#include "screen.h"
#include "Module.h"

using color = int;

static Anvil::Platform platform;

static Windows::Screen screen;
static Windows::Keyboard keyboard;

//TODO windows ones
static Anvil::Memory memory;
static Anvil::Mouse mouse;

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
	if(Init(instance) == FALSE) {
		//TODO
		return EXIT_FAILURE;
	}

	window = MakeWindow(instance, CmdShow);
	if(window == NULL) {
		//TODO
		return EXIT_FAILURE;
	}

	RAWINPUTDEVICE kb[2] = {};
	kb[0].usUsagePage = 1;
	kb[0].usUsage = 6;
	kb[0].hwndTarget = window;
	kb[1].usUsagePage = 1;
	kb[1].usUsage = 2;
	kb[1].hwndTarget = window;
	kb[1].dwFlags = 0;
	
	if(RegisterRawInputDevices(kb, 2, sizeof(kb[0])) == FALSE) {
		platform.Log("RawInput error: %d\n", GetLastError());
	}

	bool quit { false };
	int frame = 0;
	platform.QuitCallback = [&]() { quit = true; };

	auto time = std::chrono::high_resolution_clock::now();

	Windows::Module forgeDLL("forge.dll", &platform);

	while (!quit) {
		forgeDLL.ReloadModule();

		platform.GetInput();

		forgeDLL.RunModuleUpdate();

		platform.RequestRedraw();
	}

	auto passed = std::chrono::high_resolution_clock::now() - time;
	auto msPassed = std::chrono::duration_cast<std::chrono::milliseconds>(passed).count();
	auto fps = frame / (std::chrono::duration_cast<std::chrono::milliseconds>(passed).count() / 1000.f);

	platform.Log("time: %dms, %d frames, %f avg fps\n",
		(int)std::chrono::duration_cast<std::chrono::milliseconds>(passed).count(),
		frame,
		frame / (std::chrono::duration_cast<std::chrono::milliseconds>(passed).count() / 1000.f)
	);

	return EXIT_SUCCESS;
}

BOOL Init(HINSTANCE instance) {
	platform.GetInput      = &GetInput;
	platform.RequestRedraw = &RequestRedraw;
	platform.Log           = &Log;

	platform.memory   = &memory;
	platform.screen   = &screen;
	platform.keyboard = &keyboard;
	platform.mouse    = &mouse;
	
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

	case WM_ACTIVATE:
		if(wParam != 0) {
			keyboard.Reset();
		}

	case WM_INPUT: {
		RAWINPUT data = {};
		UINT size = sizeof(data);
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &data, &size, sizeof(RAWINPUTHEADER));

		switch(data.header.dwType) {
		case RIM_TYPEKEYBOARD: {
			auto key = Anvil::Keyboard::MakeKeycode(data.data.keyboard.MakeCode, (data.data.keyboard.Flags & 2) != 0);
			keyboard.SetKeyState(
				key,
				(data.data.keyboard.Flags & 1) != 0
					? Anvil::Keyboard::KeyState::Up
					: Anvil::Keyboard::KeyState::Down
			);
			break;
		}
		default:
			break;
		}
	}

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
	keyboard.FlipKeyStates();
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

	screen.SetBuffer(screenBuffer, x, y);
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
