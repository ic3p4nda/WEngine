#include "Window.h"

#include <stdexcept>

namespace Engine
{
    Window::Window(int w, int h, std::string t) : width(w), height(h), windowTitle(t)
    {
        initWindow();
    }

    Window::~Window()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void Window::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        
        window = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto engineWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        engineWindow->frameBufferResized = true;
        engineWindow->width = width;
        engineWindow->height = height;
    }
}

