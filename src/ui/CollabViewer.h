#pragma once

#include <Geode/Geode.hpp>

namespace levelcollab {
    class CollabViewer final : public geode::Popup {
    protected:
        bool init(GJGameLevel* level);

    public:
        static CollabViewer* create(GJGameLevel* level);
    };
};