#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <stdio.h>
#include <string.h>

#include "defines.h"

const char *validationLayers[]= {
    "VK_LAYER_KHRONOS_validation"
};

bool enableValidationLayers = true;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void *pUserData)
{
    if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        printf("Validation Layer: %s", pCallbackData->pMessage);
    }
    
    return VK_FALSE;
}

int main(int argc, char *argv[]) {
    glfwInit();
    
    uint32_t WindowWidth = 1920;
    uint32_t WindowHeight = 1080;
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Window", nullptr, nullptr);
    
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    VkLayerProperties *availableLayers = (VkLayerProperties *)malloc(layerCount * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
    
    for(int rLayerIndex = 0; rLayerIndex < ARRAY_SIZE(validationLayers); rLayerIndex++)
    {
        bool layerFound = false;
        char *rLayerName = (char *)validationLayers[rLayerIndex];
        
        for(int aLayerIndex = 0; aLayerIndex < layerCount; aLayerIndex++)
        {
            char *aLayerName = availableLayers[aLayerIndex].layerName;
            if(strcmp(rLayerName, aLayerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
        
        if(!layerFound)
        {
            printf("Validation Layer not found: %s\n", rLayerName);
            enableValidationLayers = false;
        }
    }
    
    VkInstance instance;
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Window";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    if(enableValidationLayers)
    {
        createInfo.enabledLayerCount = (uint32)ARRAY_SIZE(validationLayers);
        createInfo.ppEnabledLayerNames = validationLayers;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }
    
    uint32 glfwExtensionCount = 0;
    const char **glfwExtensions;
    
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    const char *appExtensions[] = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    uint32 appExtensionCount = ARRAY_SIZE(appExtensions);
    
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    
    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        printf("Failed to Create Vulkan Instance\n");
    }
    
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    VkDebugUtilsMessengerEXT debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT|
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT|
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
    debugCreateInfo.pUserData = nullptr;
    
    vkCreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger);
    
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    
    if(enableValidationLayers)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    
    vkDestroyInstance(instance, nullptr);
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    
    return 0;
}