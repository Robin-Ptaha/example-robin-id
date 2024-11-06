#pragma once

#include <string>
#include <userver/storages/postgres/io/io_fwd.hpp>
#include <userver/storages/postgres/io/pg_types.hpp>

#include "../db/types.hpp"

namespace RobinID::models {

struct User final {
    std::string id_;
    std::string username_;
    std::string email_;
    std::string password_hash_;
};

}  // namespace RobinID::models

namespace userver::storages::postgres::io {

template<>
struct CppToUserPg<RobinID::models::User> {
    static constexpr DBTypeName postgres_name {RobinID::db::types::kUser.data()};
};

}  // namespace userver::stroages::postgres::io
