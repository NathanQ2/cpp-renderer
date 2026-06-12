#pragma once

#include <iostream>

#include "Core.h"
#include "Application.h"

extern PalmTree::Application* PalmTree::CreateApplication();

int main() {
    PalmTree::Log::Init();

    PalmTree::Application* app = PalmTree::CreateApplication();

    try {
        app->Run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
