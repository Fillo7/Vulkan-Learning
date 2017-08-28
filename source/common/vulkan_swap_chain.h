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
        surface(surface),
        swapChainInfo(swapChainInfo),
        imageUsageFlags(imageUsageFlags)
    {
        initializeSwapChain(swapChainInfo, chooseExtent(swapChainInfo.getSurfaceCapabilities()), imageUsageFlags);
    }

    ~VulkanSwapChain()
    {
        destroySwapChain();
    }

    void destroySwapChain()
    {
        for (size_t i = 0; i < imageViews.size(); i++)
        {
            vkDestroyImageView(device, imageViews.at(i), nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

    void reloadSwapChain(VkExtent2D& extent)
    {
        initializeSwapChain(swapChainInfo, extent, imageUsageFlags);
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

    std::vector<VkImage> getSwapChainImages() const
    {
        return swapChainImages;
    }

    std::vector<VkImageView> getImageViews() const
    {
        return imageViews;
    }

private:
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkSwapchainKHR helperSwapChain;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D extent;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> imageViews;
    VulkanSwapChainInfo swapChainInfo;
    VkImageUsageFlags imageUsageFlags;

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

    void initializeSwapChainImages()
    {
        uint32_t imageCount;
        checkVulkanError(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr), "vkGetSwapchainImagesKHR");

        swapChainImages.resize(imageCount);
        checkVulkanError(vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data()), "vkGetSwapchainImagesKHR");

        imageViews.resize(imageCount);
        for (size_t i = 0; i < swapChainImages.size(); i++)
        {
            const VkImageViewCreateInfo imageViewCreateInfo =
            {
                VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                nullptr,
                0,
                swapChainImages.at(i),
                VK_IMAGE_VIEW_TYPE_2D,
                surfaceFormat.format,
                VkComponentMapping
                {
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY,
                    VK_COMPONENT_SWIZZLE_IDENTITY
                },
                VkImageSubresourceRange
                {
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    0,
                    1,
                    0,
                    1
                }
            };

            checkVulkanError(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews.at(i)), "vkCreateImageView");
        }
    }

    void initializeSwapChain(const VulkanSwapChainInfo& swapChainInfo, VkExtent2D& extent, const VkImageUsageFlags imageUsageFlags)
    {
        surfaceFormat = chooseSurfaceFormat(swapChainInfo.getSurfaceFormats());
        VkPresentModeKHR presentMode = choosePresentMode(swapChainInfo.getPresentModes());
        VkSurfaceCapabilitiesKHR surfaceCapabilities = swapChainInfo.getSurfaceCapabilities();
        this->extent = extent;

        uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
        if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
        {
            imageCount = surfaceCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapChainCreateInfo =
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
        initializeSwapChainImages();
    }
};

} // namespace VulkanLearning
