#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanPipeline
{
public:
    explicit VulkanPipeline(const VkShaderModule vertexShader, const VkShaderModule fragmentShader) :
        vertexShader(vertexShader),
        fragmentShader(fragmentShader)
    {
        const VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo =
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_VERTEX_BIT,
            vertexShader,
            "main",
            nullptr
        };

        const VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo =
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            fragmentShader,
            "main",
            nullptr
        };

        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos{ vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };
    }

    ~VulkanPipeline()
    {
        // to do
    }

    VkShaderModule getVertexShader() const
    {
        return vertexShader;
    }

    VkShaderModule getFragmentShader() const
    {
        return vertexShader;
    }

private:
    VkShaderModule vertexShader;
    VkShaderModule fragmentShader;
};

} // namespace VulkanLearning
