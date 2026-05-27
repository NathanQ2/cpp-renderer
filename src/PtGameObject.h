#pragma once

#include "PtModel.h"

#include<glm/gtc/matrix_transform.hpp>
#include <memory>

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
        PtGameObject(const PtGameObject&) = delete;
        PtGameObject& operator=(const PtGameObject&) = delete;
        PtGameObject(PtGameObject&&) = default;
        PtGameObject& operator=(PtGameObject&&) = default;
        
        using id_t = unsigned int;

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
