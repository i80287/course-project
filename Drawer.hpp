#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string_view>
#include <vector>

#include "ACTrie.hpp"
#include "GLFWwindowManager.hpp"
#include "Observer.hpp"

namespace AppSpace {

class Drawer {
public:
    using FoundSubstringData = ACTrieDS::ACTrie::FoundSubstringSendData;
    using BadInputStringData = ACTrieDS::ACTrie::BadInputSendData;
    using FoundSubstringObserver = Observer<FoundSubstringData>;
    using BadInputObserver = Observer<BadInputStringData>;

    Drawer();
    ~Drawer();

    [[nodiscard]] bool ShouldClose() const noexcept;
    void Render();
    FoundSubstringObserver* GetFoundStringsObserverPort() noexcept;
    BadInputObserver* GetBadInputObserverPort() noexcept;

private:
    void UpdateState();

    GLFWwindowManager window_;
    FoundSubstringObserver found_strings_port_;
    BadInputObserver bad_input_port_;
    bool suppressor_ = false;
};

}  // namespace AppSpace
