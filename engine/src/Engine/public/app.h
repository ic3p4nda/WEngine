#pragma once

#include "window.h"
#include "game_object.h"
#include "device.h"
#include "renderer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>

#include "Systems/render_system.h"

namespace Engine {
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
        void LoadGameObjects();
        
        Window Window{WIDTH , HEIGHT , "HelloWorld"};
        Device Device{Window};
        Renderer Renderer{Window, Device};
        
        std::vector<GameObject> gameObjects;
    };
    
}
