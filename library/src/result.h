#ifndef RESULT_H
#define RESULT_H

#include <optional>

#include "logger.h"

namespace Library::Result {

template<typename R, typename E>
class Result {
    public:
    using ValueType = R;
    using ErrorType = E;

    Result(ValueType const& res): m_value_opt(res) {}
    Result(ValueType&& res): m_value_opt(std::move(res)) {}

    Result(ErrorType const& error): m_error_opt(error) {}
    Result(ErrorType&& error): m_error_opt(std::move(error)){}

    bool is_error() const {
        return m_error_opt.has_value();
    }

    bool is_value() const {
        return m_value_opt.has_value();
    }

    decltype(auto) get_value() const {
        assert(!is_error() && m_value_opt.has_value());
        return m_value_opt.value();
    }

    decltype(auto) get_error() const {
        assert(is_error() && m_error_opt.has_value());
        return m_error_opt.value();
    }

    private:
    std::optional<ValueType> m_value_opt = std::nullopt;
    std::optional<ErrorType> m_error_opt = std::nullopt;
};

enum VoidValue {
    Ok
};

template<typename E>
class Result<void, E> {
    public:
    using ValueType = void;
    using ErrorType = E;

    Result(VoidValue) {}
    Result(ErrorType const& error): m_error_opt(error) {}
    Result(ErrorType&& error): m_error_opt(std::move(error)){}

    bool is_error() const {
        return m_error_opt.has_value();
    }

    bool is_value() const {
        return !m_error_opt.has_value();
    }

    decltype(auto) get_value() const {
        assert(0);
    }

    decltype(auto) get_error() const {
        assert(is_error() && m_error_opt.has_value());
        return m_error_opt.value();
    }

    private:
    std::optional<ErrorType> m_error_opt = std::nullopt;
};

}

using namespace Library::Result;

#endif // RESULT_H
