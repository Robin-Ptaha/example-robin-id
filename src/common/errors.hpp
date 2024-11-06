#pragma once

#include <userver/formats/json/value_builder.hpp>

namespace RobinID::errors {

userver::formats::json::Value MakeError(std::string field_name, std::string message);

class ValidationError {
   public:
    ValidationError(std::string field_name, std::string message);

    userver::formats::json::Value ToJson() const;

   private:
    userver::formats::json::Value json_error_body_;
};

}  // namespace RobinID::errors
