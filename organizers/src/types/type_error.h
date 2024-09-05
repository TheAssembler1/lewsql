#ifndef TYPE_ERROR_H
#define TYPE_ERROR_H

#include <cassert>
#include <exception>
#include <string>

enum class TypeErrorcode {};

class TypeError : std::exception {
    public:
    TypeError(TypeErrorcode err_code) : err_code{err_code} {
    }

    const char* what() const noexcept override {
        switch(err_code) {
        default: assert(0); break;
        }
    }

    private:
    TypeErrorcode err_code;
};


#endif // TYPE_ERROR_H
