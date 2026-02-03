#pragma once

#ifdef GEODE_IS_WINDOWS
#ifdef CW_LEVELCOLLAB_API_EXPORTING
#define CW_LEVELCOLLAB_API_DLL __declspec(dllexport)
#else
#define CW_LEVELCOLLAB_API_DLL __declspec(dllimport)
#endif
#else
#ifdef CW_LEVELCOLLAB_API_EXPORTING
#define CW_LEVELCOLLAB_API_DLL __attribute__((visibility("default")))
#else
#define CW_LEVELCOLLAB_API_DLL
#endif
#endif

#include <cocos2d.h>

#include <Geode/Result.hpp>

namespace levelcollab {
    // Type of work a collaborator did
    enum class CollaboratorType : unsigned int {
        SpecialThanks = 0, // Thank them for being there, or did miscellaneous work!
        Playtesting = 1, // Tested the level for bugs and gameplay quality
        Optimization = 2, // Optimized groups and/or objects
        Effects = 3, // Added additional visual effects
        Merging = 4, // Merged different sections of the level
        Gameplay = 5, // Worked on the layout and general gameplay
        Decoration = 6, // Decorated the gameplay of the level
        Art = 7, // Created custom art and/or themed assets for the level
        Music = 8, // Composed or selected the music for the level
    };

    struct CollaboratorIcon {
        int icon;
        IconType type;
        int color1;
        int color2;
        int glow;
        bool useGlow;

        CollaboratorIcon() = default;
        CollaboratorIcon(
            int icon,
            IconType type,
            int color1,
            int color2,
            int glow,
            bool useGlow
        );

        SimplePlayer* createIcon() const;
    };

    struct Collaborator {
        std::string name;
        int userID;
        CollaboratorIcon icon;
        CollaboratorType type;
        bool owner;

        Collaborator() = default;
        Collaborator(
            std::string name,
            int userID,
            CollaboratorIcon icon,
            CollaboratorType type,
            bool owner
        );
    };

    struct Collaboration {
        int levelID;
        std::vector<Collaborator> collaborators;

        Collaboration() = default;
        Collaboration(
            int levelID,
            std::vector<Collaborator> collaborators
        );

        GJGameLevel* getLevel() const;
        geode::Result<Collaborator> getCollaboratorByID(int userID) const noexcept;
    };

    class CW_LEVELCOLLAB_API_DLL CollaborationManager : public cocos2d::CCObject {
    private:
        std::vector<Collaboration> m_collaborations;

    protected:
        std::span<const Collaboration> getCollaborations() const noexcept;

        geode::Result<Collaboration> getCollaborationForLevel(int levelID) const noexcept;
        void requestCollaborationForLevel(int levelID, geode::FunctionRef<void(geode::Result<Collaboration>)> callback);

    public:
        static CollaborationManager* get();
    };
};