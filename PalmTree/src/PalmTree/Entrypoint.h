#pragma once

#include <iostream>

#include "Core.h"
#include "Application.h"

extern PalmTree::Application* PalmTree::createApplication();

int main() {
    PalmTree::Log::init();
    
    PalmTree::Application* app = PalmTree::createApplication();
    
    try {
        app->run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
