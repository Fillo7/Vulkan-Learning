#pragma once

#include <array>
#include <string>
#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

namespace VulkanLearning
{

class Vertex
{
public:
    Vertex() :
        position(0.0f, 0.0f),
        color(1.0f, 1.0f, 1.0f)
    {}

    Vertex(const glm::vec2& position, const glm::vec3& color) :
        position(position),
        color(color)
    {}

    glm::vec2 getPosition() const
    {
        return position;
    }

    glm::vec3 getColor() const
    {
        return color;
    }

    static VkVertexInputBindingDescription getVertexInputBindingDescription()
    {
        const VkVertexInputBindingDescription vertexInputBindingDescription =
        {
            0,
            sizeof(Vertex),
            VK_VERTEX_INPUT_RATE_VERTEX
        };

        return vertexInputBindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getVertexInputAttributeDescriptions()
    {
        const VkVertexInputAttributeDescription positionDescription =
        {
            0,
            0,
            VK_FORMAT_R32G32_SFLOAT,
            offsetof(Vertex, position)
        };

        const VkVertexInputAttributeDescription colorDescription =
        {
            1,
            0,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Vertex, color)
        };

        std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributeDescriptions = {positionDescription, colorDescription};

        return vertexInputAttributeDescriptions;
    }

private:
    glm::vec2 position;
    glm::vec3 color;
};

} // namespace VulkanLearning
