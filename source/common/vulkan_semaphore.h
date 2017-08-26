#pragma once

#include <cstdint>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanSemaphore
{
public:
    explicit VulkanSemaphore(const VkDevice device) :
        device(device)
    {
        const VkSemaphoreCreateInfo semaphoreCreateInfo =
        {
            VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            nullptr,
            0
        };

        checkVulkanError(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore), "vkCreateSemaphore");
    }

    ~VulkanSemaphore()
    {
        vkDestroySemaphore(device, semaphore, nullptr);
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkSemaphore getSemaphore() const
    {
        return semaphore;
    }

private:
    VkDevice device;
    VkSemaphore semaphore;
};

} // namespace VulkanLearning
