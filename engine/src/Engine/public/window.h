#pragma once

#define GLFW_INCLUDE_VULKAN
#include <functional>
#include <GLFW/glfw3.h>

#include <string>
#include <functional>

namespace Engine
{
    class Window
    {
    public:
        Window(int w, int h, std::string t);
        ~Window();
        
        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
        
        bool shouldClose() {return glfwWindowShouldClose(window);};
        VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
        bool wasWindowReized() { return frameBufferResized; }
        void resetWindowResizeFlag() { frameBufferResized = false; }
        
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
    private:
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
        void initWindow();
        
        int width;
        int height;
        bool frameBufferResized = false;
        
        std::string windowTitle;
        
        GLFWwindow* window;
    };
}
