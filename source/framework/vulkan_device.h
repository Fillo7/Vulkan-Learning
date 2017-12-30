#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>
#include "vulkan/vulkan.h"
#include "vulkan_swap_chain_info.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanDevice
{
public:
    explicit VulkanDevice(VkPhysicalDevice physicalDevice, const VkQueueFlagBits queueFlags, const std::vector<const char*>& validationLayers,
        const std::vector<const char*>& extensions) :
        VulkanDevice(physicalDevice, queueFlags, validationLayers, extensions, nullptr)
    {}

    explicit VulkanDevice(VkPhysicalDevice physicalDevice, const VkQueueFlagBits queueFlags, const std::vector<const char*>& validationLayers,
        const std::vector<const char*>& extensions, VkSurfaceKHR surface) :
        physicalDevice(physicalDevice),
        queueFlags(queueFlags),
        surface(surface)
    {
        if (!checkExtensionSupport(extensions))
        {
            throw std::runtime_error("One of the requested device extensions is not present");
        }

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
            static_cast<uint32_t>(extensions.size()),
            extensions.data(),
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

    uint32_t getSuitableMemoryTypeIndex(const uint32_t typeFilter, const VkMemoryPropertyFlags properties) const
    {
        VkPhysicalDeviceMemoryProperties memoryProperties = getPhysicalDeviceMemoryProperties();

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("Current device does not have any suitable memory types available");
    }

    VulkanSwapChainInfo getVulkanSwapChainInfo() const
    {
        VulkanSwapChainInfo info;
        info.setPhysicalDevice(physicalDevice);

        VkSurfaceCapabilitiesKHR capabilities;
        checkVulkanError(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities),
            "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        info.setSurfaceCapabilities(capabilities);

        uint32_t formatCount;
        checkVulkanError(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr),
            "vkGetPhysicalDeviceSurfaceFormatsKHR");

        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        checkVulkanError(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data()),
            "vkGetPhysicalDeviceSurfaceFormatsKHR");
        info.setSurfaceFormats(surfaceFormats);

        uint32_t presentModeCount;
        checkVulkanError(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr),
            "vkGetPhysicalDeviceSurfacePresentModesKHR");
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
        info.setPresentModes(presentModes);

        return info;
    }

    void waitIdle()
    {
        vkDeviceWaitIdle(device);
    }

    void queueSubmit(VkCommandBuffer commandBuffer)
    {
        const VkSubmitInfo submitInfo =
        {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,
            nullptr,
            0,
            nullptr,
            nullptr,
            1,
            &commandBuffer,
            0,
            nullptr
        };

        checkVulkanError(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE), "vkQueueSubmit");
        vkQueueWaitIdle(queue);
    }

    void queueSubmit(VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore)
    {
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        const VkSubmitInfo submitInfo =
        {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,
            nullptr,
            1,
            &waitSemaphore,
            waitStages,
            1,
            &commandBuffer,
            1,
            &signalSemaphore
        };

        checkVulkanError(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE), "vkQueueSubmit");
    }

    void queuePresent(VkSwapchainKHR swapChain, VkSemaphore waitSemaphore, const uint32_t imageIndex)
    {
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        const VkPresentInfoKHR presentInfo =
        {
            VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            nullptr,
            1,
            &waitSemaphore,
            1,
            &swapChain,
            &imageIndex,
            nullptr
        };

        vkQueuePresentKHR(queue, &presentInfo); // error checking not done on purpose
        vkQueueWaitIdle(queue);
    }

    uint32_t getNextImageIndex(VkSwapchainKHR swapChain, VkSemaphore signalSemaphore)
    {
        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), signalSemaphore, VK_NULL_HANDLE, &imageIndex);

        return imageIndex;
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

    VkSurfaceKHR getSurface() const
    {
        return surface;
    }

private:
    VkPhysicalDevice physicalDevice;
    VkQueueFlagBits queueFlags;
    VkDevice device;
    uint32_t queueFamilyIndex;
    VkQueue queue;
    VkSurfaceKHR surface;

    bool checkExtensionSupport(const std::vector<const char*>& extensions)
    {
        uint32_t extensionCount;
        checkVulkanError(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr),
            "vkEnumerateDeviceExtensionProperties");

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        for (const char* extension : extensions)
        {
            bool extensionsFound = false;
            for (const auto& comparedExtension : availableExtensions)
            {
                if (std::string(extension) == std::string(comparedExtension.extensionName))
                {
                    extensionsFound = true;
                    break;
                }
            }

            if (extensionsFound == false)
            {
                return false;
            }
        }

        return true;
    }
};

} // namespace VulkanLearning
