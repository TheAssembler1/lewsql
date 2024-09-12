#ifndef RESULT_H
#define RESULT_H

#include <optional>

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

    ValueType& get_result() const {
        assert(!is_error() && m_value_opt.has_value());
        return m_value_opt.value();
    }

    const ValueType& get_result_const() const {
        assert(!is_error() && m_value_opt.has_value());
        return m_value_opt.value();
    }

    ErrorType& get_error() const {
        assert(is_error() && m_error_opt.has_value());
        return m_value_opt.value();
    }

    const ErrorType& get_error_const() const {
        assert(is_error() && m_error_opt.has_value());
        return m_value_opt.value();
    }
private:
    std::optional<ValueType> m_value_opt = std::nullopt;
    std::optional<ErrorType> m_error_opt = std::nullopt;
};

#endif // RESULT_H