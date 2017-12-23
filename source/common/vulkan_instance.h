#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanInstance
{
public:
    VulkanInstance(const std::string& applicationName) :
        VulkanInstance(applicationName, {}, {})
    {}

    VulkanInstance(const std::string& applicationName, const std::vector<const char*>& validationLayers,
        const std::vector<const char*>& extensions) :
        debugCallbackLoaded(false)
    {
        if (!checkValidationLayerSupport(validationLayers))
        {
            throw std::runtime_error("One of the requested validation layers is not present");
        }

        const VkApplicationInfo applicationInfo =
        {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            applicationName.c_str(),
            0,
            "",
            0,
            VK_MAKE_VERSION(1, 0, 65)
        };

        const VkInstanceCreateInfo instanceCreateInfo =
        {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr,
            0,
            &applicationInfo,
            static_cast<uint32_t>(validationLayers.size()),
            validationLayers.data(),
            static_cast<uint32_t>(extensions.size()),
            extensions.data()
        };

        checkVulkanError(vkCreateInstance(&instanceCreateInfo, nullptr, &instance), "vkCreateInstance");

        if (validationLayers.size() != 0)
        {
            setupDebugCallback();
        }
    }

    ~VulkanInstance()
    {
        if (debugCallbackLoaded)
        {
            auto destroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance,
                "vkDestroyDebugReportCallbackEXT");
            destroyDebugReportCallbackEXT(instance, callback, nullptr);
        }
        vkDestroyInstance(instance, nullptr);
    }

    std::vector<VkPhysicalDevice> getPhysicalDevices() const
    {
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        return devices;
    }

    void printExtensions(std::ostream& output) const
    {
        uint32_t extensionCount;
        checkVulkanError(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr), "vkEnumerateInstanceExtensionProperties");

        std::vector<VkExtensionProperties> extensions(extensionCount);
        checkVulkanError(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()),
            "vkEnumerateInstanceExtensionProperties");

        output << "Available Vulkan extensions:" << std::endl;
        for (const auto& extension : extensions)
        {
            output << extension.extensionName << std::endl;
        }
    }

    VkInstance getInstance() const
    {
        return instance;
    }

private:
    VkInstance instance;
    VkDebugReportCallbackEXT callback;
    bool debugCallbackLoaded;

    bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers)
    {
        uint32_t layerCount;
        checkVulkanError(vkEnumerateInstanceLayerProperties(&layerCount, nullptr), "vkEnumerateInstanceLayerProperties");

        std::vector<VkLayerProperties> availableLayers(layerCount);
        checkVulkanError(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()), "vkEnumerateInstanceLayerProperties");

        for (const char* layer : validationLayers)
        {
            bool layerFound = false;
            for (const auto& comparedLayer : availableLayers)
            {
                if (std::string(layer) == std::string(comparedLayer.layerName))
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
        size_t location, int32_t code, const char* layerPrefix, const char* message, void* userData)
    {
        std::cerr << "Validation layer: " << message << std::endl;
        return VK_FALSE;
    }

    void setupDebugCallback()
    {
        const VkDebugReportCallbackCreateInfoEXT createInfo =
        {
            VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            nullptr,
            VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
            debugCallback,
            nullptr
        };

        auto createDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance,
            "vkCreateDebugReportCallbackEXT");
        createDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback);
        debugCallbackLoaded = true;
    }
};

} // namespace VulkanLearning
