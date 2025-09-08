#include <iostream>
#include <stdexcept>

#include <core/VulkanManager.hpp>

int main() {
	try {
		VulkanManager vulkanManager(1280, 720, "Speed Racer");
		vulkanManager.run();
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}