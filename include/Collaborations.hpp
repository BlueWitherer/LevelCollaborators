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
        Music = 7, // Composed or selected the music for the level
    };

    class CW_LEVELCOLLAB_API_DLL CollaboratorIcon : public cocos2d::CCObject {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        CollaboratorIcon();
        virtual ~CollaboratorIcon();

        bool init(
            int icon,
            IconType type,
            int color1,
            int color2,
            int glow,
            bool useGlow
        );

    public:
        static CollaboratorIcon* create(
            int icon,
            IconType type,
            int color1,
            int color2,
            int glow,
            bool useGlow
        );

        int getIcon() const noexcept;
        IconType getType() const noexcept;
        int getColor1() const noexcept;
        int getColor2() const noexcept;
        int getGlow() const noexcept;
        bool shouldUseGlow() const noexcept;

        SimplePlayer* createIcon() const;
    };

    class CW_LEVELCOLLAB_API_DLL Collaborator : public cocos2d::CCObject {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        Collaborator();
        virtual ~Collaborator();

        bool init(
            std::string name,
            int userID,
            bool owner
        );

    public:
        static Collaborator* create(
            std::string name,
            int userID,
            bool owner
        );

        std::string_view getName() const noexcept;
        int getUserID() const noexcept;
        bool isOwner() const noexcept;
    };

    class CW_LEVELCOLLAB_API_DLL Collaboration : public cocos2d::CCObject {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        Collaboration();
        virtual ~Collaboration();

        bool init(
            int levelID,
            std::vector<Collaborator*> collaborators
        );

    public:
        static Collaboration* create(
            int levelID,
            std::vector<Collaborator*> collaborators
        );

        int getLevelID() const noexcept;
        std::span<Collaborator*> getCollaborators() const noexcept;

        GJGameLevel* getLevel() const;
        Collaborator* getCollaboratorByID(int userID) const noexcept;
    };

    class CW_LEVELCOLLAB_API_DLL CollaborationManager : public cocos2d::CCObject {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        CollaborationManager();
        virtual ~CollaborationManager();

        std::span<Collaboration*> getCollaborations() const noexcept;

        Collaboration* getCollaborationForLevel(int levelID) const noexcept;
        void requestCollaborationForLevel(int levelID, geode::FunctionRef<void(Collaboration*)> callback);

    public:
        static CollaborationManager* get();
    };
};