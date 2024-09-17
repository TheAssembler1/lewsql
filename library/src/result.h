#ifndef RESULT_H
#define RESULT_H

#include <functional>
#include <optional>

#include "logger.h"

namespace Library::Result {

template <typename R, typename E> class Result {
    public:
    using ValueType = R;
    using ErrorType = E;

    Result() = delete;

    Result(ValueType const& res) : m_value_opt(res) {
    }
    Result(ValueType&& res) : m_value_opt(std::move(res)) {
    }

    Result(ErrorType const& error) : m_error_opt(error) {
    }
    Result(ErrorType&& error) : m_error_opt(std::move(error)) {
    }

    ValueType value_or(std::function<ValueType(ErrorType)> func) {
        if(is_error()) {
            return func(get_error());
        } else {
            return get_value();
        }
    }

    bool is_error() const {
        return m_error_opt.has_value();
    }

    bool is_value() const {
        return m_value_opt.has_value();
    }

    decltype(auto) get_value() {
        if(is_error()) {
            LOG(LogLevel::FATAL) << m_error_opt.value().what() << std::endl;
            assert("attempt to unwrap value on error");
        }
        return m_value_opt.value();
    }

    decltype(auto) get_error() {
        assert(is_error() && m_error_opt.has_value());
        return m_error_opt.value();
    }

    private:
    std::optional<ValueType> m_value_opt = std::nullopt;
    std::optional<ErrorType> m_error_opt = std::nullopt;
};

enum VoidValue { Ok };

template <typename E> class Result<void, E> {
    public:
    using ValueType = void;
    using ErrorType = E;

    Result() = delete;

    Result(VoidValue) {
    }
    Result(ErrorType const& error) : m_error_opt(error) {
    }
    Result(ErrorType&& error) : m_error_opt(std::move(error)) {
    }

    bool is_error() const {
        return m_error_opt.has_value();
    }

    bool is_value() const {
        return !m_error_opt.has_value();
    }

    decltype(auto) get_value() {
        if(is_error()) {
            LOG(LogLevel::FATAL) << m_error_opt.value().what() << std::endl;
            assert("attempt to unwrap value on error");
        }
        assert("attempt to get value of VoidValue::Ok");
    }

    decltype(auto) get_error() {
        assert(is_error() && m_error_opt.has_value());
        return m_error_opt.value();
    }

    private:
    std::optional<ErrorType> m_error_opt = std::nullopt;
};

} // namespace Library::Result

using namespace Library::Result;

#define PROP_IF_ERROR(result) \
    {                                    \
        if(result.is_error())            \
            return result.get_error();   \
    }

#define UNWRAP_OR_PROP_ERROR(result)   \
    ({                                 \
        if(result.is_error())          \
            return result.get_error(); \
        result.get_value();            \
    })


#endif // RESULT_H
