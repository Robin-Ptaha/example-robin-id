#pragma once

#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>

namespace RobinID::db::types {

inline constexpr std::string_view kUser = "robinid.user";

struct User final {
    std::string id_;
    std::string username_;
    std::string email_;
    std::string password_hash_;
};

}  // namespace RobinID::db::types

namespace userver::storages::postgres::io {

template <>
struct CppToUserPg<RobinID::db::types::User> {
    static constexpr DBTypeName postgres_name{RobinID::db::types::kUser.data()};
};

}  // namespace userver::storages::postgres::io
