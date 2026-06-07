#include <PalmTree.h>

class Sandbox : public PalmTree::Application {
public:
    Sandbox() = default;
};

PalmTree::Application* PalmTree::createApplication() {
    return new Sandbox();
}
