#ifndef LOGGER_H
#define LOGGER_H

#include <cassert>
#include <ostream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cassert>

#define LOG(level) Logger::log({level, __FILE__, __func__, __LINE__, __DATE__, __TIME__ })

enum class LogLevel {
    TRACE = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

struct LoggerStreamPrintOpts {
    LogLevel log_level;
    const char* file;
    const char* func;
    const int line;
    const char* date;
    const char* time;
};

class LoggerStream {
public:
    LoggerStream(LoggerStreamPrintOpts&& logger_stream_print_opts)
                : logger_stream_print_opts{std::move(logger_stream_print_opts)} {}
    std::ostringstream stream_buffer;

    template<typename T>
    LoggerStream& operator<<(T&& message) {
        stream_buffer << std::forward<T>(message);

        return *this;
    }

    LoggerStream& operator<<(std::ostream&(*manip)(std::ostream&)) {
        manip(stream_buffer);
        return *this;
    }

    ~LoggerStream() {
        switch(logger_stream_print_opts.log_level) {
            case LogLevel::TRACE:
                std::cerr << "[TRACE]-";
                break;
            case LogLevel::INFO:
                std::cerr << "[INFO]--";
                break;
            case LogLevel::WARNING:
                std::cerr << "[WARN]--";
                break;
            case LogLevel::ERROR:
                std::cerr << "[ERROR]-";
                break;
            case LogLevel::FATAL:
                std::cerr << "[FATAL]-";
                break;
            default:
                assert(0);
                break;
        }


        // FIXME: make these configurable
        std::cerr << "[" << logger_stream_print_opts.time << "]-" << "[";

        const char* last_slash = nullptr;
        while(*logger_stream_print_opts.file != static_cast<char>(NULL)) {
#ifdef __unix__
            if(*logger_stream_print_opts.file == '/') {
                last_slash = logger_stream_print_opts.file + 1;
            }
#elif __win32__
            if(*file == '\\') {
                last_slash = file + 1;
            }
#else
#error "system not recognized as __win32 or __unix"
#endif

            logger_stream_print_opts.file++;
        }

        assert(last_slash != nullptr);
        std::cerr << last_slash << "+" << logger_stream_print_opts.func << "+" << logger_stream_print_opts.line << "]" << "> " << stream_buffer.str();
    }

private:
    LoggerStreamPrintOpts logger_stream_print_opts;
};

class Logger {
public:
    static void init();

    static LoggerStream log(LoggerStreamPrintOpts&& logger_stream_print_opts) {
        return LoggerStream(std::move(logger_stream_print_opts));
    }
};

#endif // LOGGER_H
