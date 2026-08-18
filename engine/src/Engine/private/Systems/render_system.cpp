#include "Systems\render_system.h"

#include <array>
#include <iostream>
#include <stdexcept>

#include "app.h"

namespace Engine
{
    RenderSystem::RenderSystem(class Device& device, VkRenderPass renderpass) : Device(device)
    {
        createPipelineLayout();
        createPipeline(renderpass);
    }

    RenderSystem::~RenderSystem()
    {
        vkDestroyPipelineLayout(Device.device(), pipelineLayout, nullptr);
    }
    
    void RenderSystem::createPipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);
        
        VkPipelineLayoutCreateInfo layoutCreateInfo {};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutCreateInfo.setLayoutCount = 0;
        layoutCreateInfo.pSetLayouts = nullptr;
        layoutCreateInfo.pushConstantRangeCount = 1;
        layoutCreateInfo.pPushConstantRanges = &pushConstantRange;
        
        if (vkCreatePipelineLayout(Device.device(), &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void RenderSystem::createPipeline(VkRenderPass renderpass)
    {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        
        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderpass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(Device,
            pipelineConfig,
            vertexShaderPath,
            fragShaderPath);
    }

    void RenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera& camera)
    {
        pipeline->bind(commandBuffer);
        auto projectionView = camera.getProjection() * camera.getView();
        
        for (auto& object : gameObjects)
        {
            object.transformComponent.rotation.y = glm::mod(object.transformComponent.rotation.y + 0.01f, glm::two_pi<float>());
            object.transformComponent.rotation.x = glm::mod(object.transformComponent.rotation.x + 0.005f, glm::two_pi<float>());
            
            SimplePushConstantData push{};
            push.color = object.color;
            push.transform = projectionView* object.transformComponent.mat4();
            
            vkCmdPushConstants(commandBuffer, 
               pipelineLayout,
               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
               0,
               sizeof(SimplePushConstantData),
               &push);
            object.model->bind(commandBuffer);
            object.model->draw(commandBuffer);
        }
    }
}
