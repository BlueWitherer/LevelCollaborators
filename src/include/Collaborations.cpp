#include <Collaborations.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace levelcollab;

class CollaboratorIcon::Impl final {
public:
    int icon;
    IconType type;
    int color1;
    int color2;
    int glow;
    bool useGlow;
};

CollaboratorIcon::CollaboratorIcon() {
    m_impl = std::make_unique<Impl>();
};

CollaboratorIcon::~CollaboratorIcon() {};

bool CollaboratorIcon::init(
    int icon,
    IconType type,
    int color1,
    int color2,
    int glow,
    bool useGlow
) {
    m_impl->icon = icon;
    m_impl->type = type;
    m_impl->color1 = color1;
    m_impl->color2 = color2;
    m_impl->glow = glow;
    m_impl->useGlow = useGlow;

    return true;
};

int CollaboratorIcon::getIcon() const noexcept {
    return m_impl->icon;
};

IconType CollaboratorIcon::getType() const noexcept {
    return m_impl->type;
};

int CollaboratorIcon::getColor1() const noexcept {
    return m_impl->color1;
};

int CollaboratorIcon::getColor2() const noexcept {
    return m_impl->color2;
};

int CollaboratorIcon::getGlow() const noexcept {
    return m_impl->glow;
};

bool CollaboratorIcon::shouldUseGlow() const noexcept {
    return m_impl->useGlow;
};

SimplePlayer* CollaboratorIcon::createIcon() const {
    if (auto gm = GameManager::sharedState()) {
        auto player = SimplePlayer::create(m_impl->icon);
        player->setColor(gm->colorForIdx(m_impl->color1));
        player->setSecondColor(gm->colorForIdx(m_impl->color2));

        if (m_impl->useGlow) player->setGlowOutline(gm->colorForIdx(m_impl->glow));
        if (!m_impl->useGlow) player->disableGlowOutline();

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

class Collaborator::Impl final {
public:
    std::string name;
    int userID;
    bool owner;
};

Collaborator::Collaborator() {
    m_impl = std::make_unique<Impl>();
};

Collaborator::~Collaborator() {};

bool Collaborator::init(
    std::string name,
    int userID,
    bool owner
) {
    m_impl->name = std::move(name);
    m_impl->userID = userID;
    m_impl->owner = owner;

    return true;
};

std::string_view Collaborator::getName() const noexcept {
    return m_impl->name;
};

int Collaborator::getUserID() const noexcept {
    return m_impl->userID;
};

bool Collaborator::isOwner() const noexcept {
    return m_impl->owner;
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

class Collaboration::Impl final {
public:
    int levelID;
    std::vector<Collaborator*> collaborators;
};

Collaboration::Collaboration() {
    m_impl = std::make_unique<Impl>();
};

Collaboration::~Collaboration() {
    for (auto& collab : m_impl->collaborators) collab->release();
};

bool Collaboration::init(
    int levelID,
    std::vector<Collaborator*> collaborators
) {
    m_impl->levelID = levelID;
    m_impl->collaborators = std::move(collaborators);

    return true;
};

int Collaboration::getLevelID() const noexcept {
    return m_impl->levelID;
};

GJGameLevel* Collaboration::getLevel() const {
    if (auto glm = GameLevelManager::sharedState()) return glm->getSavedLevel(m_impl->levelID);
    return nullptr;
};

Collaborator* Collaboration::getCollaboratorByID(int userID) const noexcept {
    for (auto const& collab : m_impl->collaborators) {
        if (collab->getUserID() == userID) return collab;
    };

    return nullptr;
};

std::span<Collaborator*> Collaboration::getCollaborators() const noexcept {
    return m_impl->collaborators;
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