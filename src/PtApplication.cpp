#include "PtApplication.h"

#include "PtSimpleRenderSystem.h"
#include "PtCamera.h"
#include "PtBuffer.h"
#include "KeyboardMovementController.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>

namespace PalmTree {
    struct GlobalUBO {
        glm::mat4 projectionView = glm::mat4(1.0f);
        glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, -3.0f, -1.0f));
    };
    
    
    PtApplication::PtApplication() {
        LoadGameObjects();
    }

    void PtApplication::Run() {
        std::vector<std::unique_ptr<PtBuffer>> uboBuffers(PtSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<PtBuffer>(
                m_Device,
                sizeof(GlobalUBO),
                PtSwapChain::MAX_FRAMES_IN_FLIGHT,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                m_Device.properties.limits.minUniformBufferOffsetAlignment
            );
            uboBuffers[i]->map();
        }
        
        PtSimpleRenderSystem simpleRenderSystem(m_Device, m_Renderer.GetSwapChainRenderPass());
        PtCamera camera{};
        camera.setViewDirection(glm::vec3(0), glm::vec3(0.0, 0.0f, 1.0f));
        
        auto viewerObject = PtGameObject::CreateGameObject();
        glfwSetInputMode(m_Window.getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported())
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
                int frameIndex = m_Renderer.GetFrameIndex();
                FrameInfo frameInfo {
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera
                };
                
                // Update
                GlobalUBO ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                
                // Render
                m_Renderer.BeginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.RenderGameObjects(frameInfo, m_GameObjects);
                m_Renderer.EndSwapChainRenderPass(commandBuffer);
                m_Renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(m_Device.device());
    }

    void PtApplication::LoadGameObjects() {
        std::shared_ptr model = PtModel::CreateModelFromFile(m_Device, "../assets/models/flat_vase.obj");
        
        PtGameObject obj = PtGameObject::CreateGameObject();
        obj.model = model;
        obj.transform.translation = glm::vec3(0.0, 0.0, 2.0f);
        obj.transform.scale = glm::vec3(1);
        
        m_GameObjects.push_back(std::move(obj));
    }
}
