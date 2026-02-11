#pragma once
#include<format>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include<spdlog/fmt/fmt.h>

inline void InitLogger() {
    static bool initialized = false;
    if (!initialized) {
        auto logger = spdlog::stdout_color_mt("app");
        spdlog::set_default_logger(logger);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
        spdlog::set_level(spdlog::level::info);
        initialized = true;
    }
}