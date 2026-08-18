#pragma once

#include "pipeline.h"
#include "camera.h"
#include "game_object.h"
#include "device.h"
#include "model.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>

namespace Engine {
    const std::string vertexShaderPath = "C:/Users/IcePa/Desktop/Engine/engine/engine/src/shaders/shader.vert.spv";
    const std::string fragShaderPath = "C:/Users/IcePa/Desktop/Engine/engine/engine/src/shaders/shader.frag.spv";
    
    struct SimplePushConstantData
    {
        glm::mat4 transform {1.0f};
        alignas(16) glm::vec3 color;
    };
    
    class RenderSystem
    {
    public:
        RenderSystem(Device& device, VkRenderPass renderPass);
        ~RenderSystem();
        
        RenderSystem(const RenderSystem&) = delete;
        RenderSystem &operator=(const RenderSystem&) = delete;
    
        void renderGameObjects(
            VkCommandBuffer commandBuffer, 
            std::vector<GameObject>& gameObjects,
            const Camera& camera);
    
    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);
        
        Device& Device;
        
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };
    
}