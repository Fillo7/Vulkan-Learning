// Standard library headers
#include <chrono>
#include <cstdint>
#include <iostream>

// Additional library headers
#include "SDL2/SDL.h"
#include "vulkan/vulkan.h"
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Project headers
#include "common/sdl_instance.h"
#include "common/sdl_window.h"
#include "common/uniform_buffer_object.h"
#include "common/vertex.h"
#include "common/vulkan_buffer.h"
#include "common/vulkan_command_buffer_group.h"
#include "common/vulkan_command_pool.h"
#include "common/vulkan_descriptor_pool.h"
#include "common/vulkan_descriptor_set_group.h"
#include "common/vulkan_descriptor_set_layout.h"
#include "common/vulkan_device.h"
#include "common/vulkan_framebuffer_group.h"
#include "common/vulkan_instance.h"
#include "common/vulkan_pipeline.h"
#include "common/vulkan_render_pass.h"
#include "common/vulkan_semaphore.h"
#include "common/vulkan_shader_module.h"
#include "common/vulkan_surface.h"
#include "common/vulkan_swap_chain.h"
#include "common/vulkan_utility.h"

void draw(VulkanLearning::VulkanDevice& device, VulkanLearning::VulkanSwapChain& swapChain, VulkanLearning::VulkanCommandBufferGroup& commandBuffers)
{
    VulkanLearning::VulkanSemaphore imageReadySemaphore(device.getDevice());
    VulkanLearning::VulkanSemaphore renderFinishedSemaphore(device.getDevice());

    uint32_t imageIndex = device.getNextImageIndex(swapChain.getSwapChain(), imageReadySemaphore.getSemaphore());
    device.queueSubmit(commandBuffers.getCommandBuffers().at(imageIndex), imageReadySemaphore.getSemaphore(),
        renderFinishedSemaphore.getSemaphore());
    device.queuePresent(swapChain.getSwapChain(), renderFinishedSemaphore.getSemaphore(), imageIndex);
}

void updateUniformBuffer(VulkanLearning::VulkanBuffer& uniformBuffer, const VkExtent2D& swapChainExtent)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    VulkanLearning::UniformBufferObject ubo;;
    ubo.setModel(glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    ubo.setView(glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), swapChainExtent.width / static_cast<float>(swapChainExtent.height), 0.1f, 10.0f);
    projection[1][1] *= -1; // Y coordinate of clip coordinates is inverted (OpenGL design)
    ubo.setProjection(projection);

    uniformBuffer.uploadData(&ubo, sizeof(ubo));
}

int main(int argc, char* argv[])
{
    bool quit = false;
    VulkanLearning::SdlInstance sdlInstance(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    VulkanLearning::SdlWindow window("Part 1", 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    SDL_Event event;
    std::vector<const char*> extensions = window.getInstanceExtensions();
    extensions.push_back("VK_EXT_debug_report");

    VulkanLearning::VulkanInstance vulkanInstance("Part 1", {"VK_LAYER_LUNARG_standard_validation"}, extensions);
    std::vector<VkPhysicalDevice> devices = vulkanInstance.getPhysicalDevices();

    if (devices.size() == 0)
    {
        std::cerr << "No Vulkan device found" << std::endl;
        return -1;
    }

    const std::vector<VulkanLearning::Vertex> vertices
    {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    const std::vector<uint16_t> vertexIndices = {0, 1, 2, 2, 3, 0};

    VulkanLearning::VulkanSurface surface(vulkanInstance.getInstance(), window.getWindow());
    VulkanLearning::VulkanDevice device(devices.at(0), VK_QUEUE_GRAPHICS_BIT, {"VK_LAYER_LUNARG_standard_validation"}, {"VK_KHR_swapchain"},
        surface.getSurface());
    VulkanLearning::VulkanSwapChain swapChain(device.getDevice(), surface.getSurface(), device.getVulkanSwapChainInfo(),
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    VulkanLearning::VulkanShaderModule vertexShader(device.getDevice(), "part1_vert.spv");
    VulkanLearning::VulkanShaderModule fragmentShader(device.getDevice(), "part1_frag.spv");
    VulkanLearning::VulkanRenderPass renderPass(device.getDevice(), swapChain.getSurfaceFormat().format);
    VulkanLearning::VulkanDescriptorSetLayout setLayout(device.getDevice());
    VulkanLearning::VulkanPipeline graphicsPipeline(device.getDevice(), renderPass.getRenderPass(), vertexShader.getShaderModule(),
        fragmentShader.getShaderModule(), swapChain.getExtent(), vertices.at(0).getVertexInputBindingDescription(),
        vertices.at(0).getVertexInputAttributeDescriptions(), {setLayout.getDescriptorSetLayout()});
    VulkanLearning::VulkanFramebufferGroup framebuffers(device.getDevice(), renderPass.getRenderPass(), swapChain.getExtent(),
        swapChain.getImageViews());
    VulkanLearning::VulkanCommandPool commandPool(device.getDevice(), device.getQueueFamilyIndex());
    VulkanLearning::VulkanCommandBufferGroup commandBuffers(device.getDevice(), commandPool.getCommandPool(),
        static_cast<uint32_t>(framebuffers.getFramebuffers().size()));

    // Transfer vertex data into staging buffer
    VulkanLearning::VulkanBuffer stagingVertexBuffer(device.getDevice(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, sizeof(vertices.at(0)) * vertices.size());
    stagingVertexBuffer.allocateMemory(device.getSuitableMemoryTypeIndex(stagingVertexBuffer.getMemoryRequirements().memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
    stagingVertexBuffer.uploadData(vertices.data(), sizeof(vertices.at(0)) * vertices.size());

    // Transfer vertex data from staging buffer to device buffer
    VulkanLearning::VulkanCommandPool transferCommandPool(device.getDevice(), device.getQueueFamilyIndex(), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    VulkanLearning::VulkanCommandBufferGroup vertexTransferCommand(device.getDevice(), transferCommandPool.getCommandPool(), 1);
    VulkanLearning::VulkanBuffer vertexBuffer(device.getDevice(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        sizeof(vertices.at(0)) * vertices.size());
    vertexBuffer.allocateMemory(device.getSuitableMemoryTypeIndex(vertexBuffer.getMemoryRequirements().memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
    vertexBuffer.uploadData(stagingVertexBuffer.getBuffer(), sizeof(vertices.at(0)) * vertices.size(),
        vertexTransferCommand.getCommandBuffers().at(0));
    device.queueSubmit(vertexTransferCommand.getCommandBuffers().at(0));
    stagingVertexBuffer.destroyBuffer();

    // Transfer index data into staging buffer
    VulkanLearning::VulkanBuffer stagingIndexBuffer(device.getDevice(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        sizeof(vertexIndices.at(0)) * vertexIndices.size());
    stagingIndexBuffer.allocateMemory(device.getSuitableMemoryTypeIndex(stagingIndexBuffer.getMemoryRequirements().memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
    stagingIndexBuffer.uploadData(vertexIndices.data(), sizeof(vertexIndices.at(0)) * vertexIndices.size());

    // Transfer index data from staging buffer to device buffer
    VulkanLearning::VulkanCommandBufferGroup indexTransferCommand(device.getDevice(), transferCommandPool.getCommandPool(), 1);
    VulkanLearning::VulkanBuffer indexBuffer(device.getDevice(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        sizeof(vertexIndices.at(0)) * vertexIndices.size());
    indexBuffer.allocateMemory(device.getSuitableMemoryTypeIndex(indexBuffer.getMemoryRequirements().memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
    indexBuffer.uploadData(stagingIndexBuffer.getBuffer(), sizeof(vertexIndices.at(0)) * vertexIndices.size(),
        indexTransferCommand.getCommandBuffers().at(0));
    device.queueSubmit(indexTransferCommand.getCommandBuffers().at(0));
    stagingIndexBuffer.destroyBuffer();

    // Create uniform buffer and transfer its data into descriptor set
    VulkanLearning::VulkanBuffer uniformBuffer(device.getDevice(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(VulkanLearning::UniformBufferObject));
    uniformBuffer.allocateMemory(device.getSuitableMemoryTypeIndex(uniformBuffer.getMemoryRequirements().memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
    VulkanLearning::VulkanDescriptorPool descriptorPool(device.getDevice());
    VulkanLearning::VulkanDescriptorSetGroup descriptorSets(device.getDevice(), setLayout.getDescriptorSetLayout(),
        descriptorPool.getDescriptorPool(), 1);
    descriptorSets.addDataFromBuffer(uniformBuffer.getBuffer(), sizeof(VulkanLearning::UniformBufferObject));

    framebuffers.beginRenderPass(commandBuffers.getCommandBuffers(), graphicsPipeline.getPipeline(), {vertexBuffer.getBuffer()},
        indexBuffer.getBuffer(), vertexIndices.size(), {0}, vertices.size(), graphicsPipeline.getPipelineLayout(),
        descriptorSets.getDescriptorSets().at(0));

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                device.waitIdle();

                framebuffers.destroyFramebuffers();
                commandBuffers.destroyCommandBuffers();
                graphicsPipeline.destroyPipeline();
                renderPass.destroyRenderPass();
                swapChain.destroySwapChain();

                swapChain.reloadSwapChain(device.getVulkanSwapChainInfo());
                renderPass.reloadRenderPass(swapChain.getSurfaceFormat().format);
                graphicsPipeline.reloadPipeline(renderPass.getRenderPass(), swapChain.getExtent());
                framebuffers.reloadFramebuffers(renderPass.getRenderPass(), swapChain.getExtent(), swapChain.getImageViews());
                commandBuffers.reloadCommandBuffers();

                framebuffers.beginRenderPass(commandBuffers.getCommandBuffers(), graphicsPipeline.getPipeline(), {vertexBuffer.getBuffer()},
                    indexBuffer.getBuffer(), vertexIndices.size(), {0}, vertices.size(), graphicsPipeline.getPipelineLayout(),
                    descriptorSets.getDescriptorSets().at(0));
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                default:
                    // do nothing
                    break;
                }
            }
        }

        draw(device, swapChain, commandBuffers);
        updateUniformBuffer(uniformBuffer, swapChain.getExtent());
    }

    return 0;
}
