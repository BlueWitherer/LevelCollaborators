#pragma once

#include <API.h>

#include <Geode/Geode.hpp>

namespace levelcollab {
    class CollabPopup final : public geode::Popup {
    protected:
        bool init(GJGameLevel* level, std::weak_ptr<Collaboration> const& collab);

    public:
        static CollabPopup* create(GJGameLevel* level, std::weak_ptr<Collaboration> const& collab);
    };
};