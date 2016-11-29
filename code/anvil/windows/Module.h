#pragma once

#include "interface/Module.h"

namespace Windows {
	class Module : public Anvil::Module {
	public:
		using updateMethodType = void(*)(void*);
		using initialiseMethodType = void(*)(void*, Anvil::Platform*);
		using reloadMethodType = void(*)(Anvil::Platform*);

		Module(char const * moduleName, Anvil::Platform * platform);
		virtual ~Module(){}

		virtual void RunModuleUpdate()               override;
		virtual void ReloadModule()                  override;

	private:
		size_t GetObjectSize();
		void * thisPtr {nullptr};
		void* dllHandle;

		char const *     _name {nullptr};
		Anvil::Platform *_platform {nullptr};
	};
}