#include "signup.hpp"

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/crypto/hash.hpp>

// #include <bcrypt/BCrypt.hpp>

#include "../../common/errors.hpp"
#include "../../common/utils.hpp"
#include "../../db/sql.hpp"
#include "../../dto/http_models.hpp"

namespace RobinID::auth::v1::signup::post {

namespace {

dto::AuthV1SignupPostRequest ParseRequest(const userver::formats::json::Value& data) {
    dto::AuthV1SignupPostRequest signup_request;

    signup_request.email_ = utils::ValidateEmail(data, "email");
    signup_request.username_ = utils::ValidateUsername(data, "username");
    signup_request.password_ = utils::ValidatePassword(data, "password");

    return signup_request;
}

}  // namespace

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext&) const {
    dto::AuthV1SignupPostRequest signup_request;
    try {
        signup_request = ParseRequest(request_json);
    } catch (const errors::ValidationError& ex) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnprocessableEntity);
        return ex.ToJson();
    }

    std::string user_id;
    try {
        // const auto password_hash = BCrypt::generateHash(signup_request.password_);
        const auto password_hash = userver::crypto::hash::Sha256(signup_request.password_);
        const auto db_result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster, db::sql::kCreateUser.data(),
            signup_request.username_, signup_request.email_, password_hash);

        user_id = db_result.AsSingleRow<std::string>();
    } catch (const userver::storages::postgres::UniqueViolation& ex) {
        const auto constraint = ex.GetServerMessage().GetConstraint();

        if (constraint == "users_username_key") {
            request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
            return errors::MakeError("username", "this username is already taken");
        }

        throw;
    }

    userver::formats::json::ValueBuilder builder;
    builder["user_id"] = user_id;

    return builder.ExtractValue();
}

}  // namespace RobinID::auth::v1::signup::post
