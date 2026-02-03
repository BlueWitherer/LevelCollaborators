#include <Collaborations.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace levelcollab;

CollaboratorIcon::CollaboratorIcon(
    int icon,
    IconType type,
    int color1,
    int color2,
    int glow,
    bool useGlow
) : icon(icon), type(type), color1(color1), color2(color2), glow(glow), useGlow(useGlow) {};

SimplePlayer* CollaboratorIcon::createIcon() const {
    if (auto gm = GameManager::sharedState()) {
        auto player = SimplePlayer::create(icon);
        player->setColor(gm->colorForIdx(color1));
        player->setSecondColor(gm->colorForIdx(color2));

        if (useGlow) player->setGlowOutline(gm->colorForIdx(glow));
        if (!useGlow) player->disableGlowOutline();

        return player;
    };

    return nullptr;
};

Collaborator::Collaborator(
    std::string name,
    int userID,
    CollaboratorIcon icon,
    CollaboratorType type,
    bool owner
) : name(std::move(name)), userID(userID), icon(std::move(icon)), type(type), owner(owner) {};

Collaboration::Collaboration(
    int levelID,
    std::vector<Collaborator> collaborators
) : levelID(levelID), collaborators(std::move(collaborators)) {};

GJGameLevel* Collaboration::getLevel() const {
    if (auto glm = GameLevelManager::sharedState()) return glm->getSavedLevel(levelID);
    return nullptr;
};

std::span<const Collaboration> CollaborationManager::getCollaborations() const noexcept {
    return m_collaborations;
};

void CollaborationManager::requestCollaborationForLevel(int levelID, FunctionRef<void(Result<Collaboration>)> callback) {
    auto collab = getCollaborationForLevel(levelID);
    callback(std::move(collab)); // dummy impl
};

Result<Collaboration> CollaborationManager::getCollaborationForLevel(int levelID) const noexcept {
    for (auto const& collab : m_collaborations) {
        if (collab.levelID == levelID) return Ok(collab);
    };

    return Err("No collaboration found");
};

CollaborationManager* CollaborationManager::get() {
    static auto inst = new (std::nothrow) CollaborationManager();
    return inst;
};