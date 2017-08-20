#pragma once

#include <cstdint>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanFramebuffer
{
public:
    explicit VulkanFramebuffer(const VkDevice device, const VkRenderPass renderPass, const VkExtent2D extent,
        const std::vector<VkImageView>& imageViews) :
        device(device),
        renderPass(renderPass)
    {
        framebuffers.resize(imageViews.size());

        for (size_t i = 0; i < imageViews.size(); i++)
        {
            const VkFramebufferCreateInfo framebufferCreateInfo =
            {
                VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                nullptr,
                0,
                renderPass,
                1,
                &imageViews.at(i),
                extent.width,
                extent.height,
                1
            };

            checkVulkanError(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers.at(i)), "vkCreateFramebuffer");
        }
    }

    ~VulkanFramebuffer()
    {
        for (size_t i = 0; i < framebuffers.size(); i++)
        {
            vkDestroyFramebuffer(device, framebuffers.at(i), nullptr);
        }
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkRenderPass getRenderPass() const
    {
        return renderPass;
    }

    std::vector<VkFramebuffer> getFramebuffers() const
    {
        return framebuffers;
    }

private:
    VkDevice device;
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> framebuffers;
};

} // namespace VulkanLearning
