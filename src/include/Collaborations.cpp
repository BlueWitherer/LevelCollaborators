#include <Collaborations.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace levelcollab;

bool CollaboratorIcon::init(
    int icon,
    IconType type,
    int color1,
    int color2,
    int glow,
    bool useGlow
) {
    m_icon = icon;
    m_type = type;
    m_color1 = color1;
    m_color2 = color2;
    m_glow = glow;
    m_useGlow = useGlow;

    return true;
};

int CollaboratorIcon::getIcon() const noexcept {
    return m_icon;
};

IconType CollaboratorIcon::getType() const noexcept {
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

bool CollaboratorIcon::shouldUseGlow() const noexcept {
    return m_useGlow;
};

SimplePlayer* CollaboratorIcon::createIcon() const {
    if (auto gm = GameManager::sharedState()) {
        auto player = SimplePlayer::create(m_icon);
        player->setColor(gm->colorForIdx(m_color1));
        player->setSecondColor(gm->colorForIdx(m_color2));

        if (m_useGlow) player->setGlowOutline(gm->colorForIdx(m_glow));
        if (!m_useGlow) player->disableGlowOutline();

        return player;
    };

    return nullptr;
};

CollaboratorIcon* CollaboratorIcon::create(
    int icon,
    IconType type,
    int color1,
    int color2,
    int glow,
    bool useGlow
) {
    auto ret = new CollaboratorIcon();
    if (ret && ret->init(icon, type, color1, color2, glow, useGlow)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

bool Collaborator::init(
    std::string name,
    int userID,
    bool owner
) {
    m_name = std::move(name);
    m_userID = userID;
    m_owner = owner;

    return true;
};

std::string_view Collaborator::getName() const noexcept {
    return m_name;
};

int Collaborator::getUserID() const noexcept {
    return m_userID;
};

bool Collaborator::isOwner() const noexcept {
    return m_owner;
};

Collaborator* Collaborator::create(
    std::string name,
    int userID,
    bool owner
) {
    auto ret = new Collaborator();
    if (ret->init(std::move(name), userID, owner)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

bool Collaboration::init(
    int levelID,
    std::vector<Collaborator*> collaborators
) {
    m_levelID = levelID;
    m_collaborators = std::move(collaborators);

    return true;
};

int Collaboration::getLevelID() const noexcept {
    return m_levelID;
};

GJGameLevel* Collaboration::getLevel() const {
    if (auto glm = GameLevelManager::sharedState()) return glm->getSavedLevel(m_levelID);
    return nullptr;
};

Collaborator* Collaboration::getCollaboratorByID(int userID) const noexcept {
    for (auto const& collab : m_collaborators) {
        if (collab->getUserID() == userID) return collab;
    };

    return nullptr;
};

std::vector<Collaborator*> const& Collaboration::getCollaborators() const noexcept {
    return m_collaborators;
};

Collaboration* Collaboration::create(
    int levelID,
    std::vector<Collaborator*> collaborators
) {
    auto ret = new Collaboration();
    if (ret->init(levelID, std::move(collaborators))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

class CollaborationManager::Impl final {
public:
    std::vector<Collaboration*> collaborations;
};

CollaborationManager::CollaborationManager() {
    m_impl = std::make_unique<Impl>();
};

CollaborationManager::~CollaborationManager() {
    for (auto& collab : m_impl->collaborations) collab->release();
};

std::span<Collaboration*> CollaborationManager::getCollaborations() const noexcept {
    return m_impl->collaborations;
};

void CollaborationManager::requestCollaborationForLevel(int levelID, FunctionRef<void(Collaboration*)> callback) {
    auto collab = getCollaborationForLevel(levelID);
    callback(collab); // dummy impl
};

Collaboration* CollaborationManager::getCollaborationForLevel(int levelID) const noexcept {
    for (auto const& collab : m_impl->collaborations) {
        if (collab->getLevelID() == levelID) return collab;
    };

    return nullptr;
};

CollaborationManager* CollaborationManager::get() {
    static auto inst = new CollaborationManager();
    return inst;
};