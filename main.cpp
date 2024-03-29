#include "App.hpp"
#include "React.hpp"

int main() {
    try {
        AppSpace::App app;
        app.Run();
    } catch (...) {
        AppSpace::Except::React();
    }
    return 0;
}
