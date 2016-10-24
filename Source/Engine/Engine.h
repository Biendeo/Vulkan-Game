#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Framerate.h"

namespace Biendeo::VulkanGame {
	class Engine {
		public:
		Engine(int argc, char* argv[]);
		~Engine();

		void Run();

		private:
		Framerate* framerate;

		GLFWmonitor* monitor;
		const GLFWvidmode* vidmode;
		GLFWwindow* window;

		vk::ApplicationInfo applicationInfo;
		vk::InstanceCreateInfo instanceInfo;
		vk::PhysicalDevice physicalDevice;
		vk::Device device;
		vk::Instance instance;
		vk::SurfaceKHR surface;

		bool verbose;

		bool InitialiseGLFW();
		bool CheckVulkanCompatability();
		bool InitialiseWindow();

		void DrawBuffer();
	};
}