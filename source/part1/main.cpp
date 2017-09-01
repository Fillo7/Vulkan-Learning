// Core headers
#include <cstdint>
#include <iostream>

// Library headers
#include "SDL2/SDL.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

// Project headers
#include "common/sdl_instance.h"
#include "common/sdl_window.h"
#include "common/vertex.h"
#include "common/vulkan_command_buffer_group.h"
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

void reloadSwapChain(VulkanLearning::VulkanSwapChain& swapChain, VulkanLearning::VulkanFramebufferGroup& framebuffers,
    VulkanLearning::VulkanCommandBufferGroup& commandBuffers, VulkanLearning::VulkanRenderPass& renderPass, VulkanLearning::VulkanPipeline& pipeline,
    uint32_t width, uint32_t height)
{
    framebuffers.destroyFramebuffers();
    commandBuffers.destroyCommandBuffers();
    pipeline.destroyPipeline();
    renderPass.destroyRenderPass();
    swapChain.destroySwapChain();

    VkExtent2D newExtent = {width, height};

    swapChain.reloadSwapChain(newExtent);
    renderPass.reloadRenderPass(swapChain.getSurfaceFormat().format);
    pipeline.reloadPipeline(renderPass.getRenderPass(), newExtent);
    framebuffers.reloadFramebuffers(renderPass.getRenderPass(), newExtent, swapChain.getImageViews());
    commandBuffers.reloadCommandBuffers();

    framebuffers.beginRenderPass(commandBuffers.getCommandBuffers(), pipeline.getPipeline());
}

int main(int argc, char* argv[])
{
    bool quit = false;
    VulkanLearning::SdlInstance sdlInstance(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    VulkanLearning::SdlWindow window("Part 1", 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Event event;

    VulkanLearning::VulkanInstance vulkanInstance("Part 1", {"VK_LAYER_LUNARG_standard_validation"}, {"VK_KHR_surface", "VK_KHR_win32_surface",
        "VK_EXT_debug_report"});
    std::vector<VkPhysicalDevice> devices = vulkanInstance.getPhysicalDevices();

    if (devices.size() == 0)
    {
        return -1;
    }

    VulkanLearning::VulkanSurface surface(vulkanInstance.getInstance(), window.getWindow());
    VulkanLearning::VulkanDevice device(devices.at(0), VK_QUEUE_GRAPHICS_BIT, {"VK_LAYER_LUNARG_standard_validation"}, {"VK_KHR_swapchain"},
        surface.getSurface());
    VulkanLearning::VulkanSwapChain swapChain(device.getDevice(), surface.getSurface(), device.getVulkanSwapChainInfo(),
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    VulkanLearning::VulkanShaderModule vertexShader(device.getDevice(), "part1_vert.spv");
    VulkanLearning::VulkanShaderModule fragmentShader(device.getDevice(), "part1_frag.spv");
    VulkanLearning::VulkanRenderPass renderPass(device.getDevice(), swapChain.getSurfaceFormat().format);
    VulkanLearning::VulkanPipeline graphicsPipeline(device.getDevice(), renderPass.getRenderPass(), vertexShader.getShaderModule(),
        fragmentShader.getShaderModule(), swapChain.getExtent());
    VulkanLearning::VulkanFramebufferGroup framebuffers(device.getDevice(), renderPass.getRenderPass(), swapChain.getExtent(),
        swapChain.getImageViews());
    VulkanLearning::VulkanCommandBufferGroup commandBuffers(device.getDevice(), device.getQueueFamilyIndex(),
        static_cast<uint32_t>(framebuffers.getFramebuffers().size()));
    framebuffers.beginRenderPass(commandBuffers.getCommandBuffers(), graphicsPipeline.getPipeline());

    const std::vector<VulkanLearning::Vertex> vertices{{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

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
                int newWidth = window.getWidth();
                int newHeight = window.getHeight();

                device.waitIdle();
                reloadSwapChain(swapChain, framebuffers, commandBuffers, renderPass, graphicsPipeline, static_cast<uint32_t>(newWidth),
                    static_cast<uint32_t>(newHeight));
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
    }

    return 0;
}
