#pragma once

#include "Observer.hpp"

namespace AppSpace {

/// @brief Observable with single connection
/// @tparam DataType
template <class SubscribeData, class SendData>
class Observable {
public:
    /// @brief Here we use direct pointer instead of list/vector. In general
    /// Observable should have container with Observers
    using ObserverType = Observer<SubscribeData, SendData>;

    using SubscribeDataType = ObserverType::SubscribeDataType;
    using SubscribeCallSignature = ObserverType::SubscribeCallSignature;

    void Subscribe(ObserverType* observer, SubscribeDataType data);

private:
    ObserverType* observer_ = nullptr;
};

}  // namespace AppSpace
