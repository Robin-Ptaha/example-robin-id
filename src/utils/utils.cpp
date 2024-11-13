#include "utils.hpp"

#include <fmt/core.h>
#include <regex>
#include <string>

#include "errors.hpp"

namespace RobinID::utils {

namespace {

// email
inline const std::regex kEmailPattern {"(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+"};

// username
inline const std::regex kUsernamePattern {"^[a-zA-Z0-9_]+$"};
inline const int kUsernameLengthMin = 3;
inline const int kUsernameLengthMax = 32;

// password
inline const std::regex kPasswordPattern {"^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]+$"};
inline const int kPasswordLengthMin = 8;
inline const int kPasswordLengthMax = 256;

const std::string CheckSize(const std::string& value, std::string_view field_name, int min,
                            int max) {
    const int length = value.length();
    if (min <= length && length <= max) {
        return value;
    }

    throw errors::ValidationError{
        std::string{field_name},
        fmt::format("username length must be between {} and {} characters", min, max)};
}

}  // namespace

const std::string ValidateEmail(const userver::formats::json::Value& data,
                                std::string_view field_name) {
    if (!data.HasMember(field_name)) {
        throw errors::ValidationError{std::string{field_name}, "required"};
    }

    const auto& email = data[field_name].As<std::string>();
    if (email.empty()) {
        throw errors::ValidationError{std::string{field_name}, "cannot be empty"};
    }

    if (std::regex_match(email, kEmailPattern)) {
        return email;
    }

    throw errors::ValidationError{std::string{field_name}, "invalid"};
}

const std::string ValidateUsername(const userver::formats::json::Value& data,
                                   std::string_view field_name) {
    if (!data.HasMember(field_name)) {
        throw errors::ValidationError{std::string{field_name}, "required"};
    }

    const auto& username = data[field_name].As<std::string>();
    if (std::regex_match(username, kUsernamePattern)) {
        return CheckSize(username, field_name, kUsernameLengthMin, kUsernameLengthMax);
    }

    throw errors::ValidationError{std::string{field_name}, "invalid"};
}

const std::string ValidatePassword(const userver::formats::json::Value& data,
                                   std::string_view field_name) {
    if (!data.HasMember(field_name)) {
        throw errors::ValidationError{std::string{field_name}, "required"};
    }

    const auto& password = data[field_name].As<std::string>();
    if (std::regex_match(password, kPasswordPattern)) {
        return CheckSize(password, field_name, kPasswordLengthMin, kPasswordLengthMax);
    }

    throw errors::ValidationError{std::string{field_name}, "at least one uppercase letter, one lowercase letter, one number and one special character"};
}

const std::string CheckSize(const userver::formats::json::Value& data, std::string_view field_name,
                            int min, int max) {
    if (!data.HasMember(field_name)) {
        throw errors::ValidationError{std::string{field_name}, "required"};
    }

    const auto& data_str = data[field_name].As<std::string>();
    return CheckSize(data_str, field_name, min, max);
}

}  // namespace RobinID::utils
