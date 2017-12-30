#pragma once

#include <cstdint>
#include <vector>
#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanDescriptorSetGroup
{
public:
    explicit VulkanDescriptorSetGroup(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool,
        const uint32_t descriptorSetCount) :
        device(device),
        descriptorSetLayout(descriptorSetLayout),
        descriptorPool(descriptorPool)
    {
        const VkDescriptorSetAllocateInfo allocateInfo =
        {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            nullptr,
            descriptorPool,
            descriptorSetCount,
            &descriptorSetLayout
        };

        descriptorSets.resize(descriptorSetCount);
        checkVulkanError(vkAllocateDescriptorSets(device, &allocateInfo, descriptorSets.data()), "vkAllocateDescriptorSets");
    }

    void attachUniformBuffer(VkBuffer buffer, const VkDeviceSize bufferSize)
    {
        const VkDescriptorBufferInfo bufferInfo =
        {
            buffer,
            0,
            bufferSize
        };

        const VkWriteDescriptorSet descriptorWrite =
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            nullptr,
            descriptorSets.at(0),
            0,
            0,
            1,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            nullptr,
            &bufferInfo,
            nullptr
        };

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkDescriptorSetLayout getDescriptorSetLayout() const
    {
        return descriptorSetLayout;
    }

    VkDescriptorPool getDescriptorPool() const
    {
        return descriptorPool;
    }

    std::vector<VkDescriptorSet> getDescriptorSets() const
    {
        return descriptorSets;
    }

private:
    VkDevice device;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
};

} // namespace VulkanLearning
