#pragma once

#include <string>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"

namespace VulkanLearning
{

class SdlWindow
{
public:
    SdlWindow(const std::string& windowName, const int width, const int height, const Uint32 flags)
    {
        window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    }

    ~SdlWindow()
    {
        SDL_DestroyWindow(window);
    }

    SDL_Window* getWindow() const
    {
        return window;
    }

    int getWidth() const
    {
        int width;
        SDL_GetWindowSize(window, &width, nullptr);

        return width;
    }

    int getHeight() const
    {
        int height;
        SDL_GetWindowSize(window, nullptr, &height);

        return height;
    }

	std::vector<const char*> getInstanceExtensions() const
	{
		unsigned int extensionCount;
		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr))
		{
			throw std::runtime_error(std::string("Unable to retrieve instance extension count: ") + SDL_GetError());
		}

		std::vector<const char*> result(extensionCount);
		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, result.data()))
		{
			throw std::runtime_error(std::string("Unable to retrieve instance extensions: ") + SDL_GetError());
		}

		return result;
	}

private:
    SDL_Window* window;
};

} // namespace VulkanLearning
