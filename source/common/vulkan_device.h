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
    explicit VulkanDevice(const VkPhysicalDevice physicalDevice, const VkQueueFlagBits queueFlags,
        const std::vector<const char*>& validationLayers, const VkSurfaceKHR surface = nullptr) :
        physicalDevice(physicalDevice),
        queueFlags(queueFlags)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        bool queueFound = false;

        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            if (surface != nullptr)
            {
                VkBool32 surfaceSupport;
                checkVulkanError(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &surfaceSupport),
                    "vkGetPhysicalDeviceSurfaceSupportKHR");

                if (!surfaceSupport)
                {
                    continue;
                }
            }

            if (queueFamilies.at(i).queueCount > 0 && queueFamilies.at(i).queueFlags & queueFlags)
            {
                queueFamilyIndex = i;
                queueFound = true;
                break;
            }
        }

        if (!queueFound)
        {
            throw std::runtime_error("Current device does not have any suitable queues available");
        }

        const VkPhysicalDeviceFeatures deviceFeatures = {};
        const float queuePriority = 1.0f;
        const VkDeviceQueueCreateInfo deviceQueueCreateInfo =
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            nullptr,
            0,
            queueFamilyIndex,
            1,
            &queuePriority
        };

        const VkDeviceCreateInfo deviceCreateInfo =
        {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr,
            0,
            1,
            &deviceQueueCreateInfo,
            static_cast<uint32_t>(validationLayers.size()),
            validationLayers.data(),
            0,
            nullptr,
            &deviceFeatures
        };

        checkVulkanError(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device), "vkCreateDevice");
        vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
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

    uint32_t getQueueFamilyIndex() const
    {
        return queueFamilyIndex;
    }

    VkQueue getQueue() const
    {
        return queue;
    }

private:
    VkPhysicalDevice physicalDevice;
    VkQueueFlagBits queueFlags;
    VkDevice device;
    uint32_t queueFamilyIndex;
    VkQueue queue;
};

} // namespace VulkanLearning
