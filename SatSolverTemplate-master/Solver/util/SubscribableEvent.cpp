//
// Created by tim on 22.03.23.
//

#include "SubscribableEvent.hpp"

namespace sat {

    void SubscriberHandle::unregister() noexcept {
        if (nullptr != alive) {
            *alive = 0;
        }

        alive.reset();
    }

    SubscriberHandle::~SubscriberHandle() {
        unregister();
    }

    SubscriberHandle::SubscriberHandle(Token): alive(std::make_shared<char>(Alive)) {}

    bool SubscriberHandle::isSubscribed() const noexcept {
        return nullptr != alive and *alive == Alive;
    }
}
