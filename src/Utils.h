#pragma once

#include <API.h>

#include <Geode/Geode.hpp>

namespace levelcollab {
    class AuthState final {
    private:
        int m_accountID;
        int m_userID;
        std::string m_username;
        std::string m_token;

    public:
        static AuthState* get() noexcept;

        void setAuthInfo(int accountID, int userID, std::string username, std::string token);

        [[nodiscard]] int getAccountID() const noexcept;
        [[nodiscard]] int getUserID() const noexcept;
        [[nodiscard]] geode::ZStringView getUsername() const noexcept;
        [[nodiscard]] geode::ZStringView getToken() const noexcept;
    };

    namespace manager {  // very wip
        void requestCollabUpsert(int levelID, std::weak_ptr<Collaboration> collab, geode::CopyableFunction<void(bool success, std::string const& message)>&& callback = nullptr);
    };

    namespace fetch {
        void getUserByUsername(std::string_view username, geode::CopyableFunction<void(geode::Result<GJUserScore*>)>&& callback);
    };
};