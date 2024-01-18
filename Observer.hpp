#pragma once

#include <cassert>
#include <functional>
#include <utility>

#include "SendTypeHelper.hpp"

namespace AppSpace {

template <class SubscribeData, class SendData>
class Observable;

template <class SubscribeData, class SendData>
class Observer {
    friend class Observable<SubscribeData, SendData>;
    using ObservableType = Observable<SubscribeData, SendData>;

public:
    using SubscribeDataType = SendTypeHelper<SubscribeData>;
    using SubscribeCallSignature = void(SubscribeDataType);

    using NotifyDataType = SendTypeHelper<SubscribeData>;
    using NotifyCallSignature = void(NotifyDataType);

    using UnsubscribeDataType = NotifyDataType;
    using UnsubscribeCallSignature = NotifyCallSignature;

    template <class TOnSubscribe, class TOnNotify, class TOnUnsbscribe>
    Observer(TOnSubscribe&& on_subscribe, TOnNotify on_notify,
             TOnUnsbscribe&& on_unsubscribe) noexcept
        : on_subscribe_(std::move(on_subscribe)),
          on_notify_(std::move(on_notify)),
          on_unsubscribe_(std::move(on_unsubscribe)) {}

    void OnSubscribe(SubscribeDataType data);

    void OnNotify(NotifyDataType data);

    void OnUnsubscribe(UnsubscribeDataType data);

    void Unsubscribe();

private:
    void SetObservable(ObservableType* observable) noexcept;

    ObservableType* observable_ = nullptr;
    std::function<SubscribeCallSignature> on_subscribe_;
    std::function<NotifyCallSignature> on_notify_;
    std::function<UnsubscribeCallSignature> on_unsubscribe_;
};

template <class SubscribeData, class SendData>
void Observer<SubscribeData, SendData>::OnSubscribe(SubscribeDataType data) {
    on_subscribe_(data);
}

template <class SubscribeData, class SendData>
void Observer<SubscribeData, SendData>::OnNotify(NotifyDataType data) {
    on_notify_(data);
}

template <class SubscribeData, class SendData>
void Observer<SubscribeData, SendData>::OnUnsubscribe(
    UnsubscribeDataType data) {
    on_unsubscribe_(data);
}

template <class SubscribeData, class SendData>
void Observer<SubscribeData, SendData>::SetObservable(
    ObservableType* observable) noexcept {
    assert(observable);
    observable_ = observable;
}

}  // namespace AppSpace
