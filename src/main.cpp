#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "PtApplication.h"

using namespace PalmTree;

int main() {
    PtApplication app = PtApplication();
    try {
        app.Run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
