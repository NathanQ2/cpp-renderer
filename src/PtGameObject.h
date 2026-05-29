#pragma once

#include "PtModel.h"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

namespace PalmTree {
    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation;

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };
    
    struct PointLightComponent {
        float lightIntensity = 1.0f;
    };
    
    class PtGameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, PtGameObject>;
        
        PtGameObject(const PtGameObject&) = delete;
        PtGameObject& operator=(const PtGameObject&) = delete;
        PtGameObject(PtGameObject&&) = default;
        PtGameObject& operator=(PtGameObject&&) = default;
        

        static PtGameObject CreateGameObject() {
            static id_t currentId = 0;
            
            return PtGameObject(currentId++);
        }
        
        static PtGameObject CreatePointLight(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

        id_t getId() { return m_Id; }

        glm::vec3 color{};
        TransformComponent transform{};
        
        std::shared_ptr<PtModel> model{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

    private:
        PtGameObject(id_t objectId) : m_Id(objectId) {}
        
        id_t m_Id;
    };
}
