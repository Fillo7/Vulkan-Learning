#pragma once

#include <cstdint>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_swap_chain_info.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanSwapChain
{
public:
    explicit VulkanSwapChain(VkDevice device, VkSurfaceKHR surface, const VulkanSwapChainInfo& swapChainInfo,
        const VkImageUsageFlags imageUsageFlags) :
        device(device),
        surface(surface)
    {
        surfaceFormat = chooseSurfaceFormat(swapChainInfo.getSurfaceFormats());
        VkPresentModeKHR presentMode = choosePresentMode(swapChainInfo.getPresentModes());
        VkSurfaceCapabilitiesKHR surfaceCapabilities = swapChainInfo.getSurfaceCapabilities();
        extent = chooseExtent(surfaceCapabilities);

        uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
        if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
        {
            imageCount = surfaceCapabilities.maxImageCount;
        }

        const VkSwapchainCreateInfoKHR swapChainCreateInfo =
        {
            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            nullptr,
            0,
            surface,
            imageCount,
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            extent,
            1,
            imageUsageFlags,
            VK_SHARING_MODE_EXCLUSIVE,
            0,
            nullptr,
            surfaceCapabilities.currentTransform,
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            presentMode,
            VK_TRUE,
            VK_NULL_HANDLE
        };

        checkVulkanError(vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapChain), "vkCreateSwapchainKHR");
    }

    ~VulkanSwapChain()
    {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

    std::vector<VkImage> getSwapChainImages() const
    {
        uint32_t imageCount;
        checkVulkanError(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr), "vkGetSwapchainImagesKHR");

        std::vector<VkImage> swapChainImages(imageCount);
        checkVulkanError(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data()), "vkGetSwapchainImagesKHR");

        return swapChainImages;
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkSurfaceKHR getSurface() const
    {
        return surface;
    }

    VkSwapchainKHR getSwapChain() const
    {
        return swapChain;
    }

    VkSurfaceFormatKHR getSurfaceFormat() const
    {
        return surfaceFormat;
    }

    VkExtent2D getExtent() const
    {
        return extent;
    }

private:
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D extent;

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
    {
        VkSurfaceFormatKHR idealFormat =
        {
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        if (availableFormats.size() == 1 && availableFormats.at(0).format == VK_FORMAT_UNDEFINED)
        {
            return idealFormat;
        }

        for (const auto& format : availableFormats)
        {
            if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                return idealFormat;
            }
        }

        return availableFormats.at(0);
    }

    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
    {
        for (const auto& presentMode : availablePresentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return VK_PRESENT_MODE_MAILBOX_KHR;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        return capabilities.currentExtent;
    }
};

} // namespace VulkanLearning
