#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanDevice
{
public:
    explicit VulkanDevice(const VkPhysicalDevice physicalDevice) :
        physicalDevice(physicalDevice)
    {
        VkPhysicalDeviceFeatures physicalDeviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

        uint32_t queueFamilyPropertiesCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

        uint32_t computeQueueFamilyCount = 0;

        for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
        {
            if (VK_QUEUE_COMPUTE_BIT & queueFamilyProperties.at(i).queueFlags)
            {
                computeQueueFamilyCount++;
                computeQueueIndices.push_back(i);
            }
        }

        if (computeQueueFamilyCount == 0)
        {
            throw std::runtime_error("Current device does not have any compute queues available");
        }

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(computeQueueFamilyCount);
        size_t currentQueueIndex = 0;

        for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
        {
            if (VK_QUEUE_COMPUTE_BIT & queueFamilyProperties[i].queueFlags)
            {
                const float queuePriority = 1.0f;

                const VkDeviceQueueCreateInfo deviceQueueCreateInfo =
                {
                    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    nullptr,
                    0,
                    i,
                    1,
                    &queuePriority
                };

                queueCreateInfos.at(currentQueueIndex) = deviceQueueCreateInfo;
                currentQueueIndex++;
            }
        }

        const VkDeviceCreateInfo deviceCreateInfo =
        {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr,
            0,
            static_cast<uint32_t>(queueCreateInfos.size()),
            queueCreateInfos.data(),
            0,
            nullptr,
            0,
            nullptr,
            nullptr
        };

        checkVulkanError(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device), "vkCreateDevice");
    }

    ~VulkanDevice()
    {
        checkVulkanError(vkDeviceWaitIdle(device), "vkDeviceWaitIdle");
        vkDestroyDevice(device, nullptr);
    }

    VkPhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties() const
    {
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
        return deviceMemoryProperties;
    }

    VkPhysicalDevice getPhysicalDevice() const
    {
        return physicalDevice;
    }

    VkDevice getDevice() const
    {
        return device;
    }

    std::vector<uint32_t> getComputeQueueIndices() const
    {
        return computeQueueIndices;
    }

private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    std::vector<uint32_t> computeQueueIndices;
};

} // namespace VulkanLearning
