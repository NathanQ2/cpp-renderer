#include "PtApplication.h"

#include "KeyboardMovementController.h"
#include "PtBuffer.h"
#include "PtCamera.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "Systems/PtPointLightSystem.h"
#include "Systems/PtSimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>
#include <ostream>
#include <glm/ext/matrix_transform.hpp>

namespace PalmTree {
    PtApplication::PtApplication() {
        m_globalPool = PtDescriptorPool::Builder(m_device)
            .setMaxSets(PtSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, PtSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        loadGameObjects();
    }

    void PtApplication::run() {
        std::vector<std::unique_ptr<PtBuffer>> uboBuffers(PtSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<PtBuffer>(
                m_device,
                sizeof(GlobalUBO),
                PtSwapChain::MAX_FRAMES_IN_FLIGHT,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                m_device.properties.limits.minUniformBufferOffsetAlignment
            );
            uboBuffers[i]->map();
        }

        auto globalSetLayout = PtDescriptorSetLayout::Builder(m_device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(PtSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            PtDescriptorWriter(*globalSetLayout, *m_globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        std::shared_ptr<PtSimpleRenderSystem> simpleRenderSystem = std::make_shared<PtSimpleRenderSystem>(
            m_device,
            m_renderer.getSwapChainRenderPass(),
            globalSetLayout->getDescriptorSetLayout()
        );
        m_ecs.registerSystem<PtSimpleRenderSystem>(
            simpleRenderSystem,
            SignatureBuilder<TransformComponent, ModelComponent>(m_ecs.getComponentManager()).build()
        );

        std::shared_ptr<PtPointLightSystem> pointLightSystem = std::make_shared<PtPointLightSystem>(
            m_device,
            m_renderer.getSwapChainRenderPass(),
            globalSetLayout->getDescriptorSetLayout()
        );
        m_ecs.registerSystem<PtPointLightSystem>(
            pointLightSystem,
            SignatureBuilder<TransformComponent, PointLightComponent>(m_ecs.getComponentManager()).build()
        );

        PtCamera camera{};
        camera.setViewDirection(glm::vec3(0), glm::vec3(0.0, 0.0f, 1.0f));

        GameObject& viewerObject = m_ecs.createGameObject();
        viewerObject.getTransform().translation.z = -2.5f;

        glfwSetInputMode(m_window.getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(m_window.getGLFWWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glm::f64vec2 cursorPos = glm::f64vec2(0);
        glfwGetCursorPos(m_window.getGLFWWindow(), &cursorPos.x, &cursorPos.y);
        KeyboardMovementController cameraController = KeyboardMovementController(cursorPos);

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!m_window.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(m_window.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.getTransform().translation, viewerObject.getTransform().rotation);

            float aspect = m_renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

            if (VkCommandBuffer commandBuffer = m_renderer.beginFrame()) {
                int frameIndex = m_renderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                };

                // Update
                GlobalUBO ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                pointLightSystem->update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // Render
                m_renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem->renderGameObjects(frameInfo);
                pointLightSystem->render(frameInfo);
                m_renderer.endSwapChainRenderPass(commandBuffer);
                m_renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(m_device.device());
    }

    void PtApplication::loadGameObjects() {
        // Flat Vase
        {
            std::shared_ptr model = PtModel::createModelFromFile(m_device, "../assets/models/flat_vase.obj");

            GameObject& obj = m_ecs.createGameObject();

            obj.addComponent<ModelComponent>(ModelComponent{glm::vec3(1), model});

            obj.getTransform().translation = glm::vec3(-0.5, 0.0, 0.0f);
            obj.getTransform().scale = glm::vec3(3, 1.5, 3);
        }

        // Smooth Vase
        {
            std::shared_ptr model = PtModel::createModelFromFile(m_device, "../assets/models/smooth_vase.obj");

            GameObject& obj = m_ecs.createGameObject();
            obj.addComponent(ModelComponent{glm::vec3(1), model});
            obj.getTransform().translation = glm::vec3(0.5, 0.0, 0.0f);
            obj.getTransform().scale = glm::vec3(3, 1.5, 3);
        }

        // Floor
        {
            std::shared_ptr model = PtModel::createModelFromFile(m_device, "../assets/models/quad.obj");

            GameObject& obj = m_ecs.createGameObject();
            obj.addComponent<ModelComponent>(ModelComponent{glm::vec3(1), model});
            obj.getTransform().translation = glm::vec3(0.0f, 0.0f, 0.0f);
            obj.getTransform().scale = glm::vec3(5);
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
            GameObject& light = m_ecs.createGameObject();
            light.addComponent<PointLightComponent>(PointLightComponent{0.2f, lightColors[i]});

            auto rotateLight = glm::rotate(
                glm::mat4(1.0f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                {0.0f, -1.0f, 0.0f}
            );

            light.getTransform().translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
            light.getTransform().scale = glm::vec3(0.2);
        }
    }
}
