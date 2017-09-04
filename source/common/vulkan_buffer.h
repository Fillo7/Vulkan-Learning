#pragma once

#include <cstring>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanBuffer
{
public:
    explicit VulkanBuffer(VkDevice device, const VkDeviceSize bufferSize) :
        device(device),
        bufferSize(bufferSize),
        memoryAllocated(false)
    {
        const VkBufferCreateInfo bufferCreateInfo =
        {
            VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            nullptr,
            0,
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            0,
            nullptr
        };

        checkVulkanError(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer), "vkCreateBuffer");
    }

    ~VulkanBuffer()
    {
        vkDestroyBuffer(device, buffer, nullptr);
        if (memoryAllocated)
        {
            vkFreeMemory(device, bufferMemory, nullptr);
        }
    }

    VkMemoryRequirements getMemoryRequirements() const
    {
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(device, buffer, &requirements);
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

        checkVulkanError(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &bufferMemory), "vkAllocateMemory");
        checkVulkanError(vkBindBufferMemory(device, buffer, bufferMemory, 0), "vkBindBufferMemory");
        memoryAllocated = true;
    }

    void uploadData(const void* source, const size_t dataSize)
    {
        void* data;
        checkVulkanError(vkMapMemory(device, bufferMemory, 0, dataSize, 0, &data), "vkMapMemory");
        std::memcpy(data, source, dataSize);
        vkUnmapMemory(device, bufferMemory);
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkDeviceSize getBufferSize() const
    {
        return bufferSize;
    }

    VkBuffer getBuffer() const
    {
        return buffer;
    }

private:
    VkDevice device;
    VkDeviceSize bufferSize;
    VkBuffer buffer;
    bool memoryAllocated;
    VkDeviceMemory bufferMemory;
};

} // namespace VulkanLearning
