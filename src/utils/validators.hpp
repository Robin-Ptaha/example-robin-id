#pragma once

#include <userver/formats/json.hpp>

namespace RobinID::utils::validators {
    
const std::string ValidateEmail(const userver::formats::json::Value& data,
                                std::string_view field_name);
const std::string ValidateUsername(std::string_view username);
const std::string ValidateUsername(const userver::formats::json::Value& data,
                                   std::string_view field_name);
const std::string ValidatePassword(const userver::formats::json::Value& data,
                                   std::string_view field_name);

const std::string CheckSize(const userver::formats::json::Value& data, std::string_view field_name,
                            int min, int max);

}  // namespace RobinID::utils::validators
