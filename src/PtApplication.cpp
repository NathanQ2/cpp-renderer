#include "PtApplication.h"

#include "KeyboardMovementController.h"
#include "PtBuffer.h"
#include "PtCamera.h"
#include "systems/PtPointLightSystem.h"
#include "systems/PtSimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>
#include <ostream>

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

        PtSimpleRenderSystem simpleRenderSystem(
            m_device,
            m_renderer.getSwapChainRenderPass(),
            globalSetLayout->getDescriptorSetLayout()
        );
        PtPointLightSystem pointLightSystem(
            m_device,
            m_renderer.getSwapChainRenderPass(),
            globalSetLayout->getDescriptorSetLayout()
        );
        PtCamera camera{};
        camera.setViewDirection(glm::vec3(0), glm::vec3(0.0, 0.0f, 1.0f));

        auto viewerObject = PtGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;

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
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

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
                    m_gameObjects
                };

                // Update
                GlobalUBO ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                pointLightSystem.update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // Render
                m_renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);
                m_renderer.endSwapChainRenderPass(commandBuffer);
                m_renderer.endFrame();
            }
        }

        vkDeviceWaitIdle(m_device.device());
    }

    void PtApplication::loadGameObjects() {
        {
            std::shared_ptr model1 = PtModel::createModelFromFile(m_device, "../assets/models/flat_vase.obj");

            PtGameObject obj1 = PtGameObject::createGameObject();
            obj1.model = model1;
            obj1.transform.translation = glm::vec3(-0.5, 0.0, 0.0f);
            obj1.transform.scale = glm::vec3(3, 1.5, 3);

            m_gameObjects.emplace(obj1.getId(), std::move(obj1));
        }

        {
            std::shared_ptr model2 = PtModel::createModelFromFile(m_device, "../assets/models/smooth_vase.obj");

            PtGameObject obj2 = PtGameObject::createGameObject();
            obj2.model = model2;
            obj2.transform.translation = glm::vec3(0.5, 0.0, 0.0f);
            obj2.transform.scale = glm::vec3(3, 1.5, 3);

            m_gameObjects.emplace(obj2.getId(), std::move(obj2));
        }

        {
            std::shared_ptr model3 = PtModel::createModelFromFile(m_device, "../assets/models/quad.obj");

            PtGameObject obj3 = PtGameObject::createGameObject();
            obj3.model = model3;
            obj3.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
            obj3.transform.scale = glm::vec3(5);

            m_gameObjects.emplace(obj3.getId(), std::move(obj3));
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
            auto light = PtGameObject::createPointLight(0.2f);
            light.color = lightColors[i];
            auto rotateLight = glm::rotate(
                glm::mat4(1.0f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                {0.0f, -1.0f, 0.0f}
            );

            light.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
            m_gameObjects.emplace(light.getId(), std::move(light));
        }
    }
}
