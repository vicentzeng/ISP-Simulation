
#ifndef ISP_LOG_H
#define ISP_LOG_H
#include <queue>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include <chrono>
#define LOG_TAG "ISP-Simulation"

static std::chrono::steady_clock::time_point clock_ZERO = std::chrono::steady_clock::now();

enum LogLevel {
    LOG_LEVEL_E,
    LOG_LEVEL_I,
    LOG_LEVEL_W,
    LOG_LEVEL_D
};

// Default log level define
#define LOG_LEVEL LOG_LEVEL_D

#define log(tag, ...)           \
    do {                        \
        auto cur_clock = std::chrono::steady_clock::now(); \
        double past_seconds = std::chrono::duration<double>(cur_clock - clock_ZERO).count(); \
        printf("%.03lf %s ", past_seconds, tag);    \
        printf(__VA_ARGS__);    \
        printf("\n");           \
    } while (0)

#define LOGE(...)                   \
    if (LOG_LEVEL >= LOG_LEVEL_E) { \
        log("E", __VA_ARGS__);      \
    }

#define LOGI(...)                   \
    if (LOG_LEVEL >= LOG_LEVEL_I) { \
        log("I", __VA_ARGS__);      \
    }

#define LOGD(...)                   \
    if (LOG_LEVEL >= LOG_LEVEL_D) { \
        log("D", __VA_ARGS__);      \
    }

#endif