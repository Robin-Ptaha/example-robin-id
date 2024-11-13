#pragma once

#include <userver/formats/json.hpp>
#include <userver/formats/parse/common_containers.hpp>

namespace RobinID::dto {

struct UsersV1SignupRequest final {
    std::string username_;
    std::string email_;
    std::string password_;
};

struct UsersV1LoginRequest final {
    std::string username_;
    std::string password_;
};

struct UsersProfileResponse final {
    std::string id_;
    std::string username_;
    std::string email_;
};

UsersV1SignupRequest Parse(const userver::formats::json::Value& json,
                           userver::formats::parse::To<UsersV1SignupRequest>);

UsersV1LoginRequest Parse(const userver::formats::json::Value& json,
                          userver::formats::parse::To<UsersV1LoginRequest>);

userver::formats::json::Value Serialize(
    const UsersProfileResponse& data,
    userver::formats::serialize::To<userver::formats::json::Value>
);

}  // namespace RobinID::dto
