#pragma once

#include <cstdint>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanFramebufferGroup
{
public:
    explicit VulkanFramebufferGroup(VkDevice device, VkRenderPass renderPass, const VkExtent2D& extent,
        const std::vector<VkImageView>& imageViews) :
        device(device),
        renderPass(renderPass),
        extent(extent)
    {
        initializeFramebufferGroup(imageViews);
    }

    ~VulkanFramebufferGroup()
    {
        destroyFramebuffers();
    }

    void destroyFramebuffers()
    {
        for (size_t i = 0; i < framebuffers.size(); i++)
        {
            vkDestroyFramebuffer(device, framebuffers.at(i), nullptr);
        }
    }

    void reloadFramebuffers(VkRenderPass renderPass, const VkExtent2D& extent, const std::vector<VkImageView>& imageViews)
    {
        this->renderPass = renderPass;
        this->extent = extent;
        initializeFramebufferGroup(imageViews);
    }

    void beginRenderPass(const std::vector<VkCommandBuffer>& commandBuffers, const VkPipeline pipeline, const std::vector<VkBuffer>& vertexBuffers,
        const std::vector<VkDeviceSize>& offsets, const size_t numberOfVertices)
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

            if (vertexBuffers.size() > 0)
            {
                vkCmdBindVertexBuffers(commandBuffers[i], 0, static_cast<uint32_t>(vertexBuffers.size()), vertexBuffers.data(), offsets.data());
            }

            vkCmdDraw(commandBuffers.at(i), static_cast<uint32_t>(numberOfVertices), 1, 0, 0);

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

    VkExtent2D getExtent() const
    {
        return extent;
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

    void initializeFramebufferGroup(const std::vector<VkImageView>& imageViews)
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
};

} // namespace VulkanLearning
