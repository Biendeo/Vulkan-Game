#include <iostream>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Engine/Engine.h"

using namespace Biendeo::VulkanGame;

int main(int argc, char* argv[]) {
	// GLFW needs to initialize before the program starts.
	if (!glfwInit()) {
		std::cout << "GLFW couldn't initialize, something is wrong!\n";
		return 1;
	} else {
		std::cout << "Compiled on GLFW " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << "\n";
		int major, minor, revision;
		glfwGetVersion(&major, &minor, &revision);
		std::cout << "Currently running GLFW " << major << "." << minor << "." << revision << "\n";
	}

	// We need to make sure Vulkan is supported on our system before we continue.
	if (!glfwVulkanSupported()) {
		std::cout << "Vulkan is not supported.\n";
		return 1;
	} else {
		std::cout << "Vulkan is supported.\n";
	}

	// Then we get monitor and video properties.
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);
	GLFWwindow* window;

	const bool borderlessFullscreen = false;

	const char* windowTitle = "HELLO WORLD!";

	// This is the point when you call glfwWindowHint for features.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


	// Now for Vulkan integration into our GLFW window.
	uint32_t count;
	const char** extensions = glfwGetRequiredInstanceExtensions(&count);

	PFN_vkCreateInstance pfnCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(NULL, "vkCreateInstance");
	vk::InstanceCreateInfo instanceCreateInfo;
	instanceCreateInfo.setEnabledExtensionCount(count);
	instanceCreateInfo.setPpEnabledExtensionNames(extensions);
	vk::Instance instance = vk::createInstance(instanceCreateInfo);
	PFN_vkCreateDevice pfnCreateDevice = (PFN_vkCreateDevice)glfwGetInstanceProcAddress(instance, "vkCreateDevice");
	PFN_vkGetDeviceProcAddr pfnGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)glfwGetInstanceProcAddress(instance, "vkGetDeviceProcAddr");

	// Then, we create the window.
	if (borderlessFullscreen) {
		window = glfwCreateWindow(vidmode->width, vidmode->height, windowTitle, monitor, NULL);
	} else {
		window = glfwCreateWindow(800, 600, windowTitle, NULL, NULL);
	}

	// Finally, set up the Vulkan surface.
	vk::SurfaceKHR surface;
	VkSurfaceKHR surfaceLegacy = surface;
	VkResult err = glfwCreateWindowSurface(instance, window, NULL, &surfaceLegacy);
	if (err != (VkResult)vk::Result::eSuccess) {
		std::cout << "Vulkan did not create a surface properly.\n";
		return 1;
	} else {
		std::cout << "Vulkan was successfully created as a surface.\n";
	}

	glfwSwapInterval(1);

	// Testing a framerate class.
	Framerate framerate(144);

	while (true) {
		framerate.SleepToNextSwapBuffer();
		glfwSwapBuffers(window);
		std::cout << "Frame " << framerate.FrameCount() << "\n";
		framerate.UpdateDrawTimes();
		framerate.IncrementFrameCount();
	}


	// Destroy the Vulkan surface.
	vkDestroySurfaceKHR(instance, surface, NULL);

	// Destroy the window and terminate GLFW.
	glfwDestroyWindow(window);

	glfwTerminate();


	return 0;
}