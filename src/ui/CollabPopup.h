#pragma once

#include <API.h>

#include <Geode/Geode.hpp>

namespace levelcollab {
    class CollabPopup final : public geode::Popup {
    protected:
        bool init(std::weak_ptr<Collaboration> collab);

    public:
        static CollabPopup* create(std::weak_ptr<Collaboration> collab);
    };
};