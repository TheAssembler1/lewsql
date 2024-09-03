#ifndef ORGANIZER_ERROR_H
#define ORGANIZER_ERROR_H

#include <cassert>
#include <exception>
#include <string>

enum class OrganizerErrorCode {
    MethodNotImplemented,
};

class OrganizerError : std::exception {
    public:
    OrganizerError(OrganizerErrorCode err_code) : err_code{err_code} {
    }

    const char* what() const noexcept override {
        switch(err_code) {
        case(OrganizerErrorCode::MethodNotImplemented): return "organizer error method not implemented"; break;
        default: assert(0); break;
        }
    }

    private:
    OrganizerErrorCode err_code;
};

#endif // ORGANIZER_ERROR_H
