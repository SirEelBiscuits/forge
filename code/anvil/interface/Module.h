#pragma once

#include "platform.h"

namespace Anvil {
	class Module {
	public:
		virtual ~Module(){}

		virtual void   CallFunction(char const * name) = 0;
		virtual void   RunModuleUpdate() = 0;
		virtual void   ReloadModule() = 0;
	};
}