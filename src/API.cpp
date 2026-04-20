#include <API.h>

#include <Utils.h>

#include <asp/iter.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace levelcollab;

CollaboratorIcon::CollaboratorIcon(
    int icon,
    IconType type,
    int color1,
    int color2,
    int glow,
    bool useGlow) :
    m_icon(icon),
    m_type(type),
    m_color1(color1),
    m_color2(color2),
    m_glow(glow),
    m_useGlow(useGlow) {};

CollaboratorIcon CollaboratorIcon::create(int icon, IconType type, int color1, int color2, int glow, bool useGlow) {
    return CollaboratorIcon(icon, type, color1, color2, glow, useGlow);
};

int CollaboratorIcon::getIcon() const noexcept {
    return m_icon;
};

IconType CollaboratorIcon::getIconType() const noexcept {
    return m_type;
};

int CollaboratorIcon::getColor1() const noexcept {
    return m_color1;
};

int CollaboratorIcon::getColor2() const noexcept {
    return m_color2;
};

int CollaboratorIcon::getGlow() const noexcept {
    return m_glow;
};

bool CollaboratorIcon::isUseGlow() const noexcept {
    return m_useGlow;
};

SimplePlayer* CollaboratorIcon::createIcon() const {
    // me when gamemanager doesnt exist owo (/j (you're gonna see this a lot just ignore it))
    if (auto gm = GameManager::sharedState()) {
        auto player = SimplePlayer::create(getIcon());
        player->setColor(gm->colorForIdx(getColor1()));
        player->setSecondColor(gm->colorForIdx(getColor2()));

        if (isUseGlow()) player->setGlowOutline(gm->colorForIdx(getGlow()));
        if (!isUseGlow()) player->disableGlowOutline();

        return player;
    };

    return nullptr;
};

Collaborator::Collaborator(
    std::string name,
    int accountID,
    CollaboratorIcon icon,
    CollaboratorType type) :
    m_name(std::move(name)),
    m_accountID(accountID),
    m_icon(icon),
    m_type(type) {};

Collaborator Collaborator::create(std::string name, int accountID, CollaboratorIcon icon, CollaboratorType type) {
    return Collaborator(std::move(name), accountID, std::move(icon), type);
};

ZStringView Collaborator::getName() const noexcept {
    return m_name;
};

int Collaborator::getAccountID() const noexcept {
    return m_accountID;
};

CollaboratorIcon const& Collaborator::getIcon() const noexcept {
    return m_icon;
};

CollaboratorType Collaborator::getType() const noexcept {
    return m_type;
};

void Collaborator::reloadInfo(geode::CopyableFunction<void(geode::Result<Collaborator>)>&& callback) {
    getCollaboratorInfo(getAccountID(), [cb = std::move(callback), this](geode::Result<GJUserScore*> res) {
        if (res.isErr()) {
            log::error("Failed to reload collaborator info: {}", res.unwrapErr());
            return cb(Err(res.unwrapErr()));
        };

        auto userInfo = res.unwrapOrDefault();

        m_name = userInfo->m_userName;
        m_accountID = userInfo->m_accountID;
        m_icon = CollaboratorIcon::create(userInfo->m_iconID, userInfo->m_iconType, userInfo->m_color1, userInfo->m_color2, userInfo->m_color3, userInfo->m_glowEnabled);

        cb(Ok(*this));
    });
};

Collaboration::Collaboration(
    int levelID,
    Collaborator owner,
    std::vector<Collaborator> collaborators) :
    m_levelID(levelID),
    m_owner(std::move(owner)),
    m_collaborators(std::move(collaborators)) {};

std::shared_ptr<Collaboration> Collaboration::create(int levelID, Collaborator owner, std::vector<Collaborator> collaborators) {
    if (auto cm = CollaborationManager::get()) {
        log::trace("Checking for pre-existing collaboration for level {}", levelID);
        if (auto c = cm->getCollab(levelID).lock()) return c;

        log::debug("Creating new collaboration for level {} with {} creators", levelID, collaborators.size());

        auto c = std::make_shared<Collaboration>(levelID, std::move(owner), std::move(collaborators));
        cm->registerCollab(c);

        return c;
    };

    log::error("State manager not available");
    return nullptr;
};

int Collaboration::getLevelID() const noexcept {
    return m_levelID;
};

Collaborator const& Collaboration::getOwner() const noexcept {
    return m_owner;
};

std::span<const Collaborator> Collaboration::getCollaborators() const noexcept {
    return m_collaborators;
};

GJGameLevel* Collaboration::getLevel() const {
    if (auto glm = GameLevelManager::sharedState()) return glm->getSavedLevel(m_levelID);
    return nullptr;
};

std::string Collaboration::getFormattedString(bool prefix) const {
    std::string out;

    auto owner = getOwner();

    if (getCollaborators().size() > 2) {
        out = fmt::format("{}{} & {} more", prefix ? "By " : "", owner.getName(), getCollaborators().size() - 1);
    } else if (getCollaborators().size() >= 1) {
        std::string other = getCollaborators()[0].getName();

        if (other.empty()) return fmt::format("{}{} & more", prefix ? "By " : "", owner.getName());

        out = fmt::format("{}{} & {}", prefix ? "By " : "", owner.getName(), other);
    } else {
        return fmt::format("{}{}", prefix ? "By " : "", owner.getName());
    };

    return out;
};

void CollaborationManager::registerCollab(std::shared_ptr<Collaboration> collab) {
    m_collaborations[collab->getLevelID()] = collab;
};

std::weak_ptr<Collaboration> CollaborationManager::getCollab(int levelID) const noexcept {
    if (auto it = m_collaborations.find(levelID); it != m_collaborations.end()) return it->second;
    return std::weak_ptr<Collaboration>();
};

std::vector<std::weak_ptr<Collaboration>> CollaborationManager::getCollabs() const noexcept {
    std::vector<std::weak_ptr<Collaboration>> out;
    for (auto collab : m_collaborations) out.push_back(collab.second);
    return out;
};

std::weak_ptr<Collaboration> CollaborationManager::getCollabForLevel(int levelID) const noexcept {
    return getCollab(levelID);
};

void levelcollab::requestCollabForLevel(int levelID, CopyableFunction<void(std::weak_ptr<Collaboration>)>&& callback) {
    callback(std::weak_ptr<Collaboration>());  // dummy impl
};

void levelcollab::getCollaboratorInfo(int accountID, CopyableFunction<void(Result<GJUserScore*>)>&& callback) {
    auto req = web::WebRequest()
                   .bodyString(fmt::format("secret=Wmfd2893gb7&targetAccountID={}", utils::numToString(accountID)))
                   .userAgent("");

    async::spawn(
        req.post("https://www.boomlings.com/database/getGJUserInfo20.php"),
        [cb = std::move(callback)](web::WebResponse res) {  // copyable has const () operator woohoo!
            auto const resStr = res.string().unwrapOrDefault();

            if (res.error()) {
                log::error("Error getting user information: {}", resStr);
                return cb(Err("An error occurred while fetching user information"));
            };

            log::debug("Received response: {}", resStr);

            auto dict = CCDictionary::create();
            auto splits = asp::iter::split(resStr, ":")  // owo
                              .mapCast<std::string>()
                              .collect();

            for (size_t i = 0; i + 1 < splits.size(); i += 2) dict->setObject(CCString::create(splits[i + 1]), splits[i]);
            cb(Ok(GJUserScore::create(dict)));
        });
};

CollaborationManager* CollaborationManager::get() noexcept {
    static auto inst = new (std::nothrow) CollaborationManager();
    return inst;
};

void AuthState::setAuthInfo(int accountID, int userID, std::string username, std::string token) {
    m_accountID = accountID;
    m_userID = userID;
    m_username = std::move(username);
    m_token = std::move(token);
};

int AuthState::getAccountID() const noexcept {
    return m_accountID;
};

int AuthState::getUserID() const noexcept {
    return m_userID;
};

ZStringView AuthState::getUsername() const noexcept {
    return m_username;
};

ZStringView AuthState::getToken() const noexcept {
    return m_token;
};

AuthState* AuthState::get() noexcept {
    static auto inst = new (std::nothrow) AuthState();
    return inst;
};

void fetch::getUserByUsername(std::string_view username, CopyableFunction<void(Result<GJUserScore*>)>&& callback) {
    auto req = web::WebRequest()
                   .bodyString(fmt::format("secret=Wmfd2893gb7&str={}", username))
                   .userAgent("");

    async::spawn(
        req.post("https://www.boomlings.com/database/getGJUsers20.php"),
        [cb = std::move(callback)](web::WebResponse res) {
            auto const resStr = res.string().unwrapOrDefault();

            if (res.error()) {
                log::error("Error getting user information: {}", resStr);
                return cb(Err("An error occurred while fetching user information"));
            };

            log::debug("Received response: {}", resStr);

            auto dict = CCDictionary::create();
            auto splits = asp::iter::split(resStr, ":")
                              .mapCast<std::string>()
                              .collect();

            for (size_t i = 0; i + 1 < splits.size(); i += 2) dict->setObject(CCString::create(splits[i + 1]), splits[i]);
            cb(Ok(GJUserScore::create(dict)));
        });
};