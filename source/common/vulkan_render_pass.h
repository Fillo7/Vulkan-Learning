#pragma once

#include <cstdint>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanRenderPass
{
public:
    explicit VulkanRenderPass(const VkDevice device, const VkFormat imageFormat) :
        device(device)
    {
        const VkAttachmentDescription colorAttachment =
        {
            0,
            imageFormat,
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };

        const VkAttachmentReference colorAttachmentReference =
        {
            0,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        const VkSubpassDescription subpassDescription =
        {
            0,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            0,
            nullptr,
            1,
            &colorAttachmentReference,
            nullptr,
            nullptr,
            0,
            nullptr
        };

        const VkRenderPassCreateInfo renderPassCreateInfo =
        {
            VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            nullptr,
            0,
            1,
            &colorAttachment,
            1,
            &subpassDescription,
            0,
            nullptr
        };

        checkVulkanError(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass), "vkCreateRenderPass");
    }

    ~VulkanRenderPass()
    {
        vkDestroyRenderPass(device, renderPass, nullptr);
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkRenderPass getRenderPass() const
    {
        return renderPass;
    }

private:
    VkDevice device;
    VkRenderPass renderPass;
};

} // namespace VulkanLearning
