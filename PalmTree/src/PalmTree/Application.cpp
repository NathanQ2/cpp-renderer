#include "Application.h"

#include "KeyboardMovementController.h"
#include "Buffer.h"
#include "Camera.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Systems/PointLightSystem.h"
#include "Systems/SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>
#include <ostream>
#include <glm/ext/matrix_transform.hpp>

namespace PalmTree {
    Application::Application() {
        m_GlobalPool = DescriptorPool::Builder(m_Device)
            .SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .Build();
        LoadGameObjects();
    }

    void Application::Run() {
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<Buffer>(
                m_Device,
                sizeof(GlobalUBO),
                SwapChain::MAX_FRAMES_IN_FLIGHT,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                m_Device.m_Properties.limits.minUniformBufferOffsetAlignment
            );
            uboBuffers[i]->Map();
        }

        auto globalSetLayout = DescriptorSetLayout::Builder(m_Device)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .Build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->DescriptorInfo();
            DescriptorWriter(*globalSetLayout, *m_GlobalPool)
                .WriteBuffer(0, &bufferInfo)
                .Build(globalDescriptorSets[i]);
        }

        std::shared_ptr<SimpleRenderSystem> simpleRenderSystem = std::make_shared<SimpleRenderSystem>(
            m_Device,
            m_Renderer.GetSwapChainRenderPass(),
            globalSetLayout->GetDescriptorSetLayout()
        );
        m_Ecs.RegisterSystem<SimpleRenderSystem>(
            simpleRenderSystem,
            SignatureBuilder<TransformComponent, ModelComponent>(m_Ecs.GetComponentManager()).Build()
        );

        std::shared_ptr<PointLightSystem> pointLightSystem = std::make_shared<PointLightSystem>(
            m_Device,
            m_Renderer.GetSwapChainRenderPass(),
            globalSetLayout->GetDescriptorSetLayout()
        );
        m_Ecs.RegisterSystem<PointLightSystem>(
            pointLightSystem,
            SignatureBuilder<TransformComponent, PointLightComponent>(m_Ecs.GetComponentManager()).Build()
        );

        Camera camera{};
        camera.SetViewDirection(glm::vec3(0), glm::vec3(0.0, 0.0f, 1.0f));

        GameObject& viewerObject = m_Ecs.CreateGameObject();
        viewerObject.GetTransform().Translation.z = -2.5f;

        glfwSetInputMode(m_Window.GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(m_Window.GetGLFWWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glm::f64vec2 cursorPos = glm::f64vec2(0);
        glfwGetCursorPos(m_Window.GetGLFWWindow(), &cursorPos.x, &cursorPos.y);
        KeyboardMovementController cameraController = KeyboardMovementController(cursorPos);

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!m_Window.ShouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.MoveInPlaneXZ(m_Window.GetGLFWWindow(), frameTime, viewerObject);
            camera.SetViewYXZ(viewerObject.GetTransform().Translation, viewerObject.GetTransform().Rotation);

            float aspect = m_Renderer.GetAspectRatio();
            camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

            if (VkCommandBuffer commandBuffer = m_Renderer.BeginFrame()) {
                int frameIndex = m_Renderer.GetFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                };

                // Update
                GlobalUBO ubo{};
                ubo.Projection = camera.GetProjection();
                ubo.View = camera.GetView();
                ubo.InverseView = camera.GetInverseView();
                pointLightSystem->Update(frameInfo, ubo);
                uboBuffers[frameIndex]->WriteToBuffer(&ubo);
                uboBuffers[frameIndex]->Flush();

                // Render
                m_Renderer.BeginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem->RenderGameObjects(frameInfo);
                pointLightSystem->Render(frameInfo);
                m_Renderer.EndSwapChainRenderPass(commandBuffer);
                m_Renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(m_Device.GetDevice());
    }

    void Application::LoadGameObjects() {
        // Flat Vase
        {
            std::shared_ptr model = Model::CreateModelFromFile(m_Device, "../../assets/models/flat_vase.obj");

            GameObject& obj = m_Ecs.CreateGameObject();

            obj.AddComponent<ModelComponent>(ModelComponent{glm::vec3(1), model});

            obj.GetTransform().Translation = glm::vec3(-0.5, 0.0, 0.0f);
            obj.GetTransform().Scale = glm::vec3(3, 1.5, 3);
        }

        // Smooth Vase
        {
            std::shared_ptr model = Model::CreateModelFromFile(m_Device, "../../assets/models/smooth_vase.obj");

            GameObject& obj = m_Ecs.CreateGameObject();
            obj.AddComponent(ModelComponent{glm::vec3(1), model});
            obj.GetTransform().Translation = glm::vec3(0.5, 0.0, 0.0f);
            obj.GetTransform().Scale = glm::vec3(3, 1.5, 3);
        }

        // Floor
        {
            std::shared_ptr model = Model::CreateModelFromFile(m_Device, "../../assets/models/quad.obj");

            GameObject& obj = m_Ecs.CreateGameObject();
            obj.AddComponent<ModelComponent>(ModelComponent{glm::vec3(1), model});
            obj.GetTransform().Translation = glm::vec3(0.0f, 0.0f, 0.0f);
            obj.GetTransform().Scale = glm::vec3(5);
        }

        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}
        };

        for (int i = 0; i < lightColors.size(); i++) {
            GameObject& light = m_Ecs.CreateGameObject();
            light.AddComponent<PointLightComponent>(PointLightComponent{0.2f, lightColors[i]});

            auto rotateLight = glm::rotate(
                glm::mat4(1.0f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                {0.0f, -1.0f, 0.0f}
            );

            light.GetTransform().Translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
            light.GetTransform().Scale = glm::vec3(0.2);
        }
    }
}
