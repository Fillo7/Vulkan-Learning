#pragma once

#include <string>

#include "SDL2/SDL.h"

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

private:
    SDL_Window* window;
};

} // namespace VulkanLearning
