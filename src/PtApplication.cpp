#include "PtApplication.h"

#include "PtSimpleRenderSystem.h"
#include "PtCamera.h"
#include "KeyboardMovementController.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>

namespace PalmTree {
    PtApplication::PtApplication() {
        LoadGameObjects();
    }

    void PtApplication::Run() {
        PtSimpleRenderSystem simpleRenderSystem(m_Device, m_Renderer.GetSwapChainRenderPass());
        PtCamera camera{};
        camera.setViewDirection(glm::vec3(0), glm::vec3(0.0, 0.0f, 1.0f));
        
        auto viewerObject = PtGameObject::CreateGameObject();
        glfwSetInputMode(m_Window.getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(m_Window.getGLFWWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glm::f64vec2 cursorPos = glm::f64vec2(0);
        glfwGetCursorPos(m_Window.getGLFWWindow(), &cursorPos.x, &cursorPos.y);
        KeyboardMovementController cameraController = KeyboardMovementController(cursorPos);
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        while (!m_Window.ShouldClose()) {
            glfwPollEvents();
            
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
            currentTime = newTime;
            
            cameraController.moveInPlaneXZ(m_Window.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            
            float aspect = m_Renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

            if (VkCommandBuffer commandBuffer = m_Renderer.BeginFrame()) {
                m_Renderer.BeginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.RenderGameObjects(commandBuffer, m_GameObjects, camera);
                m_Renderer.EndSwapChainRenderPass(commandBuffer);
                m_Renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(m_Device.device());
    }
    
    // temporary helper function, creates a 1x1x1 cube centered at offset with an index buffer
    std::unique_ptr<PtModel> createCubeModel(PtDevice& device, glm::vec3 offset) {
        PtModel::Builder modelBuilder{};
        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }

        modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

        return std::make_unique<PtModel>(device, modelBuilder);
    }

    void PtApplication::LoadGameObjects() {
        std::shared_ptr<PtModel> model = createCubeModel(m_Device, {0.0f, 0.0f, 0.0f });
        
        PtGameObject cube = PtGameObject::CreateGameObject();
        cube.model = model;
        cube.transform.translation = {0.0f, 0.0f, 2.5f };
        cube.transform.scale = { 0.5, 0.5f, 0.5f };
        
        m_GameObjects.push_back(std::move(cube));
    }
}
