#include "Framerate.h"

#include <thread>

#include <GLFW/glfw3.h>

namespace Biendeo::VulkanGame {
	Framerate::Framerate(short expectedFPS) {
		this->expectedFPS = expectedFPS;
		this->frameCount = 0ull;
		UpdateDrawTimes();
	}

	void Framerate::SleepToNextSwapBuffer() {
		double currentTime = glfwGetTime();
		auto nanos = static_cast<uint64_t>((nextDraw - currentTime) * 1000000000.0);
		std::this_thread::sleep_for(std::chrono::nanoseconds(nanos));
	}

	void Framerate::UpdateDrawTimes() {
		double previousDraw = this->lastDraw;
		this->lastDraw = glfwGetTime();
		this->delta = this->lastDraw - previousDraw;
		this->nextDraw = lastDraw + 1.0 / expectedFPS;
	}

	uint64_t Framerate::IncrementFrameCount() {
		return ++frameCount;
	}

	short Framerate::ExpectedFPS() {
		return expectedFPS;
	}

	short Framerate::ExpectedFPS(short expectedFPS) {
		this->expectedFPS = expectedFPS;
		return expectedFPS;
	}

	uint64_t Framerate::FrameCount() {
		return frameCount;
	}

	double Framerate::Delta() {
		return delta;
	}
}