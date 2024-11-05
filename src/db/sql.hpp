#pragma once

#include <string_view>

namespace RobinID::db::sql {

inline constexpr std::string_view kCreateUser {R"~(
INSERT INTO
    robinid.users (username, email, password_hash)
VALUES
    ($1, $2, $3)
RETURNING users.id::text;
)~"};

}  // namespace RobinID::db::sql
