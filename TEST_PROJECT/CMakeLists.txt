cmake_minimum_required(VERSION 3.23)

project(test_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
# использую буст 1.84.0 потому что версия 1.83.0 в этом источнике битая 
#accumulator c ней не ставится весь день потерял 
FetchContent_Declare(
    boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
)

FetchContent_MakeAvailable(boost)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
)
FetchContent_MakeAvailable(Catch2)

enable_testing()

add_library(core
    src/asyncbuffer.cpp
    src/processor.cpp
    src/utility.cpp
)

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${boost_SOURCE_DIR}/boost  
)

target_link_libraries(core
    PUBLIC
        Boost::program_options
        Boost::filesystem
        Boost::system
        Boost::accumulators     
        tomlplusplus::tomlplusplus
        spdlog::spdlog
)

add_executable(test_project
    src/main.cpp
)

target_link_libraries(test_project
    PRIVATE
        core
)

add_executable(test_runner
    src/main_test.cpp
)

target_link_libraries(test_runner
    PRIVATE
        core
        Catch2::Catch2WithMain
)

add_test(NAME my_test COMMAND test_runner)
