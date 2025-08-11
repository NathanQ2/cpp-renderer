#pragma once

#include "PtModel.h"

#include <memory>

namespace PalmTree {
    struct Transform2dComponent {
        glm::vec2 translation{};
        glm::vec2 scale{1.0f, 1.0f};
        float rotation;

        glm::mat2 getMat() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotationMat{{c, s}, {-s, c}};
            
            glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} };
            
            return rotationMat * scaleMat;
        }
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
        Transform2dComponent transform{};

    private:
        PtGameObject(id_t objectId) : m_Id(objectId) {}
        
        id_t m_Id;
    };
}
