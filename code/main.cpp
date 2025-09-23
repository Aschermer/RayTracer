#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <stdio.h>
#include <string.h>

#include "defines.h"


#define LOAD(x) PFN_ ## x x = (PFN_ ## x) vkGetInstanceProcAddr(instance, #x)



struct QueueFamilyIndicies {
    bool graphicsFamilyFound;
    uint32 graphicsFamily;
};



const char *validationLayers[]= {
    "VK_LAYER_KHRONOS_validation"
};

const char *appExtensions[] = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};




global bool enableValidationLayers = true;

global uint32_t WindowWidth = 1920;
global uint32_t WindowHeight = 1080;





static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void *pUserData)
{
    switch(messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        {
            printf("\x1B[32mVERBOSE::VALIDATION LAYER::%s\x1B[0m\n", pCallbackData->pMessage);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        {
            printf("\x1B[34mINFO::VALIDATION LAYER::%s\x1B[0m\n", pCallbackData->pMessage);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        {
            printf("\x1B[33mWARNING::VALIDATION LAYER::%s\x1B[0m\n", pCallbackData->pMessage);
        } break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        {
            printf("\x1B[31mERROR::VALIDATION LAYER::%s\x1B[0m\n", pCallbackData->pMessage);
        } break;
        default:
        {
            printf("\x1B[35mUNKNOWN::VALIDATION LAYER::%s\x1B[0m\n", pCallbackData->pMessage);
        } break;
    }
    
    return VK_FALSE;
}





int main(int argc, char *argv[]) {
    glfwInit();
    
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
    
    
    
    uint32 glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    uint32 appExtensionCount = ARRAY_SIZE(appExtensions);
    uint32 extensionCount = appExtensionCount + glfwExtensionCount;
    
    char **extensions = (char**)malloc(extensionCount * sizeof(appExtensions[0]));
    for(int i = 0; i < glfwExtensionCount; i++)
    {
        extensions[i] = (char*)malloc((strlen(glfwExtensions[i]) + 1) * sizeof(char));
        strcpy(extensions[i], glfwExtensions[i]);
    }
    for(int i = 0; i < appExtensionCount; i ++)
    {
        extensions[i + glfwExtensionCount] = (char*)malloc((strlen(appExtensions[i]) + 1) * sizeof(char));
        strcpy(extensions[i + glfwExtensionCount], appExtensions[i]);
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
    createInfo.enabledLayerCount = (uint32)ARRAY_SIZE(validationLayers);
    createInfo.ppEnabledLayerNames = validationLayers;
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;
    
    
    
    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        printf("Failed to Create Vulkan Instance\n");
    }
    
    
    LOAD(vkCreateDebugUtilsMessengerEXT);
    LOAD(vkDestroyDebugUtilsMessengerEXT);
    
    
    
    VkDebugUtilsMessengerEXT debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = 0x1111; //VK_DEBUG_UTILS_MESSAGE_SEVERITY
    debugCreateInfo.messageType = 0b111; //VK_DEBUG_UTILS_MESSAGE_TYPE
    debugCreateInfo.pfnUserCallback = debugCallback;
    debugCreateInfo.pUserData = nullptr;
    
    vkCreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger);
    
    
    
    
    
    VkSurfaceKHR surface;
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        printf("Failed to create window surface\n");
    }
    
    
    
    
    
    
    
    
    
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    QueueFamilyIndicies physicalDeviceIndicies{};
    
    uint32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if(deviceCount == 0)
    {
        printf("No Devices Found\n");
    }
    
    VkPhysicalDevice *devices = (VkPhysicalDevice *)malloc(deviceCount * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
    
    for(int i = 0; i < deviceCount; i++)
    {
        QueueFamilyIndicies indicies;
        
        uint32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, nullptr);
        
        VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, queueFamilies);
        
        for(int j = 0; j < queueFamilyCount; j++)
        {
            if(queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indicies.graphicsFamilyFound = true;
                indicies.graphicsFamily = j;
            }
        }
        
        
        if(indicies.graphicsFamilyFound)
        {
            physicalDevice = devices[i];
            physicalDeviceIndicies = indicies;
        }
    }
    
    if(physicalDevice == VK_NULL_HANDLE)
    {
        printf("No Suitable Devices");
    }
    
    
    VkDevice device;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = physicalDeviceIndicies.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    float graphicsQueuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &graphicsQueuePriority;
    
    VkPhysicalDeviceFeatures deviceFeatures{};
    
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.enabledLayerCount = 0;
    
    if(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
    {
        printf("Logical Device Creation Failed\n");
    }
    
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, physicalDeviceIndicies.graphicsFamily, 0, &graphicsQueue);
    
    
    
    
    
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    
    vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    vkDestroyDevice(device, nullptr);
    
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    
    return 0;
}