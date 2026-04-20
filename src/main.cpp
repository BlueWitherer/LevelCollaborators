#include <API.h>

#include <Utils.h>

#include <ui/CollabPopup.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/Button.hpp>

#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
using namespace levelcollab;

// just for testing
static auto const aw = Collaborator::create("ArcticWoof", 7689052, CollaboratorIcon::create(28, IconType::Cube, 94, 98, 1, true), CollaboratorType::Decoration);
static auto const cw = Collaborator::create("Cheeseworks", 6408873, CollaboratorIcon::create(290, IconType::Cube, 83, 12, 3, true), CollaboratorType::Gameplay);

static auto const collaboration = Collaboration::create(104663075, aw, {cw});

$on_game(Loaded) {
    async::spawn(
        argon::startAuth(),
        [](Result<std::string> result) {
            result.isErr() ? log::error("Error getting Argon token: {}", result.unwrapErr()) : log::info("Received Argon token!");

            if (auto gjam = GJAccountManager::sharedState()) {
                if (auto as = AuthState::get()) as->setAuthInfo(gjam->m_accountID, gjam->m_uID, gjam->m_username, result.unwrapOrDefault());
                if (result.isOk()) Notification::create("Authorized with Argon", NotificationIcon::Success)->show();
            };
        });
};

// class $modify(LCLevelCell, LevelCell) {
//     void loadFromLevel(GJGameLevel* level) {
//         LevelCell::loadFromLevel(level);
//     };
// };

class $modify(LCLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (auto cm = CollaborationManager::get()) {
            if (auto const collab = cm->getCollabForLevel(m_level->m_levelID).lock()) {
                auto creatorMenu = getChildByID("creator-info-menu");

                auto btnLabel = CCLabelBMFont::create(collab->getFormattedString().c_str(), "goldFont.fnt");
                btnLabel->limitLabelWidth(37.5f, 0.75f, 0.75f);

                auto btn = Button::createWithNode(
                    btnLabel,
                    [this, collab](auto) {
                        CollabPopup::create(m_level, collab)->show();
                    });
                btn->setID("collaboration-btn"_spr);
                btn->setScale(0.75f);
                btn->setPosition({creatorMenu->getPositionX(), creatorMenu->getPositionY() + 11.25f});

                if (auto copyTag = getChildByID("copy-indicator")) {
                    copyTag->setPositionX((getScaledContentWidth() / 2.f) + (btn->getScaledContentWidth() / 2.f) + 5.f);
                    if (auto moreTags = getChildByID("raydeeux_thesillydoggo.evenmoreleveltags/more-level-tags-menu")) moreTags->setPositionX(copyTag->getPositionX() + 17.5f);
                };

                creatorMenu->setVisible(false);
                addChild(btn);
            };
        };

        return true;
    };
};