#pragma once

#include <array>
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
    explicit VulkanPipeline(VkDevice device, VkRenderPass renderPass, VkShaderModule vertexShader, VkShaderModule fragmentShader,
        const VkExtent2D& swapChainExtent) :
        device(device),
        vertexShader(vertexShader),
        fragmentShader(fragmentShader)
    {
        const VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo =
        {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            nullptr,
            0,
            0,
            nullptr,
            0,
            nullptr
        };

        this->vertexInputStateCreateInfo = vertexInputStateCreateInfo;
        initializePipeline(renderPass, swapChainExtent);
    }

    explicit VulkanPipeline(VkDevice device, VkRenderPass renderPass, VkShaderModule vertexShader, VkShaderModule fragmentShader,
        const VkExtent2D& swapChainExtent, const VkVertexInputBindingDescription& vertexInputBindingDescription,
        const std::array<VkVertexInputAttributeDescription, 2>& vertexInputAttributeDescriptions) :
        device(device),
        vertexShader(vertexShader),
        fragmentShader(fragmentShader)
    {
        const VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo =
        {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            nullptr,
            0,
            1,
            &vertexInputBindingDescription,
            static_cast<uint32_t>(vertexInputAttributeDescriptions.size()),
            vertexInputAttributeDescriptions.data()
        };

        this->vertexInputStateCreateInfo = vertexInputStateCreateInfo;
        initializePipeline(renderPass, swapChainExtent);
    }

    ~VulkanPipeline()
    {
        destroyPipeline();
    }

    void destroyPipeline()
    {
        vkDestroyPipeline(device, pipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }

    void reloadPipeline(VkRenderPass renderPass, const VkExtent2D& swapChainExtent)
    {
        initializePipeline(renderPass, swapChainExtent);
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkShaderModule getVertexShader() const
    {
        return vertexShader;
    }

    VkShaderModule getFragmentShader() const
    {
        return vertexShader;
    }

    VkPipelineLayout getPipelineLayout() const
    {
        return pipelineLayout;
    }

    VkPipeline getPipeline() const
    {
        return pipeline;
    }

private:
    VkDevice device;
    VkShaderModule vertexShader;
    VkShaderModule fragmentShader;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;

    void initializePipeline(VkRenderPass renderPass, const VkExtent2D& swapChainExtent)
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

        const VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo =
        {
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            VK_FALSE
        };

        const VkViewport viewport =
        {
            0.0f,
            0.0f,
            static_cast<float>(swapChainExtent.width),
            static_cast<float>(swapChainExtent.height),
            0.0f,
            1.0f
        };

        const VkRect2D scissor =
        {
            VkOffset2D{ 0, 0 },
            swapChainExtent
        };

        const VkPipelineViewportStateCreateInfo viewportStateCreateInfo =
        {
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            nullptr,
            0,
            1,
            &viewport,
            1,
            &scissor
        };

        const VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo =
        {
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_FALSE,
            VK_FALSE,
            VK_POLYGON_MODE_FILL,
            VK_CULL_MODE_BACK_BIT,
            VK_FRONT_FACE_CLOCKWISE,
            VK_FALSE,
            0.0f,
            0.0f,
            0.0f,
            1.0f
        };

        const VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo =
        {
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_SAMPLE_COUNT_1_BIT,
            VK_FALSE,
            1.0f,
            nullptr,
            VK_FALSE,
            VK_FALSE
        };

        const VkPipelineColorBlendAttachmentState colorBlendAttachmentState =
        {
            VK_FALSE,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
            VK_BLEND_FACTOR_ONE,
            VK_BLEND_FACTOR_ZERO,
            VK_BLEND_OP_ADD,
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };

        const VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo =
        {
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_FALSE,
            VK_LOGIC_OP_COPY,
            1,
            &colorBlendAttachmentState,
            { 0.0f, 0.0f, 0.0f, 0.0f}
        };

        const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
        {
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            nullptr,
            0,
            0,
            nullptr,
            0,
            0
        };

        checkVulkanError(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout), "vkCreatePipelineLayout");

        const VkGraphicsPipelineCreateInfo graphicsPipelineInfo =
        {
            VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            nullptr,
            0,
            2,
            shaderStageCreateInfos.data(),
            &vertexInputStateCreateInfo,
            &inputAssemblyStateCreateInfo,
            nullptr,
            &viewportStateCreateInfo,
            &rasterizationStateCreateInfo,
            &multisampleStateCreateInfo,
            nullptr,
            &colorBlendStateCreateInfo,
            nullptr,
            pipelineLayout,
            renderPass,
            0,
            VK_NULL_HANDLE,
            -1
        };

        checkVulkanError(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &pipeline),
            "vkCreateGraphicsPipelines");
    }
};

} // namespace VulkanLearning
