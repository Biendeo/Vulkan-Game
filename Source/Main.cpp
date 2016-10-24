#include <iostream>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Engine/Engine.h"

using namespace Biendeo::VulkanGame;

int main(int argc, char* argv[]) {
	Engine engine(argc, argv);

	engine.Run();


	return 0;
}