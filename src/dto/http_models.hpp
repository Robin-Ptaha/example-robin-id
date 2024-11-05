#pragma once

#include <string>

namespace RobinID::dto {

struct AuthV1SignupPostRequest final {
    std::string username_;
    std::string email_;
    std::string password_;
};

}  // namespace RobinID::dto
