#include "Module.h"
#include <Windows.h>

// Cheaty implementation of the module interface

namespace Windows {

	template<typename T>
	T GetMethod(void * handle, char const * name) {
		return (T)(GetProcAddress((HMODULE)handle, name));
	}

	Module::Module(char const * name, Anvil::Platform * platform)
		: _name(name)
		, _platform(platform)
	{
		//load the module
		dllHandle = LoadLibrary(_name);
		if(dllHandle == NULL)
			_platform->Log("Couldn't load %s\n", name);
		thisPtr = calloc(1, GetObjectSize());
		auto fun = GetMethod<initialiseMethodType>(dllHandle, "Initialise");
		if(fun != NULL)
			fun(thisPtr, platform);
		else
			_platform->Log("Missing function: Initialise\n");
	}

	void Module::RunModuleUpdate() {
		auto fun = GetMethod<updateMethodType>(dllHandle, "Update");
		if(fun != NULL)
			fun(thisPtr);
		else
			_platform->Log("Missing function: Update\n");
	}

	void Module::ReloadModule() {

		FreeLibrary((HMODULE)dllHandle);
		dllHandle = LoadLibrary(_name);

		auto fun = GetMethod<reloadMethodType>(dllHandle, "Reload");
		if(fun != NULL)
			fun(_platform);
		else
			_platform->Log("Missing function: Reload\n");
	}

	size_t Module::GetObjectSize() {
		using sizeTCall = size_t (*)();
		auto getSizeF = (sizeTCall)GetProcAddress((HMODULE)dllHandle, "GetObjectSize");
		if(getSizeF == NULL) {
			_platform->Log("Missing function: GetObjectSize\n");
			return 0;
		}
		return getSizeF();
	}
}