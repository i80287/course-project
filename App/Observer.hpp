#pragma once

#include <cassert>
#include <functional>
#include <list>
#include <optional>

#include "SendTypeHelper.hpp"

namespace AppSpace {

template <class TData, class SendTDataBy = SendTypeHelper<TData>>
class Observable;

template <class TData>
class Observer final {
    friend class Observable<TData>;
    using ObservableType = Observable<TData>;

public:
    using SendTDataBy   = SendTypeHelper<TData>;
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

template <class TData, class SendTDataBy>
class Observable final {
    friend class Observer<TData>;

public:
    using ObserverType = Observer<TData>;

    constexpr Observable() noexcept(
        std::is_nothrow_constructible_v<decltype(data_)> &&
        std::is_nothrow_constructible_v<decltype(listeners_)>) = default;
    template <class UniTData = TData>
    explicit Observable(UniTData&& data) noexcept(
        std::is_nothrow_constructible_v<TData, UniTData>)
        : data_(std::forward<UniTData>(data)) {}
    ~Observable() {
        UnsubscribeAll();
    }
    Observable(const Observable&)            = delete;
    Observable& operator=(const Observable&) = delete;
    Observable(Observable&&)                 = delete;
    Observable& operator=(Observable&&)      = delete;

    void Subscribe(ObserverType* observer) {
        assert(observer);
        if (observer->Subscribed()) {
            observer->Unsubscribe();
        }
        listeners_.push_back(observer);
        observer->SetObservable(this);
        if (data_.has_value())
            observer->on_notify_(*data_);
    }
    void UnsubscribeAll() {
        for (size_t iter = 0, max_iters = listeners_.size();
             !listeners_.empty() && iter < max_iters; iter++) {
            listeners_.front()->Unsubscribe();
        }
        assert(listeners_.empty());
    }
    void ResetData() noexcept {
        data_.reset();
    }
    template <class UniTData = TData>
    void SetDataAndNotify(UniTData&& data) {
        data_ = std::forward<UniTData>(data);
        Notify();
    }
    void Notify() const {
        if (!data_.has_value()) {
            return;
        }
        for (ObserverType* observer : listeners_) {
            assert(observer);
            observer->on_notify_(*data_);
        }
    }

private:
    void Detach_(ObserverType* observer) {
        assert(observer);
        if (data_.has_value())
            observer->on_notify_(*data_);
        listeners_.remove(observer);
    }

    std::optional<TData> data_;
    std::list<ObserverType*> listeners_;
};

template <class TData>
void Observer<TData>::Unsubscribe() {
    if (!Subscribed()) {
        return;
    }

    observable_->Detach_(this);
    observable_ = nullptr;
}

}  // namespace AppSpace
