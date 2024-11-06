#include "login.hpp"

#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/crypto/hash.hpp>
#include <userver/storages/secdist/component.hpp>

// #include <bcrypt/BCrypt.hpp>

#include "../../common/errors.hpp"
#include "../../common/utils.hpp"
#include "../../dto/http_models.hpp"
#include "../../dto/user.hpp"
#include "../../db/sql.hpp"
#include "../../models/user.hpp"

namespace RobinID::auth::v1::login::post {

namespace {

dto::AuthV1LoginPostRequest ParseRequest(const userver::formats::json::Value& data) {
    dto::AuthV1LoginPostRequest login_request;

    login_request.username_ = utils::ValidateUsername(data, "username");
    login_request.password_ = utils::ValidatePassword(data, "password");

    return login_request;
}

}  // namespace

Handler::Handler(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster())
          ,
      jwt_manager_(context.FindComponent<userver::components::Secdist>().Get().Get<jwt::JWTConfig>())
    {}

userver::formats::json::Value Handler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext&) const {
    dto::AuthV1LoginPostRequest login_request;
    try {
        login_request = ParseRequest(request_json);
    } catch (const errors::ValidationError& ex) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnprocessableEntity);
        return ex.ToJson();
    }

    const auto db_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        db::sql::kGetUserByUsername.data(),
        login_request.username_
    );
    if (db_result.IsEmpty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kForbidden);
        userver::formats::json::ValueBuilder builder;
        builder["username"] = "invalid";
        return builder.ExtractValue();
    }

    const auto user = db_result.AsSingleRow<models::User>();
    if (user.password_hash_ != userver::crypto::hash::Sha256(login_request.password_)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kForbidden);
        userver::formats::json::ValueBuilder builder;
        builder["password"] = "invalid";
        return builder.ExtractValue();
    }

    userver::formats::json::ValueBuilder builder;
    builder["user"] = dto::User {
        std::move(user.username_),
        std::move(user.email_),
        jwt_manager_.GenerateToken(std::move(user.id_))
    };

    return builder.ExtractValue();
}

}  // namespace RobinID::auth::v1::login::post
