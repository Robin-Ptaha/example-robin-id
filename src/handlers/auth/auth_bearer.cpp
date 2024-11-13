#include "auth_bearer.hpp"

#include <memory>
#include <userver/http/common_headers.hpp>
#include <userver/storages/secdist/component.hpp>

namespace RobinID::auth {

namespace {

class AuthCheckerBearer final : public userver::server::handlers::auth::AuthCheckerBase {
public:
    using AuthCheckerResult = userver::server::handlers::auth::AuthCheckResult;

    AuthCheckerBearer(
        const jwt::JWTConfig& config, bool is_required
    ) : jwt_manager_(config), is_required_(is_required) {}

    [[nodiscard]] AuthCheckerResult CheckAuth(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& request_context
    ) const override;

    [[nodiscard]] bool SupportsUserAuth() const noexcept override { return true; }

private:
    const jwt::JWTManager jwt_manager_;
    const bool is_required_;
};

AuthCheckerBearer::AuthCheckerResult AuthCheckerBearer::CheckAuth(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& request_context
) const {
    const auto& auth_header_value = request.GetHeader(userver::http::headers::kAuthorization);
    if (auth_header_value.empty()) {
        if (!is_required_) {
            request_context.SetData("id", std::optional<std::string>(std::nullopt));
            return {};
        }

        return AuthCheckerResult {
            AuthCheckerResult::Status::kTokenNotFound,
            {},
            "Empty 'Authorization' header",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    const auto bearer_sep_pos = auth_header_value.find(' ');
    if (bearer_sep_pos == std::string::npos || std::string_view {auth_header_value.data(), bearer_sep_pos} != "Token") {
        return AuthCheckerResult {
            AuthCheckerResult::Status::kTokenNotFound,
            {},
            "'Authorization' header should have 'Bearer some-token' format",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    std::string token {auth_header_value.data() + bearer_sep_pos + 1};
    std::optional<jwt::DecodedToken> decoded_token;
    try {
        decoded_token = ::jwt::decode(std::move(token));
        jwt_manager_.VerifyToken(*decoded_token);
    } catch (const std::exception&) {
        return AuthCheckerResult {
            AuthCheckerResult::Status::kInvalidToken,
            {},
            "Invalid token",
            userver::server::handlers::HandlerErrorCode::kUnauthorized
        };
    }

    const auto user_id = static_cast<std::string>(
        (*decoded_token).get_payload_claim("user_id").as_string());
    
    request_context.SetData(
        "user_auth_data",
        UserAuthData {
            std::move(user_id),
            std::move(*decoded_token)
        }
    );

    return {};
}

}  // namespace

userver::server::handlers::auth::AuthCheckerBasePtr CheckerFactory::operator()(
    const userver::components::ComponentContext& context,
    const userver::server::handlers::auth::HandlerAuthConfig& auth_config,
    const userver::server::handlers::auth::AuthCheckerSettings&
) const {
    const auto jwt_config = context.FindComponent<userver::components::Secdist>().Get().Get<jwt::JWTConfig>();
    const auto is_required = auth_config.HasMember("required") ? auth_config["required"].As<bool>() : false;

    return std::make_shared<AuthCheckerBearer>(std::move(jwt_config), is_required);
}

}  // namespace RobinID::auth
