#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "Application.h"

int main() {
    PalmTree::Application app = PalmTree::Application();
    try {
        app.Run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
