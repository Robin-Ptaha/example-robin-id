#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "handlers/auth/auth_bearer.hpp"

#include "handlers/users/login.hpp"
#include "handlers/users/profile.hpp"
#include "handlers/users/signup.hpp"

int main(int argc, char* argv[]) {
    userver::server::handlers::auth::RegisterAuthCheckerFactory(
        "bearer", std::make_unique<RobinID::auth::CheckerFactory>());

    auto component_list = userver::components::MinimalServerComponentList()
                              .Append<userver::server::handlers::Ping>()
                              .Append<userver::components::TestsuiteSupport>()
                              .Append<userver::components::HttpClient>()
                              .Append<userver::server::handlers::TestsControl>()
                              .Append<userver::components::Postgres>("postgres-db-1")
                              .Append<userver::clients::dns::Component>()
                              .Append<userver::components::Secdist>()
                              .Append<userver::components::DefaultSecdistProvider>()
                              .Append<RobinID::users::v1::signup::post::Handler>()
                              .Append<RobinID::users::v1::login::post::Handler>()
                              .Append<RobinID::users::v1::profile::get::Handler>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}
