#pragma once

#include "platform.h"

namespace Anvil {
	class Module {
	public:
		virtual ~Module(){}

		virtual void   RunModuleUpdate() = 0;
		virtual void   ReloadModule() = 0;
	};
}