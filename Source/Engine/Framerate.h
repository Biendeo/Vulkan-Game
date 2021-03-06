#pragma once

#include <cstdint>

namespace Biendeo::VulkanGame {
	class Framerate {
		public:
		Framerate(short expectedFPS);

		void SleepToNextSwapBuffer();
		void UpdateDrawTimes();
		uint64_t IncrementFrameCount();

		short ExpectedFPS();
		short ExpectedFPS(short expectedFPS);

		uint64_t FrameCount();

		double Delta();

		private:
		short expectedFPS;
		double delta;
		double lastDraw;
		double nextDraw;
		uint64_t frameCount;
	};
}
