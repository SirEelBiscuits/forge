#include <Windows.h>

#include "interface/platform.h"
#include "interface/keyboard.h"
#include "interface/mouse.h"
#include "interface/screen.h"
#include "interface/memory.h"
#include "interface/logging.h"

static Anvil::Platform platform;
static WNDCLASSEX WindowClass;

BOOL Init(HINSTANCE instance);
HWND MakeWindow(HINSTANCE instance);
LRESULT CALLBACK MainWndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	HINSTANCE instance,
	HINSTANCE prevInstance,
	LPSTR CmdLine,
	int CmdShow
) {
	Init(instance);
	MakeWindow(instance);
}

BOOL Init(HINSTANCE instance) {
	platform.memory   = std::make_unique<Anvil::Memory>();
	platform.screen   = std::make_unique<Anvil::Screen>();
	platform.keyboard = std::make_unique<Anvil::Keyboard>();
	platform.mouse    = std::make_unique<Anvil::Mouse>();
	platform.logging  = std::make_unique<Anvil::Logging>();

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

}

HWND MakeWindow(HINSTANCE instance) {
	HWND hwnd = CreateWindow(
		"Window Class",
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

	ShowWindow(hwnd, 0);

	return hwnd;
}