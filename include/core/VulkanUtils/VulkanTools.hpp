#ifndef VULKAN_TOOLS_HPP
#define VULKAN_TOOLS_HPP

#include <vector>
#include <vulkan/vulkan.h>
#include <cstring>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace VulkanTools { // Habilita validation layers apenas em modo debug

    // Habilita validation layers apenas em modo debug
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    // Lista de validation layers
    extern const std::vector<const char*> validationLayers;

    // Verifica suporte para validation layers
    bool checkValidationLayerSupport();

    // Obtém extensões necessárias
    std::vector<const char*> getRequiredExtensions();

// Callback para mensagens de validação do Vulkan
    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    // Cria o debug messenger para capturar mensagens de validação
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);

    // Destrói o debug messenger
    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);

    // Configura o debug messenger para a instância Vulkan
    void setupDebugMessenger(
        VkInstance instance,
        VkDebugUtilsMessengerEXT& debugMessenger);

    // Popula a estrutura de criação do debug messenger com configurações padrão
    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo);

}

#endif
