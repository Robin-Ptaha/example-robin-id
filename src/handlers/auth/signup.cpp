#include "signup.hpp"

#include <fmt/format.h>

#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

#include <bcrypt/BCrypt.hpp>

#include "../../common/utils.hpp"
#include "../../common/errors.hpp"
#include "../../dto/http_models.hpp"
#include "../../db/sql.hpp"

namespace RobinID::auth::v1::signup::post {

namespace {

dto::AuthV1SignupPostRequest ParseRequest(const userver::formats::json::Value& data) {
    dto::AuthV1SignupPostRequest post_request;

    post_request.email_ = utils::ValidateEmail(data, "email");
    post_request.username_ = utils::ValidateUsername(data, "username");
    post_request.password_ = utils::CheckSize(data, "password", 8, 255);

    return post_request;
}

}  // namespace

Handler::Handler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
) : userver::server::handlers::HttpHandlerJsonBase(config, context),
    pg_cluster_(
        context
            .FindComponent<userver::components::Postgres>("postgres-db-1")
            .GetCluster()
    ) {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext&
) const {
    dto::AuthV1SignupPostRequest post_request;
    try {
        post_request = ParseRequest(request_json);
    } catch (const errors::ValidationError& ex) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnprocessableEntity);
        return ex.ToJson();
    }

    std::string user_id;
    try {
        const auto password_hash = BCrypt::generateHash(post_request.password_);
        const auto db_result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            db::sql::kCreateUser.data(),
            post_request.username_,
            post_request.email_,
            password_hash
        );

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