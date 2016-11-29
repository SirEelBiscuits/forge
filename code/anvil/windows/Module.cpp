#include "Module.h"
#include <Windows.h>

// Cheaty implementation of the module interface

namespace Windows {
	Module::Module(char const * name) {
		//load the module
		dllHandle = LoadLibrary(name);
		thisPtr = calloc(1, GetObjectSize());
		CallFunction("Initialise");
	}

	void Module::CallFunction(char const * name) {
		GetMethod(name)(thisPtr);
	}

	void Module::RunModuleUpdate() {
		CallFunction("Update");
	}

	void Module::ReloadModule() {
	
	}

	Module::methodType Module::GetMethod(char const * name) {
		return (methodType)(GetProcAddress((HMODULE)dllHandle, name));
	}

	size_t Module::GetObjectSize() {
		using sizeTCall = size_t (*)();
		auto getSizeF = (sizeTCall)GetProcAddress((HMODULE)dllHandle, "GetObjectSize");
		if(getSizeF == NULL)
			return 0;
		return getSizeF();
	}
}