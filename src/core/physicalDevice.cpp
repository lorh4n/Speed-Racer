#include <core/physicalDevice.hpp>


VkPhysicalDevice PhysicalDeviceSelector::select(VkInstance instance) {
   uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
   if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
   std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
   for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			return device;
		}
	}

	throw std::runtime_error("failed to find a suitable GPU!");
}



bool PhysicalDeviceSelector::isDeviceSuitable(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    std::cout << "GPU ENCONTRADA: " << deviceProperties.deviceName << std::endl;

    bool result = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;

    std::cout << "Tipo: " << deviceProperties.deviceType << " | Geometry Shader: " << deviceFeatures.geometryShader << std::endl;
    if (!result) {
        std::cout << "This Device is NOT Suitable!!" << std::endl;
    }

    return result;
}