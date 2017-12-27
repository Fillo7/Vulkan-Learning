#pragma once

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanDescriptorSetLayout
{
public:
    explicit VulkanDescriptorSetLayout(VkDevice device) :
        device(device)
    {
        const VkDescriptorSetLayoutBinding uboLayoutBinding =
        {
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            1,
            VK_SHADER_STAGE_VERTEX_BIT,
            nullptr
        };

        const VkDescriptorSetLayoutCreateInfo layoutCreateInfo =
        {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            nullptr,
            0,
            1,
            &uboLayoutBinding
        };

        checkVulkanError(vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &descriptorSetLayout), "vkCreateDescriptorSetLayout");
    }

    ~VulkanDescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkDescriptorSetLayout getDescriptorSetLayout() const
    {
        return descriptorSetLayout;
    }

private:
    VkDevice device;
    VkDescriptorSetLayout descriptorSetLayout;
};

} // namespace VulkanLearning
