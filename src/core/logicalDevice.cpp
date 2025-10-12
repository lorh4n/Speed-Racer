#include <core/logicalDevice.hpp>

std::pair<VkDevice, LogicalDeviceCreator::DeviceQueue> LogicalDeviceCreator::create(
    VkPhysicalDevice physicalDevice,
    QueueManager& queueManager,
    bool enableValidationLayers,
    const std::vector<const char*>& validationLayers,
    const std::vector<const char*>& deviceExtensions) {
    
    // Get queue family info from QueueManager
    const auto& queueFamilies = queueManager.getQueueFamilies();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies;

    // Create queue create infos
    for (const auto& [type, info] : queueFamilies) {
        if (uniqueQueueFamilies.insert(info.index).second) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = info.index;
            queueCreateInfo.queueCount = info.queueCount;
            float queuePriority = 1.0f; // Configurable in the future
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
    }

    // Device features (customize as needed)
    VkPhysicalDeviceFeatures deviceFeatures{};

    // Device creation
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkDevice device;
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("[LogicalDeviceCreator] : Failed to create logical device");
    }

    // Retrieve queues
    DeviceQueue queues{
        queueManager.getQueue(device, QueueType::GRAPHICS),
        queueManager.getQueue(device, QueueType::PRESENT)
    };

    return {device, queues};
}