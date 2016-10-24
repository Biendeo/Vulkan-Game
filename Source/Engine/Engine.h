#pragma once

#include "Framerate.h"

namespace Biendeo::VulkanGame {
	class Engine {
		public:
		Engine();

		private:
		Framerate framerate;
	};
}