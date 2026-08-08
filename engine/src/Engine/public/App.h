#pragma once

#include "window.h"
#include "pipeline.h"
#include "device.h"
#include "swapchain.h"
#include "Model.h"

#include <memory>
#include <vector>

namespace Engine {
    const std::string vertexShaderPath = "C:/Users/IcePa/Desktop/Engine/engine/engine/src/shaders/shader.vert.spv";
    const std::string fragShaderPath = "C:/Users/IcePa/Desktop/Engine/engine/engine/src/shaders/shader.frag.spv";
    
    
    class App
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
        
        App();
        ~App();
        
        App(const App&) = delete;
        App &operator=(const App&) = delete;
        
        void run();
    
    private:
        void LoadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapchain();
        void recordCommandBuffer(int imageIndex);
        
        static void sierpinski(
            std::vector<Model::Vertex>& vertices,
            int depth,
            glm::vec2 top,
            glm::vec2 left,
            glm::vec2 right);
        
        Window Window{WIDTH , HEIGHT , "HelloWorld"};
        Device Device{Window};
        std::unique_ptr<SwapChain> Swapchain;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<Model> model;
    };
    
}