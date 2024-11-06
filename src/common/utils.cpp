#include "utils.hpp"

#include <fmt/core.h>
#include <regex>
#include <string>
// #include <unicode/translit.h>
// #include <unicode/unistr.h>
// #include <unicode/ustream.h>
// #include <unicode/uconfig.h>
// #include <unicode/platform.h>

#include "errors.hpp"

namespace RobinID::utils {

namespace {

const std::string CheckSize(const std::string& value, std::string_view field_name, int min, int max) {
    const int length = value.length();
    // const auto length = icu::UnicodeString::fromUTF8(value).countChar32(0);
    if (min < length && length < max) {
        return value;
    }

    throw errors::ValidationError{std::string{field_name},
                                  fmt::format("must be longer than {} characters and less than {}", min, max)};
}

}  // namespace

const std::string ValidateEmail(const userver::formats::json::Value& data, std::string_view field_name) {
    if (!data.HasMember(field_name)) {
        throw errors::ValidationError{std::string{field_name}, "required"};
    }

    const auto& email = data[field_name].As<std::string>();
    if (email.empty()) {
        throw errors::ValidationError{std::string{field_name}, "cannot be empty"};
    }

    const std::regex pattern{"(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+"};
    if (std::regex_match(email, pattern)) {
        return email;
    }

    throw errors::ValidationError{std::string{field_name}, "invalid"};
}

const std::string ValidateUsername(const userver::formats::json::Value& data, std::string_view field_name) {
    if (!data.HasMember(field_name)) {
        throw errors::ValidationError{std::string{field_name}, "required"};
    }

    const auto& username = data[field_name].As<std::string>();
    const std::regex pattern{"^[a-zA-Z0-9_]+$"};
    if (std::regex_match(username, pattern)) {
        return CheckSize(username, field_name, 3, 128);
    }

    throw errors::ValidationError{std::string{field_name}, "invalid"};
}

const std::string CheckSize(const userver::formats::json::Value& data, std::string_view field_name, int min, int max) {
    if (!data.HasMember(field_name)) {
        throw errors::ValidationError{std::string{field_name}, "required"};
    }

    const auto& data_str = data[field_name].As<std::string>();
    return CheckSize(data_str, field_name, min, max);
}

}  // namespace RobinID::utils
