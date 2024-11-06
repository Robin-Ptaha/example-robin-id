#pragma once

#include <userver/formats/json.hpp>

namespace RobinID::dto {

struct User final {
    std::string username_;
    std::string email_;
    std::string token_;
};

userver::formats::json::Value Serialize(const User& data, userver::formats::serialize::To<userver::formats::json::Value>);

}  // namespace RobinID::dto
