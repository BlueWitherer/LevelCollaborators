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

#include <Geode/Bindings.hpp>

namespace levelcollab {
    // Type of work a collaborator did
    enum class CollaboratorType : unsigned int {
        SpecialThanks = 0,  // Thank them for being there, or did miscellaneous work!
        Playtesting = 1,    // Tested the level for bugs and gameplay quality
        Optimization = 2,   // Optimized groups and/or objects
        Effects = 3,        // Added additional visual effects
        Merging = 4,        // Merged different sections of the level
        Gameplay = 5,       // Worked on the layout and general gameplay
        Decoration = 6,     // Decorated the gameplay of the level
        Art = 7,            // Created custom art and/or themed assets for the level
        Music = 8,          // Composed or selected the music for the level
    };

    struct CollaboratorIcon final {
    private:
        int m_icon;
        IconType m_type;
        int m_color1;
        int m_color2;
        int m_glow;
        bool m_useGlow;

    public:
        CollaboratorIcon(
            int icon,
            IconType type,
            int color1,
            int color2,
            int glow,
            bool useGlow);

        static std::shared_ptr<CollaboratorIcon> create(int icon, IconType type, int color1, int color2, int glow, bool useGlow);

        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getIcon() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL IconType getIconType() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getColor1() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getColor2() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getGlow() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL bool isUseGlow() const noexcept;

        CW_LEVELCOLLAB_API_DLL SimplePlayer* createIcon() const;
    };

    struct Collaborator final {
    private:
        std::string m_name;
        int m_accountID;
        std::shared_ptr<CollaboratorIcon> m_icon;
        CollaboratorType m_type;
        bool m_isOwner;

    public:
        Collaborator(
            std::string name,
            int accountID,
            std::shared_ptr<CollaboratorIcon> icon,
            CollaboratorType type,
            bool isOwner);

        static std::shared_ptr<Collaborator> create(std::string name, int accountID, std::shared_ptr<CollaboratorIcon> icon, CollaboratorType type, bool isOwner);

        [[nodiscard]] CW_LEVELCOLLAB_API_DLL geode::ZStringView getName() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getAccountID() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL std::weak_ptr<CollaboratorIcon> getIcon() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL CollaboratorType getType() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL bool isOwner() const noexcept;
    };

    struct Collaboration final {
    private:
        int m_levelID;
        std::vector<std::weak_ptr<Collaborator>> m_collaborators;

    public:
        Collaboration(
            int levelID,
            std::vector<std::weak_ptr<Collaborator>> collaborators);

        static std::shared_ptr<Collaboration> create(int levelID, std::vector<std::weak_ptr<Collaborator>> collaborators);

        [[nodiscard]] CW_LEVELCOLLAB_API_DLL int getLevelID() const noexcept;
        [[nodiscard]] CW_LEVELCOLLAB_API_DLL std::span<const std::weak_ptr<Collaborator>> getCollaborators() const noexcept;

        CW_LEVELCOLLAB_API_DLL GJGameLevel* getLevel() const;
    };

    class CW_LEVELCOLLAB_API_DLL CollaborationManager final {
        friend class Collaboration;
        friend class Collaborator;

    private:
        std::unordered_map<int, std::shared_ptr<Collaboration>> m_collaborations;
        std::unordered_map<int, std::shared_ptr<Collaborator>> m_collaborators;

    protected:
        void registerCollab(std::shared_ptr<Collaboration> collab);
        void registerCollaborator(std::shared_ptr<Collaborator> collaborator);

        std::weak_ptr<Collaboration> getCollab(int levelID) const noexcept;
        std::weak_ptr<Collaborator> getCollaborator(int accountID) const noexcept;

    public:
        static CollaborationManager* get() noexcept;

        std::vector<std::weak_ptr<Collaboration>> getCollabs() const noexcept;

        std::shared_ptr<Collaboration> getCollabForLevel(int levelID) const noexcept;
    };

    CW_LEVELCOLLAB_API_DLL void requestCollabForLevel(int levelID, geode::FunctionRef<void(std::weak_ptr<Collaboration>)> callback);
    CW_LEVELCOLLAB_API_DLL void getCollaboratorInfo(int accountID, geode::FunctionRef<void(geode::Result<GJUserScore*>)> callback);
};