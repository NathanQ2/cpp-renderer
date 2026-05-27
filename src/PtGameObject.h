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

        id_t getId() { return m_Id; }

        std::shared_ptr<PtModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        PtGameObject(id_t objectId) : m_Id(objectId) {}
        
        id_t m_Id;
    };
}
