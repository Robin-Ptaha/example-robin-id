cmake_minimum_required(VERSION 3.12)
project(RobinID CXX)


# Adding userver dependency
find_package(userver COMPONENTS core postgresql QUIET)
if(NOT userver_FOUND)  # Fallback to subdirectory usage
    # Enable userver libraries that are needed in this project
    set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)

    # Compatibility mode: some systems don't support these features
    set(USERVER_FEATURE_CRYPTOPP_BLAKE2 OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_GRPC_CHANNELZ OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_REDIS_HI_MALLOC ON CACHE BOOL "" FORCE)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver)
        message(STATUS "Using userver framework from third_party/userver")
        add_subdirectory(third_party/userver)
    else()
        message(FATAL_ERROR "Either install the userver or provide a path to it")
    endif()
endif()

userver_setup_environment()

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/jwt-cpp)

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/libbcrypt)

# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
    src/handlers/auth/signup.hpp
    src/handlers/auth/signup.cpp
    src/handlers/auth/login.hpp
    src/handlers/auth/login.cpp
    src/common/errors.cpp
    src/common/errors.hpp
    src/common/utils.cpp
    src/common/utils.hpp
    src/common/jwt.cpp
    src/common/jwt.hpp
    src/common/consts.hpp
    src/models/user.hpp
    src/db/sql.hpp
    src/dto/http_models.hpp
    src/dto/user.hpp
    src/dto/user.cpp
)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::postgresql)

target_include_directories(${PROJECT_NAME}_objs PUBLIC jwt-cpp)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC jwt-cpp)

target_include_directories(${PROJECT_NAME}_objs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libbcrypt/include)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC bcrypt)

# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# # Unit Tests
# add_executable(${PROJECT_NAME}_unittest
#      src/hello_test.cpp
# )
# target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
# add_google_tests(${PROJECT_NAME}_unittest)


# # Benchmarks
# add_executable(${PROJECT_NAME}_benchmark
#     src/hello_benchmark.cpp
# )
# target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver::ubench)
# add_google_benchmark_tests(${PROJECT_NAME}_benchmark)


# # Functional Tests
# include(UserverTestsuite)

# userver_testsuite_add_simple()


# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

set(CONFIG_JWT ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}/jwt_config.json)

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
