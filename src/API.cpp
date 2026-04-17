#include <API.h>

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
    CollaboratorType type,
    bool isOwner) :
    m_name(std::move(name)),
    m_accountID(accountID),
    m_icon(icon),
    m_type(type),
    m_isOwner(isOwner) {};

Collaborator Collaborator::create(std::string name, int accountID, CollaboratorIcon icon, CollaboratorType type, bool isOwner) {
    return Collaborator(std::move(name), accountID, std::move(icon), type, isOwner);
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

bool Collaborator::isOwner() const noexcept {
    return m_isOwner;
};

Collaboration::Collaboration(
    int levelID,
    std::vector<Collaborator> collaborators) :
    m_levelID(levelID),
    m_collaborators(std::move(collaborators)) {};

std::shared_ptr<Collaboration> Collaboration::create(int levelID, std::vector<Collaborator> collaborators) {
    if (auto cm = CollaborationManager::get()) {
        log::trace("Checking for pre-existing collaboration for level {}", levelID);
        if (auto c = cm->getCollab(levelID).lock()) return c;

        log::debug("Creating new collaboration for level {} with {} creators", levelID, collaborators.size());

        auto c = std::make_shared<Collaboration>(levelID, std::move(collaborators));
        cm->registerCollab(c);

        return c;
    };

    log::error("State manager not available");
    return nullptr;
};

int Collaboration::getLevelID() const noexcept {
    return m_levelID;
};

std::span<const Collaborator> Collaboration::getCollaborators() const noexcept {
    return m_collaborators;
};

Result<Collaborator> Collaboration::getOwner() const noexcept {
    for (auto const& collab : getCollaborators()) {
        if (collab.isOwner()) return Ok(collab);
    };

    return Err("No owner found");
};

GJGameLevel* Collaboration::getLevel() const {
    if (auto glm = GameLevelManager::sharedState()) return glm->getSavedLevel(m_levelID);
    return nullptr;
};

Result<std::string> Collaboration::getFormattedString() const {
    std::string out;

    auto ownerRes = getOwner();
    if (ownerRes.isErr()) return Err("Failed to get owner");

    auto owner = ownerRes.unwrap();

    if (getCollaborators().size() > 2) {
        out = fmt::format("By {} & {} more", owner.getName(), getCollaborators().size() - 1);
    } else {
        std::string other;

        for (auto const& collab : getCollaborators()) {
            if (!collab.isOwner()) other = collab.getName();
        };

        if (other.empty()) return Ok(fmt::format("By {} & more", owner.getName()));

        out = fmt::format("By {} & {}", owner.getName(), other);
    };

    return Ok(out);
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

void levelcollab::requestCollabForLevel(int levelID, FunctionRef<void(std::weak_ptr<Collaboration>)> callback) {
    callback(std::weak_ptr<Collaboration>());  // dummy impl
};

void levelcollab::getCollaboratorInfo(int accountID, FunctionRef<void(Result<GJUserScore*>)> callback) {
    auto req = web::WebRequest()
                   .bodyString(fmt::format("secret=Wmfd2893gb7&targetAccountID={}", utils::numToString(accountID)))
                   .userAgent("");

    async::spawn(
        req.post("https://www.boomlings.com/database/getGJUserInfo20.php"),
        [callback](web::WebResponse res) {
            auto const resStr = res.string().unwrapOrDefault();

            if (res.error()) {
                log::error("Error getting user information: {}", resStr);
                return callback(Err(""));
            };

            log::debug("Received response: {}", resStr);

            auto dict = CCDictionary::create();
            auto splits = asp::iter::split(resStr, ":")  // owo
                              .mapCast<std::string>()
                              .collect();

            for (size_t i = 0; i + 1 < splits.size(); i += 2) dict->setObject(CCString::create(splits[i + 1]), splits[i]);
            callback(Ok(GJUserScore::create(dict)));
        });
};

CollaborationManager* CollaborationManager::get() noexcept {
    static auto inst = new (std::nothrow) CollaborationManager();
    return inst;
};