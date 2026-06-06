#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "PtApplication.h"

using namespace PalmTree;

int main() {
    PtApplication app = PtApplication();
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";

        return EXIT_FAILURE;
    }

    // ECS::EntityComponentSystem ecs{};
    // 
    // ecs.registerComponent<ECS::TransformComponent>();
    // // ecs.registerComponent<ECS::ModelComponent>();
    // // ecs.registerComponent<ECS::PointLightComponent>();
    // auto transform1 = ECS::TransformComponent{glm::vec3(2), glm::vec3(1), glm::vec3(0)};
    // ECS::GameObject& obj = ecs.createGameObject(transform1);
    // // ecs.addComponent<ECS::PointLightComponent>(obj, ECS::PointLightComponent {5.0f});
    // 
    // ECS::TransformComponent& transform = ecs.getComponent<ECS::TransformComponent>(obj);
    // 
    // std::println("Transform.Translation: ({}, {}, {})", transform.translation.x, transform.translation.y, transform.translation.z);
    // std::println("Transform.Scale: ({}, {}, {})", transform.scale.x, transform.scale.y, transform.scale.z);

    // using namespace ECS;
    // 
    // ComponentManager cm{};
    // cm.registerComponent<TransformComponent>();
    // cm.registerComponent<PointLightComponent>();
    // 
    // cm.addComponent<TransformComponent>(0, TransformComponent{ glm::vec3(5), glm::vec3(1), glm::vec3(0)});
    // 
    // TransformComponent& transform = cm.getComponent<TransformComponent>(0);
    // std::println("Transform.Translation: ({}, {}, {})", transform.translation.x, transform.translation.y, transform.translation.z);

    return EXIT_SUCCESS;
}
