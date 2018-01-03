#pragma once

#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace VulkanLearning
{

class Image
{
public:
    Image(const std::string& fileName)
    {
        image = stbi_load(fileName.c_str(), &width, &height, &channelCount, STBI_rgb_alpha);

        if (!image)
        {
            throw std::runtime_error(std::string("Unable to load image: ") + fileName);
        }
    }

    ~Image()
    {
        stbi_image_free(image);
    }

    stbi_uc* getImage()
    {
        return image;
    }

    int getWidth() const
    {
        return width;
    }

    int getHeight() const
    {
        return height;
    }

    int getChannelCount() const
    {
        return channelCount;
    }

    uint64_t getImageSize() const
    {
        return width * height * 4;
    }

private:
    stbi_uc* image;
    int width;
    int height;
    int channelCount;
};

} // namespace VulkanLearning
