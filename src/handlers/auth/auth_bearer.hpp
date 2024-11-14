#pragma once

#include <userver/server/handlers/auth/auth_checker_factory.hpp>

#include "../../utils/jwt.hpp"

namespace RobinID::auth {

struct UserAuthData final {
    std::string user_id_;
    jwt::DecodedToken token_;
};

class CheckerFactory final : public userver::server::handlers::auth::AuthCheckerFactoryBase {
   public:
    userver::server::handlers::auth::AuthCheckerBasePtr operator()(
        const userver::components::ComponentContext& context,
        const userver::server::handlers::auth::HandlerAuthConfig& auth_config,
        const userver::server::handlers::auth::AuthCheckerSettings& settings) const override;
};

}  // namespace RobinID::auth
