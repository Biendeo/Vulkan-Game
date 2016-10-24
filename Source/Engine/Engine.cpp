#include "Engine.h"

#include <iostream>
#include <string>
#include <vector>

namespace Biendeo::VulkanGame {
	Engine::Engine(int argc, char* argv[]) {
		// Start off by turning the arguments into a vector.
		std::vector<std::string> arguments(argc);
		for (int i = 0; i < argc; ++i) {
			arguments[i] = argv[i];
		}

		// Set up the command-line variables.
		this->verbose = true;

		for (const std::string& arg : arguments) {
			// The /Q flag makes this program not print output.
			if (arg == "/Q") {
				verbose = false;
			}
		}

		// Then we set up our program.
		if (!InitialiseGLFW()) {
			abort();
		}

		if (!CheckVulkanCompatability()) {
			abort();
		}

		if (!InitialiseWindow()) {
			abort();
		}

		// This prevents GLFW from swapping buffers until it's finished.
		glfwSwapInterval(1);

		// This establishes a framerate.
		// TODO: Custom based on argument?
		framerate = new Framerate(vidmode->refreshRate);
	}

	Engine::~Engine() {
		if (framerate != nullptr) {
			delete framerate;
		}

		instance.destroySurfaceKHR(surface);
		instance.destroy();

		glfwDestroyWindow(window);

		glfwTerminate();
	}

	void Engine::Run() {
		while (true) {
			DrawBuffer();
			framerate->SleepToNextSwapBuffer();
			glfwSwapBuffers(window);
			std::cout << "Frame " << framerate->FrameCount() << "\n";
			framerate->UpdateDrawTimes();
			framerate->IncrementFrameCount();
		}
	}

	bool Engine::InitialiseGLFW() {
		if (!glfwInit()) {
			if (verbose) std::cout << "GLFW couldn't initialize, something is wrong!\n";
			return false;
		} else {
			if (verbose) {
				std::cout << "Compiled on GLFW " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << "\n";
				int major, minor, revision;
				glfwGetVersion(&major, &minor, &revision);
				std::cout << "Currently running GLFW " << major << "." << minor << "." << revision << "\n";
			}
			return true;
		}
	}

	bool Engine::CheckVulkanCompatability() {
		if (!glfwVulkanSupported()) {
			if (verbose) std::cout << "Vulkan is not supported.\n";
			return false;
		} else {
			if (verbose) std::cout << "Vulkan is supported.\n";
			return true;
		}
	}

	bool Engine::InitialiseWindow() {
		// Then we get monitor and video properties.
		monitor = glfwGetPrimaryMonitor();
		vidmode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);

		const bool borderlessFullscreen = false;

		const char* windowTitle = "HELLO WORLD!";

		// This is the point when you call glfwWindowHint for features.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// Then we set up the Vulkan application information.
		applicationInfo = vk::ApplicationInfo("HELLO WORLD!", 0U, "B-Power", 0U, 0U);

		// Now for Vulkan integration into our GLFW window.
		uint32_t count;
		const char** extensionsPtr = glfwGetRequiredInstanceExtensions(&count);
		std::vector<const char*> extensions(count);

		for (int i = 0; i < count; ++i) {
			extensions[i] = extensionsPtr[i];
		}

		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

		PFN_vkCreateInstance pfnCreateInstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(nullptr, "vkCreateInstance");

		instanceInfo = vk::InstanceCreateInfo(vk::InstanceCreateFlags(), &applicationInfo, 0, nullptr, extensions.size(), extensions.data());

		instance = vk::createInstance(instanceInfo);

		std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

		if (physicalDevices.empty()) {
			if (verbose) std::cout << "No physical devices for Vulkan to render on.\n";
			return false;
		}

		for (vk::PhysicalDevice& physicalDevice : physicalDevices) {
			vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
			if (verbose) {
				std::cout << "Driver Version: " << properties.driverVersion << "\n";
				std::cout << "Driver Name:    " << properties.deviceName << "\n";
				std::cout << "Driver Type:    " << vk::to_string(properties.deviceType) << "\n";
				std::cout << "API Version:    " << VK_VERSION_MAJOR(properties.apiVersion) << "." << VK_VERSION_MINOR(properties.apiVersion) << "." << VK_VERSION_PATCH(properties.apiVersion) << "\n";
			}
			std::vector<vk::QueueFamilyProperties> queueFamily = physicalDevice.getQueueFamilyProperties();
			if (verbose) std::cout << "Queue count:    " << queueFamily.size() << "\n";
			int index = 0;
			for (vk::QueueFamilyProperties& queueProperties : queueFamily) {
				if (verbose) {
					std::cout << "Queue " << index << "\n";
					if (queueProperties.queueFlags & vk::QueueFlagBits::eGraphics) std::cout << "    Graphics\n";
					if (queueProperties.queueFlags & vk::QueueFlagBits::eCompute) std::cout << "    Compute\n";
					if (queueProperties.queueFlags & vk::QueueFlagBits::eTransfer) std::cout << "    Transfer\n";
					if (queueProperties.queueFlags & vk::QueueFlagBits::eSparseBinding) std::cout << "    Sparse Binding\n";
				}
				++index;
			}
		}

		physicalDevice = physicalDevices[0];

		vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo();
		vk::DeviceQueueCreateInfo deviceQueueInfo = vk::DeviceQueueCreateInfo();

		float queuePriorities[] = {1.0f};

		deviceQueueInfo.setQueueCount(1);
		deviceQueueInfo.setPQueuePriorities(queuePriorities);
		deviceInfo.setQueueCreateInfoCount(1);
		deviceInfo.setPQueueCreateInfos(&deviceQueueInfo);

		VkDevice deviceLegacy;

		vkCreateDevice(physicalDevice, &(VkDeviceCreateInfo)deviceInfo, nullptr, &deviceLegacy);

		device = vk::Device(deviceLegacy);

		PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;

		*(void **)&vkGetPhysicalDeviceSurfaceFormatsKHR = vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");

		PFN_vkCreateDevice pfnCreateDevice = (PFN_vkCreateDevice)glfwGetInstanceProcAddress(instance, "vkCreateDevice");
		PFN_vkGetDeviceProcAddr pfnGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)glfwGetInstanceProcAddress(instance, "vkGetDeviceProcAddr");

		// Then, we create the window.
		if (borderlessFullscreen) {
			window = glfwCreateWindow(vidmode->width, vidmode->height, windowTitle, monitor, nullptr);
		} else {
			window = glfwCreateWindow(800, 600, windowTitle, nullptr, nullptr);
		}

		// Finally, set up the Vulkan surface.
		// TODO: Add surface properties here.
		VkSurfaceKHR surfaceLegacy = surface;
		VkResult err = glfwCreateWindowSurface(instance, window, nullptr, &surfaceLegacy);
		if (err != (VkResult)vk::Result::eSuccess) {
			std::cout << "Vulkan did not create a surface properly.\n";
			return 1;
		} else {
			std::cout << "Vulkan was successfully created as a surface.\n";
		}

		return true;
	}

	void Engine::DrawBuffer() {

	}
}