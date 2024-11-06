#include "jwt.hpp"

#include <userver/dynamic_config/storage/component.hpp>
#include <userver/formats/yaml/value_builder.hpp>

namespace RobinID::jwt {

JWTConfig::JWTConfig(const userver::formats::json::Value& config)
    : secret_key_{config["secret_key"].As<std::string>()},
      token_expiration_time_{config["jwt_expiration_time"].As<std::chrono::seconds>()} {}

JWTManager::JWTManager(const JWTConfig& config) : config_{config} {
    verifier_.allow_algorithm(::jwt::algorithm::hs256{config_.secret_key_});
}

std::string JWTManager::GenerateToken(std::string user_id) const {
    return ::jwt::create()
        .set_type("JWT")
        .set_expires_at(std::chrono::system_clock::now() + config_.token_expiration_time_)
        .set_payload_claim("user_id", ::jwt::claim(picojson::value(user_id)))
        .sign(::jwt::algorithm::hs256{config_.secret_key_});
}

void JWTManager::VerifyToken(const ::jwt::decoded_jwt<::jwt::traits::kazuho_picojson>& decoded_token) const {
    verifier_.verify(decoded_token);
}

}  // namespace RobinID::jwt
