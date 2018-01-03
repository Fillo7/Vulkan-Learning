#pragma once

#include <cstdint>
#include <vector>
#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanImage
{
public:
    explicit VulkanImage(VkDevice device, const VkExtent3D& imageExtent) :
        device(device),
        memoryAllocated(false)
    {
        const VkImageCreateInfo imageInfo =
        {
            VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            nullptr,
            0,
            VK_IMAGE_TYPE_2D,
            VK_FORMAT_R8G8B8A8_UNORM,
            imageExtent,
            1,
            1,
            VK_SAMPLE_COUNT_1_BIT,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            0,
            nullptr,
            VK_IMAGE_LAYOUT_UNDEFINED
        };

        checkVulkanError(vkCreateImage(device, &imageInfo, nullptr, &image), "vkCreateImage");
    }

    ~VulkanImage()
    {
        vkDestroyImage(device, image, nullptr);

        if (memoryAllocated)
        {
            vkFreeMemory(device, imageMemory, nullptr);
            memoryAllocated = false;
        }
    }

    VkMemoryRequirements getMemoryRequirements() const
    {
        VkMemoryRequirements requirements;
        vkGetImageMemoryRequirements(device, image, &requirements);
        return requirements;
    }

    void allocateMemory(const uint32_t memoryTypeIndex)
    {
        VkMemoryRequirements memoryRequirements = getMemoryRequirements();

        const VkMemoryAllocateInfo memoryAllocateInfo =
        {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            nullptr,
            memoryRequirements.size,
            memoryTypeIndex
        };

        checkVulkanError(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &imageMemory), "vkAllocateMemory");
        checkVulkanError(vkBindImageMemory(device, image, imageMemory, 0), "vkBindImageMemory");
        memoryAllocated = true;
    }

    void transitionLayout(VkCommandBuffer commandBuffer, VkQueue queue, const VkImageLayout oldLayout, const VkImageLayout newLayout)
    {
        const VkCommandBufferBeginInfo commandBufferBeginInfo =
        {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            nullptr,
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            nullptr
        };

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

        checkVulkanError(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo), "vkBeginCommandBuffer");
        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);

        // to do

        checkVulkanError(vkEndCommandBuffer(commandBuffer), "vkEndCommandBuffer");
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkImage getImage() const
    {
        return image;
    }

private:
    VkDevice device;
    VkImage image;
    VkDeviceMemory imageMemory;
    bool memoryAllocated;
};

} // namespace VulkanLearning
