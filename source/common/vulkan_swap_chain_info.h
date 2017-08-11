#pragma once

#include <string>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanSwapChainInfo
{
public:
    void setPhysicalDevice(const VkPhysicalDevice physicalDevice)
    {
        this->physicalDevice = physicalDevice;
    }

    void setSurfaceCapabilities(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
    {
        this->surfaceCapabilities = surfaceCapabilities;
    }

    void setSurfaceFormats(const std::vector<VkSurfaceFormatKHR>& surfaceFormats)
    {
        this->surfaceFormats = surfaceFormats;
    }

    void setPresentModes(const std::vector<VkPresentModeKHR>& presentModes)
    {
        this->presentModes = presentModes;
    }

    VkPhysicalDevice getPhysicalDevice() const
    {
        return physicalDevice;
    }

    VkSurfaceCapabilitiesKHR getSurfaceCapabilities() const
    {
        return surfaceCapabilities;
    }

    std::vector<VkSurfaceFormatKHR> getSurfaceFormats() const
    {
        return surfaceFormats;
    }

    std::vector<VkPresentModeKHR> getPresentModes() const
    {
        return presentModes;
    }

private:
    VkPhysicalDevice physicalDevice;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentModes;
};

} // namespace VulkanLearning
