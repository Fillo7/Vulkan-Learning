#pragma once

#include <string>

#include "vulkan/vulkan.h"

namespace VulkanLearning
{

std::string getVulkanEnumName(const VkResult value);
void checkVulkanError(const VkResult value);
void checkVulkanError(const VkResult value, const std::string& message);

} // namespace VulkanLearning
