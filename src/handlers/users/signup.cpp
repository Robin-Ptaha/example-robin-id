#include "signup.hpp"

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>

#include <bcrypt/BCrypt.hpp>

#include "../../db/sql.hpp"
#include "../../dto/user.hpp"
#include "../../utils/errors.hpp"

namespace RobinID::users::v1::signup::post {

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext&) const {
    dto::UsersV1SignupRequest signup_request;
    try {
        signup_request = request_json.As<dto::UsersV1SignupRequest>();
    } catch (const utils::errors::ValidationError& err) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnprocessableEntity);
        return err.GetDetails();
    }

    std::string user_id;
    try {
        const auto password_hash = BCrypt::generateHash(signup_request.password_);
        const auto db_result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster, db::sql::kCreateUser.data(),
            signup_request.username_, signup_request.email_, password_hash);

        user_id = db_result.AsSingleRow<std::string>();
    } catch (const userver::storages::postgres::UniqueViolation& ex) {
        const auto constraint = ex.GetServerMessage().GetConstraint();

        if (constraint == "users_username_key") {
            request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
            return utils::errors::MakeError("username", "this username is already taken");
        }

        throw;
    }

    userver::formats::json::ValueBuilder builder;
    builder["user_id"] = user_id;
    return builder.ExtractValue();
}

}  // namespace RobinID::users::v1::signup::post
