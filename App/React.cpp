#include <iostream>
#include <stdexcept>

namespace AppSpace::Except {

void React() noexcept {
    try {
        throw;
    } catch (const std::runtime_error& re) {
        std::cerr << "Runtime error: " << re.what() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
    } catch (...) {
        std::cerr << "Unknown exception\n";
    }
}

}  // namespace AppSpace::Except
