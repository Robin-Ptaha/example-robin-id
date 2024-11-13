#include "user.hpp"

#include "../utils/validators.hpp"

namespace RobinID::dto {

UsersV1SignupRequest Parse(
    const userver::formats::json::Value& json,
    userver::formats::parse::To<UsersV1SignupRequest>
) {
    UsersV1SignupRequest request;

    request.email_ = utils::validators::ValidateEmail(json, "email");
    request.username_ = utils::validators::ValidateUsername(json, "username");
    request.password_ = utils::validators::ValidatePassword(json, "password");

    return request;
}

UsersV1LoginRequest Parse(
    const userver::formats::json::Value& json,
    userver::formats::parse::To<UsersV1LoginRequest>
) {
    UsersV1LoginRequest request;

    request.username_ = utils::validators::ValidateUsername(json, "username");
    request.password_ = utils::validators::ValidatePassword(json, "password");

    return request;
}

userver::formats::json::Value Serialize(
    const UsersProfileResponse& data,
    userver::formats::serialize::To<userver::formats::json::Value>
) {
    userver::formats::json::ValueBuilder builder;
    builder["id"] = data.id_;
    builder["username"] = data.username_;
    builder["email"] = data.email_;

    return builder.ExtractValue();
}

}  // namespace RobinID::dto
