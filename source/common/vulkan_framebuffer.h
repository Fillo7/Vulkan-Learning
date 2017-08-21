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
        renderPass(renderPass),
        extent(extent)
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

    void beginRenderPass(const std::vector<VkCommandBuffer>& commandBuffers, const VkPipeline pipeline)
    {
        for (size_t i = 0; i < commandBuffers.size(); i++)
        {
            const VkCommandBufferBeginInfo commandBufferBeginInfo =
            {
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                nullptr,
                VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
                nullptr
            };

            checkVulkanError(vkBeginCommandBuffer(commandBuffers.at(i), &commandBufferBeginInfo), "vkBeginCommandBuffer");

            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

            const VkRenderPassBeginInfo renderPassBeginInfo =
            {
                VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                nullptr,
                renderPass,
                framebuffers.at(i),
                VkRect2D
                {
                    {0, 0},
                    extent
                },
                1,
                &clearColor
            };

            vkCmdBeginRenderPass(commandBuffers.at(i), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(commandBuffers.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
            vkCmdDraw(commandBuffers.at(i), 3, 1, 0, 0);

            vkCmdEndRenderPass(commandBuffers.at(i));
            checkVulkanError(vkEndCommandBuffer(commandBuffers.at(i)), "vkEndCommandBuffer");
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
    VkExtent2D extent;
    std::vector<VkFramebuffer> framebuffers;
};

} // namespace VulkanLearning
