#include <Collaborations.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace levelcollab;

Collaboration::Collaboration(
    int levelID,
    std::vector<Collaborator> collaborators
) : levelID(levelID), collaborators(std::move(collaborators)) {};

Collaboration::Collaborator::Collaborator(
    std::string name,
    int userID,
    bool owner
) : name(std::move(name)), userID(userID), owner(owner) {};

GJGameLevel* Collaboration::getLevel() const {
    if (auto glm = GameLevelManager::sharedState()) glm->getSavedLevel(levelID);
    return nullptr;
};

class CollaborationManager::Impl final {
public:
    std::vector<Collaboration> collaborations;
};

CollaborationManager::CollaborationManager() {
    m_impl = std::make_unique<Impl>();
};

CollaborationManager::~CollaborationManager() {};

CollaborationManager* CollaborationManager::get() {
    static auto inst = new CollaborationManager();
    return inst;
};