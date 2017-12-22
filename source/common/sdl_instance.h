#pragma once

#include <stdexcept>
#include <string>
#include "SDL2/SDL.h"

namespace VulkanLearning
{

class SdlInstance
{
public:
    SdlInstance(Uint32 flags)
    {
        if (SDL_Init(flags) != 0)
        {
            throw std::runtime_error(std::string("Unable to initialize SDL: ") + SDL_GetError());
        }
    }

    ~SdlInstance()
    {
        SDL_Quit();
    }
};

} // namespace VulkanLearning
