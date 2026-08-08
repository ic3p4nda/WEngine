#include "App.h"

#include <array>
#include <stdexcept>

namespace Engine
{
    App::App()
    {
        LoadModels();
        createPipelineLayout();
        recreateSwapchain();
        createCommandBuffers();
    }

    App::~App()
    {
        vkDestroyPipelineLayout(Device.device(), pipelineLayout, nullptr);
    }

    void App::run()
    {
        while (!Window.shouldClose()){
            glfwPollEvents();
            drawFrame();
        }
        
        vkDeviceWaitIdle(Device.device());
    }

    void App::LoadModels()
    {
        std::vector<Model::Vertex> vertices{};
        sierpinski(vertices,
            4,
            {0.0f, -0.5f},
            {0.5f, 0.5f},
            {-0.5f, 0.5f}
            );
        
        model = std::make_unique<Model>(Device, vertices);
    }

    void App::createPipelineLayout()
    {
        VkPipelineLayoutCreateInfo layoutCreateInfo {};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutCreateInfo.setLayoutCount = 0;
        layoutCreateInfo.pSetLayouts = nullptr;
        layoutCreateInfo.pushConstantRangeCount = 0;
        layoutCreateInfo.pPushConstantRanges = nullptr;
        
        if (vkCreatePipelineLayout(Device.device(), &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void App::createPipeline()
    {
        assert(Swapchain != nullptr && "Cannot create pipeline befora swapchain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline befora pipeline layout");
        
        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = Swapchain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(Device,
            pipelineConfig,
            vertexShaderPath,
            fragShaderPath);
    }

    void App::createCommandBuffers()
    {
        commandBuffers.resize(Swapchain->imageCount());
        
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = Device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        
        if (vkAllocateCommandBuffers(Device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void App::freeCommandBuffers()
    {
        vkFreeCommandBuffers(Device.device(), Device.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        
        commandBuffers.clear();
    }

    void App::drawFrame()
    {
        uint32_t imageIndex;
        auto result = Swapchain->acquireNextImage(&imageIndex);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapchain();
            return;
        }
        
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swapchain image!");
        }
        
        recordCommandBuffer(imageIndex);
        result = Swapchain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || Window.wasWindowReized())
        {
            Window.resetWindowResizeFlag();
            recreateSwapchain();
            return;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to present swapchain image!");
        }
    }

    void App::recreateSwapchain()
    {
        auto extent = Window.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = Window.getExtent();
            glfwWaitEvents();
        }
        
        vkDeviceWaitIdle(Device.device());  
        
        if (Swapchain == nullptr)
        {
            Swapchain = std::make_unique<SwapChain>(Device, extent);
        }
        else
        {
          Swapchain = std::make_unique<SwapChain>(Device, extent, std::move(Swapchain));
            if (Swapchain->imageCount() != commandBuffers.size())
            {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }
        
        createPipeline();
    }

    void App::recordCommandBuffer(int imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            
        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
            
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = Swapchain->getRenderPass();
        renderPassBeginInfo.framebuffer = Swapchain->getFrameBuffer(imageIndex);
            
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = Swapchain->getSwapChainExtent();
            
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();
            
        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(Swapchain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(Swapchain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, Swapchain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);
        
        pipeline->bind(commandBuffers[imageIndex]);
        model->bind(commandBuffers[imageIndex]);
        model->draw(commandBuffers[imageIndex]);
            
        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void App::sierpinski(std::vector<Model::Vertex>& vertices,
                         int depth, glm::vec2 top,
                         glm::vec2 left,
                         glm::vec2 right)
    {
        if (depth <= 0)
        {
            vertices.push_back({top,   {1.0f, 0.0f, 0.0f}});
            vertices.push_back({left,  {0.0f, 1.0f, 0.0f}});
            vertices.push_back({right, {0.0f, 0.0f, 1.0f}});
            return;
        }
        
        glm::vec2 topLeft = 0.5f * (top + left);
        glm::vec2 topRight = 0.5f * (top + right);
        glm::vec2 leftRight = 0.5f * (left + right);
        
        sierpinski(vertices, depth - 1, top, topLeft, topRight);
        sierpinski(vertices, depth - 1, topLeft, left, leftRight);
        sierpinski(vertices, depth - 1, topRight, leftRight, right);
    }
}
