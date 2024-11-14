#pragma once

#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>

namespace RobinID::utils::errors {

class ValidationError : public userver::server::handlers::ExceptionWithCode<
                            userver::server::handlers::HandlerErrorCode::kRequestParseError> {
   public:
    ValidationError(std::string_view field_name, std::string_view message);

    explicit ValidationError(userver::formats::json::Value&& json);
};

userver::formats::json::Value MakeError(std::string_view field_name, std::string_view message);

}  // namespace RobinID::utils::errors
