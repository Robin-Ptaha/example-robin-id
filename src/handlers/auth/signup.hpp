#pragma once

#include <string_view>

#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>

namespace RobinID::auth::v1::signup::post {

class Handler final : public userver::server::handlers::HttpHandlerJsonBase {
   public:
    static constexpr std::string_view kName = "auth-v1-signup";

    Handler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request, const userver::formats::json::Value&,
        userver::server::request::RequestContext& request_context) const override final;

   private:
    const userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace RobinID::auth::v1::signup::post
