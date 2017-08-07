// Core headers
#include <cstdint>
#include <iostream>

// Library headers
#include "vulkan/vulkan.h"
#include "SDL2/SDL.h"

// Project header
#include "common/sdl_instance.h"
#include "common/sdl_window.h"
#include "common/vulkan_instance.h"
#include "common/vulkan_utility.h"

int main(int argc, char* argv[])
{
    bool quit = false;
    VulkanLearning::SdlInstance sdlInstance(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    VulkanLearning::SdlWindow window("Part 1", 1280, 720);
    SDL_Event event;

    VulkanLearning::VulkanInstance vulkanInstance("Part 1");
    vulkanInstance.printExtensions(std::cout);

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
