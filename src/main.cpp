#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "Application.h"

using namespace PalmTree;

int main() {
    Application app = Application();
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
