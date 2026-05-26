#include "PtApplication.h"

#include "PtSimpleRenderSystem.h"
#include "PtCamera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace PalmTree {
    PtApplication::PtApplication() {
        LoadGameObjects();
    }

    void PtApplication::Run() {
        PtSimpleRenderSystem simpleRenderSystem(m_Device, m_Renderer.GetSwapChainRenderPass());
        PtCamera camera{};
        camera.setViewDirection(glm::vec3(0), glm::vec3(0.0, 0.0f, 1.0f));
        
        while (!m_Window.ShouldClose()) {
            glfwPollEvents();
            
            float aspect = m_Renderer.getAspectRatio();
            // camera.setOrthographicProjection(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
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
    
    // temporary helper function, creates a 1x1x1 cube centered at offset
    std::unique_ptr<PtModel> createCubeModel(PtDevice& device, glm::vec3 offset) {
      std::vector<PtModel::Vertex> vertices{

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
      return std::make_unique<PtModel>(device, vertices);
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
