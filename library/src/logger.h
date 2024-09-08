#ifndef LOGGER_H
#define LOGGER_H

#include <cassert>
#include <ostream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cassert>

#define LOG(level) Logger::log(level, __FILE__, __func__, __LINE__, __DATE__, __TIME__)

enum class LogLevel {
    TRACE = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

class LoggerStream {
public:
    LoggerStream(LogLevel log_level,
                 const char* file, const char* func, int line, const char* date, const char* time)
                : log_level{log_level}, file{file}, func{func}, line{line}, date{date}, time{time} {}
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
        switch(log_level) {
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
        std::cerr << "[" << time << "]-" << "[";

        const char* last_slash = nullptr;
        while(*file != static_cast<char>(NULL)) {
            if(*file == '/' || *file == '\\') {
                last_slash = file + 1;
            }

            file++;
        }

        assert(last_slash != nullptr);
        std::cerr << last_slash << "+" << func << "+" << line << "]" << "> " << stream_buffer.str();
    }

private:
    const char* file;
    const char* func;
    const int line;
    const char* date;
    const char* time;

    const LogLevel log_level;
};

class Logger {
public:
    static LoggerStream log(LogLevel log_level,
            const char* file, const char* func, int line, const char* date, const char* time) {
        return LoggerStream(log_level, file, func, line, date, time);
    }

private:

};

#endif // LOGGER_H
