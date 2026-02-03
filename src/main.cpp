#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

$on_game(Loaded) {
    async::spawn(
        argon::startAuth(),
        [](Result<std::string> result) {
            result.isErr() ? log::error("Error getting Argon token: {}", result.unwrapErr()) : log::info("Received Argon token!");
            if (auto m = Mod::get()) m->setSavedValue("authtoken", std::move(result).unwrapOrDefault());
            if (result.isOk()) Notification::create("Authorized with Argon", NotificationIcon::Success)->show();
        }
    );
};

class $modify(LCMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (auto menu = getChildByID("bottom-menu")) {
            auto myBtn = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
                this,
                menu_selector(LCMenuLayer::onMyBtn)
            );
            myBtn->setID("my-button"_spr);

            menu->addChild(myBtn);
            menu->updateLayout();
        };

        return true;
    };

    void onMyBtn(CCObject*) {
        FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
    };
};

class $modify(LCLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel * level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (auto menu = getChildByID("left-side-menu")) {
            auto collabBtn = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
                this,
                menu_selector(LCLevelInfoLayer::onCollaborators)
            );
            collabBtn->setID("collaborations-button"_spr);

            menu->addChild(collabBtn);
            menu->updateLayout();
        };

        return true;
    };

    void onCollaborators(CCObject*) {
        FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
    };
};