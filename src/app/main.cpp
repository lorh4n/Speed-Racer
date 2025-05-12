#include <iostream>
#include <stdexcept>

#include <core/VulkanManager.hpp>

int main() {

	VulkanManager app;

	try {
		app.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Hello World" << std::endl;

	return 0;
}