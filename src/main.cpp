#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

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