#include "../CollabPopup.h"

#include <API.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace levelcollab;

bool CollabPopup::init(GJGameLevel* level, std::weak_ptr<Collaboration> const& collab) {
    if (!Popup::init({350.f, 225.f})) return false;

    setID("collaboration"_spr);
    setTitle("Collaborators");

    addSideArt(m_mainLayer, SideArt::All, SideArtStyle::PopupGold);

    auto forLabel = CCLabelBMFont::create(fmt::format("for {}", level->m_levelName).c_str(), "bigFont.fnt");
    forLabel->setID("for-level-label");
    forLabel->setScale(m_title->getScale() * 0.5f);
    forLabel->ignoreAnchorPointForPosition(false);
    forLabel->setAnchorPoint(m_title->getAnchorPoint());
    forLabel->setPosition({m_title->getPositionX(), m_title->getPositionY() - 15.f});

    m_mainLayer->addChild(forLabel, 9);

    return true;
};

CollabPopup* CollabPopup::create(GJGameLevel* level, std::weak_ptr<Collaboration> const& collab) {
    auto ret = new CollabPopup();
    if (ret->init(level, collab)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};