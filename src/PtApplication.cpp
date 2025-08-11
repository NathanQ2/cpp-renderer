#include "PtApplication.h"

#include "PtSimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace PalmTree {
    PtApplication::PtApplication()  {
        LoadGameObjects();
    }

    PtApplication::~PtApplication() {}

    void PtApplication::Run() {
        PtSimpleRenderSystem simpleRenderSystem(m_Device, m_Renderer.GetSwapChainRenderPass());
        
        while (!m_Window.ShouldClose()) {
            glfwPollEvents();

            if (VkCommandBuffer commandBuffer = m_Renderer.BeginFrame()) {
                m_Renderer.BeginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.RenderGameObjects(commandBuffer, m_GameObjects);
                m_Renderer.EndSwapChainRenderPass(commandBuffer);
                m_Renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(m_Device.device());
    }

    void PtApplication::LoadGameObjects() {
        std::vector<PtModel::Vertex> vertices {
            { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.5, 0.5f }, { 0.0f, 1.0f, 0.0f } },
            { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
        };

        auto model = std::make_shared<PtModel>(m_Device, vertices);

        auto triangle = PtGameObject::CreateGameObject();
        triangle.model = model;
        triangle.color = glm::vec3(0.1f, 0.8f, 0.1f);
        triangle.transform.translation.x = 0.2f;
        triangle.transform.scale.x = 2.0f;
        triangle.transform.scale.y = 0.5f;
        triangle.transform.rotation = 0.25f * glm::two_pi<float>();

        m_GameObjects.push_back(std::move(triangle));
    }
}
