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
class Observer {
    friend class Observable<TData>;
    using Observable = Observable<TData>;    

public:
    using SendTDataBy   = SendTypeHelper<TData>;
    using CallSignature = void(SendTDataBy);

    template <class TOnSubscribe, class TOnNotify, class TOnUnsbscribe>
    Observer(TOnSubscribe&& on_subscribe, TOnNotify&& on_notify,
             TOnUnsbscribe&& on_unsubscribe) noexcept
        : on_subscribe_(std::forward<TOnSubscribe>(on_subscribe)),
          on_notify_(std::forward<TOnNotify>(on_notify)),
          on_unsubscribe_(std::forward<TOnUnsbscribe>(on_unsubscribe)) {}

    Observer(const Observer&)            = delete;
    Observer& operator=(const Observer&) = delete;
    Observer(Observer&&)                 = delete;
    Observer& operator=(Observer&&)      = delete;

    void Unsubscribe();

    ~Observer() {
        Unsubscribe();
    }

private:
    constexpr bool Subscribed() noexcept {
        return observable_ != nullptr;
    }

    void SetObservable(Observable* observable) noexcept {
        assert(observable);
        observable_ = observable;
    }

    Observable* observable_ = nullptr;
    std::function<void(SendTDataBy)> on_subscribe_;
    std::function<void(SendTDataBy)> on_notify_;
    std::function<void(SendTDataBy)> on_unsubscribe_;
};

template <class TData, class SendTDataBy>
class Observable {
    friend class Observer<TData>;
public:
    using Observer    = Observer<TData>;

    constexpr Observable() noexcept(std::is_nothrow_constructible_v<decltype(data_)> && std::is_nothrow_constructible_v<decltype(listeners_)>) = default;

    template <class UniTData = TData>
    explicit Observable(UniTData&& data) noexcept(
        std::is_nothrow_constructible_v<TData, UniTData>)
        : data_(std::forward<UniTData>(data)) {}

    Observable(const Observable&)            = delete;
    Observable& operator=(const Observable&) = delete;
    Observable(Observable&&)                 = delete;
    Observable& operator=(Observable&&)      = delete;

    void Subscribe(Observer* observer) {
        assert(observer);
        if (observer->Subscribed()) {
            observer->Unsubscribe();
        }
        listeners_.push_back(observer);
        observer->SetObservable(this);
        if (data_.has_value())
            observer->on_subscribe_(*data_);
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
        SendTDataBy data = *data_;
        for (Observer* observer : listeners_) {
            assert(observer);
            observer->on_notify_(data);
        }
    }

private:
    void Detach_(Observer* observer) {
        assert(observer);
        if (data_.has_value())
            observer->on_unsubscribe_(*data_);
        listeners_.remove(observer);
    }

    std::optional<TData> data_;
    std::list<Observer*> listeners_;
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