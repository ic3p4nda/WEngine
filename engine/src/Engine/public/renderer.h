#pragma once

#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "model.h"

#include <memory>
#include <vector>
#include <cassert>

namespace Engine {
    
    class Renderer
    {
    public:
        Renderer( Window& Window, Device& Device);
        ~Renderer();
        
        Renderer(const Renderer&) = delete;
        Renderer &operator=(const Renderer&) = delete;
        
        VkRenderPass getSwapchainRenderPass() const {return Swapchain->getRenderPass();};
        float getAspectRatio() const {return Swapchain->extentAspectRatio();};
        bool isFrameInProgress() const{ return isFrameStarted; };
        
        VkCommandBuffer getCurrentCommandBuffer() const
        {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        };
        
        int getFrameIndex() const
        {
            assert(isFrameStarted && "Cannot get Frame index when frame not in progress");
            return currentFrameIndex;
        };
        
        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapchainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapchainRenderPass(VkCommandBuffer commandBuffer);
        
        
    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapchain();
        
        Window& Window;
        Device& Device;
        std::unique_ptr<SwapChain> Swapchain;
        std::vector<VkCommandBuffer> commandBuffers;
        
        uint32_t currentImageIndex = 0;
        int currentFrameIndex;
        bool isFrameStarted = false;
    };
}