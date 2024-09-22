#ifndef LOGGER_H
#define LOGGER_H

#include <cassert>
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>

namespace Library::Logger {

using OsStreams = std::vector<std::ostream*>;

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
    LoggerStream(OsStreams os_streams, LoggerStreamPrintOpts logger_stream_print_opts)
    : os_streams{os_streams}, logger_stream_print_opts{logger_stream_print_opts} {
    }
    std::ostringstream stream_buffer;

    void flush() {
        if(!printed_tag) {
            switch(logger_stream_print_opts.log_level) {
            case LogLevel::TRACE: stream_buffer << "[TRACE]-"; break;
            case LogLevel::INFO: stream_buffer << "[INFO]--"; break;
            case LogLevel::WARNING: stream_buffer << "[WARN]--"; break;
            case LogLevel::ERROR: stream_buffer << "[ERROR]-"; break;
            case LogLevel::FATAL: stream_buffer << "[FATAL]-"; break;
            default: assert(0); break;
            }

            stream_buffer << "[" << logger_stream_print_opts.time << "]-"
                          << "[";

            const char* last_slash = nullptr;
            while(*logger_stream_print_opts.file != static_cast<char>(NULL)) {
                if(*logger_stream_print_opts.file == '/') {
                    last_slash = logger_stream_print_opts.file + 1;
                }
                logger_stream_print_opts.file++;
            }

            assert(last_slash != nullptr);
            stream_buffer << last_slash << "+" << logger_stream_print_opts.func << "+" << logger_stream_print_opts.line << "]"
                          << "> ";

            printed_tag = true;
        }

        for(const auto& os_stream : os_streams) {
            *os_stream << stream_buffer.str();

            stream_buffer.clear();
        }
    }

    template <typename T> LoggerStream& operator<<(T&& message) {
        stream_buffer << std::forward<T>(message);
        return *this;
    }

    LoggerStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
        manip(stream_buffer);
        return *this;
    }

    ~LoggerStream() {
        flush();
    }

    private:
    LoggerStreamPrintOpts logger_stream_print_opts;
    OsStreams os_streams;
    bool printed_tag{false};
};

class Logger {
    public:
    static void init(OsStreams _os_streams) {
        os_streams = _os_streams;
    }

    static LoggerStream log(LoggerStreamPrintOpts logger_stream_print_opts) {
        return LoggerStream(os_streams, logger_stream_print_opts);
    }

    private:
    static inline OsStreams os_streams{};
};

} // namespace Library::Logger

using LogLevel = Library::Logger::LogLevel;
using Logger = Library::Logger::Logger;
using OsStreams = Library::Logger::OsStreams;

#define LOG(level) Library::Logger::Logger::log({level, __FILE__, __func__, __LINE__, __DATE__, __TIME__})

#endif // LOGGER_H
