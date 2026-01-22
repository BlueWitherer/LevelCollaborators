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
    struct Collaboration {
        struct Collaborator {
            std::string name;
            int userID;
            bool owner;

            Collaborator(
                std::string name,
                int userID,
                bool owner
            );
        };

        int levelID;
        std::vector<Collaborator> collaborators;

        Collaboration(
            int levelID,
            std::vector<Collaborator> collaborators
        );

        GJGameLevel* getLevel() const;
    };

    class CW_LEVELCOLLAB_API_DLL CollaborationManager : public cocos2d::CCObject {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        CollaborationManager();
        virtual ~CollaborationManager();

    public:
        static CollaborationManager* get();
    };
};