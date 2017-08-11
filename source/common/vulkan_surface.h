#pragma once

#include <cstdint>
#include <vector>

#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanSurface
{
public:
    explicit VulkanSurface(const VkInstance instance, SDL_Window* window) :
        instance(instance)
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(window, &wmInfo);
        HWND hardwareWindow = wmInfo.info.win.window;

        VkWin32SurfaceCreateInfoKHR createInfo =
        {
            VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            nullptr,
            0,
            GetModuleHandle(nullptr),
            hardwareWindow
        };

        auto createWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
        checkVulkanError(createWin32SurfaceKHR(instance, &createInfo, nullptr, &surface), "vkCreateWin32SurfaceKHR");
    }

    ~VulkanSurface()
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    VkInstance getInstance() const
    {
        return instance;
    }

    VkSurfaceKHR getSurface() const
    {
        return surface;
    }

private:
    VkInstance instance;
    VkSurfaceKHR surface;
};

} // namespace VulkanLearning
