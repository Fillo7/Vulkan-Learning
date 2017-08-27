#pragma once

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanShaderModule
{
public:
    explicit VulkanShaderModule(VkDevice device, const std::string& filePath) :
        device(device)
    {
        std::string code = loadFileToString(filePath);

        const VkShaderModuleCreateInfo shaderModuleCreateInfo =
        {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            nullptr,
            0,
            code.length(),
            (const uint32_t*)code.data()
        };

        checkVulkanError(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule), "vkCreateShaderModule");
    }

    ~VulkanShaderModule()
    {
        vkDestroyShaderModule(device, shaderModule, nullptr);
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkShaderModule getShaderModule() const
    {
        return shaderModule;
    }

private:
    VkDevice device;
    VkShaderModule shaderModule;

    static std::string loadFileToString(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error(std::string("Unable to open file: ") + filePath);
        }

        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }
};

} // namespace VulkanLearning
