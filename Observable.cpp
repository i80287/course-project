#include "Observable.hpp"

#include <cassert>

#include "ACTrie.hpp"

namespace AppSpace {

template <class SubscribeData, class SendData>
void Observable<SubscribeData, SendData>::Subscribe(ObserverType* observer,
                                                    SubscribeDataType data) {
    assert(observer);
    assert(!observer_);
    observer_ = observer;
    observer->SetObservable(this);
    observer->OnSubscribe(data);
}

using ACTrie = ACTrieDS::ACTrie<kMinTrieChar, kMaxTrieChar, kIsCaseInsensetive>;
template class Observable<ACTrie::SubscribeData, ACTrie::SendData>;

}  // namespace AppSpace
