#include "errors.hpp"

namespace RobinID::errors {

userver::formats::json::Value MakeError(
    std::string field_name,
    std::string message
) {
    userver::formats::json::ValueBuilder error;
    error[std::move(field_name)] = std::move(message);
    return error.ExtractValue();
}

ValidationError::ValidationError(std::string field_name, std::string message) {
    json_error_body_ = MakeError(std::move(field_name), std::move(message));
}

userver::formats::json::Value ValidationError::ToJson() const {
    return json_error_body_;
}

}  // namespace RobinID::errors
