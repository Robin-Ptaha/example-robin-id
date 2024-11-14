#include "errors.hpp"

namespace RobinID::utils::errors {

ValidationError::ValidationError(std::string_view field_name, std::string_view message)
    : BaseType(MakeError(field_name, message)) {}

ValidationError::ValidationError(userver::formats::json::Value&& json)
    : BaseType(std::move(json)) {}

userver::formats::json::Value MakeError(std::string_view field_name, std::string_view message) {
    userver::formats::json::ValueBuilder error;
    error[std::move(field_name.data())] = std::move(message);
    return error.ExtractValue();
}

}  // namespace RobinID::utils::errors
