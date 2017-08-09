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
    explicit VulkanDevice(const VkPhysicalDevice physicalDevice, const VkQueueFlagBits queueFlags) :
        physicalDevice(physicalDevice),
        queueFlags(queueFlags)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        uint32_t queueCount = 0;

        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            if (queueFamilies.at(i).queueCount > 0 && queueFamilies.at(i).queueFlags & queueFlags)
            {
                queueCount++;
                queueIndices.push_back(i);
            }
        }

        if (queueCount == 0)
        {
            throw std::runtime_error("Current device does not have any suitable queues available");
        }

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(queueCount);
        size_t currentQueueIndex = 0;

        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            if (queueFamilies[i].queueFlags & queueFlags)
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

    VkQueueFlagBits getQueueFlags() const
    {
        return queueFlags;
    }

    VkDevice getDevice() const
    {
        return device;
    }

    std::vector<uint32_t> getQueueIndices() const
    {
        return queueIndices;
    }

private:
    VkPhysicalDevice physicalDevice;
    VkQueueFlagBits queueFlags;
    VkDevice device;
    std::vector<uint32_t> queueIndices;
};

} // namespace VulkanLearning
