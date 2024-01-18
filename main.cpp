#include <cstdlib>

#include "App.hpp"

int main() {
    AppSpace::App app;
    if (app.InitFailed()) {
        return EXIT_FAILURE;
    }

    try {
        app.Run();
    } catch (...) {
        // TODO: add exceptions handling
    }
    return EXIT_SUCCESS;
}
