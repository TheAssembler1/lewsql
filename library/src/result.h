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
        assert(!is_error() && m_value_opt.has_value());
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
        assert(0);
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

#define UNWRAP(result_expr)                                           \
    ({                                                                \
        auto _tmp = (result_expr);                                    \
        LOG(LogLevel::ERROR) << _tmp.get_error().what() << std::endl; \
        _tmp.get_value();                                             \
    })

#define UNWRAP_OR_PROP_ERROR(result_expr) \
    ({                                    \
        auto _tmp = (result_expr);        \
        if(_tmp.is_error())               \
            return _tmp.get_error();      \
        _tmp.get_value();                 \
    })


#endif // RESULT_H
