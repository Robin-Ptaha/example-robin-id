#include "user.hpp"

namespace RobinID::dto {

userver::formats::json::Value Serialize(
    const User& data,
    userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder builder;
    builder["username"] = data.username_;
    builder["email"] = data.email_;
    builder["token"] = data.token_;

    return builder.ExtractValue();
}

}  // namespace RobinID::dto
