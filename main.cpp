#include "App.hpp"

int main() {
    try {
        AppSpace::App app;
        app.Run();
    } catch (...) {
        // TODO: add exceptions handling
    }
    return 0;
}
