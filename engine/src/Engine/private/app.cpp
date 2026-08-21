#include "app.h"

#include "camera.h"
#include "keyboard_movement_controller.h"

#include <array>
#include <chrono>
#include <iostream>
#include <stdexcept>

namespace ZEngine
{
    ZApp::ZApp()
    {
        LoadGameObjects();
    }

    ZApp::~ZApp(){}

    void ZApp::run()
    {
        std::cout << "MaxPushConstantSize = " << Device.properties.limits.maxPushConstantsSize << std::endl;
        
        ZRenderSystem simpleRenderSystem{Device, Renderer.getSwapchainRenderPass()};
        ZCamera camera{};
        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, -2.0f), glm::vec3(0.0f, 0.0f, 2.5f));
        
        auto viewerObject = ZGameObject::createGameObject();
        ZKeyboardMovementController cameraController{Window.getGLFWwindow()};
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        while (!Window.shouldClose()){
            glfwPollEvents();
            
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
            currentTime = newTime;
        
            frameTime = std::min(frameTime, MAX_FRAME_TIME);
            
            cameraController.moveInPlaneXZ(Window, frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            
            std::cout << "Frame time = " << frameTime << std::endl;
            
            float aspect = Renderer.getAspectRatio();
            
             camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);
            
            if (auto commandBuffer = Renderer.beginFrame())
            {
                Renderer.beginSwapchainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                Renderer.endSwapchainRenderPass(commandBuffer);
                Renderer.endFrame();
            }
        }
        
        vkDeviceWaitIdle(Device.device());
    }

    std::unique_ptr<ZModel> createCubeModel(ZDevice& device, glm::vec3 offset) {
        std::vector<ZModel::Vertex> vertices{
              // left face (white)
              {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
              {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
              {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
              {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
              {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
              {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        
              // right face (yellow)
              {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
              {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
              {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
              {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
              {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
              {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        
              // top face (orange, remember y axis points down)
              {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
              {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
              {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
              {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
              {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
              {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        
              // bottom face (red)
              {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
              {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
              {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
              {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
              {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
              {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        
              // nose face (blue)
              {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
              {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
              {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
              {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
              {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
              {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        
              // tail face (green)
              {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
              {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
              {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
              {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
              {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
              {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        
          };
          for (auto& v : vertices) {
            v.position += offset;
          }
          return std::make_unique<ZModel>(device, vertices);
        }
    
    void ZApp::LoadGameObjects()
    {
        std::shared_ptr<ZModel> model = createCubeModel(Device, {0.0f, 0.0f, 0.0f});
        
        auto cube = ZGameObject::createGameObject();
        cube.model = model;
        cube.transform.translation = {0.0f, 0.0f, 2.5f};
        cube.transform.scale = {0.5f, 0.5f, 0.5f};
        gameObjects.push_back(std::move(cube));
    }
}
