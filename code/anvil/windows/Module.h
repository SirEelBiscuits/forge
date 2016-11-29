#pragma once

#include "interface/Module.h"

namespace Windows {
	class Module : public Anvil::Module {
	public:
		using methodType = void(*)(void*);

		Module(char const * moduleName);
		virtual ~Module(){}

		virtual void CallFunction(char const * name) override;
		virtual void RunModuleUpdate()               override;
		virtual void ReloadModule()                  override;

	private:
		methodType GetMethod(char const * name);
		size_t GetObjectSize();
		void * thisPtr;
		void* dllHandle;
	};
}