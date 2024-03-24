#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string_view>
#include <vector>

#include "ACTrie.hpp"
#include "Observer.hpp"
#include "app_config.hpp"
#include "glad_fix.hpp"

namespace AppSpace {

class Drawer {
public:
    using ModelType =
        ACTrieDS::ACTrie<kMinTrieChar, kMaxTrieChar, kIsCaseInsensetive>;
    using ObserverType =
        Observer<ModelType::SubscribeData, ModelType::SendData>;
    using SubscribeDataType = ObserverType::SubscribeDataType;
    using NotifyDataType = ObserverType::NotifyDataType;
    using UnsubscribeDataType = ObserverType::UnsubscribeDataType;

    Drawer();
    ~Drawer();

    [[nodiscard]] bool ShouldClose() const noexcept;
    void Render();
    ObserverType* GetObserverPort() noexcept;

private:
    void UpdateState();
    void OnError(std::string_view msg) noexcept;

    // std::unique_ptr can't be used here
    // because GLFWwindow is an incomplete type
    GLFWwindow* window_ = nullptr;
    ObserverType port_;

    // Pointer to the nodes of the DS
    std::vector<ModelType::ACTNode>* actrie_nodes_ = nullptr;
    std::vector<ModelType::word_length_t>* actrie_words_lengths_ = nullptr;

    bool suppressor_ = false;
    bool init_failed_ = false;
};

}  // namespace AppSpace
