#include "App/App.hpp"
#include "App/React.hpp"

int main() {
    try {
        AppSpace::App().Run();
    } catch (...) {
        AppSpace::Except::React();
    }
    return 0;
}
