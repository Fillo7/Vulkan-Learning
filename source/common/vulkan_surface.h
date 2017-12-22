#pragma once

#include <cstdint>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include "vulkan/vulkan.h"

namespace VulkanLearning
{

class VulkanSurface
{
public:
    explicit VulkanSurface(VkInstance instance, SDL_Window* window) :
        instance(instance)
    {
		if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
		{
			throw std::runtime_error(std::string("Unable to create surface: ") + SDL_GetError());
		}
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
