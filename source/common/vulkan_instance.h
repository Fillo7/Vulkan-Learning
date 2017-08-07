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
    VulkanInstance(const std::string& applicationName)
    {
        const VkApplicationInfo applicationInfo =
        {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            applicationName.c_str(),
            0,
            "",
            0,
            VK_MAKE_VERSION(1, 0, 51)
        };

        const VkInstanceCreateInfo instanceCreateInfo =
        {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr,
            0,
            &applicationInfo,
            0,
            nullptr,
            0,
            nullptr
        };

        checkVulkanError(vkCreateInstance(&instanceCreateInfo, nullptr, &instance), "vkCreateInstance");
    }

    ~VulkanInstance()
    {
        vkDestroyInstance(instance, nullptr);
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
};

} // namespace VulkanLearning
