function setupVulkan()
    local path = os.getenv("VULKAN_SDK")
    
    if not path then
        return false
    end
    
    includedirs { "$(VULKAN_SDK)/Include", "$(VULKAN_SDK)/Third-Party/Include", "libraries" }
    libdirs { "$(VULKAN_SDK)/Lib", "$(VULKAN_SDK)/Third-Party/Bin" }
    links { "vulkan-1", "SDL2", "SDL2main" }
    
    return true
end

-- Solution configuration
workspace "VulkanLearning"
    configurations { "Debug", "Release" }
    platforms { "x86_64" }
    language "C++"
    flags { "C++14" }
    location "build"
    architecture "x86_64"
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
    
    filter {}

-- Project configuration
project "Demo"
    kind "ConsoleApp"
    files { "source/framework/*.h", "source/framework/*.cpp", "source/demo/*.h", "source/demo/*.cpp", "source/demo/*.frag", "source/demo/*.vert" }
    includedirs { "source" }

    vulkan = setupVulkan() 
    if not vulkan then
        error("Vulkan SDK was not found")
    end
