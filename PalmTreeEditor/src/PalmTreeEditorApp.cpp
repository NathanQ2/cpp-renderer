#include <PalmTree.h>
#include <PalmTree/Entrypoint.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "ViewportMovementController.h"
#include "PalmTree/Renderer/SceneRenderer3D.h"

using namespace PalmTree;
using namespace PalmTreeEditor;

class EditorLayer : public Layer {
public:
    EditorLayer() :
        Layer("PalmTreeEditor"), m_Window(Application::Get().GetWindow()), m_Ecs(Application::Get().GetEntityComponentSystem()),
        m_Camera(Application::Get().GetCamera()), m_CameraController([]() { return !ImGui::GetIO().WantCaptureMouse; }) {
    }

    void OnStart() override {
        LoadGameObjects();
        
        m_Camera.SetViewDirection(glm::vec3(0), glm::vec3(0.0, 0.0f, 1.0f));

        m_ViewerObject = &m_Ecs.CreateGameObject();
        m_ViewerObject->GetTransform().Translation.z = -2.5f;

        m_Renderer = std::make_unique<SceneRenderer3D>(
            m_Window,
            m_Ecs,
            m_Camera
        );
    }

    void OnEnd() override {}

    void OnUpdate(float dt) override {
        m_DeltaTime = dt;
        m_FrameTimes.push(dt);
        if (m_FrameTimes.size() == 10) {
            float avgFrameTime = 0.0f;
            for (int i = 0; i < 10; i++) {
                avgFrameTime += m_FrameTimes.front();
                m_FrameTimes.pop();
            }
            
            avgFrameTime /= 10;
            
            // seconds/frame -> Frames / seconds
            
            
            m_Fps = 1 / avgFrameTime;
        }
        
        m_CameraController.MoveInPlaneXZ(dt, *m_ViewerObject);
        m_Camera.SetViewYXZ(m_ViewerObject->GetTransform().Translation, m_ViewerObject->GetTransform().EulerAngles());

        float aspect = RendererBackend::GetSwapChainAspectRatio();
        m_Camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

        m_Renderer->Update(dt);
    }

    void OnRender(float dt) override {
        m_Renderer->Render(dt);
    }

    void OnImGuiRender() override {
        ImGui::Begin("Inspector");
        auto& objs = m_Ecs.GetGameObjects();
        
        int i = 0;
        for (auto& obj : objs) {
            glm::vec3& translation = obj.GetTransform().Translation;
            glm::vec3 rotation = obj.GetTransform().EulerAngles();
            glm::vec3& scale = obj.GetTransform().Scale;
            ImGui::PushID(i);
            
            ImGui::Text("ID: %i", obj.GetId());
            ImGui::DragFloat3("Transform", &translation.x, 0.01);
            ImGui::DragFloat3("Rotation", &rotation.x, 0.01);
            ImGui::DragFloat3("Scale", &scale.x, 0.01);
            ImGui::Separator();
            
            ImGui::PopID();
            
            obj.GetTransform().Rotation = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
            
            i++;
        }
        
        ImGui::End();
        
        ImGui::Begin("Performance");
        ImGui::Text("Frame Time: %fms", m_DeltaTime * 1000);
        ImGui::Text("FPS: %f", m_Fps);
        ImGui::End();
        
        ImGui::Begin("Camera Orientation");
        glm::vec3 rot = m_ViewerObject->GetTransform().EulerAngles();
        glm::vec3 deg = degrees(rot);
        ImGui::DragFloat3("Rotation", &deg.x);
        ImGui::End();
    }

    bool OnEvent(Event& event) override {
        return false;
    }
    
    void LoadGameObjects() {
        // Flat Vase
        {
            std::shared_ptr model = Model::CreateModelFromFile("../../PalmTreeEditor/assets/models/flat_vase.obj");

            GameObject& obj = m_Ecs.CreateGameObject();

            obj.AddComponent<ModelComponent>(ModelComponent{glm::vec3(1), model});

            obj.GetTransform().Translation = glm::vec3(-0.5, 0.0, 0.0f);
            obj.GetTransform().Scale = glm::vec3(3, 1.5, 3);
        }

        // Smooth Vase
        {
            std::shared_ptr model = Model::CreateModelFromFile("../../PalmTreeEditor/assets/models/smooth_vase.obj");

            GameObject& obj = m_Ecs.CreateGameObject();
            obj.AddComponent(ModelComponent{glm::vec3(1), model});
            obj.GetTransform().Translation = glm::vec3(0.5, 0.0, 0.0f);
            obj.GetTransform().Scale = glm::vec3(3, 1.5, 3);
        }

        // Floor
        {
            std::shared_ptr model = Model::CreateModelFromFile("../../PalmTreeEditor/assets/models/quad.obj");

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
private:
    Window& m_Window;

    EntityComponentSystem& m_Ecs;
    Camera& m_Camera;

    // Owned by ECS
    GameObject* m_ViewerObject = nullptr;

    ViewportMovementController m_CameraController;

    std::unique_ptr<SceneRenderer3D> m_Renderer;
    
    float m_DeltaTime = 0.0f;
    float m_Fps = 0.0f;
    std::queue<float> m_FrameTimes;
};

class PalmTreeEditorApp : public Application {
public:
    PalmTreeEditorApp() {
        PushLayer<EditorLayer>();
    }
};

Application* PalmTree::CreateApplication() {
    return new PalmTreeEditorApp();
}
