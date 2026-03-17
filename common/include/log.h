#pragma once

#include <chrono>
#include <ctime>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "colors.h"

/**
 * @brief Enum holding the various levels of log severity.
 */
enum LogLevel {
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_NONE
};

#ifndef CURRENT_LOG_LEVEL
#    define CURRENT_LOG_LEVEL LOG_LEVEL_INFO
#endif

#pragma region Implementation Helpers

// Older MSVC versions require __VA_OPT__ support via /Zc:preprocessor;
// using a helper macro avoids needing that compiler flag.
#if defined(_MSC_VER)
#    define LOG_FMT_ARGS(...) , ##__VA_ARGS__
#else
#    define LOG_FMT_ARGS(...) __VA_OPT__(, ) __VA_ARGS__
#endif

/**
 * @brief Fetches the current time in the `HH:MM:SS.mmm` format.
 *
 * @return Current time with millisecond precision
 */
inline std::string currentTimeFormatted() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;

    std::tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &now_c);  // Use safer localtime_s on Windows
#else
    local_tm =
        *std::localtime(&now_c);  // Use standard localtime on other platforms
#endif

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%H:%M:%S") << "." << std::setfill('0')
        << std::setw(3) << millis.count();
    return oss.str();
}

/**
 * @brief Formats the current file name for logging display.
 *
 * Extracts the filename from the full path and optionally appends a tag.
 *
 * @param file Full file path from __FILE__
 * @param tag Optional tag to append to the filename
 * @return Formatted filename string
 */
inline std::string currentFileFormatted(const std::string& file,
                                        const std::string& tag) {
    size_t last_slash = file.find_last_of("/\\");
    std::string filename =
        (last_slash == std::string::npos) ? file : file.substr(last_slash + 1);

    // Remove file extension
    size_t last_dot = filename.find_last_of(".");
    if (last_dot != std::string::npos) {
        filename = filename.substr(0, last_dot);
    }

    if (tag.empty()) {
        return filename;
    }

    return filename + "#" + tag;
}

/**
 * @brief Core logging function with format support.
 *
 * @tparam Args Variadic template for format arguments
 * @param level Log level for filtering
 * @param color ANSI color code for the log level
 * @param levelStr Short string representation of the log level
 * @param file Source file name
 * @param tag Optional tag for categorizing logs
 * @param format Format string (std::format compatible)
 * @param args Arguments for the format string
 */
template <typename... Args>
void LogImpl(int level, const char* color, const char* levelStr,
             const char* file, const std::string& tag,
             const std::string& format, Args&&... args) {
    if (level >= CURRENT_LOG_LEVEL) {
        std::string message;
        if constexpr (sizeof...(args) > 0) {
            try {
                message = std::vformat(format, std::make_format_args(args...));
            } catch (const std::format_error& e) {
                message = format + " [FORMAT ERROR: " + e.what() + "]";
            }
        } else {
            message = format;
        }

        std::cerr << WHITE << currentTimeFormatted() << " " << color << "["
                  << levelStr << ": " << currentFileFormatted(file, tag) << "] "
                  << message << RESET << std::endl;
    }
}

#pragma endregion

#pragma region Log Macros

/**
 * @brief Log a trace message.
 * Usage: LOG_TRACE("Simple message") or LOG_TRACE("Format {}: {}", var1, var2)
 */
#define LOG_TRACE(format, ...)                            \
    ::LogImpl(LOG_LEVEL_TRACE, CYAN, "TRC", __FILE__, "", \
              format LOG_FMT_ARGS(__VA_ARGS__))

/**
 * @brief Log a trace message with a tag.
 * Usage: LOG_TRACE_TAG("MyTag", "Format {}: {}", var1, var2)
 */
#define LOG_TRACE_TAG(tag, format, ...)                    \
    ::LogImpl(LOG_LEVEL_TRACE, CYAN, "TRC", __FILE__, tag, \
              format LOG_FMT_ARGS(__VA_ARGS__))

/**
 * @brief Log a debug message.
 * Usage: LOG_DEBUG("Simple message") or LOG_DEBUG("Value: {}", someValue)
 */
#define LOG_DEBUG(format, ...)                               \
    ::LogImpl(LOG_LEVEL_DEBUG, MAGENTA, "DBG", __FILE__, "", \
              format LOG_FMT_ARGS(__VA_ARGS__))

/**
 * @brief Log a debug message with a tag.
 */
#define LOG_DEBUG_TAG(tag, format, ...)                       \
    ::LogImpl(LOG_LEVEL_DEBUG, MAGENTA, "DBG", __FILE__, tag, \
              format LOG_FMT_ARGS(__VA_ARGS__))

/**
 * @brief Log an info message.
 * Usage: LOG_INFO("Player position: ({}, {}, {})", x, y, z)
 */
#define LOG_INFO(format, ...)                             \
    ::LogImpl(LOG_LEVEL_INFO, WHITE, "INF", __FILE__, "", \
              format LOG_FMT_ARGS(__VA_ARGS__))

/**
 * @brief Log an info message with a tag.
 */
#define LOG_INFO_TAG(tag, format, ...)                     \
    ::LogImpl(LOG_LEVEL_INFO, WHITE, "INF", __FILE__, tag, \
              format LOG_FMT_ARGS(__VA_ARGS__))

/**
 * @brief Log a warning message.
 * Usage: LOG_WARN("Asset not found: {}", filename)
 */
#define LOG_WARN(format, ...)                              \
    ::LogImpl(LOG_LEVEL_WARN, YELLOW, "WRN", __FILE__, "", \
              format LOG_FMT_ARGS(__VA_ARGS__))

/**
 * @brief Log a warning message with a tag.
 */
#define LOG_WARN_TAG(tag, format, ...)                      \
    ::LogImpl(LOG_LEVEL_WARN, YELLOW, "WRN", __FILE__, tag, \
              format LOG_FMT_ARGS(__VA_ARGS__))

/**
 * @brief Log an error message.
 * Usage: LOG_ERROR("Failed to load texture: {}", errorMessage)
 */
#define LOG_ERROR(format, ...)                           \
    ::LogImpl(LOG_LEVEL_ERROR, RED, "ERR", __FILE__, "", \
              format LOG_FMT_ARGS(__VA_ARGS__))

/**
 * @brief Log an error message with a tag.
 */
#define LOG_ERROR_TAG(tag, format, ...)                   \
    ::LogImpl(LOG_LEVEL_ERROR, RED, "ERR", __FILE__, tag, \
              format LOG_FMT_ARGS(__VA_ARGS__))

#pragma endregion
