// Core headers
#include <cstdint>
#include <iostream>

// Library headers
#include "SDL2/SDL.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

// Project header
#include "common/sdl_instance.h"
#include "common/sdl_window.h"
#include "common/vulkan_device.h"
#include "common/vulkan_instance.h"
#include "common/vulkan_surface.h"
#include "common/vulkan_swap_chain.h"
#include "common/vulkan_utility.h"

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
        SDL_UpdateWindowSurface(window.getWindow());
    }

    return 0;
}
