#pragma once

#include <cassert>
#include <functional>
#include <list>
#include <optional>

#include "SendTypeHelper.hpp"

namespace AppSpace {

template <class TData, class SendTDataBy = SendTypeHelper<TData>>
class Observable;

template <class TData, class SendTDataBy = SendTypeHelper<TData>>
class Observer final {
    friend class Observable<TData, SendTDataBy>;
    using ObservableType = Observable<TData, SendTDataBy>;

public:
    using CallSignature = void(SendTDataBy);

    template <class TOnNotify>
    Observer(TOnNotify&& on_notify) noexcept
        : on_notify_(std::forward<TOnNotify>(on_notify)) {}
    Observer(const Observer&)            = delete;
    Observer& operator=(const Observer&) = delete;
    Observer(Observer&&)                 = delete;
    Observer& operator=(Observer&&)      = delete;
    ~Observer() {
        Unsubscribe();
    }
    void Unsubscribe();

private:
    constexpr bool Subscribed() noexcept {
        return observable_ != nullptr;
    }
    void SetObservable(ObservableType* observable) noexcept {
        assert(observable);
        observable_ = observable;
    }

    ObservableType* observable_ = nullptr;
    std::function<void(SendTDataBy)> on_notify_;
};

/// @brief Simple Observable with 1 to 1 supported connection.
/// @tparam TData
/// @tparam SendTDataBy
template <class TData, class SendTDataBy>
class Observable final {
    friend class Observer<TData, SendTDataBy>;

public:
    using ObserverType = Observer<TData, SendTDataBy>;

    constexpr Observable() noexcept = default;
    ~Observable() {
        UnsubscribeObserver();
    }
    Observable(const Observable&)            = delete;
    Observable& operator=(const Observable&) = delete;
    Observable(Observable&&)                 = delete;
    Observable& operator=(Observable&&)      = delete;

    void Subscribe(ObserverType* observer) {
        assert(observer);
        assert(listener_ == nullptr);
        if (observer->Subscribed()) {
            observer->Unsubscribe();
        }
        listener_ = observer;
        observer->SetObservable(this);
    }
    void UnsubscribeObserver() {
        if (listener_ != nullptr) {
            listener_->Unsubscribe();
            listener_ = nullptr;
        }
    }
    template <class UniTData = TData>
    void Notify(UniTData&& data) {
        if (listener_ != nullptr) {
            listener_->on_notify_(std::forward<UniTData>(data));
        }
    }

private:
    void Detach_(ObserverType* observer) {
        assert(observer);
        if (observer == listener_) {
            listener_ = nullptr;
        }
    }

    ObserverType* listener_ = nullptr;
};

template <class TData, class SendTDataBy>
void Observer<TData, SendTDataBy>::Unsubscribe() {
    if (!Subscribed()) {
        return;
    }

    observable_->Detach_(this);
    observable_ = nullptr;
}

}  // namespace AppSpace
