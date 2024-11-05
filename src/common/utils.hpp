#pragma once

#include <userver/formats/json/value.hpp>

namespace RobinID::utils {

const std::string ValidateEmail(
    const userver::formats::json::Value& data,
    std::string_view field_name
);

const std::string ValidateUsername(
    const userver::formats::json::Value& data,
    std::string_view field_name
);

const std::string CheckSize(
    const userver::formats::json::Value& data,
    std::string_view field_name,
    int min,
    int max
);

}  // namespace RobinID::utils
