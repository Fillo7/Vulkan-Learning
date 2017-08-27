// Core headers
#include <cstdint>
#include <iostream>
#include <limits>

// Library headers
#include "SDL2/SDL.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

// Project headers
#include "common/sdl_instance.h"
#include "common/sdl_window.h"
#include "common/vulkan_command_buffer.h"
#include "common/vulkan_device.h"
#include "common/vulkan_framebuffer.h"
#include "common/vulkan_instance.h"
#include "common/vulkan_pipeline.h"
#include "common/vulkan_render_pass.h"
#include "common/vulkan_semaphore.h"
#include "common/vulkan_shader_module.h"
#include "common/vulkan_surface.h"
#include "common/vulkan_swap_chain.h"
#include "common/vulkan_utility.h"

#ifdef max
#undef max
#endif

void draw(VulkanLearning::VulkanDevice& device, VulkanLearning::VulkanSwapChain& swapChain, VulkanLearning::VulkanCommandBuffer& commandBuffer)
{
    VulkanLearning::VulkanSemaphore imageReadySemaphore(device.getDevice());
    VulkanLearning::VulkanSemaphore renderFinishedSemaphore(device.getDevice());

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device.getDevice(), swapChain.getSwapChain(), std::numeric_limits<uint64_t>::max(), imageReadySemaphore.getSemaphore(),
        VK_NULL_HANDLE, &imageIndex);
    device.queueSubmit(commandBuffer.getCommandBuffers().at(imageIndex), imageReadySemaphore.getSemaphore(), renderFinishedSemaphore.getSemaphore());
    device.queuePresent(swapChain.getSwapChain(), renderFinishedSemaphore.getSemaphore(), imageIndex);
}

int main(int argc, char* argv[])
{
    bool quit = false;
    VulkanLearning::SdlInstance sdlInstance(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    VulkanLearning::SdlWindow window("Part 1", 1280, 720);
    SDL_Event event;

    VulkanLearning::VulkanInstance vulkanInstance("Part 1", { "VK_LAYER_LUNARG_standard_validation" }, { "VK_KHR_surface", "VK_KHR_win32_surface",
        "VK_EXT_debug_report" });
    vulkanInstance.printExtensions(std::cout);
    std::vector<VkPhysicalDevice> devices = vulkanInstance.getPhysicalDevices();

    if (devices.size() == 0)
    {
        return -1;
    }

    VulkanLearning::VulkanSurface vulkanSurface(vulkanInstance.getInstance(), window.getWindow());
    VulkanLearning::VulkanDevice vulkanDevice(devices.at(0), VK_QUEUE_GRAPHICS_BIT, { "VK_LAYER_LUNARG_standard_validation" },
        { "VK_KHR_swapchain" }, vulkanSurface.getSurface());
    VulkanLearning::VulkanSwapChain vulkanSwapChain(vulkanDevice.getDevice(), vulkanSurface.getSurface(), vulkanDevice.getVulkanSwapChainInfo(),
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    VulkanLearning::VulkanShaderModule vertexShader(vulkanDevice.getDevice(), "part1_vert.spv");
    VulkanLearning::VulkanShaderModule fragmentShader(vulkanDevice.getDevice(), "part1_frag.spv");
    VulkanLearning::VulkanRenderPass renderPass(vulkanDevice.getDevice(), vulkanSwapChain.getSurfaceFormat().format);
    VulkanLearning::VulkanPipeline graphicsPipeline(vulkanDevice.getDevice(), renderPass.getRenderPass(), vertexShader.getShaderModule(),
        fragmentShader.getShaderModule(), vulkanSwapChain.getExtent());
    VulkanLearning::VulkanFramebuffer framebuffer(vulkanDevice.getDevice(), renderPass.getRenderPass(), vulkanSwapChain.getExtent(),
        vulkanSwapChain.getImageViews());
    VulkanLearning::VulkanCommandBuffer commandBuffer(vulkanDevice.getDevice(), vulkanDevice.getQueueFamilyIndex(),
        static_cast<uint32_t>(framebuffer.getFramebuffers().size()));
    framebuffer.beginRenderPass(commandBuffer.getCommandBuffers(), graphicsPipeline.getPipeline());

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
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

        draw(vulkanDevice, vulkanSwapChain, commandBuffer);
        //SDL_UpdateWindowSurface(window.getWindow());
    }

    vkDeviceWaitIdle(vulkanDevice.getDevice());
    return 0;
}
