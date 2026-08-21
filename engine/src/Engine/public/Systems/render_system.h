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

namespace ZEngine {
    const std::string vertexShaderPath = "C:/Users/IcePa/Desktop/Engine/engine/engine/src/shaders/shader.vert.spv";
    const std::string fragShaderPath = "C:/Users/IcePa/Desktop/Engine/engine/engine/src/shaders/shader.frag.spv";
    
    struct SimplePushConstantData
    {
        glm::mat4 transform {1.0f};
        alignas(16) glm::vec3 color;
    };
    
    class ZRenderSystem
    {
    public:
        ZRenderSystem(ZDevice& device, VkRenderPass renderPass);
        ~ZRenderSystem();
        
        ZRenderSystem(const ZRenderSystem&) = delete;
        ZRenderSystem &operator=(const ZRenderSystem&) = delete;
    
        void renderGameObjects(
            VkCommandBuffer commandBuffer, 
            std::vector<ZGameObject>& gameObjects,
            const ZCamera& camera);
    
    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);
        
        ZDevice& Device;
        
        std::unique_ptr<ZPipeline> pipeline;
        VkPipelineLayout pipelineLayout;
    };
    
}